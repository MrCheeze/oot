#include "z_en_crow.h"
#include "assets/objects/object_crow/object_crow.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void En_Crow_actor_ct(Actor* thisx, PlayState* play);
void En_Crow_actor_dt(Actor* thisx, PlayState* play);
void En_Crow_actor_move(Actor* thisx, PlayState* play);
void En_Crow_actor_draw(Actor* thisx, PlayState* play);

static void mode_fly_init(EnCrow* this);
static void mode_fly(EnCrow* this, PlayState* play);
static void mode_scale_up(EnCrow* this, PlayState* play);
static void mode_attack(EnCrow* this, PlayState* play);
void mode_scale(EnCrow* this, PlayState* play);
static void mode_escape(EnCrow* this, PlayState* play);
static void mode_dead(EnCrow* this, PlayState* play);

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

ActorProfile En_Crow_Profile = {
    /**/ ACTOR_EN_CROW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_CROW,
    /**/ sizeof(EnCrow),
    /**/ En_Crow_actor_ct,
    /**/ En_Crow_actor_dt,
    /**/ En_Crow_actor_move,
    /**/ En_Crow_actor_draw,
};

static ColliderJntSphElementInit CrowAllJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit CrowAllJntSphData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    CrowAllJntSphElemData,
};

static CollisionCheckInfoInit CrowStatusData = { 1, 15, 30, 30 };

static DamageTable CrowBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
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
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
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

static u32 dead_cnt = 0;

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_GUAY, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -200, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

static Vec3f tmp_pos = { 2500.0f, 0.0f, 0.0f };

void En_Crow_actor_ct(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable, 9);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &CrowAllJntSphData, this->colliderItems);
    this->collider.elements[0].dim.worldSphere.radius = CrowAllJntSphData.elements[0].dim.modelSphere.radius;
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &CrowBtlData, &CrowStatusData);
    Shape_Info_init(&this->actor.shape, 2000.0f, Actor_shadow_circle, 20.0f);
    dead_cnt = 0;
    mode_fly_init(this);
}

void En_Crow_actor_dt(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

// Setup Action functions

static void mode_fly_init(EnCrow* this) {
    this->timer = 100;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = mode_fly;
    this->skelAnime.playSpeed = 1.0f;
}

static void mode_attack_init(EnCrow* this) {
    this->timer = 300;
    this->actor.speed = 4.0f;
    this->skelAnime.playSpeed = 2.0f;
    this->actionFunc = mode_attack;
}

static void mode_dead_init(EnCrow* this, PlayState* play) {
    s32 i;
    f32 scale;
    Vec3f iceParticlePos;

    this->actor.speed *= cos_s(this->actor.world.rot.x);
    this->actor.velocity.y = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    scale = this->actor.scale.x * 100.0f;
    this->actor.world.pos.y += 20.0f * scale;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.shape.yOffset = 0.0f;
    this->actor.lockOnArrowOffset = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_KAICHO_DEAD);

    if (this->actor.colChkInfo.damageEffect == 3) { // Ice arrows
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
        for (i = 0; i < 8; i++) {
            iceParticlePos.x = ((i & 1 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.x;
            iceParticlePos.y = ((i & 2 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.y;
            iceParticlePos.z = ((i & 4 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.z;
            Effect_En_Ice_ct0(play, &this->actor, &iceParticlePos, 150, 150, 150, 250, 235, 245, 255,
                                           ((fqrand() * 0.15f) + 0.85f) * scale);
        }
    } else if (this->actor.colChkInfo.damageEffect == 2) { // Fire arrows and Din's Fire
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);

        for (i = 0; i < 4; i++) {
            Effect_En_Fire_ct(play, &this->actor, &this->actor.world.pos, 50.0f * scale, 0, 0, i);
        }
    } else {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    }

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.speed = 0.0f;
    }

    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

    this->actionFunc = mode_dead;
}

void mode_scale_init(EnCrow* this) {
    this->actor.colorFilterTimer = 0;
    this->actionFunc = mode_scale;
}

static void mode_escape_init(EnCrow* this) {
    this->timer = 100;
    this->actor.speed = 3.5f;
    this->aimRotX = -0x1000;
    this->aimRotY = this->actor.yawTowardsPlayer + 0x8000;
    this->skelAnime.playSpeed = 2.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 5);
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = mode_escape;
}

static void mode_scale_up_init(EnCrow* this) {
    if (dead_cnt == 10) {
        this->actor.params = 1;
        dead_cnt = 0;
        this->collider.elements[0].dim.worldSphere.radius =
            CrowAllJntSphData.elements[0].dim.modelSphere.radius * 0.03f * 100.0f;
    } else {
        this->actor.params = 0;
        this->collider.elements[0].dim.worldSphere.radius = CrowAllJntSphData.elements[0].dim.modelSphere.radius;
    }

    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGuayFlyAnim);
    xyz_t_move(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->timer = 300;
    this->actor.shape.yOffset = 2000;
    this->actor.lockOnArrowOffset = 2000.0f;
    this->actor.draw = NULL;
    this->actionFunc = mode_scale_up;
}

// Action functions

static void mode_fly(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 skelanimeUpdated;
    s16 var;

    Skeleton_Info2_anime_play(&this->skelAnime);
    skelanimeUpdated = Skeleton_Info_frame_check(&this->skelAnime, 0.0f);
    this->actor.speed = (fqrand() * 1.5f) + 3.0f;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->aimRotY = this->actor.wallYaw;
    } else if (Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) > 300.0f) {
        this->aimRotY = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
    }

    if ((add_calc_short_angle2(&this->actor.shape.rot.y, this->aimRotY, 5, 0x300, 0x10) == 0) && skelanimeUpdated &&
        (fqrand() < 0.1f)) {
        var = Actor_search_position_angleY(&this->actor, &this->actor.home.pos) - this->actor.shape.rot.y;
        if (var > 0) {
            this->aimRotY += 0x1000 + (0x1000 * fqrand());
        } else {
            this->aimRotY -= 0x1000 + (0x1000 * fqrand());
        }
        Actor_SE_set(&this->actor, NA_SE_EN_KAICHO_CRY);
    }

    if (this->actor.depthInWater > -40.0f) {
        this->aimRotX = -0x1000;
    } else if (this->actor.world.pos.y < (this->actor.home.pos.y - 50.0f)) {
        this->aimRotX = -0x800 - (fqrand() * 0x800);
    } else if (this->actor.world.pos.y > (this->actor.home.pos.y + 50.0f)) {
        this->aimRotX = 0x800 + (fqrand() * 0x800);
    }

    if ((add_calc_short_angle2(&this->actor.shape.rot.x, this->aimRotX, 10, 0x100, 8) == 0) && skelanimeUpdated &&
        (fqrand() < 0.1f)) {
        if (this->actor.home.pos.y < this->actor.world.pos.y) {
            this->aimRotX -= (0x400 * fqrand()) + 0x400;
        } else {
            this->aimRotX += (0x400 * fqrand()) + 0x400;
        }
        this->aimRotX = CLAMP(this->aimRotX, -0x1000, 0x1000);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        chase_angle(&this->actor.shape.rot.x, -0x100, 0x400);
    }

    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 300.0f) && !(player->stateFlags1 & PLAYER_STATE1_23) &&
        (this->actor.depthInWater < -40.0f) && (mask_check(play) != PLAYER_MASK_SKULL)) {
        mode_attack_init(this);
    }
}

static void mode_attack(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 facingPlayer;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    facingPlayer = Actor_player_direction_check(&this->actor, 0x2800);

    if (facingPlayer) {
        Vec3f pos;
        s16 target;

        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 20.0f;
        pos.z = player->actor.world.pos.z;
        target = Actor_search_position_angleX(&this->actor, &pos);
        if (target > 0x3000) {
            target = 0x3000;
        }
        adds(&this->actor.shape.rot.x, target, 2, 0x400);
    } else {
        adds(&this->actor.shape.rot.x, -0x1000, 2, 0x100);
    }

    if (facingPlayer || (this->actor.xzDistToPlayer > 80.0f)) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
    }

    if ((this->timer == 0) || (mask_check(play) == PLAYER_MASK_SKULL) || (this->collider.base.atFlags & AT_HIT) ||
        (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL)) ||
        (player->stateFlags1 & PLAYER_STATE1_23) || (this->actor.depthInWater > -40.0f)) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            Actor_SE_set(&this->actor, NA_SE_EN_KAICHO_ATTACK);
        }

        mode_fly_init(this);
    }
}

static void mode_dead(EnCrow* this, PlayState* play) {
    chase_f(&this->actor.speed, 0.0f, 0.5f);
    this->actor.colorFilterTimer = 40;

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->actor.colorFilterParams & 0x4000) {
            chase_angle(&this->actor.shape.rot.x, 0x4000, 0x200);
            this->actor.shape.rot.z += 0x1780;
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            _Effect_SS_Db_ct(play, &this->actor.world.pos, &zero_vec, &zero_vec,
                                 this->actor.scale.x * 10000.0f, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
            mode_scale_init(this);
        }
    }
}

void mode_scale(EnCrow* this, PlayState* play) {
    f32 step;

    if (this->actor.params != 0) {
        step = 0.006f;
    } else {
        step = 0.002f;
    }

    if (chase_f(&this->actor.scale.x, 0.0f, step)) {
        if (this->actor.params == 0) {
            dead_cnt++;
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0);
        } else {
            Item_set0(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
        }
        mode_scale_up_init(this);
    }

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
}

static void mode_escape(EnCrow* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->aimRotY = this->actor.wallYaw;
    } else {
        this->aimRotY = this->actor.yawTowardsPlayer + 0x8000;
    }

    adds(&this->actor.shape.rot.y, this->aimRotY, 3, 0xC00);
    adds(&this->actor.shape.rot.x, this->aimRotX, 5, 0x100);

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        mode_fly_init(this);
    }
}

static void mode_scale_up(EnCrow* this, PlayState* play) {
    f32 target;

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        this->actor.draw = En_Crow_actor_draw;
        if (this->actor.params != 0) {
            target = 0.03f;
        } else {
            target = 0.01f;
        }
        if (chase_f(&this->actor.scale.x, target, target * 0.1f)) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.colChkInfo.health = 1;
            mode_fly_init(this);
        }
        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    }
}

void En_Crow_damage_proc(EnCrow* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elements[0].base, true);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->actor.colChkInfo.damageEffect == 1) { // Deku Nuts
                mode_escape_init(this);
            } else {
                hp_down(&this->actor);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_info_finish(play, &this->actor);
                mode_dead_init(this, play);
            }
        }
    }
}

void En_Crow_actor_move(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;
    f32 pad;
    f32 height;
    f32 scale;

    En_Crow_damage_proc(this, play);
    this->actionFunc(this, play);
    scale = this->actor.scale.x * 100.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.x = -this->actor.shape.rot.x;

    if (this->actionFunc != mode_scale_up) {
        if (this->actor.colChkInfo.health != 0) {
            height = 20.0f * scale;
            Actor_position_moveF_XY(&this->actor);
        } else {
            height = 0.0f;
            Actor_position_moveF(&this->actor);
        }
        Actor_BGcheck2(play, &this->actor, 12.0f * scale, 25.0f * scale, 50.0f * scale,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    } else {
        height = 0.0f;
    }

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y + height;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    if (this->actionFunc == mode_attack) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionFunc != mode_scale_up) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }

    Actor_world_to_eye(&this->actor, height);

    if (this->actor.colChkInfo.health != 0 && Skeleton_Info_frame_check(&this->skelAnime, 3.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_KAICHO_FLUTTER);
    }
}

s32 en_crow_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCrow* this = (EnCrow*)thisx;

    if (this->actor.colChkInfo.health != 0) {
        if (limbIndex == 7) {
            rot->y += 0xC00 * sinf(this->skelAnime.curFrame * (M_PI / 4));
        } else if (limbIndex == 8) {
            rot->y += 0x1400 * sinf((this->skelAnime.curFrame + 2.5f) * (M_PI / 4));
        }
    }
    return false;
}

void en_crow_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnCrow* this = (EnCrow*)thisx;
    Vec3f* vec;

    if (limbIndex == 2) {
        Matrix_Position(&tmp_pos, &this->bodyPartsPos[0]);
        this->bodyPartsPos[0].y -= 20.0f;
    } else if ((limbIndex == 4) || (limbIndex == 6) || (limbIndex == 8)) {
        vec = &this->bodyPartsPos[(limbIndex >> 1) - 1];
        Matrix_Position(&zero_vec, vec);
        vec->y -= 20.0f;
    }
}

void En_Crow_actor_draw(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_crow_display1, en_crow_display2, this);
}
