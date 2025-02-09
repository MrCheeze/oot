/*
 * File: z_bg_heavy_block.c
 * Overlay: ovl_Bg_Heavy_Block
 * Description: Large block that can only be lifted with Golden Gauntlets
 */

#include "z_bg_heavy_block.h"

#include "libu64/debug.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "quake.h"
#include "rand.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_math.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_heavy_object/object_heavy_object.h"

#define FLAGS 0

#define PIECE_FLAG_HIT_FLOOR (1 << 0)

void Bg_Heavy_Block_actor_ct(Actor* thisx, PlayState* play);
void Bg_Heavy_Block_actor_dt(Actor* thisx, PlayState* play);
void Bg_Heavy_Block_actor_move(Actor* thisx, PlayState* play);
void Bg_Heavy_Block_actor_draw(Actor* thisx, PlayState* play);

void Bg_Heavy_Block_hahen_actor_draw(Actor* thisx, PlayState* play);

static void move_fall(BgHeavyBlock* this, PlayState* play);
static void move_wait(BgHeavyBlock* this, PlayState* play);
static void move_carry(BgHeavyBlock* this, PlayState* play);
static void move_throw(BgHeavyBlock* this, PlayState* play);
void move_end(BgHeavyBlock* this, PlayState* play);
static void move_stop(BgHeavyBlock* this, PlayState* play);

ActorProfile Bg_Heavy_Block_Profile = {
    /**/ ACTOR_BG_HEAVY_BLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HEAVY_OBJECT,
    /**/ sizeof(BgHeavyBlock),
    /**/ Bg_Heavy_Block_actor_ct,
    /**/ Bg_Heavy_Block_actor_dt,
    /**/ Bg_Heavy_Block_actor_move,
    /**/ Bg_Heavy_Block_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

void set_rnd_angle(BgHeavyBlock* this, f32 scale) {
    this->dyna.actor.world.rot.x = rnd_fx(1024.0f) * scale;
    this->dyna.actor.world.rot.y = rnd_fx(1024.0f) * scale;
    this->dyna.actor.world.rot.z = rnd_fx(1024.0f) * scale;
}

void init_speed_angle(BgHeavyBlock* this, f32 scale) {
    f32 rand;
    f32 yawSinCos;
    f32 randChoice;

    this->dyna.actor.gravity = -0.6f;
    this->dyna.actor.minVelocityY = -12.0f;
    randChoice = rnd_fx(12.0f * scale);
    rand = (randChoice < 0.0f) ? randChoice - 2.0f : randChoice + 2.0f;
    this->dyna.actor.velocity.y = (rnd_f(8.0f) + 4.0f) * scale;
    this->dyna.actor.velocity.z = rnd_f(-8.0f * scale);
    yawSinCos = cos_s(this->dyna.actor.world.rot.y);
    this->dyna.actor.velocity.x =
        (sin_s(this->dyna.actor.world.rot.y) * this->dyna.actor.velocity.z + (yawSinCos * rand));
    yawSinCos = sin_s(this->dyna.actor.world.rot.y);
    this->dyna.actor.velocity.z =
        (cos_s(this->dyna.actor.world.rot.y) * this->dyna.actor.velocity.z) + (-yawSinCos * rand);
    set_rnd_angle(this, scale);
    Actor_set_scale(&this->dyna.actor, rnd_fx(0.2f) + 1.0f);
}

void Heavy_Block_common_ct(BgHeavyBlock* this, PlayState* play) {
    s32 pad[2];
    CollisionHeader* colHeader = NULL;
    this->dyna.actor.flags |=
        ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_CARRY_X_ROT_INFLUENCE;
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gHeavyBlockCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Heavy_Block_actor_ct(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;

    ValueSet_process(thisx, value_init);
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);
    this->pieceFlags = 0;

    if (play->sceneId == SCENE_OUTSIDE_GANONS_CASTLE) {
        thisx->params &= 0xFF00;
        thisx->params |= 4;
    }

    switch (PARAMS_GET_U(thisx->params, 0, 8)) {
        case HEAVYBLOCK_BIG_PIECE:
            thisx->draw = Bg_Heavy_Block_hahen_actor_draw;
            this->actionFunc = move_fall;
            init_speed_angle(this, 1.0f);
            this->timer = 120;
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->unk_164.y = -50.0f;
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            thisx->draw = Bg_Heavy_Block_hahen_actor_draw;
            this->actionFunc = move_fall;
            init_speed_angle(this, 2.0f);
            this->timer = 120;
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->unk_164.y = -20.0f;
            break;
        case HEAVYBLOCK_BREAKABLE:
            Heavy_Block_common_ct(this, play);

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 6))) {
                Actor_delete(thisx);
                return;
            }

            this->actionFunc = move_wait;
            break;
        case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
            Heavy_Block_common_ct(this, play);

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 6))) {
                this->actionFunc = move_stop;
                thisx->shape.rot.x = thisx->world.rot.x = 0x8AD0;
                thisx->shape.rot.y = thisx->world.rot.y = 0xC000;
                thisx->shape.rot.z = thisx->world.rot.z = 0x0;
                thisx->world.pos.x = 1704.0f;
                thisx->world.pos.y = 1504.0f;
                thisx->world.pos.z = 516.0f;
            }

            this->actionFunc = move_wait;
            break;
        case HEAVYBLOCK_UNBREAKABLE:
            Heavy_Block_common_ct(this, play);
            this->actionFunc = move_wait;
            break;
        default:
            Heavy_Block_common_ct(this, play);
            this->actionFunc = move_wait;
            break;
    }
    // "Largest Block Save Bit %x"
    PRINTF(VT_FGCOL(CYAN) " 最大 ブロック セーブビット %x\n" VT_RST, thisx->params);
}

void Bg_Heavy_Block_actor_dt(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case HEAVYBLOCK_BIG_PIECE:
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            break;
        default:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void move_fall(BgHeavyBlock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += thisx->gravity;

    if (thisx->velocity.y < thisx->minVelocityY) {
        thisx->velocity.y = thisx->minVelocityY;
    }

    thisx->velocity.x *= 0.98f;
    thisx->velocity.z *= 0.98f;
    Actor_position_move(thisx);
    thisx->shape.rot.x += thisx->world.rot.x;
    thisx->shape.rot.y += thisx->world.rot.y;
    thisx->shape.rot.z += thisx->world.rot.z;

    if (!(this->pieceFlags & PIECE_FLAG_HIT_FLOOR)) {
        thisx->world.pos.y += this->unk_164.y;
        thisx->prevPos.y += this->unk_164.y;
        Actor_BGcheck2(play, thisx, 50.0f, 50.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        thisx->world.pos.y -= this->unk_164.y;
        thisx->prevPos.y -= this->unk_164.y;
        if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->pieceFlags |= PIECE_FLAG_HIT_FLOOR;
            thisx->velocity.y = rnd_f(4.0f) + 2.0f;
            thisx->velocity.x = rnd_fx(8.0f);
            thisx->velocity.z = rnd_fx(8.0f);
            set_rnd_angle(this, 1.0f);
            Actor_SE_set(thisx, NA_SE_EV_ROCK_BROKEN);
            z_vibctl2_vib_setQ(thisx->xzDistToPlayer, 150, 10, 8);
        }
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        Actor_delete(thisx);
    }
}

void make_hahen_smoke(PlayState* play, f32 posX, f32 posY, f32 posZ, f32 velX, f32 velY, f32 velZ,
                            u8 dustParams) {
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    Vec3f eye;
    Vec3f at;
    s16 sp6E;
    s16 sp6C;
    Vec3f accel;
    Vec3f velocity;
    Vec3f pos;
    f32 sp44;
    s16 scaleStep;
    s16 scale;

    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;

    if (dustParams & 1) {
        // red dust, landed in fire
        primColor.r = 150;
        primColor.g = primColor.b = envColor.g = envColor.b = 0;
        envColor.r = 80;
        primColor.a = envColor.a = 0;
    } else {
        // brown dust
        // clang-format off
        primColor.r = 170; primColor.g = 130; primColor.b = 90; primColor.a = 255;
        envColor.r = 100; envColor.g = 60; envColor.b = 20; envColor.a = 255;
        // clang-format on
    }

    accel.z = 0.0f;
    accel.x = 0.0f;
    accel.y = (dustParams & 8) ? 0.0f : 0.5f;

    eye = GET_ACTIVE_CAM(play)->eye;
    at = GET_ACTIVE_CAM(play)->at;

    scale = 1000;
    scaleStep = 160;

    switch (dustParams & 6) {
        case 4:
        case 6:
            velocity.x = velX;
            velocity.y = velY;
            velocity.z = velZ;
            scale = 300;
            scaleStep = 50;
            break;
        case 2:
            sp44 = rnd_f(5.0f) + 5.0f;
            sp6E = rnd_fx(65280.0f);

            velocity.x = (sin_s(sp6E) * sp44) + velX;
            velocity.y = velY;
            velocity.z = (cos_s(sp6E) * sp44) + velZ;
            break;
        case 0:
            sp6E = search_position_angleY(&eye, &at);
            sp6C = -search_position_angleX(&eye, &at);

            velocity.x = ((5.0f * sin_s(sp6E)) * cos_s(sp6C)) + velX;
            velocity.y = (sin_s(sp6C) * 5.0f) + velY;
            velocity.z = ((5.0f * cos_s(sp6E)) * cos_s(sp6C)) + velZ;

            pos.x -= (velocity.x * 20.0f);
            pos.y -= (velocity.y * 20.0f);
            pos.z -= (velocity.z * 20.0f);
            break;
    }

    Effect_SS_Dust_sc_cl_co_nofog_ct(play, &pos, &velocity, &accel, &primColor, &envColor, scale, scaleStep,
                  (s32)rnd_f(10.0f) + 20);
}

void make_hahens(BgHeavyBlock* this, PlayState* play) {
    s32 i;
    Vec3f spA4[] = {
        { 0.0f, 300.0f, -20.0f }, { 50.0f, 200.0f, -20.0f }, { -50.0f, 200.0f, -20.0f },
        { 0.0f, 100.0f, 30.0f },  { 0.0f, 100.0f, -70.0f },  { 0.0f, 0.0f, -20.0f },
    };
    s32 pad;
    Vec3f pos;
    f32 sinPitch;
    f32 cosPitch;
    f32 sinYaw;
    f32 cosYaw;

    sinPitch = sin_s(this->dyna.actor.world.rot.x);
    cosPitch = cos_s(this->dyna.actor.world.rot.x);
    sinYaw = sin_s(this->dyna.actor.world.rot.y);
    cosYaw = cos_s(this->dyna.actor.world.rot.y);

    for (i = 0; i < ARRAY_COUNT(spA4); i++) {
        pos.z = (spA4[i].y * sinPitch) + (spA4[i].z * cosPitch);

        pos.x = this->dyna.actor.world.pos.x + (spA4[i].x * cosYaw) + (sinYaw * pos.z);
        pos.y = this->dyna.actor.world.pos.y + (spA4[i].y * cosPitch) + (-spA4[i].z * sinPitch);
        pos.z = this->dyna.actor.world.pos.z + (spA4[i].x * -sinYaw) + (cosYaw * pos.z);

        Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_HEAVY_BLOCK, pos.x, pos.y, pos.z, this->dyna.actor.shape.rot.x,
                    this->dyna.actor.shape.rot.y, 0, 2);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_HEAVY_BLOCK, pos.x, pos.y, pos.z, this->dyna.actor.shape.rot.x,
                    this->dyna.actor.shape.rot.y, 0, 3);

        make_hahen_smoke(play, pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, 0);
    }
}

static void move_wait(BgHeavyBlock* this, PlayState* play) {
    s32 quakeIndex;

    // if block has a parent link has lifted it, start one point cutscene and quake
    if (Actor_carry_check(&this->dyna.actor, play)) {
        this->timer = 0;

        switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
            case HEAVYBLOCK_BREAKABLE:
                makeOnepointDemo(play, 4020, 270, &this->dyna.actor, CAM_ID_MAIN);
                break;
            case HEAVYBLOCK_UNBREAKABLE:
                makeOnepointDemo(play, 4021, 220, &this->dyna.actor, CAM_ID_MAIN);
                break;
            case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                makeOnepointDemo(play, 4022, 210, &this->dyna.actor, CAM_ID_MAIN);
                break;
        }

        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        setSpeedQuake(quakeIndex, 25000);
        setScaleQuake(quakeIndex, 1, 1, 5, 0);
        setTimerQuake(quakeIndex, 10);
        this->actionFunc = move_carry;
    }
}

static void move_carry(BgHeavyBlock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 cosYaw;
    f32 zOffset;
    f32 sinYaw;
    f32 xOffset;

    if (this->timer == 11) {
        z_vibctl2_vib_setQ(0.0f, 255, 20, 20);
        player_SE_set(player, NA_SE_PL_PULL_UP_BIGROCK);
        LOG_STRING("NA_SE_PL_PULL_UP_BIGROCK", "../z_bg_heavy_block.c", 691);
    }

    if (this->timer < 40) {
        xOffset = rnd_fx(110.0f);
        sinYaw = sin_s(this->dyna.actor.shape.rot.y);
        zOffset = rnd_fx(110.0f);
        cosYaw = cos_s(this->dyna.actor.shape.rot.y);

        make_hahen_smoke(play, (sinYaw * -70.0f) + (this->dyna.actor.world.pos.x + xOffset),
                               this->dyna.actor.world.pos.y + 10.0f,
                               (cosYaw * -70.0f) + (this->dyna.actor.world.pos.z + zOffset), 0.0f, -1.0f, 0.0f, 0xC);
    }

    this->timer++;

    player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_8);

    // if parent is NULL, link threw it
    if (Actor_carry_end_check(&this->dyna.actor, play)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_HEAVY_THROW);
        this->actionFunc = move_throw;
    }
}

static void move_throw(BgHeavyBlock* this, PlayState* play) {
    s32 bgId;
    s32 quakeIndex;
    Vec3f checkPos;
    f32 yIntersect;

    Actor_position_moveF(&this->dyna.actor);
    checkPos.x = this->dyna.actor.home.pos.x;
    checkPos.y = this->dyna.actor.home.pos.y + 1000.0f;
    checkPos.z = this->dyna.actor.home.pos.z;
    yIntersect =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &checkPos);
    this->dyna.actor.floorHeight = yIntersect;

    if (this->dyna.actor.home.pos.y <= yIntersect) {
        z_vibctl2_vib_setQ(0.0f, 255, 60, 4);

        switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
            case HEAVYBLOCK_BREAKABLE:
                make_hahens(this, play);
                Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
                Actor_delete(&this->dyna.actor);

                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, 28000);
                setScaleQuake(quakeIndex, 14, 2, 100, 0);
                setTimerQuake(quakeIndex, 30);

                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_2);
                setSpeedQuake(quakeIndex, 12000);
                setScaleQuake(quakeIndex, 5, 0, 0, 0);
                setTimerQuake(quakeIndex, 999);

                Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_ELECTRIC_EXPLOSION);
                return;
            case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);

                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, 28000);
                setScaleQuake(quakeIndex, 16, 2, 120, 0);
                setTimerQuake(quakeIndex, 40);

                this->actionFunc = move_end;
                Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
                break;
            case HEAVYBLOCK_UNBREAKABLE:
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);

                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, 28000);
                setScaleQuake(quakeIndex, 14, 2, 100, 0);
                setTimerQuake(quakeIndex, 40);

                this->actionFunc = move_end;
                break;
            default:
                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, 28000);
                setScaleQuake(quakeIndex, 14, 2, 100, 0);
                setTimerQuake(quakeIndex, 40);

                this->actionFunc = move_end;
        }
    }
    this->dyna.actor.shape.rot.x = atans_table(this->dyna.actor.velocity.y, this->dyna.actor.speed);
}

static void move_stop(BgHeavyBlock* this, PlayState* play) {
}

void move_end(BgHeavyBlock* this, PlayState* play) {
    s32 pad;

    if (add_calc_short_angle2(&this->dyna.actor.shape.rot.x, 0x8AD0, 6, 2000, 100) != 0) {
        chase_f(&this->dyna.actor.speed, 0.0f, 20.0f);
        chase_f(&this->dyna.actor.velocity.y, 0.0f, 3.0f);
        this->dyna.actor.gravity = 0.0f;
        this->dyna.actor.world.pos = this->dyna.actor.home.pos;
        Actor_position_moveF(&this->dyna.actor);
        this->dyna.actor.home.pos = this->dyna.actor.world.pos;
        switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
            case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                make_hahen_smoke(play, rnd_fx(30.0f) + 1678.0f, rnd_f(100.0f) + 1286.0f,
                                       rnd_fx(30.0f) + 552.0f, 0.0f, 0.0f, 0.0f, 0);
                make_hahen_smoke(play, rnd_fx(30.0f) + 1729.0f, rnd_f(80.0f) + 1269.0f,
                                       rnd_fx(30.0f) + 600.0f, 0.0f, 0.0f, 0.0f, 0);
                break;
            case HEAVYBLOCK_UNBREAKABLE:
                make_hahen_smoke(play, rnd_fx(100.0f) + -735.0f, 29.0f,
                                       rnd_fx(100.0f) + -3418.0f, 0.0f, 0.0f, 0.0f, 3);
                break;
        }
    } else {
        this->dyna.actor.flags &= ~(ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
        this->actionFunc = move_stop;
    }
}

void Bg_Heavy_Block_actor_move(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;

    this->actionFunc(this, play);
}

void Bg_Heavy_Block_actor_draw(Actor* thisx, PlayState* play) {
    static Vec3f local_base = { 0.0f, 0.0f, 0.0f };
    static Vec3f local_BGcheck = { 0.0f, 400.0f, 0.0f };
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_heavy_block.c", 904);

    if (1) {}

    if (move_carry == this->actionFunc) {
        Matrix_softcv3_load(player->leftHandPos.x, player->leftHandPos.y, player->leftHandPos.z,
                                     &thisx->shape.rot);
        Matrix_translate(-this->unk_164.x, -this->unk_164.y, -this->unk_164.z, MTXMODE_APPLY);
    } else if ((thisx->gravity == 0.0f) && (move_end == this->actionFunc)) {
        Matrix_softcv3_load(thisx->home.pos.x, thisx->home.pos.y, thisx->home.pos.z, &thisx->shape.rot);
        Matrix_translate(-local_BGcheck.x, -local_BGcheck.y, -local_BGcheck.z, MTXMODE_APPLY);
    }

    Matrix_Position(&local_base, &thisx->world.pos);
    Matrix_Position(&local_BGcheck, &thisx->home.pos);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_heavy_block.c", 931);
    gSPDisplayList(POLY_OPA_DISP++, gHeavyBlockEntirePillarDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_heavy_block.c", 935);
}

void Bg_Heavy_Block_hahen_actor_draw(Actor* thisx, PlayState* play) {
    switch (PARAMS_GET_U(thisx->params, 0, 8)) {
        case HEAVYBLOCK_BIG_PIECE:
            Matrix_translate(50.0f, -260.0f, -20.0f, MTXMODE_APPLY);
            Cheap_gfx_display(play, gHeavyBlockBigPieceDL);
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            Matrix_translate(45.0f, -280.0f, -5.0f, MTXMODE_APPLY);
            Cheap_gfx_display(play, gHeavyBlockSmallPieceDL);
            break;
    }
}
