#include "z_en_bom_chu.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#include "libc64/math64.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "sys_math3d.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define BOMBCHU_SCALE 0.01f

void En_Bom_Chu_actor_ct(Actor* thisx, PlayState* play);
void En_Bom_Chu_actor_dt(Actor* thisx, PlayState* play);
void En_Bom_Chu_actor_move(Actor* thisx, PlayState* play2);
void En_Bom_Chu_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(EnBomChu* this, PlayState* play);
static void mode_move(EnBomChu* this, PlayState* play);
static void mode_explode(EnBomChu* this, PlayState* play);

ActorProfile En_Bom_Chu_Profile = {
    /**/ ACTOR_EN_BOM_CHU,
    /**/ ACTORCAT_EXPLOSIVE,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnBomChu),
    /**/ En_Bom_Chu_actor_ct,
    /**/ En_Bom_Chu_actor_dt,
    /**/ En_Bom_Chu_actor_move,
    /**/ En_Bom_Chu_actor_draw,
};

static ColliderJntSphElementInit BomChuAllJntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
};

static ColliderJntSphInit BomChuAllJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(BomChuAllJntSphElemData),
    BomChuAllJntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000 * BOMBCHU_SCALE, ICHAIN_STOP),
};

void En_Bom_Chu_actor_ct(Actor* thisx, PlayState* play) {
    static u8 start_top[] = { 250, 0, 0, 250 };
    static u8 start_root[] = { 200, 0, 0, 130 };
    static u8 end_top[] = { 150, 0, 0, 100 };
    static u8 end_root[] = { 100, 0, 0, 50 };
    EnBomChu* this = (EnBomChu*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;

    ValueSet_process(&this->actor, value_init);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &BomChuAllJntSphData, this->colliderElements);

    this->collider.elements[0].dim.worldSphere.radius = this->collider.elements[0].dim.modelSphere.radius;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = start_top[i];
        blureInit.p2StartColor[i] = start_root[i];
        blureInit.p1EndColor[i] = end_top[i];
        blureInit.p2EndColor[i] = end_root[i];
    }

    blureInit.elemDuration = 16;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 0;

    EffectAdd(play, &this->blure1Index, EFFECT_BLURE1, 0, 0, &blureInit);
    EffectAdd(play, &this->blure2Index, EFFECT_BLURE1, 0, 0, &blureInit);

    this->actor.room = -1;
    this->timer = 120;
    this->actionFunc = mode_wait;
}

void En_Bom_Chu_actor_dt(Actor* thisx, PlayState* play) {
    EnBomChu* this = (EnBomChu*)thisx;

    EffectFreeIndex(play, this->blure1Index);
    EffectFreeIndex(play, this->blure2Index);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void mode_explode_init(EnBomChu* this, PlayState* play) {
    EnBom* bomb;
    s32 i;

    bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y,
                               this->actor.world.pos.z, 0, 0, 0, BOMB_BODY);
    if (bomb != NULL) {
        bomb->timer = 0;
    }

    this->timer = 1;
    this->actor.speed = 0.0f;

    if (this->actor.depthInWater > 0.0f) {
        for (i = 0; i < 40; i++) {
            Effect_SS_Bubble_ct(play, &this->actor.world.pos, 1.0f, 5.0f, 30.0f, 0.25f);
        }
    }

    this->actionFunc = mode_explode;
}

static void vector_mult(Vec3f* a, Vec3f* b, Vec3f* dest) {
    dest->x = (a->y * b->z) - (a->z * b->y);
    dest->y = (a->z * b->x) - (a->x * b->z);
    dest->z = (a->x * b->y) - (a->y * b->x);
}

static void set_new_shape_angle(EnBomChu* this, CollisionPoly* floorPoly, PlayState* play) {
    Vec3f normal;
    Vec3f vec;
    f32 angle;
    f32 magnitude;
    f32 normDotUp;
    MtxF mf;

    this->actor.floorPoly = floorPoly;

    normal.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    normal.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    normal.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    normDotUp = DOTXYZ(normal, this->axisUp);

    if (!(fabsf(normDotUp) >= 1.0f)) {
        angle = facos(normDotUp);

        if (!(angle < 0.001f)) {
            vector_mult(&this->axisUp, &normal, &vec);
            //! @bug this function expects a unit vector but `vec` is not normalized
            Matrix_rotateVector(angle, &vec, MTXMODE_NEW);

            Matrix_Position(&this->axisLeft, &vec);
            this->axisLeft = vec;

            vector_mult(&this->axisLeft, &normal, &this->axisForwards);

            magnitude = Math3DVecLength(&this->axisForwards);

            if (magnitude < 0.001f) {
                mode_explode_init(this, play);
                return;
            }

            this->axisForwards.x *= 1.0f / magnitude;
            this->axisForwards.y *= 1.0f / magnitude;
            this->axisForwards.z *= 1.0f / magnitude;

            this->axisUp = normal;

            if (1) {}

            // mf = (axisLeft | axisUp | axisForwards)

            mf.xx = this->axisLeft.x;
            mf.yx = this->axisLeft.y;
            mf.zx = this->axisLeft.z;

            mf.xy = normal.x;
            mf.yy = normal.y;
            mf.zy = normal.z;

            mf.xz = this->axisForwards.x;
            mf.yz = this->axisForwards.y;
            mf.zz = this->axisForwards.z;

            Matrix_to_rotate_new(&mf, &this->actor.world.rot, 0);

            // A hack for preventing bombchus from sticking to ledges.
            // The visual rotation reverts the sign inversion (shape.rot.x = -world.rot.x).
            // The better fix would be making Actor_position_speed_set_XY compute XYZ velocity better,
            // or not using it and make the bombchu compute its own velocity.
            this->actor.world.rot.x = -this->actor.world.rot.x;
        }
    }
}

static void mode_wait(EnBomChu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        mode_explode_init(this, play);
        return;
    }

    if (Actor_carry_end_check(&this->actor, play)) {
        this->actor.world.pos = player->actor.world.pos;
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
        this->actor.shape.rot.y = player->actor.shape.rot.y;

        // rot.y = 0 -> +z (forwards in model space)
        this->axisForwards.x = sin_s(this->actor.shape.rot.y);
        this->axisForwards.y = 0.0f;
        this->axisForwards.z = cos_s(this->actor.shape.rot.y);

        // +y (up in model space)
        this->axisUp.x = 0.0f;
        this->axisUp.y = 1.0f;
        this->axisUp.z = 0.0f;

        // rot.y = 0 -> +x (left in model space)
        this->axisLeft.x = sin_s(this->actor.shape.rot.y + 0x4000);
        this->axisLeft.y = 0;
        this->axisLeft.z = cos_s(this->actor.shape.rot.y + 0x4000);

        this->actor.speed = 8.0f;
        //! @bug there is no NULL check on the floor poly.  If the player is out of bounds the floor poly will be NULL
        //! and will cause a crash inside this function.
        set_new_shape_angle(this, this->actor.floorPoly, play);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED; // make chu targetable
        Actor_bound_SE_set(play, &this->actor);
        this->actionFunc = mode_move;
    }
}

static void mode_move(EnBomChu* this, PlayState* play) {
    CollisionPoly* polySide;
    CollisionPoly* polyUpDown;
    s32 bgIdSide;
    s32 bgIdUpDown;
    s32 i;
    f32 lineLength;
    Vec3f posA;
    Vec3f posB;
    Vec3f posSide;
    Vec3f posUpDown;

    this->actor.speed = 8.0f;
    lineLength = this->actor.speed * 2.0f;

    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer == 0) || (this->collider.base.acFlags & AC_HIT) ||
        ((this->collider.base.ocFlags1 & OC1_HIT) && (this->collider.base.oc->category != ACTORCAT_PLAYER))) {
        mode_explode_init(this, play);
        return;
    }

    posA.x = this->actor.world.pos.x + (this->axisUp.x * 2.0f);
    posA.y = this->actor.world.pos.y + (this->axisUp.y * 2.0f);
    posA.z = this->actor.world.pos.z + (this->axisUp.z * 2.0f);

    posB.x = this->actor.world.pos.x - (this->axisUp.x * 4.0f);
    posB.y = this->actor.world.pos.y - (this->axisUp.y * 4.0f);
    posB.z = this->actor.world.pos.z - (this->axisUp.z * 4.0f);

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &posA, &posB, &posUpDown, &polyUpDown, true, true, true, true,
                                &bgIdUpDown) &&
        !(T_BGCheck_getWallCodeBit_ai(&play->colCtx, polyUpDown, bgIdUpDown) & WALL_FLAG_CRAWLSPACE) &&
        !T_BGCheck_CheckArrowNoHit(&play->colCtx, polyUpDown, bgIdUpDown)) {
        // forwards
        posB.x = (this->axisForwards.x * lineLength) + posA.x;
        posB.y = (this->axisForwards.y * lineLength) + posA.y;
        posB.z = (this->axisForwards.z * lineLength) + posA.z;

        if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                    &bgIdSide) &&
            !(T_BGCheck_getWallCodeBit_ai(&play->colCtx, polySide, bgIdSide) & WALL_FLAG_CRAWLSPACE) &&
            !T_BGCheck_CheckArrowNoHit(&play->colCtx, polySide, bgIdSide)) {
            set_new_shape_angle(this, polySide, play);
            this->actor.world.pos = posSide;
            this->actor.floorBgId = bgIdSide;
            this->actor.speed = 0.0f;
        } else {
            if (this->actor.floorPoly != polyUpDown) {
                set_new_shape_angle(this, polyUpDown, play);
            }

            this->actor.world.pos = posUpDown;
            this->actor.floorBgId = bgIdUpDown;
        }
    } else {
        this->actor.speed = 0.0f;
        lineLength *= 3.0f;
        posA = posB;

        for (i = 0; i < 3; i++) {
            if (i == 0) {
                // backwards
                posB.x = posA.x - (this->axisForwards.x * lineLength);
                posB.y = posA.y - (this->axisForwards.y * lineLength);
                posB.z = posA.z - (this->axisForwards.z * lineLength);
            } else if (i == 1) {
                // left
                posB.x = posA.x + (this->axisLeft.x * lineLength);
                posB.y = posA.y + (this->axisLeft.y * lineLength);
                posB.z = posA.z + (this->axisLeft.z * lineLength);
            } else {
                // right
                posB.x = posA.x - (this->axisLeft.x * lineLength);
                posB.y = posA.y - (this->axisLeft.y * lineLength);
                posB.z = posA.z - (this->axisLeft.z * lineLength);
            }

            if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                        &bgIdSide) &&
                !(T_BGCheck_getWallCodeBit_ai(&play->colCtx, polySide, bgIdSide) & WALL_FLAG_CRAWLSPACE) &&
                !T_BGCheck_CheckArrowNoHit(&play->colCtx, polySide, bgIdSide)) {
                set_new_shape_angle(this, polySide, play);
                this->actor.world.pos = posSide;
                this->actor.floorBgId = bgIdSide;
                break;
            }
        }

        if (i == 3) {
            // no collision nearby
            mode_explode_init(this, play);
        }
    }

    chase_angle(&this->actor.shape.rot.x, -this->actor.world.rot.x, 0x800);
    chase_angle(&this->actor.shape.rot.y, this->actor.world.rot.y, 0x800);
    chase_angle(&this->actor.shape.rot.z, this->actor.world.rot.z, 0x800);

    Actor_player_level_SE_set(&this->actor, NA_SE_IT_BOMBCHU_MOVE - SFX_FLAG);
}

static void mode_explode(EnBomChu* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_delete(&this->actor);
    }
}

/**
 * Transform coordinates from model space to world space, according to current orientation.
 * `modelPos` is expected to already be at world scale (1/100 compared to model scale)
 */
void get_blure_pos(EnBomChu* this, Vec3f* modelPos, Vec3f* dest) {
    f32 x = modelPos->x + this->visualJitter;

    dest->x = this->actor.world.pos.x + (this->axisLeft.x * x) + (this->axisUp.x * modelPos->y) +
              (this->axisForwards.x * modelPos->z);
    dest->y = this->actor.world.pos.y + (this->axisLeft.y * x) + (this->axisUp.y * modelPos->y) +
              (this->axisForwards.y * modelPos->z);
    dest->z = this->actor.world.pos.z + (this->axisLeft.z * x) + (this->axisUp.z * modelPos->y) +
              (this->axisForwards.z * modelPos->z);
}

void set_ripple(EnBomChu* this, PlayState* play, f32 y) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = y;
    pos.z = this->actor.world.pos.z;

    Effect_SS_G_Ripple_ct2(play, &pos, 70, 500, 0);
    Effect_SS_G_Ripple_ct2(play, &pos, 70, 500, 4);
    Effect_SS_G_Ripple_ct2(play, &pos, 70, 500, 8);
}

void En_Bom_Chu_actor_move(Actor* thisx, PlayState* play2) {
    static Vec3f local_blure_top = { 0.0f, 7.0f, -6.0f };
    static Vec3f local_blure_root0 = { 12.0f, 0.0f, -5.0f };
    static Vec3f local_blure_root1 = { -12.0f, 0.0f, -5.0f };
    PlayState* play = play2;
    EnBomChu* this = (EnBomChu*)thisx;
    s16 yaw;
    f32 sin;
    f32 cos;
    f32 tempX;
    Vec3f blureP1;
    Vec3f blureP2;
    WaterBox* waterBox;
    f32 waterY;

    if (this->actor.floorBgId != BGCHECK_SCENE) {
        yaw = this->actor.shape.rot.y;
        BGCheckCollection_proc(&play->colCtx, this->actor.floorBgId, &this->actor);

        if (yaw != this->actor.shape.rot.y) {
            yaw = this->actor.shape.rot.y - yaw;

            sin = sin_s(yaw);
            cos = cos_s(yaw);

            tempX = this->axisForwards.x;
            this->axisForwards.x = (this->axisForwards.z * sin) + (cos * tempX);
            this->axisForwards.z = (this->axisForwards.z * cos) - (sin * tempX);

            tempX = this->axisUp.x;
            this->axisUp.x = (this->axisUp.z * sin) + (cos * tempX);
            this->axisUp.z = (this->axisUp.z * cos) - (sin * tempX);

            tempX = this->axisLeft.x;
            this->axisLeft.x = (this->axisLeft.z * sin) + (cos * tempX);
            this->axisLeft.z = (this->axisLeft.z * cos) - (sin * tempX);
        }
    }

    this->actionFunc(this, play);
    Actor_position_moveF_XY(&this->actor);

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);

    if (this->actionFunc != mode_wait) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }

    Actor_world_to_eye(&this->actor, 0.0f);

    if (this->actionFunc == mode_move) {
        this->visualJitter =
            (5.0f + (fqrand() * 3.0f)) * sin_s(((fqrand() * (f32)0x200) + (f32)0x3000) * this->timer);

        get_blure_pos(this, &local_blure_top, &blureP1);

        get_blure_pos(this, &local_blure_root0, &blureP2);
        EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blure1Index), &blureP1, &blureP2);

        get_blure_pos(this, &local_blure_root1, &blureP2);
        EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blure2Index), &blureP1, &blureP2);

        waterY = this->actor.world.pos.y;

        if (T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterY,
                                 &waterBox)) {
            this->actor.depthInWater = waterY - this->actor.world.pos.y;

            if (this->actor.depthInWater < 0.0f) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
                    set_ripple(this, play, waterY);
                }

                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;
            } else {
                if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->timer != 120)) {
                    set_ripple(this, play, waterY);
                } else {
                    Effect_SS_Bubble_ct(play, &this->actor.world.pos, 0.0f, 3.0f, 15.0f, 0.25f);
                }

                this->actor.bgCheckFlags |= BGCHECKFLAG_WATER;
            }
        } else {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;
            this->actor.depthInWater = BGCHECK_Y_MIN;
        }
    }
}

void En_Bom_Chu_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBomChu* this = (EnBomChu*)thisx;
    f32 colorIntensity;
    s32 blinkHalfPeriod;
    s32 blinkTime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bom_chu.c", 921);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);

    if (this->timer >= 40) {
        blinkTime = this->timer % 20;
        blinkHalfPeriod = 10;
    } else if (this->timer >= 10) {
        blinkTime = this->timer % 10;
        blinkHalfPeriod = 5;
    } else {
        blinkTime = this->timer & 1;
        blinkHalfPeriod = 1;
    }

    if (blinkTime > blinkHalfPeriod) {
        blinkTime = 2 * blinkHalfPeriod - blinkTime;
    }

    colorIntensity = blinkTime / (f32)blinkHalfPeriod;

    gDPSetEnvColor(POLY_OPA_DISP++, 9.0f + (colorIntensity * 209.0f), 9.0f + (colorIntensity * 34.0f),
                   35.0f + (colorIntensity * -35.0f), 255);
    Matrix_translate(this->visualJitter * (1.0f / BOMBCHU_SCALE), 0.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_bom_chu.c", 956);
    gSPDisplayList(POLY_OPA_DISP++, gBombchuDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bom_chu.c", 961);
}
