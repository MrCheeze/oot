#include "z_en_bigokuta.h"
#include "assets/objects/object_bigokuta/object_bigokuta.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Bigokuta_actor_ct(Actor* thisx, PlayState* play);
void En_Bigokuta_actor_dt(Actor* thisx, PlayState* play);
void En_Bigokuta_actor_move(Actor* thisx, PlayState* play2);
void En_Bigokuta_actor_draw(Actor* thisx, PlayState* play);

static void mode_event_wait_init(EnBigokuta* this);
static void mode_wait_init(EnBigokuta* this);
static void mode_jump(EnBigokuta* this, PlayState* play);
static void mode_event_wait(EnBigokuta* this, PlayState* play);
void mode_event_jump(EnBigokuta* this, PlayState* play);
void mode_event_turn(EnBigokuta* this, PlayState* play);
static void mode_wait(EnBigokuta* this, PlayState* play);
static void mode_move(EnBigokuta* this, PlayState* play);
static void mode_rotate(EnBigokuta* this, PlayState* play);
static void mode_down(EnBigokuta* this, PlayState* play);
static void mode_up(EnBigokuta* this, PlayState* play);
void bigokuta_set_splash(EnBigokuta* this, PlayState* play);
static void mode_turn(EnBigokuta* this, PlayState* play);
static void mode_dead(EnBigokuta* this, PlayState* play);
static void mode_damage(EnBigokuta* this, PlayState* play);
static void mode_stop(EnBigokuta* this, PlayState* play);
void bigokuta_set_land(EnBigokuta* this, PlayState* play);

static Color_RGBA8 prim = { 255, 255, 255, 255 };
static Color_RGBA8 env = { 100, 255, 255, 255 };
static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

ActorProfile En_Bigokuta_Profile = {
    /**/ ACTOR_EN_BIGOKUTA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BIGOKUTA,
    /**/ sizeof(EnBigokuta),
    /**/ En_Bigokuta_actor_ct,
    /**/ En_Bigokuta_actor_dt,
    /**/ En_Bigokuta_actor_move,
    /**/ En_Bigokuta_actor_draw,
};

static ColliderJntSphElementInit BigokutaAllJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0x20000000, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 45, -30 }, 75 }, 100 },
    },
};

static ColliderJntSphInit BigokutaAllJntSphData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(BigokutaAllJntSphElemData),
    BigokutaAllJntSphElemData,
};

static ColliderCylinderInit BigokutaAllPipeData[] = {
    { {
          COL_MATERIAL_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEM_MATERIAL_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          ATELEM_ON | ATELEM_SFX_HARD,
          ACELEM_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { 30, 0, 12 } } },
    { {
          COL_MATERIAL_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEM_MATERIAL_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          ATELEM_ON | ATELEM_SFX_HARD,
          ACELEM_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { -30, 0, 12 } } },
};

static CollisionCheckInfoInit BigokutaStatusData[] = { 4, 130, 120, 200 };

static DamageTable BigokutaBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_BIGOCTO, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 33, ICHAIN_STOP),
};

// possibly color data
static u32 smoke_prim[] = { 0xFFFFFFFF, 0x969696FF };

void En_Bigokuta_actor_ct(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;

    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_bigokuta_Skel_006BC0, &object_bigokuta_Anim_0014B8,
                       this->jointTable, this->morphTable, 20);

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &BigokutaAllJntSphData, &this->element);

    this->collider.elements[0].dim.worldSphere.radius = this->collider.elements[0].dim.modelSphere.radius;

    for (i = 0; i < ARRAY_COUNT(BigokutaAllPipeData); i++) {
        ClObjPipe_ct(play, &this->cylinder[i]);
        ClObjPipe_set5(play, &this->cylinder[i], &this->actor, &BigokutaAllPipeData[i]);
    }

    CollisionCheck_Status_set2(&this->actor.colChkInfo, &BigokutaBtlData, BigokutaStatusData);

    this->unk_194 = 1;

    if (this->actor.params == 0) {
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        mode_event_wait_init(this);
    } else {
        mode_wait_init(this);
        this->unk_19A = 0;
        this->unk_196 = 1;
        this->actor.home.pos.y = -1025.0f;
    }
}

void En_Bigokuta_actor_dt(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;

    ClObjJntSph_dt_nzf(play, &this->collider);
    for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
        ClObjPipe_dt(play, &this->cylinder[i]);
    }
}

void bigokuta_set_move_pos(EnBigokuta* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y + this->unk_194 * -0x4000;
    this->actor.world.pos.x = sin_s(this->actor.world.rot.y) * 263.0f + this->actor.home.pos.x;
    this->actor.world.pos.z = cos_s(this->actor.world.rot.y) * 263.0f + this->actor.home.pos.z;
}

void bigokuta_set_ripple(EnBigokuta* this, PlayState* play) {
    Vec3f pos;
    f32 yDistFromHome = this->actor.world.pos.y - this->actor.home.pos.y;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.home.pos.y + 3.0f;
    pos.z = this->actor.world.pos.z;

    if (((play->gameplayFrames % 7) == 0) && (yDistFromHome <= 0.0f) && (yDistFromHome > -100.0f)) {
        Effect_SS_G_Ripple_ct2(play, &pos, 800, 1300, 0);
    }
}

void bigokuta_set_splash(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;
    s16 rot;

    if (play->gameplayFrames & 1) {
        rot = get_random_timer(0x1200, 0xC00) + this->actor.shape.rot.y - this->unk_194 * 0xA00;
    } else {
        rot = this->actor.shape.rot.y - this->unk_194 * 0xA00 - get_random_timer(0x1200, 0xC00);
    }
    if (this->actionFunc != mode_down) {
        if (this->actionFunc == mode_rotate || (play->gameplayFrames & 2)) {
            effectPos.x = this->actor.world.pos.x - sin_s(rot) * 80.0f;
            effectPos.z = this->actor.world.pos.z - cos_s(rot) * 80.0f;
            effectPos.y = this->actor.home.pos.y + 1.0f;
            Effect_SS_G_Ripple_ct2(play, &effectPos, 100, 500, 0);
        } else {
            effectPos.x = this->actor.world.pos.x - sin_s(rot) * 120.0f;
            effectPos.z = this->actor.world.pos.z - cos_s(rot) * 120.0f;
            effectPos.y = this->actor.home.pos.y + 5.0f;
        }
    } else {
        effectPos.x = this->actor.world.pos.x - sin_s(rot) * 50.0f;
        effectPos.z = this->actor.world.pos.z - cos_s(rot) * 50.0f;
        effectPos.y = this->actor.home.pos.y + 1.0f;
        Effect_SS_G_Ripple_ct2(play, &effectPos, 100, 500, 0);
    }
    Effect_SS_G_Splash_sc_cl_ct(play, &effectPos, NULL, NULL, 1, 800);
    if (this->actionFunc != mode_down) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_DAIOCTA_SPLASH - SFX_FLAG);
    }
}

void bigokuta_set_land(EnBigokuta* this, PlayState* play) {
    s32 i;
    Vec3f effectPos;

    effectPos.y = this->actor.world.pos.y;

    for (i = 0; i < 4; i++) {
        effectPos.x = ((i >= 2) ? 1 : -1) * 60.0f + this->actor.world.pos.x;
        effectPos.z = ((i & 1) ? 1 : -1) * 60.0f + this->actor.world.pos.z;
        Effect_SS_G_Splash_sc_cl_ct(play, &effectPos, NULL, NULL, 1, 2000);
    }

    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
    Actor_SE_set(&this->actor, NA_SE_EN_GOLON_LAND_BIG);
    ActorQuakeRumbleSet(&this->actor, play, 10, 8);
}

void bigokuta_set_object_rot_speed(EnBigokuta* this) {
    Actor* parent = this->actor.parent;

    if (parent != NULL) {
        chase_angle(&parent->world.rot.y, this->unk_19A, 0x10);
    }
}

static void mode_event_wait_init(EnBigokuta* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_bigokuta_Anim_0014B8);
    this->unk_19A = 0;
    this->actor.home.pos.y = -1025.0f;
    this->unk_196 = 121;
    this->actionFunc = mode_event_wait;
}

void mode_event_jump_init(EnBigokuta* this) {
    this->unk_196 = 21;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actionFunc = mode_event_jump;
}

void mode_event_turn_init(EnBigokuta* this) {
    this->actor.world.pos.x = this->actor.home.pos.x + 263.0f;
    this->unk_196 = 10;
    this->actionFunc = mode_event_turn;
    this->actor.world.pos.y = this->actor.home.pos.y;
}

static void mode_wait_init(EnBigokuta* this) {
    this->unk_196 = 40;
    this->actionFunc = mode_wait;
}

static void mode_move_init(EnBigokuta* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &object_bigokuta_Anim_001CA4, -5.0f);
    this->unk_196 = 350;
    this->unk_198 = 80;
    this->unk_19A = this->unk_194 * -0x200;
    bigokuta_set_move_pos(this);
    this->cylinder[0].base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = mode_move;
}

static void mode_jump_init(EnBigokuta* this) {
    this->unk_196 = 16;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.colorFilterTimer = 0;
    this->actionFunc = mode_jump;
}

static void mode_turn_init(EnBigokuta* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &object_bigokuta_Anim_0014B8, -5.0f);
    this->unk_195 = true;
    this->actor.world.rot.x = this->actor.shape.rot.y + 0x8000;
    this->unk_19A = this->unk_194 * 0x200;
    this->collider.base.acFlags &= ~AC_ON;
    this->cylinder[0].base.atFlags |= AT_ON;
    this->actionFunc = mode_turn;
}

static void mode_stop_init(EnBigokuta* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_bigokuta_Anim_000D1C, -5.0f);
    this->unk_196 = 80;
    this->unk_19A = 0;
    this->cylinder[0].base.atFlags |= AT_ON;
    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_MAHI);
    if (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & DMG_DEKU_NUT) {
        this->unk_195 = true;
        this->unk_196 = 20;
    } else {
        this->unk_195 = false;
        this->unk_196 = 80;
    }
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, this->unk_196);
    this->actionFunc = mode_stop;
}

static void mode_damage_init(EnBigokuta* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_bigokuta_Anim_000444, -5.0f);
    this->unk_196 = 24;
    this->unk_19A = 0;
    this->cylinder[0].base.atFlags &= ~AT_ON;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 24);
    this->actionFunc = mode_damage;
}
static void mode_dead_init(EnBigokuta* this) {

    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_bigokuta_Anim_000A74, -5.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_DEAD2);
    this->unk_196 = 38;
    this->unk_198 = 10;
    this->actionFunc = mode_dead;
}

static void mode_rotate_init(EnBigokuta* this) {
    if (!this->unk_195) {
        if (fqrand() < 0.5f) {
            this->unk_196 = 24;
        } else {
            this->unk_196 = 28;
        }
    } else {
        if (ABS(this->actor.shape.rot.y - this->actor.yawTowardsPlayer) >= 0x4000) {
            this->unk_196 = 4;
        } else {
            this->unk_196 = 0;
        }
    }
    this->unk_19A = 0;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_rotate;
}

static void mode_down_init(EnBigokuta* this) {
    this->unk_194 = fqrand() < 0.5f ? -1 : 1;
    this->unk_19A = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->cylinder[0].base.atFlags &= ~AT_ON;
    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_SINK);
    this->actionFunc = mode_down;
}

static void mode_up_init(EnBigokuta* this, PlayState* play) {
    this->actor.world.rot.y = Actor_search_position_angleY(&GET_PLAYER(play)->actor, &this->actor.home.pos);
    this->actor.shape.rot.y = this->actor.world.rot.y + (this->unk_194 * 0x4000);
    bigokuta_set_move_pos(this);
    this->actionFunc = mode_up;
}

static void mode_event_wait(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    this->unk_196--;

    if (this->unk_196 == 13 || this->unk_196 == -20) {
        Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if (this->unk_196 == 1) {
        Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
    }
    if (this->actor.params == 1) {
        mode_event_jump_init(this);
    }
}

void mode_event_jump(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;

    this->unk_196--;

    if (this->unk_196 >= 0) {
        chase_f(&this->actor.world.pos.x, this->actor.home.pos.x + 263.0f, 263.0f / 21);

        if (this->unk_196 < 14) {
            this->actor.world.pos.y = sinf(this->unk_196 * (M_PI / 28)) * 200.0f + this->actor.home.pos.y;
        } else {
            this->actor.world.pos.y =
                sinf((this->unk_196 - 7) * (M_PI / 14)) * 130.0f + (this->actor.home.pos.y + 70.0f);
        }
        if (this->unk_196 == 0) {
            effectPos.x = this->actor.world.pos.x + 40.0f;
            effectPos.y = this->actor.world.pos.y;
            effectPos.z = this->actor.world.pos.z - 70.0f;
            Effect_SS_G_Splash_sc_cl_ct(play, &effectPos, NULL, NULL, 1, 2000);
            effectPos.x = this->actor.world.pos.x - 40.0f;
            Effect_SS_G_Splash_sc_cl_ct(play, &effectPos, NULL, NULL, 1, 2000);
            Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
            Actor_SE_set(&this->actor, NA_SE_EN_GOLON_LAND_BIG);
            z_vibctl2_vib_setQ(0.0f, 255, 20, 150);
        }
    } else if (this->unk_196 < -1) {
        this->actor.world.pos.y = this->actor.home.pos.y - (sinf((this->unk_196 + 1) * (M_PI / 10)) * 20.0f);
        if (this->unk_196 == -10) {
            mode_event_turn_init(this);
        }
    }
}

void mode_event_turn(EnBigokuta* this, PlayState* play) {
    if (chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y + 0x4000, 0x400)) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        if (this->unk_196 == 0) {
            bigokuta_set_move_pos(this);
            this->actor.home.pos.y = this->actor.world.pos.y;
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            this->actor.params = 2;
            Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
            mode_wait_init(this);
        }
    }
}

static void mode_wait(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 0) {
        if (this->actor.params == 3) {
            Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
        }
        mode_move_init(this);
    }
}

static void mode_move(EnBigokuta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 phi_v0;
    s16 pad;
    s16 phi_v1;
    Vec3f sp28;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_BUBLE);
    }

    if (this->unk_196 < 0) {
        this->actor.shape.rot.y += this->unk_194 * 0x200;
        bigokuta_set_move_pos(this);
        this->unk_196++;
        if (this->unk_196 == 0) {
            this->unk_196 = 350;
        }
        bigokuta_set_splash(this, play);
        return;
    }

    phi_v1 = (Actor_search_position_distanceXZ(&player->actor, &this->actor.home.pos) - 180.0f) * (8.0f / 15);
    Actor_search_position_project_distanceXZ(&this->actor, &sp28, &player->actor.world.pos);
    if (fabsf(sp28.x) > 263.0f || ((sp28.z > 0.0f) && !Actor_player_direction_check(&this->actor, 0x1B00) &&
                                   !Actor_player_look_direction_check(&this->actor, 0x2000, play))) {
        phi_v1 -= 0x80;
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
    }

    if ((this->actor.xzDistToPlayer < 250.0f) && !Actor_player_direction_check(&this->actor, 0x6000)) {
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
        if (this->actor.xzDistToPlayer < 180.0f) {
            phi_v1 += 0x20;
        }
    } else {
        this->unk_198 = 80;
    }
    if (this->actor.colChkInfo.health == 1) {
        phi_v1 = (phi_v1 + 0x130) * 1.1f;
    } else {
        phi_v1 += 0x130;
    }
    this->actor.shape.rot.y += phi_v1 * this->unk_194;
    bigokuta_set_move_pos(this);
    bigokuta_set_splash(this, play);
    if (this->unk_198 == 0) {
        mode_down_init(this);
    } else if (this->unk_196 == 0) {
        mode_turn_init(this);
    } else if (this->unk_195) {
        phi_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (phi_v0 < 0) {
            phi_v0 = -phi_v0;
        }
        if (phi_v0 < 0x4100 && phi_v0 > 0x3F00) {
            if (fqrand() < 0.6f) {
                this->unk_196 = 0;
                mode_turn_init(this);
            }
            this->unk_195 = false;
        }
    }
}

static void mode_jump(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    this->actor.world.pos.y = (sinf(this->unk_196 * (M_PI / 16)) * 100.0f) + this->actor.home.pos.y;
    if (this->unk_196 == 0) {
        bigokuta_set_land(this, play);
        mode_move_init(this);
    }
}

static void mode_turn(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 20) {
        Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if ((this->unk_196 == 0) && chase_angle(&this->actor.shape.rot.y, this->actor.world.rot.x, 0x800)) {
        this->unk_194 = -this->unk_194;
        mode_move_init(this);
    }
}

static void mode_stop(EnBigokuta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 speedXZ;

    if (this->unk_196 != 0) {
        this->unk_196--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->collider.base.ocFlags1 & OC1_HIT) || (this->cylinder[0].base.ocFlags1 & OC1_HIT) ||
        (this->cylinder[1].base.ocFlags1 & OC1_HIT)) {
        speedXZ = CLAMP_MIN(player->actor.speed, 1.0f);
        if (!(this->collider.base.ocFlags1 & OC1_HIT)) {
            this->cylinder[0].base.ocFlags1 &= ~OC1_HIT;
            this->cylinder[1].base.ocFlags1 &= ~OC1_HIT;
            speedXZ *= -1.0f;
        }
        player->actor.world.pos.x -= speedXZ * sin_s(this->actor.shape.rot.y);
        player->actor.world.pos.z -= speedXZ * cos_s(this->actor.shape.rot.y);
    }
    if (this->unk_196 == 0) {
        mode_rotate_init(this);
    }
}

static void mode_damage(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 0) {
        if (this->actor.colChkInfo.health != 0) {
            mode_turn_init(this);
        } else {
            mode_dead_init(this);
        }
    } else if (this->unk_196 >= 8) {
        this->actor.shape.rot.y += this->unk_194 * 0x200;
        this->actor.world.pos.y = sinf((this->unk_196 - 8) * (M_PI / 16)) * 100.0f + this->actor.home.pos.y;
        bigokuta_set_move_pos(this);
        if (this->unk_196 == 8) {
            bigokuta_set_land(this, play);
        }
    }
}
static void mode_dead(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;

    if (this->unk_196 != 0) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        if (this->unk_196 >= 10) {
            this->actor.shape.rot.y += 0x2000;
        }
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
        if (this->unk_198 == 6) {
            effectPos.x = this->actor.world.pos.x;
            effectPos.y = this->actor.world.pos.y + 150.0f;
            effectPos.z = this->actor.world.pos.z;
            Effect_SS_Dust_sc_cl_ct(play, &effectPos, &zero_vec, &zero_vec, &prim, &env,
                          1200, 20);
            Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_DEAD2);
        }
        if (this->unk_198 == 0 && chase_f(&this->actor.scale.y, 0.0f, 0.001f)) {
            Actor_Environment_room_clear_On(play, this->actor.room);
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
            setCameraFlag(play->cameraPtrs[CAM_ID_MAIN], CAM_STATE_CHECK_BG);
            Effect_SE_Info_new(play, &this->actor.world.pos, 50, NA_SE_EN_OCTAROCK_BUBLE);
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xB0);
            Actor_delete(&this->actor);
        }
    }
}

static void mode_rotate(EnBigokuta* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_196 != 0) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        this->actor.shape.rot.y += 0x2000;
    }
    if (this->unk_196 == 0) {
        if ((s16)(this->actor.shape.rot.y - this->actor.world.rot.y) > 0) {
            this->unk_194 = 1;
        } else {
            this->unk_194 = -1;
        }

        mode_move_init(this);
        if (!this->unk_195) {
            this->unk_196 = -40;
        }
    }
    bigokuta_set_splash(this, play);
}

static void mode_down(EnBigokuta* this, PlayState* play) {
    this->actor.world.pos.y -= 10.0f;
    this->actor.shape.rot.y += 0x2000;
    if (this->actor.world.pos.y < (this->actor.home.pos.y + -200.0f)) {
        mode_up_init(this, play);
    }
    bigokuta_set_splash(this, play);
}

static void mode_up(EnBigokuta* this, PlayState* play) {
    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, 10.0f)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        mode_move_init(this);
    }
}

void bigokuta_set_collision(EnBigokuta* this) {
    s32 i;
    f32 sin = sin_s(this->actor.shape.rot.y);
    f32 cos = cos_s(this->actor.shape.rot.y);

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x +
                                                          (this->collider.elements[0].dim.modelSphere.center.x * cos) +
                                                          (this->collider.elements[0].dim.modelSphere.center.z * sin);
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z +
                                                          (this->collider.elements[0].dim.modelSphere.center.z * cos) -
                                                          (this->collider.elements[0].dim.modelSphere.center.x * sin);
    this->collider.elements[0].dim.worldSphere.center.y =
        this->actor.world.pos.y + this->collider.elements[0].dim.modelSphere.center.y;

    for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
        this->cylinder[i].dim.pos.x =
            this->actor.world.pos.x + BigokutaAllPipeData[i].dim.pos.z * sin + BigokutaAllPipeData[i].dim.pos.x * cos;
        this->cylinder[i].dim.pos.z =
            this->actor.world.pos.z + BigokutaAllPipeData[i].dim.pos.z * cos - BigokutaAllPipeData[i].dim.pos.x * sin;
        this->cylinder[i].dim.pos.y = this->actor.world.pos.y;
    }
}

void En_Bigokuta_attack_proc(EnBigokuta* this, PlayState* play) {
    s16 effectRot;
    s16 yawDiff;

    if ((this->cylinder[0].base.atFlags & AT_HIT) || (this->cylinder[1].base.atFlags & AT_HIT) ||
        (this->collider.base.atFlags & AT_HIT)) {
        this->cylinder[0].base.atFlags &= ~AT_HIT;
        this->cylinder[1].base.atFlags &= ~AT_HIT;
        this->collider.base.atFlags &= ~AT_HIT;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (yawDiff > 0x4000) {
            effectRot = 0x4000;
        } else if (yawDiff > 0) {
            effectRot = 0x6000;
        } else if (yawDiff < -0x4000) {
            effectRot = -0x4000;
        } else {
            effectRot = -0x6000;
        }
        Actor_player_power_damage_set(play, &this->actor, 10.0f, this->actor.world.rot.y + effectRot, 5.0f);
        if (this->actionFunc == mode_move) {
            mode_turn_init(this);
            this->unk_196 = 40;
        } else if (this->actionFunc == mode_rotate) {
            if ((effectRot * this->unk_194) > 0) {
                this->unk_194 = 0 - this->unk_194;
                this->unk_196 += 4;
            }
        }
    }
}

void En_Bigokuta_damage_proc(EnBigokuta* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (this->actor.colChkInfo.damageEffect == 1) {
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == 0xF) {
                mode_jump_init(this);
            } else if (!Actor_player_direction_check(&this->actor, 0x4000)) {
                if (hp_down(&this->actor) == 0) { // Dead
                    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_DEAD);
                    Actor_info_finish(play, &this->actor);
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_DAIOCTA_DAMAGE);
                }
                mode_damage_init(this);
            }
        }
    }
}

void En_Bigokuta_actor_move(Actor* thisx, PlayState* play2) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;
    PlayState* play = play2;

    En_Bigokuta_attack_proc(this, play);
    En_Bigokuta_damage_proc(this, play);
    this->actionFunc(this, play);
    bigokuta_set_object_rot_speed(this);
    bigokuta_set_collision(this);
    changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BIG_OCTO);
    clearCameraFlag(play->cameraPtrs[CAM_ID_MAIN], CAM_STATE_CHECK_BG);

    if (this->cylinder[0].base.atFlags & AT_ON) {
        if (this->actionFunc != mode_stop) {
            for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
                CollisionCheck_setAT(play, &play->colChkCtx, &this->cylinder[i].base);
            }
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
        } else {
            for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->cylinder[i].base);
            }
        }
        for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder[i].base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_world_to_eye(&this->actor, this->actor.scale.y * 25.0f * 100.0f);
    bigokuta_set_ripple(this, play);
}

s32 bigokuta_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    u8 intensity;
    f32 temp_f0;
    s32 temp_hi;

    if (limbIndex == 15) {
        if (this->actionFunc == mode_stop || this->actionFunc == mode_damage) {
            OPEN_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 1914);

            if (this->actionFunc == mode_stop) {
                temp_hi = this->unk_196 % 12;
                if (temp_hi >= 8) {
                    temp_f0 = (12 - temp_hi) * (M_PI / 8);
                } else {
                    temp_f0 = temp_hi * (M_PI / 16);
                }
            } else {
                temp_hi = this->unk_196 % 6;
                if (temp_hi >= 4) {
                    temp_f0 = (6 - temp_hi) * (M_PI / 4);
                } else {
                    temp_f0 = temp_hi * (M_PI / 8);
                }
            }

            temp_f0 = sinf(temp_f0) * 125.0f + 130.0f;
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetEnvColor(POLY_OPA_DISP++, temp_f0, temp_f0, temp_f0, 255);
            CLOSE_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 1945);
        }
    } else if (limbIndex == 10) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 1950);
        if (this->actionFunc == mode_dead) {
            intensity = this->unk_196 * (255.0f / 38);
        } else {
            intensity = 255;
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, intensity, intensity, intensity, intensity);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 1972);
    } else if (limbIndex == 17 && this->actionFunc == mode_dead) {
        if (this->unk_198 < 5) {
            Matrix_scale((this->unk_198 * 0.2f * 0.25f) + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        } else if (this->unk_198 < 8) {
            temp_f0 = (this->unk_198 - 5) * (1.0f / 12);
            Matrix_scale(1.25f - temp_f0, 1.0f + temp_f0, 1.0f + temp_f0, MTXMODE_APPLY);
        } else {
            temp_f0 = ((this->unk_198 - 8) * 0.125f);
            Matrix_scale(1.0f, 1.25f - temp_f0, 1.25f - temp_f0, MTXMODE_APPLY);
        }
    }
    return false;
}

void En_Bigokuta_actor_draw(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 2017);

    if ((this->actionFunc != mode_dead) || (this->unk_196 != 0) || (this->unk_198 != 0)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        if (this->unk_196 & 1) {
            if ((this->actionFunc == mode_damage && this->unk_196 >= 8) ||
                (this->actionFunc == mode_dead && this->unk_196 >= 10)) {
                f32 rotX = fqrand() * (M_PI * 2.0f);
                f32 rotY = fqrand() * (M_PI * 2.0f);

                Matrix_rotateY(rotY, MTXMODE_APPLY);
                Matrix_rotateX(rotX, MTXMODE_APPLY);
                Matrix_scale(0.78999996f, 1.3f, 0.78999996f, MTXMODE_APPLY);
                Matrix_rotateX(-rotX, MTXMODE_APPLY);
                Matrix_rotateY(-rotY, MTXMODE_APPLY);
            }
        }
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              bigokuta_display1, NULL, this);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (this->actor.scale.y * (255 / 0.033f)));
        POLY_XLU_DISP = Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, NULL, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bigokuta.c", 2076);
}
