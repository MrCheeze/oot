#include "z_en_dh.h"
#include "assets/objects/object_dh/object_dh.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

typedef enum EnDhAction {
    /* 0 */ DH_WAIT,
    /* 1 */ DH_RETREAT,
    /* 2 */ DH_BURROW,
    /* 3 */ DH_WALK,
    /* 4 */ DH_ATTACK,
    /* 5 */ DH_DEATH,
    /* 6 */ DH_DAMAGE
} EnDhAction;

void En_dh_Actor_ct(Actor* thisx, PlayState* play);
void En_dh_Actor_dt(Actor* thisx, PlayState* play);
void En_dh_move(Actor* thisx, PlayState* play);
void En_dh_display(Actor* thisx, PlayState* play);

void En_dh_Actor_mode_wait_init(EnDh* this);
void En_dh_Actor_mode_forward_init(EnDh* this);
void En_dh_Actor_mode_attack_init(EnDh* this);
void En_dh_Actor_mode_div_init(EnDh* this);

void En_dh_Actor_mode_wait(EnDh* this, PlayState* play);
void En_dh_Actor_mode_forward(EnDh* this, PlayState* play);
void En_dh_Actor_mode_lose(EnDh* this, PlayState* play);
void En_dh_Actor_mode_attack(EnDh* this, PlayState* play);
void En_dh_Actor_mode_div(EnDh* this, PlayState* play);
void En_dh_Actor_mode_damage(EnDh* this, PlayState* play);
void En_dh_Actor_mode_down(EnDh* this, PlayState* play);

ActorProfile En_Dh_Profile = {
    /**/ ACTOR_EN_DH,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DH,
    /**/ sizeof(EnDh),
    /**/ En_dh_Actor_ct,
    /**/ En_dh_Actor_dt,
    /**/ En_dh_move,
    /**/ En_dh_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 35, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(2, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
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
    /* Kokiri spin   */ DMG_ENTRY(2, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(4, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_DEAD_HAND, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

void En_dh_actor_set_process(EnDh* this, EnDhActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_dh_Actor_ct(Actor* thisx, PlayState* play) {
    EnDh* this = (EnDh*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_dh_Skel_007E88, &object_dh_Anim_005880, this->jointTable,
                       this->limbRotTable, 16);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 64.0f);
    this->actor.params = ENDH_WAIT_UNDERGROUND;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = LINK_IS_ADULT ? 14 : 20;
    this->alpha = this->unk_258 = 255;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    ClObjPipe_ct(play, &this->collider1);
    ClObjPipe_set5(play, &this->collider1, &this->actor, &OcInfoData);
    ClObjJntSph_ct(play, &this->collider2);
    ClObjJntSph_set5_nzm(play, &this->collider2, &this->actor, &JntSphData, this->elements);
    En_dh_Actor_mode_wait_init(this);
}

void En_dh_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;

    Na_StopMiddleBossBgm();
    ClObjPipe_dt(play, &this->collider1);
    ClObjJntSph_dt_nzf(play, &this->collider2);
}

static void dust_ground_setx(PlayState* play, EnDh* this, Vec3f* spawnPos, f32 spread, s32 arg4, f32 accelXZ, f32 scale) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 spreadAngle;
    f32 scaleMod;

    spreadAngle = (fqrand() - 0.5f) * 6.28f;
    pos.y = this->actor.floorHeight;
    pos.x = (sinf_table(spreadAngle) * spread) + spawnPos->x;
    pos.z = (cosf_table(spreadAngle) * spread) + spawnPos->z;
    accel.x = (fqrand() - 0.5f) * accelXZ;
    accel.z = (fqrand() - 0.5f) * accelXZ;
    vel.y += (fqrand() - 0.5f) * 4.0f;
    scaleMod = (fqrand() * 5.0f) + 12.0f;
    Effect_Hahen_ct3(play, &pos, &vel, &accel, arg4, scaleMod * scale, -1, 10, NULL);
}

void En_dh_Actor_mode_wait_init(EnDh* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_dh_Anim_003A8C);
    this->curAction = DH_WAIT;
    this->actor.world.pos.x = rnd_fx(600.0f) + this->actor.home.pos.x;
    this->actor.world.pos.z = rnd_fx(600.0f) + this->actor.home.pos.z;
    this->actor.shape.yOffset = -15000.0f;
    this->dirtWaveSpread = this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    this->dirtWavePhase = this->actionState = this->actor.params = ENDH_WAIT_UNDERGROUND;
    En_dh_actor_set_process(this, En_dh_Actor_mode_wait);
}

void En_dh_Actor_mode_wait(EnDh* this, PlayState* play) {
    if ((s32)this->skelAnime.curFrame == 5) {
        Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
    }
    if (BlastVsMyCheck(play, &this->collider1.base)) {
        this->actor.params = ENDH_START_ATTACK_BOMB;
    }
    if ((this->actor.params >= ENDH_START_ATTACK_GRAB) || (this->actor.params <= ENDH_HANDS_KILLED_4)) {
        switch (this->actionState) {
            case 0:
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
                this->actionState++;
                this->drawDirtWave++;
                Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_HIDE);
                FALLTHROUGH;
            case 1:
                this->dirtWavePhase += 0x3A7;
                add_calc(&this->dirtWaveSpread, 300.0f, 1.0f, 5.0f, 0.0f);
                this->dirtWaveHeight = sin_s(this->dirtWavePhase) * 55.0f;
                this->dirtWaveAlpha = (s16)(sin_s(this->dirtWavePhase) * 255.0f);
                dust_ground_setx(play, this, &this->actor.world.pos, this->dirtWaveSpread, 4, 2.05f, 1.2f);
                if (this->actor.shape.yOffset == 0.0f) {
                    this->drawDirtWave = false;
                    this->actionState++;
                } else if (this->dirtWavePhase > 0x12C0) {
                    this->actor.shape.yOffset += 500.0f;
                }
                break;
            case 2:
                En_dh_Actor_mode_forward_init(this);
                break;
        }
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x7D0, 0);
        Skeleton_Info2_anime_play(&this->skelAnime);
        if (this->actor.params != ENDH_START_ATTACK_BOMB) {
            to_anchor_set(play, &this->actor);
        }
    }
}

void En_dh_Actor_mode_forward_init(EnDh* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_dh_Anim_003A8C, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_dh_Anim_003A8C) - 3.0f, ANIMMODE_LOOP, -6.0f);
    this->curAction = DH_WALK;
    this->timer = 300;
    this->actor.speed = 1.0f;
    En_dh_actor_set_process(this, En_dh_Actor_mode_forward);
}

void En_dh_Actor_mode_forward(EnDh* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (((s32)this->skelAnime.curFrame % 8) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_WALK);
    }
    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_LAUGH);
    }
    if (this->actor.xzDistToPlayer <= 100.0f) {
        this->actor.speed = 0.0f;
        if (Actor_player_direction_check(&this->actor, 60 * 0x10000 / 360)) {
            En_dh_Actor_mode_attack_init(this);
        }
    } else if (--this->timer == 0) {
        En_dh_Actor_mode_div_init(this);
    }
}

void En_dh_Actor_mode_lose_init(EnDh* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &object_dh_Anim_005880, -4.0f);
    this->curAction = DH_RETREAT;
    this->timer = 70;
    this->actor.speed = 1.0f;
    En_dh_actor_set_process(this, En_dh_Actor_mode_lose);
}

void En_dh_Actor_mode_lose(EnDh* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        this->retreat = false;
        En_dh_Actor_mode_div_init(this);
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, (s16)(this->actor.yawTowardsPlayer + 0x8000), 1, 0xBB8, 0);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void En_dh_Actor_mode_attack_init(EnDh* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_dh_Anim_004658, -6.0f);
    this->timer = this->actionState = 0;
    this->curAction = DH_ATTACK;
    this->actor.speed = 0.0f;
    En_dh_actor_set_process(this, En_dh_Actor_mode_attack);
}

void En_dh_Actor_mode_attack(EnDh* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actionState++;
    } else if ((this->actor.xzDistToPlayer > 100.0f) || !Actor_player_direction_check(&this->actor, 60 * 0x10000 / 360)) {
        Skeleton_Info2_init(&this->skelAnime, &object_dh_Anim_004658, -1.0f, this->skelAnime.curFrame, 0.0f, ANIMMODE_ONCE,
                         -4.0f);
        this->actionState = 4;
        this->collider2.base.atFlags = this->collider2.elements[0].base.atElemFlags = AT_NONE; // also ATELEM_NONE
        this->collider2.elements[0].base.atDmgInfo.dmgFlags = this->collider2.elements[0].base.atDmgInfo.damage = 0;
    }
    switch (this->actionState) {
        case 1:
            Skeleton_Info2_init_standard_stop(&this->skelAnime, &object_dh_Anim_001A3C);
            this->actionState++;
            Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_BITE);
            FALLTHROUGH;
        case 0:
            add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            break;
        case 2:
            if (this->skelAnime.curFrame >= 4.0f) {
                this->collider2.base.atFlags = this->collider2.elements[0].base.atElemFlags =
                    AT_ON | AT_TYPE_ENEMY; // also ATELEM_ON | ATELEM_SFX_WOOD
                this->collider2.elements[0].base.atDmgInfo.dmgFlags = DMG_DEFAULT;
                this->collider2.elements[0].base.atDmgInfo.damage = 8;
            }
            if (this->collider2.base.atFlags & AT_BOUNCED) {
                this->collider2.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
                this->collider2.base.atFlags = this->collider2.elements[0].base.atElemFlags =
                    AT_NONE; // also ATELEM_NONE
                this->collider2.elements[0].base.atDmgInfo.dmgFlags =
                    this->collider2.elements[0].base.atDmgInfo.damage = 0;
                this->actionState++;
            } else if (this->collider2.base.atFlags & AT_HIT) {
                this->collider2.base.atFlags &= ~AT_HIT;
                Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.shape.rot.y, 8.0f);
            }
            break;
        case 3:
            if ((this->actor.xzDistToPlayer <= 100.0f) && (Actor_player_direction_check(&this->actor, 60 * 0x10000 / 360))) {
                s32 pad;

                Skeleton_Info2_init(&this->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f,
                                 Si2_anime_end_frame(&object_dh_Anim_004658), ANIMMODE_ONCE, -6.0f);
                this->actionState = 0;
            } else {
                Skeleton_Info2_init(&this->skelAnime, &object_dh_Anim_004658, -1.0f,
                                 Si2_anime_end_frame(&object_dh_Anim_004658), 0.0f, ANIMMODE_ONCE, -4.0f);
                this->actionState++;
                this->collider2.base.atFlags = this->collider2.elements[0].base.atElemFlags =
                    AT_NONE; // also ATELEM_NONE
                this->collider2.elements[0].base.atDmgInfo.dmgFlags =
                    this->collider2.elements[0].base.atDmgInfo.damage = 0;
            }
            break;
        case 5:
            En_dh_Actor_mode_forward_init(this);
            break;
        case 4:
            break;
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void En_dh_Actor_mode_div_init(EnDh* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_dh_Anim_002148, -6.0f);
    this->curAction = DH_BURROW;
    this->dirtWaveSpread = this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->dirtWavePhase = 0;
    this->actionState = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_HIDE);
    En_dh_actor_set_process(this, En_dh_Actor_mode_div);
}

void En_dh_Actor_mode_div(EnDh* this, PlayState* play) {
    switch (this->actionState) {
        case 0:
            this->actionState++;
            this->drawDirtWave++;
            this->collider1.base.atFlags = this->collider1.elem.atElemFlags =
                AT_ON | AT_TYPE_ENEMY; // also ATELEM_ON | ATELEM_SFX_WOOD
            this->collider1.elem.atDmgInfo.dmgFlags = DMG_DEFAULT;
            this->collider1.elem.atDmgInfo.damage = 4;
            FALLTHROUGH;
        case 1:
            this->dirtWavePhase += 0x47E;
            add_calc(&this->dirtWaveSpread, 300.0f, 1.0f, 8.0f, 0.0f);
            this->dirtWaveHeight = sin_s(this->dirtWavePhase) * 55.0f;
            this->dirtWaveAlpha = (s16)(sin_s(this->dirtWavePhase) * 255.0f);
            dust_ground_setx(play, this, &this->actor.world.pos, this->dirtWaveSpread, 4, 2.05f, 1.2f);
            this->collider1.dim.radius = this->dirtWaveSpread * 0.6f;
            if (Skeleton_Info2_anime_play(&this->skelAnime)) {
                this->actionState++;
            }
            break;
        case 2:
            this->drawDirtWave = false;
            this->collider1.dim.radius = 35;
            this->collider1.base.atFlags = this->collider1.elem.atElemFlags = AT_NONE; // Also ATELEM_NONE
            this->collider1.elem.atDmgInfo.dmgFlags = this->collider1.elem.atDmgInfo.damage = 0;
            En_dh_Actor_mode_wait_init(this);
            break;
    }
}

void En_dh_Actor_mode_damage_init(EnDh* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_dh_Anim_003D6C, -6.0f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -1.0f;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_DAMAGE);
    this->curAction = DH_DAMAGE;
    En_dh_actor_set_process(this, En_dh_Actor_mode_damage);
}

void En_dh_Actor_mode_damage(EnDh* this, PlayState* play) {
    if (this->actor.speed < 0.0f) {
        this->actor.speed += 0.15f;
    }
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->retreat) {
            En_dh_Actor_mode_lose_init(this, play);
        } else if ((this->actor.xzDistToPlayer <= 105.0f) && Actor_player_direction_check(&this->actor, 60 * 0x10000 / 360)) {
            f32 frames = Si2_anime_end_frame(&object_dh_Anim_004658);

            En_dh_Actor_mode_attack_init(this);
            Skeleton_Info2_init(&this->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f, frames, ANIMMODE_ONCE, -6.0f);
        } else {
            En_dh_Actor_mode_forward_init(this);
        }
        this->unk_258 = 255;
    }
}

void En_dh_Actor_mode_down_init(EnDh* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &object_dh_Anim_0032BC, -1.0f);
    this->curAction = DH_DEATH;
    this->timer = 300;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Na_StopMiddleBossBgm();
    this->actor.params = ENDH_DEATH;
    Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_DEAD);
    En_dh_actor_set_process(this, En_dh_Actor_mode_down);
}

void En_dh_Actor_mode_down(EnDh* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime) || (this->timer != 300)) {
        if (this->timer == 300) {
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_dh_Anim_00375C);
        }
        this->timer--;
        if (this->timer < 150) {
            if (this->alpha != 0) {
                this->actor.scale.y -= 0.000075f;
                this->actor.shape.shadowAlpha = this->alpha -= 5;
            } else {
                Actor_delete(&this->actor);
                return;
            }
        }
    } else {
        if (((s32)this->skelAnime.curFrame == 53) || ((s32)this->skelAnime.curFrame == 56) ||
            ((s32)this->skelAnime.curFrame == 61)) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
        if ((s32)this->skelAnime.curFrame == 61) {
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        }
    }
}

void En_dh_damage_proc(EnDh* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 lastHealth;

    if ((this->collider2.base.acFlags & AC_HIT) && !this->retreat) {
        this->collider2.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) && (this->actor.colChkInfo.damageEffect != 6)) {
            this->collider2.base.atFlags = this->collider2.elements[0].base.atElemFlags = AT_NONE; // also ATELEM_NONE
            this->collider2.elements[0].base.atDmgInfo.dmgFlags = this->collider2.elements[0].base.atDmgInfo.damage = 0;
            if (player->unk_844 != 0) {
                this->unk_258 = player->unk_845;
            }
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            lastHealth = this->actor.colChkInfo.health;
            if (hp_down(&this->actor) == 0) {
                En_dh_Actor_mode_down_init(this);
                Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x90);
            } else {
                if (((lastHealth >= 15) && (this->actor.colChkInfo.health < 15)) ||
                    ((lastHealth >= 9) && (this->actor.colChkInfo.health < 9)) ||
                    ((lastHealth >= 3) && (this->actor.colChkInfo.health < 3))) {

                    this->retreat++;
                }
                En_dh_Actor_mode_damage_init(this);
            }
        }
    }
}

void En_dh_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad40;

    En_dh_damage_proc(this, play);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 45.0f, 45.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    this->actor.focus.pos = this->headPos;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider1);
    if (this->actor.colChkInfo.health > 0) {
        if (this->curAction == DH_WAIT) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider1.base);
        } else {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
        }
        if (((this->curAction != DH_DAMAGE) && (this->actor.shape.yOffset == 0.0f)) ||
            ((player->unk_844 != 0) && (player->unk_845 != this->unk_258))) {

            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider2.base);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider2.base);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider1.base);
        }
    } else {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider2.base);
    }
}

void en_dh_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Vec3f headOffset = { 2000.0f, 1000.0f, 0.0f };
    EnDh* this = (EnDh*)thisx;

    if (limbIndex == 13) {
        Matrix_Position(&headOffset, &this->headPos);
        Matrix_push();
        Matrix_translate(headOffset.x, headOffset.y, headOffset.z, MTXMODE_APPLY);
        CollisionCheck_Uty_convJntSphL2G(1, &this->collider2);
        Matrix_pull();
    }
}

void En_dh_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dh.c", 1099);
    if (this->alpha == 255) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        POLY_OPA_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               NULL, en_dh_display2, &this->actor, POLY_OPA_DISP);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x08, &Actor_change_render_mode[0]);
        POLY_XLU_DISP = Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
    }
    if (this->drawDirtWave) {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 85, 55, 0, 130);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->state.frames * -3) % 0x80, 0, 0x20,
                                    0x40, 1, (play->state.frames * -10) % 0x80, (play->state.frames * -20) % 0x100,
                                    0x20, 0x40));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, this->dirtWaveAlpha);

        Matrix_translate(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
        Matrix_scale(this->dirtWaveSpread * 0.01f, this->dirtWaveHeight * 0.01f, this->dirtWaveSpread * 0.01f,
                     MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_dh.c", 1160);
        gSPDisplayList(POLY_XLU_DISP++, object_dh_DL_007FC0);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dh.c", 1166);
}
