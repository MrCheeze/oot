/*
 * File: z_en_dodojr.c
 * Overlay: ovl_En_Dodojr
 * Description: Baby Dodongo
 */

#include "z_en_dodojr.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/object_dodojr/object_dodojr.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Dodojr_Actor_ct(Actor* thisx, PlayState* play);
void En_Dodojr_Actor_dt(Actor* thisx, PlayState* play);
void En_Dodojr_move(Actor* thisx, PlayState* play);
void En_Dodojr_display(Actor* thisx, PlayState* play);

void dj_wait_underground(EnDodojr* this, PlayState* play);
void dj_dead(EnDodojr* this, PlayState* play);
void dj_appear(EnDodojr* this, PlayState* play);
void dj_move(EnDodojr* this, PlayState* play);
void dj_freeze_jump(EnDodojr* this, PlayState* play);
void dj_attack(EnDodojr* this, PlayState* play);
void dj_freeze(EnDodojr* this, PlayState* play);
void dj_bom_eat_after(EnDodojr* this, PlayState* play);
void dj_bom_eat_jump(EnDodojr* this, PlayState* play);
void dj_bom_eat_dead(EnDodojr* this, PlayState* play);
void dj_dead_jump(EnDodojr* this, PlayState* play);
void dj_hide(EnDodojr* this, PlayState* play);
void dj_explosion(EnDodojr* this, PlayState* play);
void dj_hit_now(EnDodojr* this, PlayState* play);
void dj_bom_eat(EnDodojr* this, PlayState* play);

ActorProfile En_Dodojr_Profile = {
    /**/ ACTOR_EN_DODOJR,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DODOJR,
    /**/ sizeof(EnDodojr),
    /**/ En_Dodojr_Actor_ct,
    /**/ En_Dodojr_Actor_dt,
    /**/ En_Dodojr_move,
    /**/ En_Dodojr_display,
};

static ColliderCylinderInit EnDjOcInfoData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFC5FFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 20, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 DjStatusData = { 1, 2, 25, 25, MASS_IMMOVABLE };

void En_Dodojr_Actor_ct(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 18.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_dodojr_Skel_0020E0, &object_dodojr_Anim_0009D4, this->jointTable,
                   this->morphTable, 15);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnDjOcInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(4), &DjStatusData);

    this->actor.naviEnemyId = NAVI_ENEMY_BABY_DODONGO;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Actor_set_scale(&this->actor, 0.02f);

    this->actionFunc = dj_wait_underground;
}

void En_Dodojr_Actor_dt(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void set_effect_in_stomach(EnDodojr* this) {
    Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 8);
}

static void set_dust_effect(EnDodojr* this, PlayState* play, s32 count) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((fqrand() - 0.5f) * 65536.0f);
    s32 i;

    pos.y = this->dustPos.y;

    for (i = count; i >= 0; i--, angle += (s16)(0x10000 / count)) {
        accel.x = (fqrand() - 0.5f) * 4.0f;
        accel.z = (fqrand() - 0.5f) * 4.0f;

        pos.x = (sin_s(angle) * 22.0f) + this->dustPos.x;
        pos.z = (cos_s(angle) * 22.0f) + this->dustPos.z;

        Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &prim, &env, 120, 40, 10);
    }
}

void set_move_dust_effect(EnDodojr* this, PlayState* play, Vec3f* arg2) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((fqrand() - 0.5f) * 65536.0f);

    pos.y = this->actor.floorHeight;

    accel.x = (fqrand() - 0.5f) * 2;
    accel.z = (fqrand() - 0.5f) * 2;

    pos.x = (sin_s(angle) * 11.0f) + arg2->x;
    pos.z = (cos_s(angle) * 11.0f) + arg2->z;

    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &prim, &env, 100, 60, 8);
}

static s32 bound(EnDodojr* this, PlayState* play) {
    if (this->actor.velocity.y >= 0.0f) {
        return false;
    }

    if (this->counter == 0) {
        return false;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        this->dustPos = this->actor.world.pos;
        set_dust_effect(this, play, 10);
        this->actor.velocity.y = 10.0f / (4 - this->counter);
        this->counter--;

        if (this->counter == 0) {
            this->actor.velocity.y = 0.0f;
            return true;
        }
    }

    return false;
}

void dj_move_ct(EnDodojr* this) {
    f32 lastFrame = Si2_anime_end_frame(&object_dodojr_Anim_000860);

    Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -10.0f);
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 2.6f;
    this->actor.gravity = -0.8f;
}

void dj_freeze_jump_ct(EnDodojr* this) {
    f32 lastFrame = Si2_anime_end_frame(&object_dodojr_Anim_0004A0);

    Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_0004A0, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -10.0f);
    this->actor.speed = 0.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.gravity = -0.8f;

    if (this->counter == 0) {
        this->counter = 3;
        this->actor.velocity.y = 10.0f;
    }
}

void dj_freeze_ct(EnDodojr* this) {
    f32 lastFrame = Si2_anime_end_frame(&object_dodojr_Anim_0005F0);

    Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_0005F0, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, 0.0f);
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -0.8f;
}

void dj_attack_ct(EnDodojr* this) {
    f32 lastFrame = Si2_anime_end_frame(&object_dodojr_Anim_000724);

    Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_000724, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.gravity = -0.8f;
    this->counter = 3;
    this->actor.velocity.y = 10.0f;
}

void dj_hide_ct(EnDodojr* this) {
    this->actor.shape.shadowDraw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.home.pos = this->actor.world.pos;
    this->actor.speed = 0.0f;
    this->actor.gravity = -0.8f;
    this->timer = 30;
    this->dustPos = this->actor.world.pos;
}

void dj_bom_eat_ct(EnDodojr* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_000724, 1.0f, 8.0f, 12.0f, ANIMMODE_ONCE, 0.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_EAT);
    this->actor.speed = 0.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.gravity = -0.8f;
}

s32 dj_bomb_search(EnDodojr* this, PlayState* play) {
    Actor* bomb;
    Vec3f maxBombRange = { 99999.0f, 99999.0f, 99999.0f };
    s32 foundBomb = false;
    f32 xDist;
    f32 yDist;
    f32 zDist;

    bomb = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    this->bomb = NULL;

    while (bomb != NULL) {
        if ((bomb->params != 0) || (bomb->parent != NULL) || (bomb->update == NULL)) {
            bomb = bomb->next;
            continue;
        }

        if (bomb->id != ACTOR_EN_BOM) {
            bomb = bomb->next;
            continue;
        }

        xDist = bomb->world.pos.x - this->actor.world.pos.x;
        yDist = bomb->world.pos.y - this->actor.world.pos.y;
        zDist = bomb->world.pos.z - this->actor.world.pos.z;

        if ((fabsf(xDist) >= fabsf(maxBombRange.x)) || (fabsf(yDist) >= fabsf(maxBombRange.y)) ||
            (fabsf(zDist) >= fabsf(maxBombRange.z))) {
            bomb = bomb->next;
            continue;
        }

        this->bomb = bomb;
        maxBombRange = bomb->world.pos;
        foundBomb = true;
        bomb = bomb->next;
    }

    return foundBomb;
}

s32 bom_eat_check(EnDodojr* this) {
    if (this->bomb == NULL) {
        return false;
    } else if (this->bomb->parent != NULL) {
        return false;
    } else if (search_position_distance(&this->actor.world.pos, &this->bomb->world.pos) > 30.0f) {
        return false;
    } else {
        this->bomb->parent = &this->actor;
        return true;
    }
}

void dj_search_move_sub(EnDodojr* this, PlayState* play) {
    f32 angles[] = { 0.0f, 210.0f, 60.0f, 270.0f, 120.0f, 330.0f, 180.0f, 30.0f, 240.0f, 90.0f, 300.0f, 150.0f };
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f crawlTargetPos;
    s16 angleIndex;

    if ((this->bomb == NULL) || (this->bomb->update == NULL) ||
        ((this->bomb != NULL) && (this->bomb->parent != NULL))) {
        dj_bomb_search(this, play);
    }

    if (this->bomb != NULL) {
        crawlTargetPos = this->bomb->world.pos;
    } else {
        crawlTargetPos = player->actor.world.pos;
    }

    if (search_position_distance(&this->actor.world.pos, &crawlTargetPos) > 80.0f) {
        angleIndex =
            (s16)(this->actor.home.pos.x + this->actor.home.pos.y + this->actor.home.pos.z + play->state.frames / 30) %
            12;
        angleIndex = ABS(angleIndex);
        crawlTargetPos.x += 80.0f * sinf(angles[angleIndex]);
        crawlTargetPos.z += 80.0f * cosf(angles[angleIndex]);
    }

    add_calc_short_angle2(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &crawlTargetPos), 10, 1000, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

s32 dj_attack_check(EnDodojr* this) {
    if (this->actor.xzDistToPlayer > 40.0f) {
        return false;
    } else {
        return true;
    }
}

void dj_dead_ct(EnDodojr* this) {
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    dj_freeze_jump_ct(this);
    this->actionFunc = dj_dead_jump;
}

static s32 hitcheck(EnDodojr* this, PlayState* play) {
    if ((this->actionFunc == dj_bom_eat_after) || (this->actionFunc == dj_bom_eat_jump) ||
        (this->actionFunc == dj_bom_eat_dead) || (this->actionFunc == dj_hide) ||
        (this->actionFunc == dj_dead_jump) || (this->actionFunc == dj_explosion) ||
        (this->actionFunc == dj_dead)) {
        return false;
    }

    if (play->actorCtx.unk_02 != 0) {
        if (this->actionFunc != dj_wait_underground) {
            if (this->actionFunc == dj_appear) {
                this->actor.shape.shadowDraw = Actor_shadow_circle;
            }

            dj_dead_ct(this);
        }
        return false;
    }

    if (!(this->collider.base.acFlags & AC_HIT)) {
        return false;
    } else {
        this->collider.base.acFlags &= ~AC_HIT;

        if ((this->actionFunc == dj_wait_underground) || (this->actionFunc == dj_appear)) {
            this->actor.shape.shadowDraw = Actor_shadow_circle;
        }

        if ((this->actor.colChkInfo.damageEffect == 0) && (this->actor.colChkInfo.damage != 0)) {
            Actor_info_finish(play, &this->actor);
            this->freezeFrameTimer = 2;
            this->actionFunc = dj_hit_now;
            return true;
        }

        if ((this->actor.colChkInfo.damageEffect == 1) && (this->actionFunc != dj_freeze) &&
            (this->actionFunc != dj_freeze_jump)) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            this->stunTimer = 120;
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 120);
            dj_freeze_jump_ct(this);
            this->actionFunc = dj_freeze_jump;
        }

        return false;
    }
}

void dj_collision_check(EnDodojr* this, PlayState* play) {
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if ((this->actionFunc != dj_wait_underground) && (this->actionFunc != dj_dead)) {
        if ((this->actionFunc == dj_appear) || (this->actionFunc == dj_move) ||
            (this->actionFunc == dj_attack)) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((this->actionFunc == dj_appear) || (this->actionFunc == dj_move) ||
            (this->actionFunc == dj_freeze_jump) || (this->actionFunc == dj_freeze) ||
            (this->actionFunc == dj_attack)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void dj_wait_underground(EnDodojr* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&object_dodojr_Anim_000860);
    Player* player = GET_PLAYER(play);
    f32 dist;

    if (!(this->actor.xzDistToPlayer >= 320.0f)) {
        dist = this->actor.world.pos.y - player->actor.world.pos.y;

        if (!(dist >= 40.0f)) {
            Skeleton_Info2_init(&this->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                             -10.0f);
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_UP);
            this->actor.world.pos.y -= 60.0f;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.world.rot.x -= 0x4000;
            this->actor.shape.rot.x = this->actor.world.rot.x;
            this->dustPos = this->actor.world.pos;
            //! @bug floorHeight is always 0 at this point, so the dust is consistently drawn at y=0
            this->dustPos.y = this->actor.floorHeight;
            this->actionFunc = dj_appear;
        }
    }
}

void dj_appear(EnDodojr* this, PlayState* play) {
    f32 sp2C;

    add_calc_short_angle2(&this->actor.shape.rot.x, 0, 4, 0x3E8, 0x64);
    sp2C = this->actor.shape.rot.x;
    sp2C /= 16384.0f;
    this->actor.world.pos.y = this->actor.home.pos.y + (60.0f * sp2C);
    set_dust_effect(this, play, 3);

    if (sp2C == 0.0f) {
        this->actor.shape.shadowDraw = Actor_shadow_circle;
        this->actor.world.rot.x = this->actor.shape.rot.x;
        this->actor.speed = 2.6f;
        this->actionFunc = dj_move;
    }
}

void dj_move(EnDodojr* this, PlayState* play) {
    Actor_position_speed_set(&this->actor);
    set_move_dust_effect(this, play, &this->actor.world.pos);

    if (DECR(this->crawlSfxTimer) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_MOVE);
        this->crawlSfxTimer = 5;
    }

    if (bom_eat_check(this)) {
        dj_bom_eat_ct(this);
        this->actionFunc = dj_bom_eat;
        return;
    }

    dj_search_move_sub(this, play);

    if (dj_attack_check(this)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_CRY);
        dj_attack_ct(this);
        this->actionFunc = dj_attack;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_DOWN);
        dj_hide_ct(this);
        this->actionFunc = dj_hide;
    }
}

void dj_bom_eat(EnDodojr* this, PlayState* play) {
    EnBom* bomb;

    if (((s16)this->skelAnime.curFrame - 8) < 4) {
        bomb = (EnBom*)this->bomb;
        bomb->timer++;
        this->bomb->world.pos = this->headPos;
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DRINK);
        Actor_delete(this->bomb);
        this->timer = 24;
        this->counter = 0;
        this->actionFunc = dj_bom_eat_after;
    }
}

void dj_bom_eat_after(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer) == 0) {
        set_effect_in_stomach(this);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        dj_freeze_jump_ct(this);
        this->actionFunc = dj_bom_eat_jump;
    }
}

void dj_bom_eat_jump(EnDodojr* this, PlayState* play) {
    // Scale up briefly to expand from the swallowed bomb exploding.
    this->rootScale = 1.2f;
    this->rootScale *= ((f32)this->actor.colorFilterTimer / 8);
    Actor_position_speed_set(&this->actor);

    if (bound(this, play)) {
        this->timer = 60;
        dj_freeze_ct(this);
        this->counter = 7;
        this->actionFunc = dj_bom_eat_dead;
    }
}

void dj_bom_eat_dead(EnDodojr* this, PlayState* play) {
    dj_explosion(this, play);
}

void dj_freeze_jump(EnDodojr* this, PlayState* play) {
    Actor_position_speed_set(&this->actor);

    if (bound(this, play)) {
        dj_freeze_ct(this);
        this->actionFunc = dj_freeze;
    }

    add_calc_short_angle2(&this->actor.shape.rot.y, 0, 4, 1000, 10);
    this->actor.world.rot.x = this->actor.shape.rot.x;
    this->actor.colorFilterTimer = this->stunTimer;
}

void dj_freeze(EnDodojr* this, PlayState* play) {
    if (DECR(this->stunTimer) != 0) {
        if (this->stunTimer < 30) {
            // Shake back and forth to indicate recovering from stun.
            if ((this->stunTimer & 1) != 0) {
                this->actor.world.pos.x += 1.5f;
                this->actor.world.pos.z += 1.5f;
            } else {
                this->actor.world.pos.x -= 1.5f;
                this->actor.world.pos.z -= 1.5f;
            }

            return;
        }
    } else {
        dj_move_ct(this);
        this->actionFunc = dj_move;
    }
}

void dj_attack(EnDodojr* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    Actor_position_speed_set(&this->actor);

    if (bound(this, play)) {
        dj_move_ct(this);
        this->actionFunc = dj_move;
    }
}

void dj_hide(EnDodojr* this, PlayState* play) {
    f32 burrowStep;

    add_calc_short_angle2(&this->actor.shape.rot.x, 0x4000, 4, 1000, 100);

    if (DECR(this->timer) != 0) {
        burrowStep = (30 - this->timer) / 30.0f;
        this->actor.world.pos.y = this->actor.home.pos.y - (60.0f * burrowStep);
    } else {
        Actor_delete(&this->actor);
    }

    set_dust_effect(this, play, 3);
}

void dj_dead_jump(EnDodojr* this, PlayState* play) {
    Actor_position_speed_set(&this->actor);
    add_calc_short_angle2(&this->actor.shape.rot.y, 0, 4, 1000, 10);
    this->actor.world.rot.x = this->actor.shape.rot.x;

    if (bound(this, play)) {
        this->timer = 60;
        dj_freeze_ct(this);
        this->counter = 7;
        this->actionFunc = dj_explosion;
    }
}

void dj_explosion(EnDodojr* this, PlayState* play) {
    EnBom* bomb;

    if (this->counter != 0) {
        if (this->actor.colorFilterTimer == 0) {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, this->counter);
            this->counter--;
        }
    } else {
        bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        this->timer = 8;
        this->actionFunc = dj_dead;
    }
}

void dj_dead(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer) == 0) {
        Item_Set_Std(play, NULL, &this->actor.world.pos, 0x40);
        Actor_delete(&this->actor);
    }
}

void dj_hit_now(EnDodojr* this, PlayState* play) {
    if (DECR(this->freezeFrameTimer) == 0) {
        dj_dead_ct(this);
    }
}

void En_Dodojr_move(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_position_moveF(&this->actor);
    hitcheck(this, play);

    if (this->actionFunc != dj_wait_underground) {
        Actor_BGcheck2(play, &this->actor, this->collider.dim.radius, this->collider.dim.height, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    }

    this->actionFunc(this, play);
    Actor_world_to_eye(&this->actor, 10.0f);
    dj_collision_check(this, play);
}

s32 en_ddj_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDodojr* this = (EnDodojr*)thisx;
    Vec3f D_809F7F64 = { 480.0f, 620.0f, 0.0f };

    if (limbIndex == 1) {
        Matrix_scale((this->rootScale * 0.5f) + 1.0f, this->rootScale + 1.0f, (this->rootScale * 0.5f) + 1.0f,
                     MTXMODE_APPLY);
    }

    if (limbIndex == 4) {
        Matrix_Position(&D_809F7F64, &this->headPos);
    }

    return false;
}

void en_ddj_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void En_Dodojr_display(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    if ((this->actionFunc != dj_wait_underground) && (this->actionFunc != dj_dead)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_ddj_display1,
                          en_ddj_display2, &this->actor);
    }
}
