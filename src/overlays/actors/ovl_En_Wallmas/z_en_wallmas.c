/*
 * File: z_en_wallmas
 * Overlay: En_Wallmas
 * Description: Wallmaster (Ceiling monster)
 */

#include "z_en_wallmas.h"
#include "assets/objects/object_wallmaster/object_wallmaster.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define TIMER_SCALE ((f32)OS_CLOCK_RATE / 10000000000)

#define DAMAGE_EFFECT_BURN 2
#define DAMAGE_EFFECT_STUN_WHITE 4
#define DAMAGE_EFFECT_STUN_BLUE 1

void En_Wallmas_actor_ct(Actor* thisx, PlayState* play);
void En_Wallmas_actor_dt(Actor* thisx, PlayState* play);
void En_Wallmas_actor_move(Actor* thisx, PlayState* play);
void En_Wallmas_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnWallmas* this, PlayState* play);
static void mode_dead_init(EnWallmas* this);
static void mode_wait(EnWallmas* this, PlayState* play);
static void mode_drop(EnWallmas* this, PlayState* play);
void mode_miss(EnWallmas* this, PlayState* play);
void mode_escape_start(EnWallmas* this, PlayState* play);
void mode_escape_end(EnWallmas* this, PlayState* play);
static void mode_jump(EnWallmas* this, PlayState* play);
static void mode_damage(EnWallmas* this, PlayState* play);
static void mode_damage_after(EnWallmas* this, PlayState* play);
static void mode_down(EnWallmas* this, PlayState* play);
static void mode_catch(EnWallmas* this, PlayState* play);
static void mode_dead(EnWallmas* this, PlayState* play);
void mode_sw_wait(EnWallmas* this, PlayState* play);
static void mode_stop(EnWallmas* this, PlayState* play);
static void mode_escape(EnWallmas* this, PlayState* play);

ActorProfile En_Wallmas_Profile = {
    /**/ ACTOR_EN_WALLMAS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WALLMASTER,
    /**/ sizeof(EnWallmas),
    /**/ En_Wallmas_actor_ct,
    /**/ En_Wallmas_actor_dt,
    /**/ En_Wallmas_actor_move,
    /**/ En_Wallmas_actor_draw,
};

static ColliderCylinderInit WallmasAcOcPipeData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0 } },
};

static CollisionCheckInfoInit WallmasStatusData = { 4, 30, 40, 150 };

static DamageTable WallmasBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(4, 0x4),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x4),
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
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_WALLMASTER, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1500, ICHAIN_STOP),
};

void En_Wallmas_actor_ct(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    ValueSet_process(thisx, value_init);
    Shape_Info_init(&thisx->shape, 0, NULL, 0.5f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gWallmasterSkel, &gWallmasterWaitAnim, this->jointTable,
                       this->morphTable, 25);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &WallmasAcOcPipeData);
    CollisionCheck_Status_set2(&thisx->colChkInfo, &WallmasBtlData, &WallmasStatusData);
    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params = PARAMS_GET_U(thisx->params, 0, 8);

    if (thisx->params == WMT_FLAG) {
        if (Actor_Environment_sw_Check(play, this->switchFlag) != 0) {
            Actor_delete(thisx);
            return;
        }

        mode_dead_init(this);
    } else if (thisx->params == WMT_PROXIMITY) {
        mode_dead_init(this);
    } else {
        mode_wait_init(this, play);
    }
}

void En_Wallmas_actor_dt(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait_init(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->timer = 0x82;
    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y = player->actor.world.pos.y;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.draw = En_Wallmas_actor_draw;
    this->actionFunc = mode_wait;
}

static void mode_drop_init(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    AnimationHeader* objSegChangee = &gWallmasterLungeAnim;

    Skeleton_Info2_init(&this->skelAnime, objSegChangee, 0.0f, 20.0f, Si2_anime_end_frame(&gWallmasterLungeAnim),
                     ANIMMODE_ONCE, 0.0f);

    this->yTarget = player->actor.world.pos.y;
    this->actor.world.pos.y = player->actor.world.pos.y + 300.0f;
    this->actor.world.rot.y = player->actor.shape.rot.y + 0x8000;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = mode_drop;
}

void mode_miss_init(EnWallmas* this, PlayState* play) {
    AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    Skeleton_Info2_init(&this->skelAnime, objSegChangee, 1.0f, 41.0f, Si2_anime_end_frame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    _dust_ground_set(play, &this->actor, &this->actor.world.pos, 15.0f, 6, 20.0f, 300, 100, true);
    Actor_SE_set(&this->actor, NA_SE_EN_FALL_LAND);
    this->actionFunc = mode_miss;
}

void mode_escape_start_init(EnWallmas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterStandUpAnim);
    this->actionFunc = mode_escape_start;
}

static void mode_escape_init(EnWallmas* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gWallmasterWalkAnim, 3.0f);
    this->actionFunc = mode_escape;
    this->actor.speed = 3.0f;
}

void mode_escape_end_init(EnWallmas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterStopWalkAnim);
    this->actionFunc = mode_escape_end;
    this->actor.speed = 0.0f;
}
static void mode_jump_init(EnWallmas* this) {
    AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    this->timer = 0;
    this->actor.speed = 0.0f;

    Skeleton_Info2_init(&this->skelAnime, objSegChangee, 3.0f, 0.0f, Si2_anime_end_frame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    this->actionFunc = mode_jump;
}

static void mode_damage_init(EnWallmas* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gWallmasterDamageAnim, -3.0f);
    if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT)) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, this->collider.base.ac) + 0x8000;
    }

    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->actionFunc = mode_damage;
    this->actor.speed = 5.0f;
    this->actor.velocity.y = 10.0f;
}

static void mode_damage_after_init(EnWallmas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterRecoverFromDamageAnim);
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = mode_damage_after;
}

static void mode_down_init(EnWallmas* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;

    _Effect_SS_Db_ct(play, &this->actor.world.pos, &zero_vec, &zero_vec, 250, -10, 255, 255, 255, 255, 0, 0, 255, 1,
                         9, true);

    Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xC0);
    this->actionFunc = mode_down;
}

static void mode_catch_init(EnWallmas* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gWallmasterHoverAnim, -5.0f);
    this->timer = -0x1E;
    this->actionFunc = mode_catch;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;

    this->yTarget = this->actor.yDistToPlayer;
    player_demo_mode_set2(play, &this->actor, PLAYER_CSACTION_37);
    makeOnepointDemo(play, 9500, 9999, &this->actor, CAM_ID_MAIN);
}

static void mode_dead_init(EnWallmas* this) {
    this->timer = 0;
    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->actor.params == WMT_PROXIMITY) {
        this->actionFunc = mode_dead;
    } else {
        this->actionFunc = mode_sw_wait;
    }
}

static void mode_stop_init(EnWallmas* this) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 1.5f, 0, 20.0f, ANIMMODE_ONCE, -3.0f);

    this->actor.speed = 0.0f;
    if (this->actor.colChkInfo.damageEffect == 4) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 255, COLORFILTER_BUFFLAG_OPA, 80);
    } else {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }

    this->timer = 0x50;
    this->actionFunc = mode_stop;
}

static void mode_wait(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;

    this->actor.world.pos = *playerPos;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.floorPoly = player->actor.floorPoly;

    if (this->timer != 0) {
        this->timer--;
    }

    if ((player->stateFlags1 & PLAYER_STATE1_20) || (player->stateFlags1 & PLAYER_STATE1_27) ||
        !(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        ((this->actor.params == 1) && (320.0f < search_position_distanceXZ(&this->actor.home.pos, playerPos)))) {
        Nai_StopFx(NA_SE_EN_FALL_AIM);
        this->timer = 0x82;
    }

    if (this->timer == 0x50) {
        Actor_SE_set(&this->actor, NA_SE_EN_FALL_AIM);
    }

    if (this->timer == 0) {
        mode_drop_init(this, play);
    }
}

static void mode_drop(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!player_demo_check(play) && !(player->stateFlags2 & PLAYER_STATE2_4) && (player->invincibilityTimer >= 0) &&
        (this->actor.xzDistToPlayer < 30.0f) && (this->actor.yDistToPlayer < -5.0f) &&
        (-(f32)(player->cylinder.dim.height + 10) < this->actor.yDistToPlayer)) {
        mode_catch_init(this, play);
    }
}

void mode_miss(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_escape_start_init(this);
    }
}

void mode_escape_start(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_escape_init(this);
    }

    chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0xB6);
}

static void mode_escape(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_escape_end_init(this);
    }

    chase_angle(&this->actor.world.rot.y, (s16)((s32)this->actor.yawTowardsPlayer + 0x8000), 0xB6);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f) ||
        Skeleton_Info_frame_check(&this->skelAnime, 24.0f) || Skeleton_Info_frame_check(&this->skelAnime, 36.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FALL_WALK);
    }
}

void mode_escape_end(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_jump_init(this);
    }
}

static void mode_jump(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->skelAnime.curFrame > 20.0f) {
        this->timer += 9;
        this->actor.world.pos.y += 30.0f;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 20.0f) != 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_FALL_UP);
    }

    if (this->actor.yDistToPlayer < -900.0f) {
        if (this->actor.params == WMT_FLAG) {
            Actor_delete(&this->actor);
            return;
        }

        if (this->actor.params == WMT_TIMER ||
            search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < 200.0f) {
            mode_wait_init(this, play);
        } else {
            mode_dead_init(this);
        }
    }
}

static void mode_damage(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this, play);
        } else {
            mode_damage_after_init(this);
        }
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 13.0f) != 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    chase_f(&this->actor.speed, 0.0f, 0.2f);
}

static void mode_damage_after(EnWallmas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_jump_init(this);
    }
}

static void mode_down(EnWallmas* this, PlayState* play) {
    if (chase_f(&this->actor.scale.x, 0.0f, 0.0015) != 0) {
        Actor_set_scale(&this->actor, 0.01f);
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xC0);
        Actor_delete(&this->actor);
    }
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = this->actor.scale.x;
}

static void mode_catch(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Skeleton_Info_frame_check(&this->skelAnime, 1.0f) != 0) {
        if (!LINK_IS_ADULT) {
            //! @bug: This is an unsafe cast, although the sound effect will still play
            player_SE_set((Player*)&this->actor, NA_SE_VO_LI_DAMAGE_S_KID);
        } else {
            //! @bug: This is an unsafe cast, although the sound effect will still play
            player_SE_set((Player*)&this->actor, NA_SE_VO_LI_DAMAGE_S);
        }

        Actor_SE_set(&this->actor, NA_SE_EN_FALL_CATCH);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        player->actor.world.pos.x = this->actor.world.pos.x;
        player->actor.world.pos.z = this->actor.world.pos.z;

        if (this->timer < 0) {
            this->actor.world.pos.y += 2.0f;
        } else {
            this->actor.world.pos.y += 10.0f;
        }

        if (!LINK_IS_ADULT) {
            player->actor.world.pos.y = this->actor.world.pos.y - 30.0f;
        } else {
            player->actor.world.pos.y = this->actor.world.pos.y - 50.0f;
        }

        if (this->timer == -0x1E) {
            if (!LINK_IS_ADULT) {
                //! @bug: This is an unsafe cast, although the sound effect will still play
                player_SE_set((Player*)&this->actor, NA_SE_VO_LI_TAKEN_AWAY_KID);
            } else {
                //! @bug: This is an unsafe cast, although the sound effect will still play
                player_SE_set((Player*)&this->actor, NA_SE_VO_LI_TAKEN_AWAY);
            }
        }
        if (this->timer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_FALL_UP);
        }

        this->timer += 2;
    } else {
        chase_f(&this->actor.world.pos.y, player->actor.world.pos.y + (!LINK_IS_ADULT ? 30.0f : 50.0f), 5.0f);
    }

    chase_f(&this->actor.world.pos.x, player->actor.world.pos.x, 3.0f);
    chase_f(&this->actor.world.pos.z, player->actor.world.pos.z, 3.0f);

    if (this->timer == 0x1E) {
        Na_StartSystemSe_F(NA_SE_OC_ABYSS);
        Game_play_down_restart_top(play);
    }
}

static void mode_dead(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < 200.0f) {
        mode_wait_init(this, play);
    }
}

void mode_sw_wait(EnWallmas* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag) != 0) {
        mode_wait_init(this, play);
        this->timer = 0x51;
    }
}

static void mode_stop(EnWallmas* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this, play);
        } else {
            mode_jump_init(this);
        }
    }
}

void En_Wallmas_damage_proc(EnWallmas* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_FALL_DEAD);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            } else {
                if (this->actor.colChkInfo.damage != 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_FALL_DAMAGE);
                }
            }

            if ((this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_WHITE) ||
                (this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_BLUE)) {
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else {
                if (this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_BURN) {
                    Effect_Fcircle_ct(play, &this->actor, &this->actor.world.pos, 40, 40);
                }

                mode_damage_init(this);
            }
        }
    }
}

void En_Wallmas_actor_move(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;
    char pad[4];

    En_Wallmas_damage_proc(this, play);
    this->actionFunc(this, play);

    if ((this->actionFunc == mode_wait) || (this->actionFunc == mode_dead) ||
        (this->actionFunc == mode_catch) || (this->actionFunc == mode_sw_wait)) {
        return;
    }

    if ((this->actionFunc != mode_jump) && (this->actionFunc != mode_catch)) {
        Actor_position_moveF(&this->actor);
    }

    if (this->actionFunc != mode_drop) {
        Actor_BGcheck2(play, &this->actor, 20.0f, 25.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
    } else if (this->actor.world.pos.y <= this->yTarget) {
        this->actor.world.pos.y = this->yTarget;
        this->actor.velocity.y = 0.0f;
        mode_miss_init(this, play);
    }

    if ((this->actionFunc != mode_down) && (this->actionFunc != mode_drop)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

        if ((this->actionFunc != mode_damage) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->actor.freezeTimer == 0)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    Actor_world_to_eye(&this->actor, 25.0f);

    if (this->actionFunc == mode_damage) {
        return;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void wallmas_shadow_draw(EnWallmas* this, PlayState* play) {
    s32 pad;
    f32 xzScale;
    MtxF mf;

    if ((this->actor.floorPoly == NULL) || ((this->timer >= 0x51) && (this->actionFunc != mode_stop))) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx, "../z_en_wallmas.c", 1386);

    _texture_decal_shadow(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    T_Polygon_Ground_Matrix(this->actor.floorPoly, this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z,
                  &mf);
    Matrix_mult(&mf, MTXMODE_NEW);

    if ((this->actionFunc != mode_wait) && (this->actionFunc != mode_jump) &&
        (this->actionFunc != mode_catch) && (this->actionFunc != mode_sw_wait)) {
        xzScale = this->actor.scale.x * 50.0f;
    } else {
        xzScale = ((0x50 - this->timer) >= 0x51 ? 0x50 : (0x50 - this->timer)) * TIMER_SCALE;
    }

    Matrix_scale(xzScale, 1.0f, xzScale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_wallmas.c", 1421);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_wallmas.c", 1426);
}

s32 en_wallmas_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnWallmas* this = (EnWallmas*)thisx;

    if (limbIndex == 1) {
        if (this->actionFunc != mode_catch) {
            pos->z -= 1600.0f;
        } else {
            pos->z -= ((1600.0f * (this->skelAnime.endFrame - this->skelAnime.curFrame)) / this->skelAnime.endFrame);
        }
    }
    return false;
}

void en_wallmas_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 2) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_wallmas.c", 1478);

        Matrix_push();
        Matrix_translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_rotateY(DEG_TO_RAD(60), MTXMODE_APPLY);
        Matrix_rotateZ(DEG_TO_RAD(15), MTXMODE_APPLY);
        Matrix_scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_wallmas.c", 1489);
        gSPDisplayList(POLY_OPA_DISP++, gWallmasterFingerDL);

        Matrix_pull();

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_wallmas.c", 1495);
    }
}

void En_Wallmas_actor_draw(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    if (this->actionFunc != mode_wait) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              en_wallmas_display1, en_wallmas_display2, this);
    }

    wallmas_shadow_draw(this, play);
}
