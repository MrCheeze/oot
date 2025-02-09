/*
 * File: z_bg_hidan_hamstep.c
 * Overlay: ovl_Bg_Hidan_Hamstep
 * Description: Stone Steps and Platforms (Fire Temple)
 */

#include "z_bg_hidan_hamstep.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "quake.h"
#include "regs.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "versions.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Hamstep_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Hamstep_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Hamstep_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Hamstep_actor_draw(Actor* thisx, PlayState* play);

void mode_tatakareWait(BgHidanHamstep* this, PlayState* play);
void mode_gatan(BgHidanHamstep* this, PlayState* play);
void mode_gatanWait(BgHidanHamstep* this, PlayState* play);
void mode_domino(BgHidanHamstep* this, PlayState* play);
void mode_soshiteKaidan(BgHidanHamstep* this, PlayState* play);

static f32 Hamstep_takasaDT[] = {
    -20.0f, -120.0f, -220.0f, -320.0f, -420.0f,
};

static ColliderTrisElementInit HidanHamstepAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 3.0f, -20.0f }, { -20.0f, 3.0f, 20.0f }, { 20.0f, 3.0f, 20.0f } } },
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
        { { { 20.0f, 3.0f, 20.0f }, { 20.0f, 3.0f, -20.0f }, { -20.0f, 3.0f, -20.0f } } },
    },
};

static ColliderTrisInit HidanHamstepAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    HidanHamstepAcTrisElemData,
};

ActorProfile Bg_Hidan_Hamstep_Profile = {
    /**/ ACTOR_BG_HIDAN_HAMSTEP,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanHamstep),
    /**/ Bg_Hidan_Hamstep_actor_ct,
    /**/ Bg_Hidan_Hamstep_actor_dt,
    /**/ Bg_Hidan_Hamstep_actor_move,
    /**/ Bg_Hidan_Hamstep_actor_draw,
};

static BgHidanHamstepActionFunc processTbl[] = {
    mode_tatakareWait, mode_gatan, mode_gatanWait, mode_domino, mode_soshiteKaidan,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static f32 xz[][2] = {
    { -100.0f, 40.0f },  { 100.0f, 40.0f }, { -100.0f, 0.0f },   { 100.0f, 0.0f },
    { -100.0f, -40.0f }, { 100.0f, 40.0f }, { -100.0f, -80.0f }, { -50.0f, -80.0f },
    { 0.0f, -80.0f },    { 50.0f, -80.0f }, { 100.0f, -80.0f },
};

void Bg_Hidan_Hamstep_actor_set_process(BgHidanHamstep* this, s32 action) {
    this->action = action;
    this->actionFunc = processTbl[action];
}

s32 ideyoAshiba(BgHidanHamstep* this, PlayState* play2) {
    BgHidanHamstep* step = this;
    s32 i;
    Vec3f pos;
    f32 sin;
    f32 cos;
    s16 params;
    PlayState* play = play2;

    pos.y = this->dyna.actor.home.pos.y - 100.0f;
    pos = pos; // Required to match
    sin = sin_s(this->dyna.actor.shape.rot.y + 0x8000);
    cos = cos_s(this->dyna.actor.shape.rot.y + 0x8000);

    for (i = 0; i < 5; i++) {
        pos.x = (((i * 160.0f) + 60.0f) * sin) + this->dyna.actor.home.pos.x;
        pos.z = (((i * 160.0f) + 60.0f) * cos) + this->dyna.actor.home.pos.z;

        params = (i + 1) & 0xFF;
        params |= PARAMS_GET_NOSHIFT(this->dyna.actor.params, 8, 8);

        step = (BgHidanHamstep*)Actor_info_make_child_actor(&play->actorCtx, &step->dyna.actor, play, ACTOR_BG_HIDAN_HAMSTEP,
                                                   pos.x, pos.y, pos.z, this->dyna.actor.world.rot.x,
                                                   this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z, params);

        if (step == NULL) {
            return 0;
        }
    }
    return 1;
}

void Bg_Hidan_Hamstep_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanHamstep* this = (BgHidanHamstep*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    Vec3f sp48[3];
    s32 i;
    s32 i2;
    BgHidanHamstep* step;

    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    ValueSet_process(&this->dyna.actor, value_init);

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
        ClObjTris_ct(play, &this->collider);
        ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &HidanHamstepAcTrisData, this->colliderItems);

        for (i = 0; i < 2; i++) {
            for (i2 = 0; i2 < 3; i2++) {
                sp48[i2].x = HidanHamstepAcTrisData.elements[i].dim.vtx[i2].x + this->dyna.actor.home.pos.x;
                sp48[i2].y = HidanHamstepAcTrisData.elements[i].dim.vtx[i2].y + this->dyna.actor.home.pos.y;
                sp48[i2].z = HidanHamstepAcTrisData.elements[i].dim.vtx[i2].z + this->dyna.actor.home.pos.z;
            }
            CollisionCheck_Uty_setTrisPos(&this->collider, i, &sp48[0], &sp48[1], &sp48[2]);
        }
    }

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStoneStep1Col, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStoneStep2Col, &colHeader);
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 8))) {
        if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + (-20.0f);
            Bg_Hidan_Hamstep_actor_set_process(this, 4);
        } else {
            this->dyna.actor.world.pos.y =
                Hamstep_takasaDT[PARAMS_GET_U(this->dyna.actor.params, 0, 8) - 1] + this->dyna.actor.home.pos.y;
            Bg_Hidan_Hamstep_actor_set_process(this, 4);
        }
    } else if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
        Bg_Hidan_Hamstep_actor_set_process(this, 0);
    } else {
        Bg_Hidan_Hamstep_actor_set_process(this, 2);
    }

    this->dyna.actor.gravity = -1.2f;
    this->dyna.actor.minVelocityY = -12.0f;

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
        // "Fire Temple Object [Hammer Step] appears"
        PRINTF("◯◯◯炎の神殿オブジェクト【ハンマーステップ】出現\n");
        if (ideyoAshiba(this, play) == 0) {
            step = this;

            // "[Hammer Step] I can't create a step!"
            PRINTF("【ハンマーステップ】 足場産れない！！\n");
            PRINTF("%s %d\n", "../z_bg_hidan_hamstep.c", 425);

            while (step != NULL) {
                Actor_delete(&step->dyna.actor);
                step = (BgHidanHamstep*)step->dyna.actor.child;
            }
        }
    }
}

void Bg_Hidan_Hamstep_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanHamstep* this = (BgHidanHamstep*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
        ClObjTris_dt_nzf(play, &this->collider);
    }
}

void set_kemuri_ashiba(BgHidanHamstep* step, PlayState* play) {
    Vec3f pos = step->dyna.actor.world.pos;
    s32 i;
    f32 sin;
    f32 cos;

    pos.y -= 20.0f;

    dust_fly_set2(play, &pos, 0.0f, 0, 600, 300, 0);

    sin = sin_s(step->dyna.actor.shape.rot.y + 0x8000);
    cos = cos_s(step->dyna.actor.shape.rot.y + 0x8000);

    pos.y = step->dyna.actor.world.pos.y;

    for (i = 0; i < ARRAY_COUNT(xz); i++) {
        pos.x = (xz[i][1] * sin) + (xz[i][0] * cos) + step->dyna.actor.world.pos.x;
        pos.z = ((xz[i][1] * cos) - (xz[i][0] * sin)) + step->dyna.actor.world.pos.z;
        dust_fly_set2(play, &pos, 0.0f, 0, 150, 150, 0);
    }
}

void set_kemuri_zentaiGatan(BgHidanHamstep* this, PlayState* play) {
    BgHidanHamstep* child = (BgHidanHamstep*)this->dyna.actor.child;

    while (child != NULL) {
        if (PARAMS_GET_U(child->dyna.actor.params, 0, 8) != 0) {
            set_kemuri_ashiba(child, play);
        }
        child = (BgHidanHamstep*)child->dyna.actor.child;
    }
}

void Actor_position_hosei(BgHidanHamstep* this, BgHidanHamstep* parent) {
    BgHidanHamstep* child;

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) >= 2) {
        if (parent->dyna.actor.world.pos.y < this->dyna.actor.world.pos.y) {
            this->dyna.actor.world.pos.y = parent->dyna.actor.world.pos.y;
        } else if ((this->dyna.actor.world.pos.y - parent->dyna.actor.world.pos.y) < -100.0f) {
            this->dyna.actor.world.pos.y = parent->dyna.actor.world.pos.y - 100.0f;
        }
    }

    child = (BgHidanHamstep*)this->dyna.actor.child;

    while (child != NULL) {
        if (this->dyna.actor.world.pos.y < child->dyna.actor.world.pos.y) {
            child->dyna.actor.world.pos.y = this->dyna.actor.world.pos.y;
        }
        child = (BgHidanHamstep*)child->dyna.actor.child;
    }
}

void Actor_position_moveF_LookMother(BgHidanHamstep* this) {
    BgHidanHamstep* parent = (BgHidanHamstep*)this->dyna.actor.parent;
    f32 frameDivisor = R_UPDATE_RATE * 0.5f;

    if (parent != NULL) {
        this->dyna.actor.velocity.y = parent->dyna.actor.velocity.y;

        if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 1) {
            this->dyna.actor.world.pos.y = parent->dyna.actor.world.pos.y - 100.0f;
        } else {
            this->dyna.actor.world.pos.y += (this->dyna.actor.velocity.y * frameDivisor);
        }

        Actor_position_hosei(this, parent);
    }
}

void mode_tatakareWait(BgHidanHamstep* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        makeOnepointDemo(play, 3310, 100, &this->dyna.actor, CAM_ID_MAIN);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_HAMMER_SWITCH);
        this->collider.base.acFlags = AC_NONE;
        Bg_Hidan_Hamstep_actor_set_process(this, 1);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 8));
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mode_gatan(BgHidanHamstep* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 quakeIndex;

    Actor_position_moveF(&this->dyna.actor);

    if (((this->dyna.actor.world.pos.y - this->dyna.actor.home.pos.y) < (-20.0f - this->dyna.actor.minVelocityY)) &&
        (this->dyna.actor.velocity.y <= 0.0f)) {
        this->unk_244++;

        if (this->unk_244 >= 7) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + -20.0f;
            Bg_Hidan_Hamstep_actor_set_process(this, 4);
        } else {
            this->dyna.actor.velocity.y *= -0.24f;

            if (1) {}

            if (this->unk_244 == 1) {
                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, -15536);
                setScaleQuake(quakeIndex, 0, 0, 500, 0);
                setTimerQuake(quakeIndex, 20);
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
                z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 255, 20, 150);
                set_kemuri_zentaiGatan(this, play);
                PRINTF("A(%d)\n", this->dyna.actor.params);
            }
        }
    }
}

void mode_gatanWait(BgHidanHamstep* this, PlayState* play) {
    s32 pad;
    BgHidanHamstep* parent = (BgHidanHamstep*)this->dyna.actor.parent;

    Actor_position_moveF_LookMother(this);

    if ((parent->action == 4) || ((parent->action == 3) && (parent->unk_244 >= 5))) {
        if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 1) {
            this->dyna.actor.world.pos.y =
                Hamstep_takasaDT[PARAMS_GET_U(this->dyna.actor.params, 0, 8) - 1] + this->dyna.actor.home.pos.y;
            Bg_Hidan_Hamstep_actor_set_process(this, 4);
        } else {
            Bg_Hidan_Hamstep_actor_set_process(this, 3);
        }
    }
}

void mode_domino(BgHidanHamstep* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 quakeIndex;

    Actor_position_moveF(&this->dyna.actor);
    Actor_position_hosei(this, (BgHidanHamstep*)this->dyna.actor.parent);

#if DEBUG_FEATURES
    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) <= 0 || PARAMS_GET_U(this->dyna.actor.params, 0, 8) >= 6) {
        // "[Hammer Step] arg_data strange (arg_data = %d)"
        PRINTF("【ハンマーステップ】 arg_data おかしい (arg_data = %d)", this->dyna.actor.params);
        PRINTF("%s %d\n", "../z_bg_hidan_hamstep.c", 696);
    }
#endif

    if (((this->dyna.actor.world.pos.y - this->dyna.actor.home.pos.y) <=
         Hamstep_takasaDT[PARAMS_GET_U(this->dyna.actor.params, 0, 8) - 1]) &&
        (this->dyna.actor.velocity.y <= 0.0f)) {
        this->unk_244++;

        if (this->unk_244 >= 7) {
            this->dyna.actor.world.pos.y =
                Hamstep_takasaDT[PARAMS_GET_U(this->dyna.actor.params, 0, 8) - 1] + this->dyna.actor.home.pos.y;
            Bg_Hidan_Hamstep_actor_set_process(this, 3);
        } else {
            this->dyna.actor.velocity.y *= -0.24f;

            if (1) {}

            if (this->unk_244 == 1) {
                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, -15536);
                setScaleQuake(quakeIndex, 20, 1, 0, 0);
                setTimerQuake(quakeIndex, 7);

                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
                z_vibctl2_vib_setQ(SQ(100.0f), 255, 20, 150);
                set_kemuri_ashiba(this, play);

#if OOT_VERSION >= PAL_1_0
                if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 5) {
                    Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                }
#endif

                PRINTF("B(%d)\n", this->dyna.actor.params);
            }
        }
    }
}

void mode_soshiteKaidan(BgHidanHamstep* this, PlayState* play) {
}

void Bg_Hidan_Hamstep_actor_move(Actor* thisx, PlayState* play) {
    BgHidanHamstep* this = (BgHidanHamstep*)thisx;

    this->actionFunc(this, play);
}

void Bg_Hidan_Hamstep_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_hamstep.c", 782);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_hidan_hamstep.c", 787);

    if (PARAMS_GET_U(thisx->params, 0, 8) == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStoneStep1DL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStoneStep2DL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_hamstep.c", 796);
}
