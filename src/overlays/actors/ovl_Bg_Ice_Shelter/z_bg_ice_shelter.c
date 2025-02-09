/*
 * File: z_bg_ice_shelter.c
 * Overlay: Bg_Ice_Shelter
 * Description: Red Ice
 */

#include "z_bg_ice_shelter.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"

#define FLAGS 0

#define BGICESHELTER_GET_TYPE(thisx) PARAMS_GET_U((thisx)->params, 8, 3)
#define BGICESHELTER_NO_SWITCH_FLAG(thisx) PARAMS_GET_U((thisx)->params, 6, 1)

void Bg_Ice_Shelter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ice_Shelter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ice_Shelter_actor_move(Actor* thisx, PlayState* play);
void Bg_Ice_Shelter_actor_draw(Actor* thisx, PlayState* play2);

static void mvSet_stop(BgIceShelter* this);
void mvSet_delete(BgIceShelter* this);

static void mv_stop(BgIceShelter* this, PlayState* play);
void mv_delete(BgIceShelter* this, PlayState* play);

ActorProfile Bg_Ice_Shelter_Profile = {
    /**/ ACTOR_BG_ICE_SHELTER,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_ICE_OBJECTS,
    /**/ sizeof(BgIceShelter),
    /**/ Bg_Ice_Shelter_actor_ct,
    /**/ Bg_Ice_Shelter_actor_dt,
    /**/ Bg_Ice_Shelter_actor_move,
    /**/ Bg_Ice_Shelter_actor_draw,
};

static f32 scaleData[] = { 0.1f, 0.06f, 0.1f, 0.1f, 0.25f };

static Color_RGBA8 prim_Ice = { 250, 250, 250, 255 };
static Color_RGBA8 env_Ice = { 180, 180, 180, 255 };

static ColliderCylinderInit ClPipeDt_hono = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit ClPipeDt_shield = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC1FFF6, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

/**
 * Initializes either one or both cylinder colliders, depending on the actor's type.
 */
void set_collision_data_shelter(BgIceShelter* this, PlayState* play) {
    static s16 coll_r[] = { 47, 33, 44, 41, 100 };
    static s16 coll_h[] = { 80, 54, 90, 60, 200 };
    s32 pad;
    s32 type = BGICESHELTER_GET_TYPE(&this->dyna.actor);

    // All types use at least one collider in order to detect blue fire
    ClObjPipe_ct(play, &this->cylinder1);
    ClObjPipe_set5(play, &this->cylinder1, &this->dyna.actor, &ClPipeDt_hono);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->cylinder1);

    this->cylinder1.dim.radius = coll_r[type];
    this->cylinder1.dim.height = coll_h[type];

    // The wall and platform types use DynaPoly for collision, so they don't need the second collider
    if (type == RED_ICE_LARGE || type == RED_ICE_SMALL || type == RED_ICE_KING_ZORA) {
        ClObjPipe_ct(play, &this->cylinder2);
        ClObjPipe_set5(play, &this->cylinder2, &this->dyna.actor, &ClPipeDt_shield);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->cylinder2);
        this->cylinder2.dim.radius = coll_r[type];
        this->cylinder2.dim.height = coll_h[type];
    }

    if (type == RED_ICE_KING_ZORA) {
        this->cylinder1.dim.pos.z += 30;
        this->cylinder2.dim.pos.z += 30;
    }
}

static void set_dynaPoly(BgIceShelter* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG registration failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_ice_shelter.c", 362,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void rotY_ice_shelter(Vec3f* dest, Vec3f* src, s16 angle) {
    f32 sin = sin_s(angle);
    f32 cos = cos_s(angle);

    dest->x = (src->z * sin) + (src->x * cos);
    dest->y = src->y;
    dest->z = (src->z * cos) - (src->x * sin);
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Ice_Shelter_actor_ct(Actor* thisx, PlayState* play) {
    BgIceShelter* this = (BgIceShelter*)thisx;
    s16 type = BGICESHELTER_GET_TYPE(&this->dyna.actor);

    ValueSet_process(&this->dyna.actor, value_init);

    if (type == RED_ICE_KING_ZORA) {
        this->dyna.actor.world.rot.x += 0xBB8;
        this->dyna.actor.world.pos.y -= 45.0f;
        this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
        this->dyna.actor.world.pos.z -= 38.0f;
    }

    if (type == RED_ICE_KING_ZORA) {
        static Vec3f scl_kz = { 0.18f, 0.27f, 0.24f };

        xyz_t_move(&this->dyna.actor.scale, &scl_kz);
    } else {
        Actor_set_scale(&this->dyna.actor, scaleData[type]);
    }

    // Only 2 types use DynaPoly
    switch (type) {
        case RED_ICE_PLATFORM:
            set_dynaPoly(this, play, &gRedIcePlatformCol, 0);
            break;
        case RED_ICE_WALL:
            set_dynaPoly(this, play, &gRedIceWallCol, 0);
            break;
    }

    // All types use at least one collider
    set_collision_data_shelter(this, play);

    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    // The only red ice actor in the game that doesn't use a switch flag is the one for King Zora
    if (!BGICESHELTER_NO_SWITCH_FLAG(&this->dyna.actor) &&
        (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)))) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    mvSet_stop(this);

    PRINTF("(ice shelter)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Ice_Shelter_actor_dt(Actor* thisx, PlayState* play) {
    BgIceShelter* this = (BgIceShelter*)thisx;

    switch (BGICESHELTER_GET_TYPE(&this->dyna.actor)) {
        case RED_ICE_PLATFORM:
        case RED_ICE_WALL:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            break;

        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            ClObjPipe_dt(play, &this->cylinder2);
            break;
    }

    ClObjPipe_dt(play, &this->cylinder1);
}

/**
 * Angles used to spawn steam particles in a circle.
 */
static s16 offset_angle[] = { 0x0000, 0x4000, 0x2000, 0x6000, 0x1000, 0x5000, 0x3000, 0x7000 };

/**
 * Positions used to spawn steam particles in a straight line.
 */
static s16 posX[] = { 0x0000, 0x003C, 0x0018, 0x0054, 0x0030, 0x000C, 0x0048, 0x0024 };

/**
 * Spawns steam particle effects in a circle around the ice block.
 *
 * On each frame the function is called, two particles have a chance to appear, at the same distance and opposite
 * sides from the center.
 */
void set_eff_shelter(BgIceShelter* this, PlayState* play, f32 particleSpawningChance,
                                   f32 steamEffectScale) {
    f32 cos;
    f32 sin;
    f32 distance;
    Vec3f* icePos;
    s16 angle;
    s16 frameCounter;
    s32 i;
    s32 pad[2];
    Vec3f steamPos;
    Vec3f steamVel;
    Vec3f steamAccel;

    frameCounter = (s16)play->state.frames & 7;

    for (i = 0; i < 2; i++) {
        if (particleSpawningChance < fqrand()) {
            continue;
        }

        // The steamEffectScale is used here to make the particles appear at the edges of the red ice.
        distance = 42.0f * steamEffectScale;
        icePos = &this->dyna.actor.world.pos;
        angle = offset_angle[frameCounter] + (i * 0x8000);
        sin = sin_s(angle);
        cos = cos_s(angle);

        steamPos.x = (distance * sin) + icePos->x;
        steamPos.y = (16.0f * steamEffectScale) + icePos->y;
        steamPos.z = (distance * cos) + icePos->z;

        steamVel.x = ((fqrand() * 3.0f) - 1.0f) * sin;
        steamVel.y = 0.0f;
        steamVel.z = ((fqrand() * 3.0f) - 1.0f) * cos;

        steamAccel.x = 0.07f * sin;
        steamAccel.y = 0.8f;
        steamAccel.z = 0.07f * cos;

        Effect_SS_Dust_sc_cl_ct(play, &steamPos, &steamVel, &steamAccel, &prim_Ice, &env_Ice,
                      450.0f * steamEffectScale, (s16)((fqrand() * 40.0f) + 40.0f) * steamEffectScale);
    }
}

/**
 * Spawns steam particle effects in a straight line. Only used for the ice wall type.
 *
 * On each frame the function is called, two particles have a chance to appear, at the same distance and opposite
 * sides from the midpoint.
 *
 * The last argument is unused because only one red ice type can call this function, so the scale isn't needed.
 */
void set_eff_ice_wall(BgIceShelter* this, PlayState* play, f32 particleSpawningChance, f32 unusedArg) {
    static f32 dir[] = { -1.0f, 1.0f };
    Vec3f* icePos;
    s16 frameCounter;
    s32 pad[2];
    Vec3f steamPos;
    Vec3f steamVel;
    Vec3f steamAccel;
    Vec3f posOffset;
    s32 i;

    frameCounter = (s16)play->state.frames & 7;

    for (i = 0; i < 2; i++) {
        icePos = &this->dyna.actor.world.pos;

        if (particleSpawningChance < fqrand()) {
            continue;
        }

        posOffset.x = (posX[frameCounter] + ((fqrand() * 12.0f) - 6.0f)) * dir[i];
        posOffset.y = 15.0f;
        posOffset.z = ((84.0f - posOffset.x) * 0.2f) + (fqrand() * 20.0f);

        // Convert the position offset from relative to the ice wall to absolute.
        rotY_ice_shelter(&steamPos, &posOffset, this->dyna.actor.world.rot.y);
        xyz_t_add(&steamPos, icePos, &steamPos);

        steamVel.x = (fqrand() * 3.0f) - 1.5f;
        steamVel.y = 0.0f;
        steamVel.z = (fqrand() * 3.0f) - 1.5f;

        steamAccel.x = (fqrand() * 0.14f) - 0.07f;
        steamAccel.y = 0.8f;
        steamAccel.z = (fqrand() * 0.14f) - 0.07f;

        Effect_SS_Dust_sc_cl_ct(play, &steamPos, &steamVel, &steamAccel, &prim_Ice, &env_Ice, 450,
                      (fqrand() * 40.0f) + 40.0f);
    }
}

static void mvSet_stop(BgIceShelter* this) {
    this->actionFunc = mv_stop;
    this->alpha = 255;
}

/**
 * Checks for collision with blue fire. Also used to freeze King Zora's actor.
 */
static void mv_stop(BgIceShelter* this, PlayState* play) {
    s32 pad;
    s16 type = BGICESHELTER_GET_TYPE(&this->dyna.actor);

    // Freeze King Zora
    if (type == RED_ICE_KING_ZORA) {
        if (this->dyna.actor.parent != NULL) {
            this->dyna.actor.parent->freezeTimer = 10000;
        }
    }

    // Detect blue fire
    if (this->cylinder1.base.acFlags & AC_HIT) {
        this->cylinder1.base.acFlags &= ~AC_HIT;

        if ((this->cylinder1.base.ac != NULL) && (this->cylinder1.base.ac->id == ACTOR_EN_ICE_HONO)) {
            if (type == RED_ICE_KING_ZORA) {
                if (this->dyna.actor.parent != NULL) {
                    this->dyna.actor.parent->freezeTimer = 50;
                }
            }

            mvSet_delete(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_ICE_MELT);
        }
    }

    switch (type) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            CollisionCheck_setOC(play, &play->colChkCtx, &this->cylinder1.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder2.base);
            break;
    }

    CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder1.base);
}

void mvSet_delete(BgIceShelter* this) {
    this->actionFunc = mv_delete;
    this->alpha = 255;
}

/**
 * Values added to the ice block's height every frame while it's melting.
 */
static f32 scale_speed[] = { -0.0015f, -0.0009f, -0.0016f, -0.0016f, -0.00375f };

/**
 * Values used to scale and position the steam effects so they match the ice block's size.
 */
static f32 eff_scale[] = { 1.0f, 0.6f, 1.2f, 1.0f, 1.8f };

/**
 * Functions used to spawn steam effects at the base of the red ice.
 */
static void (*eff_prcTbl[])(BgIceShelter* this, PlayState* play, f32 particleSpawningChance,
                                  f32 steamEffectScale) = {
    set_eff_shelter, set_eff_shelter, set_eff_shelter,
    set_eff_ice_wall,  set_eff_shelter,
};

/**
 * Progressively reduces the height and opacity of the red ice, while spawning steam effects at its base.
 */
void mv_delete(BgIceShelter* this, PlayState* play) {

    s32 pad;
    s32 type = BGICESHELTER_GET_TYPE(&this->dyna.actor);
    f32 particleSpawningChance;

    this->alpha -= 5;
    this->alpha = CLAMP(this->alpha, 0, 255);

    this->dyna.actor.scale.y += scale_speed[type];
    this->dyna.actor.scale.y = CLAMP_MIN(this->dyna.actor.scale.y, 0.0001f);

    if (this->alpha > 80) {
        switch (type) {
            case RED_ICE_LARGE:
            case RED_ICE_SMALL:
            case RED_ICE_KING_ZORA:
                CollisionCheck_setOC(play, &play->colChkCtx, &this->cylinder1.base);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder2.base);
                break;
        }
    }

    // As the ice melts, the steam intensity is reduced by lowering the chance of each steam particle to appear
    if (this->alpha > 180) {
        particleSpawningChance = 1.0f;
    } else if (this->alpha > 60) {
        particleSpawningChance = 0.5f;
    } else {
        particleSpawningChance = 0.0f;
    }

    eff_prcTbl[type](this, play, particleSpawningChance, eff_scale[type]);

    if (this->alpha <= 0) {
        if (!BGICESHELTER_NO_SWITCH_FLAG(&this->dyna.actor)) {
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        }

        if (type == RED_ICE_KING_ZORA) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }

        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Ice_Shelter_actor_move(Actor* thisx, PlayState* play) {
    BgIceShelter* this = (BgIceShelter*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ice_Shelter_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgIceShelter* this = (BgIceShelter*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_ice_shelter.c", 748);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ice_shelter.c", 751);

    switch (BGICESHELTER_GET_TYPE(&this->dyna.actor)) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_PLATFORM:
        case RED_ICE_KING_ZORA:
            Actor_HiliteReflect_xlu_set_init(&this->dyna.actor, play, 0);
            break;
    }

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, this->alpha);

    switch (BGICESHELTER_GET_TYPE(&this->dyna.actor)) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -play->gameplayFrames & 0x7F,
                                        -play->gameplayFrames & 0x7F, 0x20, 0x20, 1, -play->gameplayFrames & 0x7F,
                                        play->gameplayFrames & 0x7F, 0x20, 0x20));
            gSPDisplayList(POLY_XLU_DISP++, gRedIceBlockDL);
            break;

        case RED_ICE_PLATFORM:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, play->gameplayFrames & 0xFF, 0x40, 0x40,
                                        1, 0, -play->gameplayFrames & 0xFF, 0x40, 0x40));
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -play->gameplayFrames & 0xFF,
                                        play->gameplayFrames & 0xFF, 0x40, 0x40, 1, play->gameplayFrames & 0xFF,
                                        play->gameplayFrames & 0xFF, 0x40, 0x40));
            gSPDisplayList(POLY_XLU_DISP++, gRedIcePlatformDL);
            break;

        case RED_ICE_WALL:
            gSPDisplayList(POLY_XLU_DISP++, gRedIceWallDL);
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_ice_shelter.c", 815);
}
