#include "z_en_fd_fire.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Fd_Fire_Actor_ct(Actor* thisx, PlayState* play);
void En_Fd_Fire_Actor_dt(Actor* thisx, PlayState* play);
void En_Fd_Fire_Actor_move(Actor* thisx, PlayState* play);
void En_Fd_Fire_Actor_draw(Actor* thisx, PlayState* play);
static void dead(EnFdFire* this, PlayState* play);
void grow(EnFdFire* this, PlayState* play);
void move(EnFdFire* this, PlayState* play);
void burn(EnFdFire* this, PlayState* play);

ActorProfile En_Fd_Fire_Profile = {
    /**/ ACTOR_EN_FD_FIRE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(EnFdFire),
    /**/ En_Fd_Fire_Actor_ct,
    /**/ En_Fd_Fire_Actor_dt,
    /**/ En_Fd_Fire_Actor_move,
    /**/ En_Fd_Fire_Actor_draw,
};

static ColliderCylinderInit EnFdFireOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0x0D840008, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 EnFdFireStatusData = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

void move_to_target(EnFdFire* this, Vec3f* targetPos) {
    f32 dist;
    f32 xDiff = targetPos->x - this->actor.world.pos.x;
    f32 yDiff = targetPos->y - this->actor.world.pos.y;
    f32 zDiff = targetPos->z - this->actor.world.pos.z;

    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (fabsf(dist) > fabsf(this->actor.speed)) {
        this->actor.velocity.x = (xDiff / dist) * this->actor.speed;
        this->actor.velocity.z = (zDiff / dist) * this->actor.speed;
    } else {
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.z = 0.0f;
    }

    this->actor.velocity.y += this->actor.gravity;
    if (!(this->actor.minVelocityY <= this->actor.velocity.y)) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

static s32 hitcheck(EnFdFire* this, PlayState* play) {
    if (this->actionFunc == dead) {
        return false;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return true;
    }

    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        return true;
    }
    return false;
}

void En_Fd_Fire_Actor_ct(Actor* thisx, PlayState* play) {
    EnFdFire* this = (EnFdFire*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnFdFireOcInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, &btl_data, &EnFdFireStatusData);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -0.6f;
    this->actor.speed = 5.0f;
    this->actor.velocity.y = 12.0f;
    this->spawnRadius = search_position_distance(&this->actor.world.pos, &player->actor.world.pos);
    this->scale = 3.0f;
    this->tile2Y = (s16)rnd_f(5.0f) - 25;
    this->actionFunc = grow;
}

void En_Fd_Fire_Actor_dt(Actor* thisx, PlayState* play) {
    EnFdFire* this = (EnFdFire*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void grow(EnFdFire* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f targetPos = this->actor.parent->world.pos;

    targetPos.x += this->spawnRadius * sin_s(this->actor.world.rot.y);
    targetPos.z += this->spawnRadius * cos_s(this->actor.world.rot.y);
    move_to_target(this, &targetPos);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (!(this->actor.velocity.y > 0.0f))) {
        this->actor.velocity = velocity;
        this->actor.speed = 0.0f;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        if (PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
            this->deathTimer = 200;
            this->actionFunc = move;
        } else {
            this->deathTimer = 300;
            this->actionFunc = burn;
        }
    }
}

void burn(EnFdFire* this, PlayState* play) {
    if (DECR(this->deathTimer) == 0) {
        this->actionFunc = dead;
    }
}

void move(EnFdFire* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 angles[] = {
        0.0f, 210.0f, 60.0f, 270.0f, 120.0f, 330.0f, 180.0f, 30.0f, 240.0f, 90.0f, 300.0f, 150.0f,
    };
    Vec3f pos;
    s16 idx;

    idx = ((play->state.frames / 10) + PARAMS_GET_S(this->actor.params, 0, 15)) % ARRAY_COUNT(angles);
    pos = player->actor.world.pos;
    pos.x += 120.0f * sinf(angles[idx]);
    pos.z += 120.0f * cosf(angles[idx]);

    if (DECR(this->deathTimer) == 0) {
        this->actionFunc = dead;
    } else {
        add_calc_short_angle2(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &pos), 8, 0xFA0, 1);
        add_calc(&this->actor.speed, 0.0f, 0.4f, 1.0f, 0.0f);
        if (this->actor.speed < 0.1f) {
            this->actor.speed = 5.0f;
        }
        Actor_position_speed_set(&this->actor);
    }
}

static void dead(EnFdFire* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 0.6f, 9.0f, 0.0f);
    Actor_position_speed_set(&this->actor);
    add_calc(&this->scale, 0.0f, 0.3f, 0.1f, 0.0f);
    this->actor.shape.shadowScale = 20.0f;
    this->actor.shape.shadowScale *= (this->scale / 3.0f);
    if (!(this->scale > 0.01f)) {
        Actor_delete(&this->actor);
    }
}

void En_Fd_Fire_Actor_move(Actor* thisx, PlayState* play) {
    EnFdFire* this = (EnFdFire*)thisx;
    s32 pad;

    if (this->actionFunc != dead) {
        if ((this->actor.parent->update == NULL) || hitcheck(this, play)) {
            this->actionFunc = dead;
        }
    }

    Actor_position_move(&this->actor);
    this->actionFunc(this, play);
    Actor_BGcheck2(play, &this->actor, 12.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if (this->actionFunc != dead) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Fd_Fire_Actor_draw(Actor* thisx, PlayState* play) {
    Color_RGBA8 primColors[] = {
        { 255, 255, 0, 255 },
        { 255, 255, 255, 255 },
    };
    Color_RGBA8 envColors[] = {
        { 255, 10, 0, 255 },
        { 0, 10, 255, 255 },
    };
    s32 pad;
    EnFdFire* this = (EnFdFire*)thisx;
    Vec3f scale = { 0.0f, 0.0f, 0.0f };
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    s16 sp8E;
    f32 sp88;
    f32 sp84;
    f32 sp80;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fd_fire.c", 572);

    Matrix_translate(thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, MTXMODE_NEW);
    sp8E = search_position_angleY(&scale, &thisx->velocity) - getRealCameraAngleY(GET_ACTIVE_CAM(play));
    sp84 = fabsf(cos_s(sp8E));
    sp88 = sin_s(sp8E);
    sp80 = search_position_distanceXZ(&scale, &thisx->velocity) / 1.5f;
    Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000)), MTXMODE_APPLY);
    Matrix_rotateZ(DEG_TO_RAD((sp88 * -10.0f) * sp80), MTXMODE_APPLY);
    scale.x = scale.y = scale.z = this->scale * 0.001f;
    Matrix_scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    sp84 = sp80 * ((0.01f * -15.0f) * sp84) + 1.0f;
    if (sp84 < 0.1f) {
        sp84 = 0.1f;
    }
    Matrix_scale(1.0f, sp84, 1.0f / sp84, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_fd_fire.c", 623);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                play->state.frames * this->tile2Y, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, primColors[PARAMS_GET_S(thisx->params, 15, 1)].r,
                    primColors[PARAMS_GET_S(thisx->params, 15, 1)].g, primColors[PARAMS_GET_S(thisx->params, 15, 1)].b,
                    primColors[PARAMS_GET_S(thisx->params, 15, 1)].a);
    gDPSetEnvColor(POLY_XLU_DISP++, envColors[PARAMS_GET_S(thisx->params, 15, 1)].r,
                   envColors[PARAMS_GET_S(thisx->params, 15, 1)].g, envColors[PARAMS_GET_S(thisx->params, 15, 1)].b,
                   envColors[PARAMS_GET_S(thisx->params, 15, 1)].a);
    gDPPipeSync(POLY_XLU_DISP++);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fd_fire.c", 672);
}
