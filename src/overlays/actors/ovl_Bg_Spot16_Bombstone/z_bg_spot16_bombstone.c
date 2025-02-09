#include "z_bg_spot16_bombstone.h"
#include "assets/objects/object_spot16_obj/object_spot16_obj.h"
#include "assets/objects/object_bombiwa/object_bombiwa.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Spot16_Bombstone_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot16_Bombstone_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot16_Bombstone_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot16_Bombstone_actor_draw(Actor* thisx, PlayState* play);

void mv_play(BgSpot16Bombstone* this, PlayState* play);
static void mv_dmaWait(BgSpot16Bombstone* this, PlayState* play);
static void mv_break(BgSpot16Bombstone* this, PlayState* play);
void mvSet_break(BgSpot16Bombstone* this);
static void mv_stop(BgSpot16Bombstone* this, PlayState* play);
static void mvSet_stop(BgSpot16Bombstone* this);
static void mvSet_dmaWait(BgSpot16Bombstone* this);
void mvSet_play(BgSpot16Bombstone* this);

static EnBombf* BombfPtr = NULL;

static s16 Demo_timer = 0;

static s16 Break_Dt[][10] = {
    { 0x0008, 0x0004, 0x0046, 0x07D0, 0xFCE0, 0x0000, 0x0064, 0x0000, 0x0000, 0x0000 },
    { 0x0006, 0x0003, 0x0032, 0x00C8, 0x0A28, 0xC350, 0x005A, 0x0000, 0x0000, 0x0000 },
    { 0x0005, 0x0003, 0x0028, 0xF63C, 0x0190, 0x30B0, 0x0032, 0x0000, 0x0000, 0x0000 },
    { 0x0003, 0x0001, 0x003C, 0x0258, 0xFF9C, 0xAFC8, 0x0032, 0x0000, 0x0000, 0x0000 },
    { 0x0003, 0x0001, 0x0028, 0xF2B8, 0xFF9C, 0x6590, 0x001E, 0x0000, 0x0000, 0x0000 },
    { 0x0006, 0x0009, 0x0028, 0x0000, 0x0BB8, 0xD8F0, 0x001E, 0x0000, 0x0000, 0x0000 },
};

static ColliderJntSphElementInit ClSphElemDt_playerOC[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFF6, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 50, 0 }, 288 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_playerOC = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_playerOC,
};

static ColliderCylinderInit ClPipeDt_bombBurst = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 190, 80, 0, { 10, 0, 50 } },
};

static s16 KemT_data[][7] = {
    { 0x0000, 0x000A, 0x003C, 0xFFF6, 0x0104, 0x01E0, 0x0007 },
    { 0x0000, 0x0000, 0x0032, 0x0000, 0x0104, 0x017C, 0x000D },
    { 0x0001, 0x001E, 0x0014, 0x0014, 0x00A0, 0x0104, 0x000A },
    { 0x0002, 0x0014, 0x0028, 0x0014, 0x00C8, 0x0096, 0x0007 },
    { 0x0003, 0xFFD8, 0x0064, 0x001E, 0x00D2, 0x0064, 0x0007 },
    { 0x0004, 0x000A, 0x0078, 0x000A, 0x00A0, 0x0028, 0x000A },
    { 0x0005, 0xFFA7, 0x006E, 0x0041, 0x0096, 0x0118, 0x0007 },
    { 0x0006, 0xFFC4, 0x0070, 0x006D, 0x006E, 0x0082, 0x0007 },
    { 0x0007, 0xFFF2, 0x0050, 0x007F, 0x008C, 0x00D2, 0x0007 },
    { 0x0008, 0x0014, 0x0032, 0x001E, 0x008C, 0x00C8, 0x0007 },
    { 0x0009, 0xFFEC, 0x0014, 0x0028, 0x00C8, 0x00B4, 0x0007 },
    { 0x000A, 0x001E, 0x0028, 0xFFE2, 0x0064, 0x0078, 0x0007 },
    { 0x000A, 0xFF92, 0x001D, 0x0078, 0x008C, 0x0118, 0x000A },
    { 0x000B, 0x001E, 0x0014, 0x001E, 0x0050, 0x00C8, 0x0006 },
    { 0x000C, 0x0028, 0x001E, 0xFFE2, 0x00AA, 0x010E, 0x0011 },
    { 0x000D, 0xFFD8, 0x0032, 0xFFEC, 0x008C, 0x0056, 0x0006 },
    { 0x000D, 0x0032, 0x0032, 0x0032, 0x0096, 0x00C8, 0x000A },
    { 0x000E, 0x0028, 0x0028, 0xFFD8, 0x003C, 0x00A0, 0x0006 },
    { 0x000F, 0xFFE2, 0x0014, 0x0032, 0x00AA, 0x012C, 0x0006 },
    { 0x0010, 0x0028, 0x0032, 0x0014, 0x00AA, 0x0078, 0x0007 },
    { 0x0010, 0x001E, 0x0032, 0x003C, 0x0096, 0x00C8, 0x0006 },
    { 0x0011, 0x000A, 0x000A, 0x0000, 0x0082, 0x00DC, 0x0006 },
    { 0x0012, 0x000A, 0x0028, 0x0014, 0x00B4, 0x00DC, 0x000B },
    { 0x0013, 0x000A, 0x0005, 0x0000, 0x006E, 0x0046, 0x0011 },
    { 0x0013, 0xFFEC, 0x0032, 0xFFE2, 0x0096, 0x00C8, 0x0006 },
    { 0x0014, 0x0050, 0x0032, 0x0000, 0x0096, 0x00C8, 0x0008 },
};

ActorProfile Bg_Spot16_Bombstone_Profile = {
    /**/ ACTOR_BG_SPOT16_BOMBSTONE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT16_OBJ,
    /**/ sizeof(BgSpot16Bombstone),
    /**/ Bg_Spot16_Bombstone_actor_ct,
    /**/ Bg_Spot16_Bombstone_actor_dt,
    /**/ Bg_Spot16_Bombstone_actor_move,
    /**/ Bg_Spot16_Bombstone_actor_draw,
};

void set_arg_data(BgSpot16Bombstone* this) {
    this->switchFlag = PARAMS_GET_U(this->actor.params, 8, 6);
    this->actor.params = PARAMS_GET_U(this->actor.params, 0, 8);
}

void setCollision_playerOC(BgSpot16Bombstone* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->colliderJntSph);
    ClObjJntSph_set5_nzm(play, &this->colliderJntSph, &this->actor, &ClSphDt_playerOC, this->colliderElements);
    this->colliderJntSph.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->colliderJntSph.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y + 50.0f;
    this->colliderJntSph.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
    this->colliderJntSph.elements[0].dim.worldSphere.radius = 120;
}

void setCollision_bombBurst(BgSpot16Bombstone* this, PlayState* play) {
    s32 pad;

    ClObjPipe_ct(play, &this->colliderCylinder);
    ClObjPipe_set5(play, &this->colliderCylinder, &this->actor, &ClPipeDt_bombBurst);
    this->colliderCylinder.dim.pos.x += (s16)this->actor.world.pos.x;
    this->colliderCylinder.dim.pos.y += (s16)this->actor.world.pos.y;
    this->colliderCylinder.dim.pos.z += (s16)this->actor.world.pos.z;
}

s32 bombstone_stop_ct(BgSpot16Bombstone* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
    };

    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        PRINTF("Spot16 obj 爆弾石 破壊済み\n");
        return false;
    }
    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.4f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    setCollision_playerOC(this, play);
    setCollision_bombBurst(this, play);
    this->sinRotation = sin_s(this->actor.shape.rot.y);
    this->cosRotation = cos_s(this->actor.shape.rot.y);
    this->dList = gDodongosCavernRock3DL;

    mvSet_stop(this);
    return true;
}

s32 bombstone_break_ct(BgSpot16Bombstone* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
        ICHAIN_F32(minVelocityY, -10, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
    };

    Actor* actor = &this->actor;
    f32 scaleFactor = 1.0f / 600.0f;
    f32 sinCosPosFactor = 50.0f;
    f32 sinValue;
    f32 cosValue;

    ValueSet_process(actor, value_init);

    actor->speed = Break_Dt[actor->params][0];
    actor->velocity.y = Break_Dt[actor->params][1];

    Actor_set_scale(actor, Break_Dt[actor->params][2] * scaleFactor);

    this->unk_210 = (f32)Break_Dt[actor->params][3];
    this->unk_212 = (f32)Break_Dt[actor->params][4];

    actor->world.rot.y = Break_Dt[actor->params][5];

    sinValue = sin_s(this->actor.world.rot.y);
    cosValue = cos_s(this->actor.world.rot.y);

    actor->world.pos.x = (sinValue * sinCosPosFactor) + actor->home.pos.x;
    actor->world.pos.y = Break_Dt[actor->params][6] + actor->home.pos.y;
    actor->world.pos.z = (cosValue * sinCosPosFactor) + actor->home.pos.z;

    actor->shape.rot.x = Break_Dt[actor->params][7];
    actor->shape.rot.y = Break_Dt[actor->params][8];
    actor->shape.rot.z = Break_Dt[actor->params][9];

    this->dList = object_bombiwa_DL_0009E0;
    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_BOMBIWA);

    if (this->requiredObjectSlot < 0) {
        PRINTF("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", actor->params, "../z_bg_spot16_bombstone.c", 589);
        return false;
    }

    mvSet_dmaWait(this);
    return true;
}

static Vec3f vec_w = { 0.0f, 0.0f, 0.0f };

static Vec3f acc_w = { 0.0f, 0.4f, 0.0f };

static f32 scale_dt[] = { 66.0f, 51.0f, 48.0f, 36.0f, 21.0f };

static s16 arg[] = { 0, 1, 2, 3, 4 };

void Bg_Spot16_Bombstone_actor_ct(Actor* thisx, PlayState* play) {
    s16 shouldLive = true;
    BgSpot16Bombstone* this = (BgSpot16Bombstone*)thisx;

    set_arg_data(this);

    switch (this->actor.params) {
        case 0xFF:
            // The boulder is intact
            shouldLive = bombstone_stop_ct(this, play);
            break;

        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            // The boulder is debris
            shouldLive = bombstone_break_ct(this, play);
            break;

#if DEBUG_FEATURES
        default:
            PRINTF("Error : arg_data おかしいな(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot16_bombstone.c", 668,
                   this->actor.params);
            shouldLive = false;
            break;
#endif
    }

    if (!shouldLive) {
        Actor_delete(&this->actor);
        return;
    }
    PRINTF("Spot16 obj 爆弾石 (scaleX %f)(arg_data 0x%04x)\n", this->actor.scale.x, this->actor.params);
}

void Bg_Spot16_Bombstone_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot16Bombstone* this = (BgSpot16Bombstone*)thisx;

    if (this->actor.params == 0xFF) {
        // Boulder is intact so remove its collider
        ClObjJntSph_dt_nzf(play, &this->colliderJntSph);
        ClObjPipe_dt(play, &this->colliderCylinder);
    }
}

void eff_kemuri_break(BgSpot16Bombstone* this, PlayState* play) {
    f32 scaleX1 = this->actor.scale.x * 150;
    s16 scaleX2 = this->actor.scale.x * 250;
    Vec3f world;

    world.x = this->actor.world.pos.x;
    world.y = this->actor.world.pos.y + 50.0f;
    world.z = this->actor.world.pos.z;

    dust_fly_set2(play, &world, scaleX1, 2, scaleX2, 0xA0, 1);
}

void eff_kemuri_data(BgSpot16Bombstone* this, PlayState* play) {
    f32 tempUnk6;
    f32 tempUnk2;
    s16 index;
    Vec3f position;
    Vec3f* actorPosition = &this->actor.world.pos;

    if (1) {}

    while (true) {
        if ((u32)this->unk_158 >= ARRAY_COUNTU(KemT_data) || this->unk_154 < KemT_data[this->unk_158][0]) {
            break;
        }

        index = this->unk_158;

        tempUnk2 = KemT_data[index][1];
        tempUnk6 = KemT_data[index][3];

        position.x = ((this->sinRotation * tempUnk6) + (tempUnk2 * this->cosRotation)) + actorPosition->x;
        position.y = KemT_data[index][2] + actorPosition->y;
        position.z = ((this->cosRotation * tempUnk6) - (tempUnk2 * this->sinRotation)) + actorPosition->z;

        Effect_SS_Dust_sc_co_li_ct(play, &position, &vec_w, &acc_w, KemT_data[index][4], KemT_data[index][5],
                      KemT_data[index][6]);

        this->unk_158++;
    }
}

void eff_bombstone(BgSpot16Bombstone* this, PlayState* play) {
    f32 velocityYMultiplier = 1.3f;
    Vec3f pos;
    Vec3f velocity;
    s32 index;
    s16 scale;

    if (this->actor.params == 0) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_SPOT16_BOMBSTONE, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 5);
        index = 3;
    } else {
        index = 0;
    }

    if (index < ARRAY_COUNT(scale_dt)) {
        do {
            pos.x = ((fqrand() - 0.5f) * 8.0f) + this->actor.world.pos.x;
            pos.y = ((fqrand() * 5.0f) + this->actor.world.pos.y) + 8.0f;
            pos.z = ((fqrand() - 0.5f) * 8.0f) + this->actor.world.pos.z;

            velocity.x = (fqrand() - 0.5f) * 16.0f;
            velocity.y = (fqrand() * 14.0) + (fabsf(this->actor.velocity.y) * velocityYMultiplier);
            velocity.z = (fqrand() - 0.5f) * 16.0f;

            scale = scale_dt[index] * this->actor.scale.x * 3;

            Effect_Kakera_ct2(play, &pos, &velocity, &this->actor.world.pos, -420, 0x31, 0xF, 0xF, 0, scale, 2, 0x40,
                                 160, KAKERA_COLOR_NONE, OBJECT_BOMBIWA, object_bombiwa_DL_0009E0);
            index++;
        } while (index != ARRAY_COUNT(scale_dt));
    }
}

void set_breakStone(BgSpot16Bombstone* this, PlayState* play) {
    s32 index;
    PosRot* world;

    world = &this->actor.world;
    for (index = 0; index < ARRAY_COUNT(arg); index++) {
        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_SPOT16_BOMBSTONE, world->pos.x, world->pos.y, world->pos.z, 0,
                        0, 0, arg[index]) == NULL) {
            break;
        }
    }
}

void revisePlayerPersonal(BgSpot16Bombstone* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 sinValue;
    s16 adjustedYawDiff;
    s32 yawDiff;
    s32 absYawDiff;

    if (this->actor.xzDistToPlayer < 130.0f && this->actor.yDistToPlayer < 160.0f &&
        this->actor.yDistToPlayer >= -10.0f) {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);

        adjustedYawDiff = absYawDiff - 0x3FFF;

        if (adjustedYawDiff > 0) {
            sinValue = sin_s(adjustedYawDiff) * this->actor.xzDistToPlayer;

            if (sinValue >= 0.0f) {
                player->actor.world.pos.x += sinValue * this->sinRotation;
                player->actor.world.pos.z += sinValue * this->cosRotation;
            } else {
                PRINTF("Error 補正出来ない(%s %d)(arg_data 0x%04x)(hosei_angY %x)\n", "../z_bg_spot16_bombstone.c", 935,
                       this->actor.params, adjustedYawDiff);
            }
        }
    }
}

void watchDemoStart(BgSpot16Bombstone* this, PlayState* play) {
    Actor* playerHeldActor;
    Player* player = GET_PLAYER(play);
    EnBombf* currentBomb;

    if (Demo_timer > 0) {
        Demo_timer--;
    }

    if (BombfPtr != NULL) {
        if (BombfPtr->actor.update == NULL) {
            BombfPtr = NULL;
        } else if (Demo_timer <= 0 && BombfPtr->actor.world.pos.y < 1400.0f &&
                   Math3DVecLengthSquare2D(BombfPtr->actor.world.pos.x + 1579.0f, BombfPtr->actor.world.pos.z + 790.0f) <
                       SQ(400.0f) &&
                   BombfPtr->actor.params == 0) {
            currentBomb = BombfPtr;
            if (currentBomb->timer > 0) {
                Demo_timer = currentBomb->timer + 20;
                makeOnepointDemo(play, 4180, Demo_timer, NULL, CAM_ID_MAIN);
            }
        }
    } else if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        playerHeldActor = player->heldActor;
        if (playerHeldActor != NULL && playerHeldActor->category == ACTORCAT_EXPLOSIVE &&
            playerHeldActor->id == ACTOR_EN_BOMBF) {
            BombfPtr = (EnBombf*)playerHeldActor;
        }
    }
}

static void mvSet_stop(BgSpot16Bombstone* this) {
    this->actor.draw = Bg_Spot16_Bombstone_actor_draw;
    this->actionFunc = mv_stop;
}

static void mv_stop(BgSpot16Bombstone* this, PlayState* play) {
    s32 pad;

    revisePlayerPersonal(this, play);
    watchDemoStart(this, play);

    if (play) {}

    if (this->colliderCylinder.base.acFlags & AC_HIT) {
        this->colliderCylinder.base.acFlags &= ~AC_HIT;

        set_breakStone(this, play);

        makeOnepointDemo(play, 4180, 50, NULL, CAM_ID_MAIN);

        Actor_Environment_sw_On(play, this->switchFlag);
        SET_EVENTCHKINF(EVENTCHKINF_23);

        mvSet_play(this);
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCylinder.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderJntSph.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

#if DEBUG_FEATURES
    if (mREG(64) == 1) {
        set_breakStone(this, play);
        mREG(64) = -10;
    } else if (mREG(64) < 0) {
        mREG(64)++;
    }
#endif
}

void mvSet_play(BgSpot16Bombstone* this) {
    this->unk_154 = 0;
    this->unk_158 = 0;
    this->actor.draw = NULL;
    this->actionFunc = mv_play;
}

void mv_play(BgSpot16Bombstone* this, PlayState* play) {

    eff_kemuri_data(this, play);

    if (this->unk_154 == 56) {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
    }

    if (this->unk_154 > 60) {
        Actor_delete(&this->actor);
    }
}

static void mvSet_dmaWait(BgSpot16Bombstone* this) {
    this->actionFunc = mv_dmaWait;
    this->actor.draw = NULL;
}

static void mv_dmaWait(BgSpot16Bombstone* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        mvSet_break(this);
        this->actor.draw = Bg_Spot16_Bombstone_actor_draw;
    }
}

void mvSet_break(BgSpot16Bombstone* this) {
    this->unk_154 = 0;
    this->actionFunc = mv_break;
}

static void mv_break(BgSpot16Bombstone* this, PlayState* play) {
    Actor* actor = &this->actor;

    Actor_position_moveF(actor);
    actor->shape.rot.x += this->unk_210;
    actor->shape.rot.z += this->unk_212;

    if (this->unk_154 > 60) {
        Actor_delete(actor);
        return;
    }

    if ((actor->bgCheckFlags & BGCHECKFLAG_WALL) ||
        ((actor->bgCheckFlags & BGCHECKFLAG_GROUND) && actor->velocity.y < 0.0f)) {
        eff_bombstone(this, play);
        eff_kemuri_break(this, play);
        Effect_SE_Info_new(play, &actor->world.pos, 20, NA_SE_EV_ROCK_BROKEN);
        Actor_delete(actor);
        return;
    }

    Actor_BGcheck2(play, actor, 17.5f, 35.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

void Bg_Spot16_Bombstone_actor_move(Actor* thisx, PlayState* play) {
    BgSpot16Bombstone* this = (BgSpot16Bombstone*)thisx;

    this->unk_154++;
    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Spot16_Bombstone_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot16Bombstone* this = (BgSpot16Bombstone*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1253);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1257);

    if (this->actor.params == 0xFF) {
        // The boulder is intact
        gSPDisplayList(POLY_OPA_DISP++, this->dList);
    } else {
        // The boulder is debris
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->requiredObjectSlot].segment);
        gSPDisplayList(POLY_OPA_DISP++, this->dList);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1274);
}
