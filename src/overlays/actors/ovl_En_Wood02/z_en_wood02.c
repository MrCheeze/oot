/*
 * File: z_en_wood02.c
 * Overlay: ovl_En_Wood02
 * Description: Trees, bushes, leaves
 */

#include "z_en_wood02.h"
#include "assets/objects/object_wood02/object_wood02.h"

#define FLAGS 0

void En_Wood02_Actor_ct(Actor* thisx, PlayState* play2);
void En_wood02_Actor_dt(Actor* thisx, PlayState* play);
void En_wood02_move(Actor* thisx, PlayState* play2);
void En_wood02_display(Actor* thisx, PlayState* play);

/**
 * WOOD_SPAWN_SPAWNER is also used by some individual trees: En_wood02_move also checks for parent before running any
 * despawning code.
 *  */
typedef enum WoodSpawnType {
    /* 0 */ WOOD_SPAWN_NORMAL,
    /* 1 */ WOOD_SPAWN_SPAWNED,
    /* 2 */ WOOD_SPAWN_SPAWNER
} WoodSpawnType;

typedef enum WoodDrawType {
    /* 0 */ WOOD_DRAW_TREE_CONICAL,
    /* 1 */ WOOD_DRAW_TREE_OVAL,
    /* 2 */ WOOD_DRAW_TREE_KAKARIKO_ADULT,
    /* 3 */ WOOD_DRAW_BUSH_GREEN,
    /* 4 */ WOOD_DRAW_4, // Used for black bushes and green leaves
    /* 5 */ WOOD_DRAW_LEAF_YELLOW
} WoodDrawType;

ActorProfile En_Wood02_Profile = {
    /**/ ACTOR_EN_WOOD02,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_WOOD02,
    /**/ sizeof(EnWood02),
    /**/ En_Wood02_Actor_ct,
    /**/ En_wood02_Actor_dt,
    /**/ En_wood02_move,
    /**/ En_wood02_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_TREE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK5,
        { 0x00000000, 0x00, 0x00 },
        { 0x0FC0074A, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 60, 0, { 0, 0, 0 } },
};

static f32 wood_reng[] = { 707.0f, 525.0f, 510.0f, 500.0f, 566.0f, 141.0f };

static s16 wood_ang[] = { 0x1FFF, 0x4C9E, 0x77F5, 0xA5C9, 0xD6C3, 0xA000 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 5600, ICHAIN_STOP),
};

static Gfx* main_mdl[] = {
    object_wood02_DL_0078D0, object_wood02_DL_007CA0, object_wood02_DL_0080D0, object_wood02_DL_000090,
    object_wood02_DL_000340, object_wood02_DL_000340, object_wood02_DL_000700,
};

static Gfx* main_mdl2[] = {
    object_wood02_DL_007968,
    object_wood02_DL_007D38,
    object_wood02_DL_0081A8,
    NULL,
    NULL,
    NULL,
    object_wood02_DL_007AD0,
    object_wood02_DL_007E20,
    object_wood02_DL_008350,
    object_wood02_DL_000160,
    object_wood02_DL_000440,
    object_wood02_DL_000700,
};

static f32 cr;

static f32 sr;

s32 c_check(EnWood02* this, PlayState* play, Vec3f* pos) {
    f32 phi_f12;

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, pos, &this->actor.projectedPos, &this->actor.projectedW);

    phi_f12 = ((this->actor.projectedW == 0.0f) ? 1000.0f : fabsf(1.0f / this->actor.projectedW));

    if ((-this->actor.cullingVolumeScale < this->actor.projectedPos.z) &&
        (this->actor.projectedPos.z < (this->actor.cullingVolumeDistance + this->actor.cullingVolumeScale)) &&
        (((fabsf(this->actor.projectedPos.x) - this->actor.cullingVolumeScale) * phi_f12) < 1.0f) &&
        (((this->actor.projectedPos.y + this->actor.cullingVolumeDownward) * phi_f12) > -1.0f) &&
        (((this->actor.projectedPos.y - this->actor.cullingVolumeScale) * phi_f12) < 1.0f)) {
        return true;
    }
    return false;
}

/** Spawns similar-looking trees or bushes only when the player is sufficiently close. Presumably done this way to keep
 * memory usage down in Hyrule Field. */
static void pos_set(EnWood02* this, PlayState* play) {
    EnWood02* childWood;
    s16* childSpawnAngle;
    Vec3f childPos;
    s16 extraRot;
    s16 childParams;
    s32 i;

    for (i = 4; i >= 0; i--) {
        if ((this->unk_14E[i] & 0x7F) == 0) {
            extraRot = 0;
            if (this->actor.params == WOOD_BUSH_GREEN_LARGE_SPAWNER) {
                extraRot = 0x4000;
            }
            childSpawnAngle = &wood_ang[i];
            cr = cos_s(*childSpawnAngle + this->actor.world.rot.y + extraRot);
            sr = sin_s(*childSpawnAngle + this->actor.world.rot.y + extraRot);
            childPos.x = (wood_reng[i] * sr) + this->actor.home.pos.x;
            childPos.y = this->actor.home.pos.y;
            childPos.z = (wood_reng[i] * cr) + this->actor.home.pos.z;
            if (c_check(this, play, &childPos)) {
                if ((this->unk_14E[i] & 0x80) != 0) {
                    childParams = (0xFF00 | (this->actor.params + 1));
                } else {
                    childParams = (((this->drawType & 0xF0) << 4) | (this->actor.params + 1));
                }
                childWood = (EnWood02*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_WOOD02,
                                                          childPos.x, childPos.y, childPos.z, this->actor.world.rot.x,
                                                          *childSpawnAngle, 0, childParams);
                if (childWood != NULL) {
                    childWood->unk_14E[0] = i;
                    this->unk_14E[i] |= 1;
                    childWood->actor.projectedPos = this->actor.projectedPos;
                } else {
                    this->unk_14E[i] &= 0x80;
                }
            }
        }
    }
}

void En_Wood02_Actor_ct(Actor* thisx, PlayState* play2) {
    s16 spawnType;
    f32 actorScale;
    PlayState* play = play2;
    EnWood02* this = (EnWood02*)thisx;
    CollisionPoly* poly;
    s32 bgId;
    f32 floorY;
    s16 extraRot;

    spawnType = WOOD_SPAWN_NORMAL;
    actorScale = 1.0f;
    this->unk_14C = PARAMS_GET_U(this->actor.params, 8, 8);

    if (this->actor.home.rot.z != 0) {
        this->actor.home.rot.z = (this->actor.home.rot.z << 8) | this->unk_14C;
        this->unk_14C = -1;
        this->actor.world.rot.z = this->actor.shape.rot.z = 0;
    } else if (this->unk_14C & 0x80) {
        this->unk_14C = -1;
    }

    this->actor.params &= 0xFF;
    ValueSet_process(&this->actor, value_init);

    if (this->actor.params <= WOOD_TREE_KAKARIKO_ADULT) {
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    }

    switch (this->actor.params) {
        case WOOD_BUSH_GREEN_LARGE_SPAWNER:
        case WOOD_BUSH_BLACK_LARGE_SPAWNER:
            spawnType = 1;
            FALLTHROUGH;
        case WOOD_BUSH_GREEN_LARGE_SPAWNED:
        case WOOD_BUSH_BLACK_LARGE_SPAWNED:
            spawnType++;
            FALLTHROUGH;
        case WOOD_TREE_CONICAL_LARGE:
        case WOOD_BUSH_GREEN_LARGE:
        case WOOD_BUSH_BLACK_LARGE:
            actorScale = 1.5f;
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 2000.0f;
            this->actor.cullingVolumeDownward = 2400.0f;
            break;
        case WOOD_TREE_CONICAL_SPAWNER:
        case WOOD_TREE_OVAL_YELLOW_SPAWNER:
        case WOOD_TREE_OVAL_GREEN_SPAWNER:
        case WOOD_BUSH_GREEN_SMALL_SPAWNER:
        case WOOD_BUSH_BLACK_SMALL_SPAWNER:
            spawnType = 1;
            FALLTHROUGH;
        case WOOD_TREE_CONICAL_SPAWNED:
        case WOOD_TREE_OVAL_YELLOW_SPAWNED:
        case WOOD_TREE_OVAL_GREEN_SPAWNED:
        case WOOD_BUSH_GREEN_SMALL_SPAWNED:
        case WOOD_BUSH_BLACK_SMALL_SPAWNED:
            spawnType++;
            FALLTHROUGH;
        case WOOD_TREE_CONICAL_MEDIUM:
        case WOOD_TREE_OVAL_GREEN:
        case WOOD_TREE_KAKARIKO_ADULT:
        case WOOD_BUSH_GREEN_SMALL:
        case WOOD_BUSH_BLACK_SMALL:
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 800.0f;
            this->actor.cullingVolumeDownward = 1800.0f;
            break;
        case WOOD_TREE_CONICAL_SMALL:
            actorScale = 0.6f;
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 400.0f;
            this->actor.cullingVolumeDownward = 1000.0f;
            break;
        case WOOD_LEAF_GREEN:
        case WOOD_LEAF_YELLOW:
            this->unk_14E[0] = 0x4B;
            actorScale = 0.02f;
            this->actor.velocity.x = rnd_fx(6.0f);
            this->actor.velocity.z = rnd_fx(6.0f);
            this->actor.velocity.y = (fqrand() * 1.25f) + -3.1f;
            break;
    }

    if (this->actor.params <= WOOD_TREE_CONICAL_SPAWNED) {
        this->drawType = WOOD_DRAW_TREE_CONICAL;
    } else if (this->actor.params <= WOOD_TREE_OVAL_GREEN_SPAWNED) {
        this->drawType = WOOD_DRAW_TREE_OVAL;
    } else if (this->actor.params <= WOOD_TREE_KAKARIKO_ADULT) {
        this->drawType = WOOD_DRAW_TREE_KAKARIKO_ADULT;
    } else if (this->actor.params <= WOOD_BUSH_GREEN_LARGE_SPAWNED) {
        this->drawType = WOOD_DRAW_BUSH_GREEN;
    } else if (this->actor.params <= WOOD_LEAF_GREEN) { // Black bushes and green leaves
        this->drawType = WOOD_DRAW_4;
    } else {
        this->drawType = WOOD_DRAW_LEAF_YELLOW;
    }

    Actor_set_scale(&this->actor, actorScale);
    this->spawnType = spawnType;

    if (spawnType != WOOD_SPAWN_NORMAL) {
        extraRot = 0;

        if (this->actor.params == WOOD_BUSH_GREEN_LARGE_SPAWNER) {
            extraRot = 0x4000;
        }

        if (spawnType == WOOD_SPAWN_SPAWNER) {
            this->drawType |= this->unk_14C << 4;
            pos_set(this, play);
            cr = cos_s(wood_ang[5] + this->actor.world.rot.y + extraRot);
            sr = sin_s(wood_ang[5] + this->actor.world.rot.y + extraRot);
            this->actor.world.pos.x += (sr * wood_reng[5]);
            this->actor.world.pos.z += (cr * wood_reng[5]);
        } else {
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }

        // Snap to floor, or remove if over void
        this->actor.world.pos.y += 200.0f;
        floorY = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &poly, &bgId, &this->actor, &this->actor.world.pos);

        if (floorY > BGCHECK_Y_MIN) {
            this->actor.world.pos.y = floorY;
        } else {
            Actor_delete(&this->actor);
            return;
        }
    }
    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
    this->actor.home.rot.y = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void En_wood02_Actor_dt(Actor* thisx, PlayState* play) {
    EnWood02* this = (EnWood02*)thisx;

    if (this->actor.params <= WOOD_TREE_KAKARIKO_ADULT) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void En_wood02_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnWood02* this = (EnWood02*)thisx;
    f32 wobbleAmplitude;

    // Despawn extra trees in a group if out of range
    if ((this->spawnType == WOOD_SPAWN_SPAWNED) && (this->actor.parent != NULL)) {
        if (!(this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
            u8 new_var = this->unk_14E[0];
            u8 phi_v0 = 0;
            s32 pad;

            if (this->unk_14C < 0) {
                phi_v0 = 0x80;
            }

            ((EnWood02*)this->actor.parent)->unk_14E[new_var] = phi_v0;
            Actor_delete(&this->actor);
            return;
        }
    } else if (this->spawnType == WOOD_SPAWN_SPAWNER) {
        pos_set(this, play);
    }

    if (this->actor.params <= WOOD_TREE_KAKARIKO_ADULT) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            Actor_SE_set(&this->actor, NA_SE_IT_REFLECTION_WOOD);
        }

        if (this->actor.home.rot.y != 0) {
            Vec3f dropsSpawnPt = this->actor.world.pos;

            dropsSpawnPt.y += 200.0f;

            if ((this->unk_14C >= 0) && (this->unk_14C < 0x64)) {
                Item_Set_Std(play, &this->actor, &dropsSpawnPt, this->unk_14C << 4);
            } else {
                if (this->actor.home.rot.z != 0) {
                    this->actor.home.rot.z &= 0x1FFF;
                    this->actor.home.rot.z |= 0xE000;
                    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_SW, dropsSpawnPt.x, dropsSpawnPt.y, dropsSpawnPt.z, 0,
                                this->actor.world.rot.y, 0, this->actor.home.rot.z);
                    this->actor.home.rot.z = 0;
                }
            }

            // Spawn falling leaves
            if (this->unk_14C >= -1) {
                s32 i;
                s32 leavesParams = WOOD_LEAF_GREEN;

                if ((this->actor.params == WOOD_TREE_OVAL_YELLOW_SPAWNER) ||
                    (this->actor.params == WOOD_TREE_OVAL_YELLOW_SPAWNED)) {
                    leavesParams = WOOD_LEAF_YELLOW;
                }
                Actor_SE_set(&this->actor, NA_SE_EV_TREE_SWING);

                for (i = 3; i >= 0; i--) {
                    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_WOOD02, dropsSpawnPt.x, dropsSpawnPt.y, dropsSpawnPt.z,
                                0, rnd_fx(65535.0f), 0, leavesParams);
                }
            }
            this->unk_14C = -0x15;
            this->actor.home.rot.y = 0;
        }

        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
    } else if (this->actor.params < 0x17) { // Bush
        Player* player = GET_PLAYER(play);

        if (this->unk_14C >= -1) {
            if (((player->rideActor == NULL) && (sqrt(this->actor.xyzDistToPlayerSq) < 20.0) &&
                 (player->speedXZ != 0.0f)) ||
                ((player->rideActor != NULL) && (sqrt(this->actor.xyzDistToPlayerSq) < 60.0) &&
                 (player->rideActor->speed != 0.0f))) {
                if ((this->unk_14C >= 0) && (this->unk_14C < 0x64)) {
                    Item_Set_Std(play, &this->actor, &this->actor.world.pos,
                                               ((this->unk_14C << 4) | 0x8000));
                }
                this->unk_14C = -0x15;
                Actor_SE_set(&this->actor, NA_SE_EV_TREE_SWING);
            }
        }
    } else { // Leaves
        this->unk_14C++;
        add_calc2(&this->actor.velocity.x, 0.0f, 1.0f, 5 * 0.01f);
        add_calc2(&this->actor.velocity.z, 0.0f, 1.0f, 5 * 0.01f);
        Actor_position_move(&this->actor);
        this->actor.shape.rot.z = sin_s(3000 * this->unk_14C) * 0x4000;
        this->unk_14E[0]--;

        if (this->unk_14E[0] == 0) {
            Actor_delete(&this->actor);
        }
    }

    // Wobble from impact
    if (this->unk_14C < -1) {
        this->unk_14C++;
        wobbleAmplitude = sin_s((this->unk_14C ^ 0xFFFF) * 0x3332) * 250.0f;
        this->actor.shape.rot.x = (cos_s(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) * wobbleAmplitude);
        this->actor.shape.rot.z = (sin_s(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) * wobbleAmplitude);
    }
}

void En_wood02_display(Actor* thisx, PlayState* play) {
    EnWood02* this = (EnWood02*)thisx;
    s16 type;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 red;
    u8 green;
    u8 blue;

    OPEN_DISPS(gfxCtx, "../z_en_wood02.c", 775);
    type = this->actor.params;

    if ((type == WOOD_TREE_OVAL_GREEN_SPAWNER) || (type == WOOD_TREE_OVAL_GREEN_SPAWNED) ||
        (type == WOOD_TREE_OVAL_GREEN) || (type == WOOD_LEAF_GREEN)) {
        red = 50;
        green = 170;
        blue = 70;
    } else if ((type == WOOD_TREE_OVAL_YELLOW_SPAWNER) || (type == WOOD_TREE_OVAL_YELLOW_SPAWNED) ||
               (type == WOOD_LEAF_YELLOW)) {
        red = 180;
        green = 155;
        blue = 0;
    } else {
        red = green = blue = 255;
    }

    _texture_z_light_fog_prim_xlu(gfxCtx);

    if ((this->actor.params == WOOD_LEAF_GREEN) || (this->actor.params == WOOD_LEAF_YELLOW)) {
        _texture_z_light_fog_prim(gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, 127);
        Cheap_gfx_display(play, object_wood02_DL_000700);
    } else if (main_mdl2[this->drawType & 0xF] != NULL) {
        Cheap_gfx_display(play, main_mdl[this->drawType & 0xF]);
        gDPSetEnvColor(POLY_XLU_DISP++, red, green, blue, 0);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_wood02.c", 808);
        gSPDisplayList(POLY_XLU_DISP++, main_mdl2[this->drawType & 0xF]);
    } else {
        _texture_z_light_fog_prim_xlu(gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_wood02.c", 814);
        gSPDisplayList(POLY_XLU_DISP++, main_mdl[this->drawType & 0xF]);
    }

    CLOSE_DISPS(gfxCtx, "../z_en_wood02.c", 840);
}
