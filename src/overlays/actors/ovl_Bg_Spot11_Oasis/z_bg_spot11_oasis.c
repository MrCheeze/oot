/*
 * File: z_bg_spot11_oasis.c
 * Overlay: ovl_Bg_Spot11_Oasis
 * Description: Refilling Oasis (Desert Colossus)
 */

#include "z_bg_spot11_oasis.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/objects/object_spot11_obj/object_spot11_obj.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Spot11_Oasis_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot11_Oasis_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot11_Oasis_actor_draw(Actor* thisx, PlayState* play);
static void mv_stop_init(BgSpot11Oasis* this);
static void mv_stop(BgSpot11Oasis* this, PlayState* play);
static void mv_up_init(BgSpot11Oasis* this);
static void mv_up(BgSpot11Oasis* this, PlayState* play);
static void mv_end_init(BgSpot11Oasis* this);
static void mv_end(BgSpot11Oasis* this, PlayState* play);

ActorProfile Bg_Spot11_Oasis_Profile = {
    /**/ ACTOR_BG_SPOT11_OASIS,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT11_OBJ,
    /**/ sizeof(BgSpot11Oasis),
    /**/ Bg_Spot11_Oasis_actor_ct,
    /**/ Cheap_non_move,
    /**/ Bg_Spot11_Oasis_actor_move,
    /**/ NULL,
};

static s16 Oasis_ocarina_posXZ[][2] = {
    { 1260, 2040 }, { 1259, 1947 }, { 1135, 1860 }, { 1087, 1912 }, { 1173, 2044 },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static Vec3f awa_pos[] = {
    { 0.0f, -100.0f, 0.0f },   { 100.0f, -80.0f, -50.0f }, { -50.0f, -80.0f, -100.0f },
    { -75.0f, -90.0f, 90.0f }, { 30.0f, -100.0f, 40.0f },
};

static void set_waterPosY(PlayState* play, s16 waterSurface) {
    WaterBox* waterBox = &play->colCtx.colHeader->waterBoxes[0];

    waterBox->ySurface = waterSurface;
}

s32 check_ocarina_pos(PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    s32 i;

    sp58.x = Oasis_ocarina_posXZ[0][0];
    sp58.z = Oasis_ocarina_posXZ[0][1];
    sp58.y = 0.0f;

    sp4C.y = 0.0f;
    sp40.y = 0.0f;

    for (i = 1; i < ARRAY_COUNT(Oasis_ocarina_posXZ) - 1; i++) {
        sp4C.x = Oasis_ocarina_posXZ[i][0];
        sp4C.z = Oasis_ocarina_posXZ[i][1];
        sp40.x = Oasis_ocarina_posXZ[i + 1][0];
        sp40.z = Oasis_ocarina_posXZ[i + 1][1];
        if (Math3DTriangleCrossYCheck(&sp58, &sp4C, &sp40, player->actor.world.pos.z, player->actor.world.pos.x)) {
            return 1;
        }
    }
    return 0;
}

void Bg_Spot11_Oasis_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot11Oasis* this = (BgSpot11Oasis*)thisx;

    ValueSet_process(&this->actor, value_init);
    mv_stop_init(this);
    this->actor.world.pos.y = -100.0f;
    set_waterPosY(play, -100);
}

static void mv_stop_init(BgSpot11Oasis* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgSpot11Oasis* this, PlayState* play) {
    if (eventbit_check(play, 5) && check_ocarina_pos(play)) {
        makeOnepointDemo(play, 4150, -99, &this->actor, CAM_ID_MAIN);
        mv_up_init(this);
    }
}

static void mv_up_init(BgSpot11Oasis* this) {
    this->actionFunc = mv_up;
}

static void mv_up(BgSpot11Oasis* this, PlayState* play) {
    if (chase_f(&this->actor.world.pos.y, 0.0f, 0.7f)) {
        mv_end_init(this);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x, this->actor.world.pos.y + 40.0f,
                    this->actor.world.pos.z, 0, 0, 0, FAIRY_SPAWNER);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
    }
    set_waterPosY(play, this->actor.world.pos.y);
}

static void mv_end_init(BgSpot11Oasis* this) {
    this->actionFunc = mv_end;
}

static void mv_end(BgSpot11Oasis* this, PlayState* play) {
}

void Bg_Spot11_Oasis_actor_move(Actor* thisx, PlayState* play) {
    BgSpot11Oasis* this = (BgSpot11Oasis*)thisx;
    s32 pad;
    u32 gameplayFrames;

    this->actionFunc(this, play);
    if (this->actionFunc == mv_stop) {
        this->actor.draw = NULL;
        return;
    }
    this->actor.draw = Bg_Spot11_Oasis_actor_draw;
    if (this->unk_150 && (this->actor.projectedPos.z < 400.0f) && (this->actor.projectedPos.z > -40.0f)) {
        gameplayFrames = play->gameplayFrames;
        if (gameplayFrames & 4) {
            Vec3f sp30;

            xyz_t_add(&this->actor.world.pos, &awa_pos[this->unk_151], &sp30);
            Effect_SS_Bubble_ct(play, &sp30, 0.0f, 15.0f, 50.0f, (fqrand() * 0.12f) + 0.02f);
            if (fqrand() < 0.3f) {
                this->unk_151 = fqrand() * 4.9f;
            }
        }
    } else {
        this->unk_150 = 1;
    }
}

void Bg_Spot11_Oasis_actor_draw(Actor* thisx, PlayState* play) {
    u32 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot11_oasis.c", 327);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot11_oasis.c", 331);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - (gameplayFrames % 128),
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPDisplayList(POLY_XLU_DISP++, gDesertColossusOasisDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot11_oasis.c", 346);
}
