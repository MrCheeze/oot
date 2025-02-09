#include "z_bg_spot18_basket.h"
#include "assets/objects/object_spot18_obj/object_spot18_obj.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Spot18_Basket_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot18_Basket_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot18_Basket_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot18_Basket_actor_draw(Actor* thisx, PlayState* play);

static void mv_walk(BgSpot18Basket* this, PlayState* play);
static void mv_stop_init(BgSpot18Basket* this);
void mv_stop2_init(BgSpot18Basket* this);
static void mv_walk_init(BgSpot18Basket* this);
void mv_gurungurun1_init(BgSpot18Basket* this);
void mv_gurungurun2_init(BgSpot18Basket* this);
void mv_item_init(BgSpot18Basket* this);
static void mv_stop(BgSpot18Basket* this, PlayState* play);
void mv_stop2(BgSpot18Basket* this, PlayState* play);
void mv_gurungurun1(BgSpot18Basket* this, PlayState* play);
void mv_gurungurun2(BgSpot18Basket* this, PlayState* play);
void mv_item(BgSpot18Basket* this, PlayState* play);

ActorProfile Bg_Spot18_Basket_Profile = {
    /**/ ACTOR_BG_SPOT18_BASKET,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT18_OBJ,
    /**/ sizeof(BgSpot18Basket),
    /**/ Bg_Spot18_Basket_actor_ct,
    /**/ Bg_Spot18_Basket_actor_dt,
    /**/ Bg_Spot18_Basket_actor_move,
    /**/ Bg_Spot18_Basket_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_basket[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 2040, 0 }, 54 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 1400, 0 }, 13 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_basket = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    2,
    ClSphElemDt_basket,
};

static s16 Basket_FaceAngle[] = { 0x8000, 0x2AAA, 0xD555, 0x0000 };

void set_collision_data_basket(Actor* thisx, PlayState* play) {
    BgSpot18Basket* this = (BgSpot18Basket*)thisx;

    ClObjJntSph_ct(play, &this->colliderJntSph);
    ClObjJntSph_set5_nzm(play, &this->colliderJntSph, &this->dyna.actor, &ClSphDt_basket, this->ColliderJntSphElements);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void dust_set_basket(BgSpot18Basket* this, PlayState* play, f32 arg2) {
    static s16 ang = 0;
    Vec3f acceleration;
    Vec3f velocity;
    Vec3f position;
    f32 cosValue;
    s32 i;
    f32 randomValue;
    f32 sinValue;

    for (i = 0; i != 2; i++) {
        if (arg2 < fqrand()) {
            continue;
        }

        ang += 0x7530;

        sinValue = sin_s(ang);
        cosValue = cos_s(ang);

        randomValue = (fqrand() * 35.0f) + 35.0f;

        position.x = (randomValue * sinValue) + this->dyna.actor.world.pos.x;
        position.y = this->dyna.actor.world.pos.y + 10.0f;
        position.z = (randomValue * cosValue) + this->dyna.actor.world.pos.z;

        velocity.x = sinValue;
        velocity.y = 0.0f;
        velocity.z = cosValue;

        acceleration.x = 0.0f;
        acceleration.y = 0.5f;
        acceleration.z = 0.0f;

        Effect_SS_Dust_sc_li_ct(play, &position, &velocity, &acceleration, ((fqrand() * 16) + 80),
                      ((fqrand() * 30) + 80));
    }
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Spot18_Basket_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot18Basket* this = (BgSpot18Basket*)thisx;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    set_collision_data_basket(&this->dyna.actor, play);
    DynaPolyUty_bgdi_SG2KSG(&gGoronCityVaseCol, &colHeader);

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    ValueSet_process(&this->dyna.actor, value_init);
    Shape_Info_init(&this->dyna.actor.shape, 0.0f, Actor_shadow_circle, 15.0f);
    this->dyna.actor.home.pos.y += 0.01f;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        mv_walk_init(this);
        return;
    }

    mv_stop_init(this);
    Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_SPOT18_FUTA, this->dyna.actor.world.pos.x,
                       this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                       this->dyna.actor.shape.rot.y + 0x1555, this->dyna.actor.shape.rot.z, -1);

    if (this->dyna.actor.child == NULL) {
        PRINTF_COLOR_RED();
        PRINTF("Ｅｒｒｏｒ : 変化壷蓋発生失敗(%s %d)\n", "../z_bg_spot18_basket.c", 351);
        PRINTF_RST();
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot18_Basket_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot18Basket* this = (BgSpot18Basket*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjJntSph_dt_nzf(play, &this->colliderJntSph);
}

static void mv_stop_init(BgSpot18Basket* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgSpot18Basket* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        makeOnepointDemo(play, 4220, 80, &this->dyna.actor, CAM_ID_MAIN);
        mv_stop2_init(this);
    }
}

void mv_stop2_init(BgSpot18Basket* this) {
    this->actionFunc = mv_stop2;
    this->unk_216 = 0;
}

void mv_stop2(BgSpot18Basket* this, PlayState* play) {
    if (this->unk_216 > 20) {
        mv_walk_init(this);
        this->dyna.actor.child->parent = NULL;
        this->dyna.actor.child = NULL;
    }
}

static void mv_walk_init(BgSpot18Basket* this) {
    this->actionFunc = mv_walk;
    this->unk_210 = this->unk_20C = 0;
}

static void mv_walk(BgSpot18Basket* this, PlayState* play) {
    f32 positionDiff;
    Actor* colliderBaseAc;

    chase_s(&this->unk_210, 0x1F4, 0x1E);

    this->dyna.actor.shape.rot.y += this->unk_210;

    chase_f(&this->unk_208, 50.0f, 1.5f);
    chase_s(&this->unk_20C, 400, 15);

    this->unk_20E += this->unk_20C;

    this->dyna.actor.world.pos.x = (sin_s(this->unk_20E) * this->unk_208) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (cos_s(this->unk_20E) * this->unk_208) + this->dyna.actor.home.pos.z;

    if (this->colliderJntSph.base.acFlags & AC_HIT) {
        colliderBaseAc = this->colliderJntSph.base.ac;

        if (colliderBaseAc != NULL) {
            positionDiff = colliderBaseAc->world.pos.y - this->dyna.actor.world.pos.y;

            if (positionDiff > 120.0f && positionDiff < 200.0f) {
                if (Math3DLengthSquare2D(colliderBaseAc->world.pos.z, this->colliderJntSph.base.ac->world.pos.x,
                                    this->dyna.actor.world.pos.z, this->dyna.actor.world.pos.x) < SQ(32.0f)) {
                    makeOnepointDemo(play, 4210, 240, &this->dyna.actor, CAM_ID_MAIN);
                    mv_gurungurun1_init(this);
                    DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                }
            }
        }
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void mv_gurungurun1_init(BgSpot18Basket* this) {
    this->actionFunc = mv_gurungurun1;
    this->unk_216 = 0;
    this->unk_214 = 0;
}

void mv_gurungurun1(BgSpot18Basket* this, PlayState* play) {
    if (this->unk_216 > 120) {
        chase_s(&this->unk_210, 0x3E8, 0x32);
    } else {
        chase_s(&this->unk_210, 0xBB8, 0x64);
    }

    this->dyna.actor.shape.rot.y += this->unk_210;

    if (this->unk_216 < 70) {
        chase_f(&this->unk_208, 100.0f, 2.0f);
    } else {
        chase_f(&this->unk_208, 0.0f, 2.0f);
    }

    chase_s(&this->unk_20C, 1000, 20);

    this->unk_20E += this->unk_20C;

    this->dyna.actor.world.pos.x = (sin_s(this->unk_20E) * this->unk_208) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (cos_s(this->unk_20E) * this->unk_208) + this->dyna.actor.home.pos.z;

    this->unk_212 += 0xBB8;

    chase_s(&this->unk_214, 0x5DC, 0x1E);

    this->dyna.actor.shape.rot.x = cos_s(this->unk_212) * this->unk_214;
    this->dyna.actor.shape.rot.z = -sin_s(this->unk_212) * this->unk_214;

    if (this->unk_216 > 140) {
        mv_gurungurun2_init(this);
    }

    if (this->unk_216 < 80) {
        dust_set_basket(this, play, 1.0f);
    } else {
        dust_set_basket(this, play, 0.8f);
    }

    {
        f32 tempValue2 = (this->unk_210 - 500) * 0.0006f;
        f32 tempValue = CLAMP(tempValue2, 0.0f, 1.5f);

        Na_SetMotorSe(&this->dyna.actor.projectedPos, NA_SE_EV_WALL_MOVE_SP - SFX_FLAG, tempValue);
    }
}

void mv_gurungurun2_init(BgSpot18Basket* this) {
    s16 shapeRotY;

    shapeRotY = this->dyna.actor.shape.rot.y;
    this->actionFunc = mv_gurungurun2;

    if ((shapeRotY < -0x2E93) || (shapeRotY >= 0x7C19)) {
        this->unk_218 = 2;
    } else if (shapeRotY < 0x26C2) {
        this->unk_218 = 1;
    } else {
        this->unk_218 = 0;
    }

    this->unk_216 = 0;
}

void mv_gurungurun2(BgSpot18Basket* this, PlayState* play) {
    s32 pad;
    s32 tempUnk214;
    s16 arrayValue;

    this->unk_212 += 0xBB8;

    if (this->unk_216 >= 13) {
        tempUnk214 = chase_s(&this->unk_214, 0, 55);
    } else {
        tempUnk214 = 0;
    }

    this->dyna.actor.shape.rot.x = cos_s(this->unk_212) * this->unk_214;
    this->dyna.actor.shape.rot.z = -sin_s(this->unk_212) * this->unk_214;

    chase_s(&this->unk_210, 0x1F4, 0xA);
    this->dyna.actor.shape.rot.y += this->unk_210;

    if (tempUnk214 != 0) {
        arrayValue = Basket_FaceAngle[this->unk_218];

        if ((s16)(this->dyna.actor.shape.rot.y - arrayValue) >= 0) {
            this->dyna.actor.shape.rot.y = arrayValue;

            mv_item_init(this);
            DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    }

    if (this->unk_216 < 30) {
        dust_set_basket(this, play, 0.5f);
    } else {
        dust_set_basket(this, play, 0.3f);
    }

    {
        f32 tempUnk210 = (this->unk_210 - 500) * 0.0006f;
        f32 clampedTempUnk210 = CLAMP(tempUnk210, 0.0f, 1.5f);

        Na_SetMotorSe(&this->dyna.actor.projectedPos, NA_SE_EV_WALL_MOVE_SP - SFX_FLAG, clampedTempUnk210);
    }
}

void mv_item_init(BgSpot18Basket* this) {
    this->actionFunc = mv_item;
    this->unk_216 = 0;
}

static s16 item_ang_y[] = { -0x0FA0, 0x0320, 0x0FA0 };

void mv_item(BgSpot18Basket* this, PlayState* play) {
    s32 i;
    Actor* actor = &this->dyna.actor;
    Vec3f tempVector;
    EnItem00* collectible;

    if (this->unk_216 == 1) {
        tempVector.x = actor->world.pos.x;
        tempVector.y = actor->world.pos.y + 170.0f;
        tempVector.z = actor->world.pos.z;

        if (this->unk_218 == 0) {
            for (i = 0; i < ARRAY_COUNT(item_ang_y); i++) {
                collectible = Item_set0(play, &tempVector, ITEM00_BOMBS_A);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = item_ang_y[i];
                }
            }
        } else if (this->unk_218 == 1) {
            for (i = 0; i < ARRAY_COUNT(item_ang_y); i++) {
                collectible = Item_set0(play, &tempVector, ITEM00_RUPEE_GREEN);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = item_ang_y[i];
                }
            }
        } else if (this->unk_218 == 2) {
            if ((this->unk_21A != 0) || Actor_Environment_item_Check(play, PARAMS_GET_U(actor->params, 0, 6))) {
                collectible = Item_set0(play, &tempVector, ITEM00_RUPEE_PURPLE);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = item_ang_y[1];
                }
            } else {
                collectible = Item_set0(play, &tempVector,
                                                   (PARAMS_GET_U(actor->params, 0, 6) << 8) | ITEM00_HEART_PIECE);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = item_ang_y[1];
                    this->unk_21A = 1;
                }
            }

            collectible = Item_set0(play, &tempVector, ITEM00_RUPEE_RED);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 11.0f;
                collectible->actor.world.rot.y = item_ang_y[0];
            }

            collectible = Item_set0(play, &tempVector, ITEM00_RUPEE_BLUE);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 11.0f;
                collectible->actor.world.rot.y = item_ang_y[2];
            }
        }
    } else if (this->unk_216 == 2) {
        if (this->unk_218 == 2) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        } else {
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (this->unk_216 == 200) {
        mv_walk_init(this);
    }
}

void Bg_Spot18_Basket_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot18Basket* this = (BgSpot18Basket*)thisx;
    s32 bgId;

    this->unk_216++;
    this->actionFunc(this, play);
    this->dyna.actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId,
                                                              &this->dyna.actor, &this->dyna.actor.world.pos);
    if (this->actionFunc != mv_stop) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderJntSph.base);
        if (this->actionFunc != mv_stop2) {
            this->colliderJntSph.base.acFlags &= ~AC_HIT;
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
        }
    }
}

void Bg_Spot18_Basket_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot18Basket* this = (BgSpot18Basket*)thisx;

    CollisionCheck_Uty_convJntSphL2G(0, &this->colliderJntSph);
    CollisionCheck_Uty_convJntSphL2G(1, &this->colliderJntSph);
    Cheap_gfx_display(play, gGoronCityVaseDL);
}
