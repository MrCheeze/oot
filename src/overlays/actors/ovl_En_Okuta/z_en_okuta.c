#include "z_en_okuta.h"
#include "assets/objects/object_okuta/object_okuta.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Okuta_actor_ct(Actor* thisx, PlayState* play);
void En_Okuta_actor_dt(Actor* thisx, PlayState* play);
void En_Okuta_actor_move(Actor* thisx, PlayState* play2);
void En_Okuta_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnOkuta* this);
static void mode_wait(EnOkuta* this, PlayState* play);
void mode_water_up(EnOkuta* this, PlayState* play);
static void mode_water_down(EnOkuta* this, PlayState* play);
void mode_float(EnOkuta* this, PlayState* play);
static void mode_attack(EnOkuta* this, PlayState* play);
static void mode_damage(EnOkuta* this, PlayState* play);
static void mode_dead(EnOkuta* this, PlayState* play);
void mode_ice(EnOkuta* this, PlayState* play);
void mode_rock(EnOkuta* this, PlayState* play);

ActorProfile En_Okuta_Profile = {
    /**/ ACTOR_EN_OKUTA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_OKUTA,
    /**/ sizeof(EnOkuta),
    /**/ En_Okuta_actor_ct,
    /**/ En_Okuta_actor_dt,
    /**/ En_Okuta_actor_move,
    /**/ En_Okuta_actor_draw,
};

static ColliderCylinderInit OkutaAtPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 13, 20, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit OkutaAcOcPipeData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, -30, { 0, 0, 0 } },
};

static CollisionCheckInfoInit OkutaStatusData = { 1, 15, 60, 100 };

static DamageTable OkutaBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_OCTOROK, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6500, ICHAIN_STOP),
};

void En_Okuta_actor_ct(Actor* thisx, PlayState* play) {
    EnOkuta* this = (EnOkuta*)thisx;
    s32 pad;
    WaterBox* outWaterBox;
    f32 ySurface;
    s32 floorBgId;

    ValueSet_process(thisx, value_init);
    this->numShots = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    if (thisx->params == 0) {
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gOctorokSkel, &gOctorokAppearAnim, this->jointTable, this->morphTable,
                       38);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, thisx, &OkutaAcOcPipeData);
        CollisionCheck_Status_set2(&thisx->colChkInfo, &OkutaBtlData, &OkutaStatusData);
        if ((this->numShots == 0xFF) || (this->numShots == 0)) {
            this->numShots = 1;
        }
        thisx->floorHeight =
            T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &thisx->floorPoly, &floorBgId, thisx, &thisx->world.pos);
        //! @bug calls T_BGCheck_WaterSurfaceCheck2 directly
        if (!T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, thisx->world.pos.x, thisx->world.pos.z, &ySurface,
                                     &outWaterBox) ||
            (ySurface <= thisx->floorHeight)) {
            Actor_delete(thisx);
        } else {
            thisx->home.pos.y = ySurface;
        }
        mode_wait_init(this);
    } else {
        Shape_Info_init(&thisx->shape, 1100.0f, Actor_shadow_circle, 18.0f);
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, thisx, &OkutaAtPipeData);
        Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_PROP);
        this->timer = 30;
        thisx->shape.rot.y = 0;
        this->actionFunc = mode_rock;
        thisx->speed = 10.0f;
    }
}

void En_Okuta_actor_dt(Actor* thisx, PlayState* play) {
    EnOkuta* this = (EnOkuta*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void set_okuta_bubble(EnOkuta* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 10; i++) {
        Effect_SS_Bubble_ct(play, &this->actor.world.pos, -10.0f, 10.0f, 30.0f, 0.25f);
    }
}

void set_okuta_smoke(Vec3f* pos, Vec3f* velocity, s16 scaleStep, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 prim = { 255, 255, 255, 255 };
    static Color_RGBA8 env = { 150, 150, 150, 255 };

    Effect_SS_Dust_sc_cl_ct(play, pos, velocity, &zero_vec, &prim, &env, 0x190, scaleStep);
}

void set_okuta_splash(EnOkuta* this, PlayState* play) {
    Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.home.pos, NULL, NULL, 0, 1300);
}

void set_okuta_ripple(EnOkuta* this, PlayState* play) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.home.pos.y;
    pos.z = this->actor.world.pos.z;
    if ((play->gameplayFrames % 7) == 0 &&
        ((this->actionFunc != mode_attack) || ((this->actor.world.pos.y - this->actor.home.pos.y) < 50.0f))) {
        Effect_SS_G_Ripple_ct2(play, &pos, 250, 650, 0);
    }
}

static void mode_wait_init(EnOkuta* this) {
    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_wait;
    this->actor.world.pos.y = this->actor.home.pos.y;
}

void mode_water_up_init(EnOkuta* this, PlayState* play) {
    this->actor.draw = En_Okuta_actor_draw;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gOctorokAppearAnim);
    set_okuta_bubble(this, play);
    this->actionFunc = mode_water_up;
}

void mode_water_down_init(EnOkuta* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gOctorokHideAnim);
    this->actionFunc = mode_water_down;
}

void mode_float_init(EnOkuta* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gOctorokFloatAnim);
    this->timer = (this->actionFunc == mode_attack) ? 2 : 0;
    this->actionFunc = mode_float;
}

static void mode_attack_init(EnOkuta* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gOctorokShootAnim);
    if (this->actionFunc != mode_attack) {
        this->timer = this->numShots;
    }
    this->jumpHeight = this->actor.yDistToPlayer + 20.0f;
    this->jumpHeight = CLAMP_MIN(this->jumpHeight, 10.0f);
    if (this->jumpHeight > 50.0f) {
        set_okuta_splash(this, play);
    }
    if (this->jumpHeight > 50.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
    }
    this->actionFunc = mode_attack;
}

static void mode_damage_init(EnOkuta* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gOctorokHitAnim, -5.0f);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 11);
    this->collider.base.acFlags &= ~AC_HIT;
    Actor_set_scale(&this->actor, 0.01f);
    Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_DEAD1);
    this->actionFunc = mode_damage;
}

static void mode_dead_init(EnOkuta* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gOctorokDieAnim, -3.0f);
    this->timer = 0;
    this->actionFunc = mode_dead;
}

void mode_ice_init(EnOkuta* this) {
    this->timer = 80;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
    this->actionFunc = mode_ice;
}

void mode_rock_init(EnOkuta* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    f32 sinY = sin_s(this->actor.shape.rot.y);
    f32 cosY = cos_s(this->actor.shape.rot.y);

    pos.x = this->actor.world.pos.x + (25.0f * sinY);
    pos.y = this->actor.world.pos.y - 6.0f;
    pos.z = this->actor.world.pos.z + (25.0f * cosY);
    if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_OKUTA, pos.x, pos.y, pos.z, this->actor.shape.rot.x,
                    this->actor.shape.rot.y, this->actor.shape.rot.z, 0x10) != NULL) {
        pos.x = this->actor.world.pos.x + (40.0f * sinY);
        pos.z = this->actor.world.pos.z + (40.0f * cosY);
        pos.y = this->actor.world.pos.y;
        velocity.x = 1.5f * sinY;
        velocity.y = 0.0f;
        velocity.z = 1.5f * cosY;
        set_okuta_smoke(&pos, &velocity, 20, play);
    }
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
}

static void mode_wait(EnOkuta* this, PlayState* play) {
    this->actor.world.pos.y = this->actor.home.pos.y;
    if ((this->actor.xzDistToPlayer < 480.0f) && (this->actor.xzDistToPlayer > 200.0f)) {
        mode_water_up_init(this, play);
    }
}

void mode_water_up(EnOkuta* this, PlayState* play) {
    s32 pad;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 160.0f) {
            mode_water_down_init(this);
        } else {
            mode_float_init(this);
        }
    } else if (this->skelAnime.curFrame <= 4.0f) {
        Actor_set_scale(&this->actor, this->skelAnime.curFrame * 0.25f * 0.01f);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
        Actor_set_scale(&this->actor, 0.01f);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 2.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_LAND);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 3.0f) || Skeleton_Info_frame_check(&this->skelAnime, 15.0f)) {
        set_okuta_splash(this, play);
    }
}

static void mode_water_down(EnOkuta* this, PlayState* play) {
    s32 pad;

    add_calc2(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 30.0f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_BUBLE);
        set_okuta_bubble(this, play);
        mode_wait_init(this);
    } else if (this->skelAnime.curFrame >= 4.0f) {
        Actor_set_scale(&this->actor, (6.0f - this->skelAnime.curFrame) * 0.5f * 0.01f);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 2.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_SINK);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 4.0f)) {
        set_okuta_splash(this, play);
    }
}

void mode_float(EnOkuta* this, PlayState* play) {
    s16 yawDiff;
    s32 absYawDiff;

    this->actor.world.pos.y = this->actor.home.pos.y;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        if (this->timer != 0) {
            this->timer--;
        }
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.5f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_FLOAT);
    }
    if (this->actor.xzDistToPlayer < 160.0f || this->actor.xzDistToPlayer > 560.0f) {
        mode_water_down_init(this);
    } else {
        yawDiff = add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C, 0x38E);
        absYawDiff = ABS(yawDiff);
        if ((absYawDiff < 0x38E) && (this->timer == 0) && (this->actor.yDistToPlayer < 200.0f)) {
            mode_attack_init(this, play);
        }
    }
}

static void mode_attack(EnOkuta* this, PlayState* play) {
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            mode_float_init(this);
        } else {
            mode_attack_init(this, play);
        }
    } else {
        f32 curFrame = this->skelAnime.curFrame;

        if (curFrame < 13.0f) {
            this->actor.world.pos.y = (sinf((0.08333f * M_PI) * curFrame) * this->jumpHeight) + this->actor.home.pos.y;
        }
        if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
            mode_rock_init(this, play);
        }
        if ((this->jumpHeight > 50.0f) && Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
            set_okuta_splash(this, play);
        }
        if ((this->jumpHeight > 50.0f) && Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_LAND);
        }
    }
    if (this->actor.xzDistToPlayer < 160.0f) {
        mode_water_down_init(this);
    }
}

static void mode_damage(EnOkuta* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_dead_init(this);
    }
    add_calc2(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 5.0f);
}

static void mode_dead(EnOkuta* this, PlayState* play) {
    static Vec3f acc = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 prim = { 255, 255, 255, 255 };
    static Color_RGBA8 env = { 150, 150, 150, 0 };
    Vec3f velocity;
    Vec3f pos;
    s32 i;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->timer++;
    }
    add_calc2(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 5.0f);
    if (this->timer == 5) {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 40.0f;
        pos.z = this->actor.world.pos.z;
        velocity.x = 0.0f;
        velocity.y = -0.5f;
        velocity.z = 0.0f;
        set_okuta_smoke(&pos, &velocity, -0x14, play);
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_DEAD2);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 15.0f)) {
        set_okuta_splash(this, play);
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_LAND);
    }
    if (this->timer < 3) {
        Actor_set_scale(&this->actor, ((this->timer * 0.25f) + 1.0f) * 0.01f);
    } else if (this->timer < 6) {
        Actor_set_scale(&this->actor, (1.5f - ((this->timer - 2) * 0.2333f)) * 0.01f);
    } else if (this->timer < 11) {
        Actor_set_scale(&this->actor, (((this->timer - 5) * 0.04f) + 0.8f) * 0.01f);
    } else {
        if (chase_f(&this->actor.scale.x, 0.0f, 0.0005f)) {
            Effect_SE_Info_new(play, &this->actor.world.pos, 30, NA_SE_EN_OCTAROCK_BUBLE);
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x70);
            for (i = 0; i < 20; i++) {
                velocity.x = (fqrand() - 0.5f) * 7.0f;
                velocity.y = fqrand() * 7.0f;
                velocity.z = (fqrand() - 0.5f) * 7.0f;
                Effect_SS_Dt_Bubble_sc_cl_co_ct(play, &this->actor.world.pos, &velocity, &acc, &prim,
                                                  &env, get_random_timer(100, 50), 25, 0);
            }
            Actor_delete(&this->actor);
        }
        this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    }
}

void mode_ice(EnOkuta* this, PlayState* play) {
    Vec3f pos;
    s16 posParam;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        mode_dead_init(this);
    }
    if ((this->timer >= 64) && (this->timer & 1)) {
        posParam = (this->timer - 64) >> 1;
        pos.y = (this->actor.world.pos.y - 32.0f) + (8.0f * (8 - posParam));
        pos.x = this->actor.world.pos.x + ((posParam & 2) ? 10.0f : -10.0f);
        pos.z = this->actor.world.pos.z + ((posParam & 1) ? 10.0f : -10.0f);
        Effect_En_Ice_ct0(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255,
                                       (fqrand() * 0.2f) + 1.9f);
    }
    add_calc2(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 5.0f);
}

void mode_rock(EnOkuta* this, PlayState* play) {
    Vec3f pos;
    Player* player = GET_PLAYER(play);
    Vec3s shieldRot;

    this->timer--;
    if (this->timer == 0) {
        this->actor.gravity = -1.0f;
    }
    this->actor.home.rot.z += 0x1554;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        this->actor.gravity = -1.0f;
        this->actor.speed -= 0.1f;
        this->actor.speed = CLAMP_MIN(this->actor.speed, 1.0f);
    }
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        (this->collider.base.atFlags & AT_HIT) || this->collider.base.acFlags & AC_HIT ||
        this->collider.base.ocFlags1 & OC1_HIT || this->actor.floorHeight == BGCHECK_Y_MIN) {
        if ((player->currentShield == PLAYER_SHIELD_DEKU ||
             (player->currentShield == PLAYER_SHIELD_HYLIAN && LINK_IS_ADULT)) &&
            this->collider.base.atFlags & AT_HIT && this->collider.base.atFlags & AT_TYPE_ENEMY &&
            this->collider.base.atFlags & AT_BOUNCED) {
            this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED | AT_TYPE_ENEMY);
            this->collider.base.atFlags |= AT_TYPE_PLAYER;
            this->collider.elem.atDmgInfo.dmgFlags = DMG_DEKU_STICK;
            Matrix_to_rotate_new(&player->shieldMf, &shieldRot, 0);
            this->actor.world.rot.y = shieldRot.y + 0x8000;
            this->timer = 30;
        } else {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y + 11.0f;
            pos.z = this->actor.world.pos.z;
            Effect_Hahen_Kakusan_ct3(play, &pos, 6.0f, 0, 1, 2, 15, 7, 10, gOctorokProjectileDL);
            Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
            Actor_delete(&this->actor);
        }
    } else if (this->timer == -300) {
        Actor_delete(&this->actor);
    }
}

void set_head_model_scale(EnOkuta* this) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->actionFunc == mode_water_up) {
        if (curFrame < 8.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = 1.0f;
        } else if (curFrame < 10.0f) {
            this->headScale.x = this->headScale.z = 1.0f;
            this->headScale.y = ((curFrame - 7.0f) * 0.4f) + 1.0f;
        } else if (curFrame < 14.0f) {
            this->headScale.x = this->headScale.z = ((curFrame - 9.0f) * 0.075f) + 1.0f;
            this->headScale.y = 1.8f - ((curFrame - 9.0f) * 0.25f);
        } else {
            this->headScale.x = this->headScale.z = 1.3f - ((curFrame - 13.0f) * 0.05f);
            this->headScale.y = ((curFrame - 13.0f) * 0.0333f) + 0.8f;
        }
    } else if (this->actionFunc == mode_water_down) {
        if (curFrame < 3.0f) {
            this->headScale.y = 1.0f;
        } else if (curFrame < 4.0f) {
            this->headScale.y = (curFrame - 2.0f) + 1.0f;
        } else {
            this->headScale.y = 2.0f - ((curFrame - 3.0f) * 0.333f);
        }
        this->headScale.x = this->headScale.z = 1.0f;
    } else if (this->actionFunc == mode_attack) {
        if (curFrame < 5.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = (curFrame * 0.125f) + 1.0f;
        } else if (curFrame < 7.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = 1.5f - ((curFrame - 4.0f) * 0.35f);
        } else if (curFrame < 17.0f) {
            this->headScale.x = this->headScale.z = ((curFrame - 6.0f) * 0.05f) + 0.8f;
            this->headScale.y = 0.8f;
        } else {
            this->headScale.x = this->headScale.z = 1.3f - ((curFrame - 16.0f) * 0.1f);
            this->headScale.y = ((curFrame - 16.0f) * 0.0666f) + 0.8f;
        }
    } else if (this->actionFunc == mode_float) {
        this->headScale.x = this->headScale.z = 1.0f;
        this->headScale.y = (sinf((M_PI / 16) * curFrame) * 0.2f) + 1.0f;
    } else {
        this->headScale.x = this->headScale.y = this->headScale.z = 1.0f;
    }
}

void En_Okuta_damage_proc(EnOkuta* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            Actor_info_finish(play, &this->actor);
            this->actor.colChkInfo.health = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            if (this->actor.colChkInfo.damageEffect == 3) {
                mode_ice_init(this);
            } else {
                mode_damage_init(this);
            }
        }
    }
}

void En_Okuta_actor_move(Actor* thisx, PlayState* play2) {
    EnOkuta* this = (EnOkuta*)thisx;
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    WaterBox* outWaterBox;
    f32 ySurface;
    Vec3f prevPos;
    s32 canRestorePrevPos;

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29))) {
        if (this->actor.params == 0) {
            En_Okuta_damage_proc(this, play);
            if (!T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                         &ySurface, &outWaterBox) ||
                (ySurface < this->actor.floorHeight)) {
                if (this->actor.colChkInfo.health != 0) {
                    Actor_delete(&this->actor);
                    return;
                }
            } else {
                this->actor.home.pos.y = ySurface;
            }
        }
        this->actionFunc(this, play);
        if (this->actor.params == 0) {
            set_head_model_scale(this);
            this->collider.dim.height =
                (((OkutaAcOcPipeData.dim.height * this->headScale.y) - this->collider.dim.yShift) *
                 this->actor.scale.y * 100.0f);
        } else {
            canRestorePrevPos = false;
            Actor_position_moveF(&this->actor);
            xyz_t_move(&prevPos, &this->actor.world.pos);
            Actor_BGcheck2(play, &this->actor, 10.0f, 15.0f, 30.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
                T_BGCheck_CheckArrowNoHit(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                canRestorePrevPos = true;
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
            }
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                T_BGCheck_CheckArrowNoHit(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
                canRestorePrevPos = true;
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            }
            if (canRestorePrevPos && !(this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL))) {
                xyz_t_move(&this->actor.world.pos, &prevPos);
            }
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if ((this->actionFunc == mode_water_up) || (this->actionFunc == mode_water_down)) {
            this->collider.dim.pos.y = this->actor.world.pos.y + (this->skelAnime.jointTable->y * this->actor.scale.y);
            this->collider.dim.radius = OkutaAcOcPipeData.dim.radius * this->actor.scale.x * 100.0f;
        }
        if (this->actor.params == 0x10) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != mode_wait) {
            if ((this->actionFunc != mode_dead) && (this->actionFunc != mode_damage) &&
                (this->actionFunc != mode_ice)) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            }
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
        Actor_world_to_eye(&this->actor, 15.0f);
        if ((this->actor.params == 0) && (this->actor.draw != NULL)) {
            set_okuta_ripple(this, play);
        }
    }
}

s32 set_mouth_model_scale(EnOkuta* this, f32 curFrame, Vec3f* scale) {
    if (this->actionFunc == mode_float) {
        scale->x = scale->z = 1.0f;
        scale->y = (sinf((M_PI / 16) * curFrame) * 0.4f) + 1.0f;
    } else if (this->actionFunc == mode_attack) {
        if (curFrame < 5.0f) {
            scale->x = 1.0f;
            scale->y = scale->z = (curFrame * 0.25f) + 1.0f;
        } else if (curFrame < 7.0f) {
            scale->x = (curFrame - 4.0f) * 0.5f + 1.0f;
            scale->y = scale->z = 2.0f - (curFrame - 4.0f) * 0.5f;
        } else {
            scale->x = 2.0f - ((curFrame - 6.0f) * 0.0769f);
            scale->y = scale->z = 1.0f;
        }
    } else if (this->actionFunc == mode_dead) {
        if (curFrame >= 35.0f || curFrame < 25.0f) {
            return false;
        }
        if (curFrame < 27.0f) {
            scale->x = 1.0f;
            scale->y = scale->z = ((curFrame - 24.0f) * 0.5f) + 1.0f;
        } else if (curFrame < 30.0f) {
            scale->x = (curFrame - 26.0f) * 0.333f + 1.0f;
            scale->y = scale->z = 2.0f - (curFrame - 26.0f) * 0.333f;
        } else {
            scale->x = 2.0f - ((curFrame - 29.0f) * 0.2f);
            scale->y = scale->z = 1.0f;
        }
    } else {
        return false;
    }

    return true;
}

s32 en_okuta_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnOkuta* this = (EnOkuta*)thisx;
    f32 curFrame;
    Vec3f scale;
    s32 doScale = false;

    curFrame = this->skelAnime.curFrame;
    if (this->actionFunc == mode_dead) {
        curFrame += this->timer;
    }
    if (limbIndex == 5) {
        if ((this->headScale.x != 1.0f) || (this->headScale.y != 1.0f) || (this->headScale.z != 1.0f)) {
            scale = this->headScale;
            doScale = true;
        }
    } else if (limbIndex == 8) {
        doScale = set_mouth_model_scale(this, curFrame, &scale);
    }
    if (doScale) {
        Matrix_scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    }
    return false;
}

void En_Okuta_actor_draw(Actor* thisx, PlayState* play) {
    EnOkuta* this = (EnOkuta*)thisx;
    s32 pad;

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->actor.params == 0) {
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_okuta_display1, NULL,
                          this);
    } else {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_okuta.c", 1653);

        Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD(this->actor.home.rot.z), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_okuta.c", 1657);
        gSPDisplayList(POLY_OPA_DISP++, gOctorokProjectileDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_okuta.c", 1662);
    }
}
