/*
 * File: z_en_dha.c
 * Overlay: ovl_En_Dha
 * Description: Dead Hand's Hand
 */

#include "z_en_dha.h"
#include "versions.h"
#include "overlays/actors/ovl_En_Dh/z_en_dh.h"
#include "assets/objects/object_dh/object_dh.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_dha_Actor_ct(Actor* thisx, PlayState* play);
void En_dha_Actor_dt(Actor* thisx, PlayState* play);
void En_dha_move(Actor* thisx, PlayState* play);
void En_dha_display(Actor* thisx, PlayState* play);

void En_dha_Actor_mode_wait_init(EnDha* this);
void En_dha_Actor_mode_wait(EnDha* this, PlayState* play);
void En_dha_Actor_mode_damage_init(EnDha* this);
void En_dha_Actor_mode_damage(EnDha* this, PlayState* play);
void En_dha_Actor_mode_down_init(EnDha* this);
void En_dha_Actor_mode_down(EnDha* this, PlayState* play);
void En_dha_damage_proc(EnDha* this, PlayState* play);

ActorProfile En_Dha_Profile = {
    /**/ ACTOR_EN_DHA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DH,
    /**/ sizeof(EnDha),
    /**/ En_dha_Actor_ct,
    /**/ En_dha_Actor_dt,
    /**/ En_dha_move,
    /**/ En_dha_display,
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

static ColliderJntSphElementInit JntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 2, { { 3200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 3, { { 1200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 4, { { 2700, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 5, { { 1200, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    5,
    JntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_DEAD_HANDS_HAND, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void En_dha_actor_set_process(EnDha* this, EnDhaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_dha_Actor_ct(Actor* thisx, PlayState* play) {
    EnDha* this = (EnDha*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_dh_Skel_000BD8, &object_dh_Anim_0015B0, this->jointTable,
                       this->morphTable, 4);
    Shape_Info_init(&this->actor.shape, 0, Actor_shadow_foot, 90.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 8;
    this->limbAngleX[0] = -0x4000;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &JntSphData, this->colliderItem);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    En_dha_Actor_mode_wait_init(this);
}

void En_dha_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void En_dha_Actor_mode_wait_init(EnDha* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_dh_Anim_0015B0);
    this->unk_1C0 = 0;
    this->actionTimer = ((fqrand() * 10.0f) + 5.0f);
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.home.rot.z = 1;
    En_dha_actor_set_process(this, En_dha_Actor_mode_wait);
}

void En_dha_Actor_mode_wait(EnDha* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f armPosMultiplier1 = { 0.0f, 0.0f, 55.0f };
    Vec3f armPosMultiplier2 = { 0.0f, 0.0f, -54.0f };
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 pad2;
    Vec3f playerPos = player->actor.world.pos;
    Vec3s angle;
    s16 yaw;

    playerPos.x += sin_s(player->actor.shape.rot.y) * -5.0f;
    playerPos.z += cos_s(player->actor.shape.rot.y) * -5.0f;

    if (!LINK_IS_ADULT) {
        playerPos.y += 38.0f;
    } else {
        playerPos.y += 56.0f;
    }

    if (this->actor.xzDistToPlayer <= 100.0f) {
        this->handAngle.y = this->handAngle.x = this->limbAngleY = 0;

        if (search_position_distance(&playerPos, &this->handPos[0]) <= 12.0f) {
            if (this->unk_1CC == 0) {
                if (play->grabPlayer(play, player)) {
                    this->timer = 0;
                    this->unk_1CC++;

                    if (this->actor.parent != NULL) {
                        this->actor.parent->params = ENDH_START_ATTACK_GRAB;
                    }

                    Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            } else {
                this->timer += 0x1194;
                this->limbAngleY = sin_s(this->timer) * 1820.0f;

                if (!(player->stateFlags2 & PLAYER_STATE2_7)) {
                    this->unk_1CC = 0;
                    En_dha_Actor_mode_damage_init(this);
                    return;
                }

                if (this->timer < -0x6E6B) {
                    Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            }

            search_position_angleXY(&this->handPos[1], &playerPos, &this->handAngle, 0);
            this->handAngle.y -= this->actor.shape.rot.y + this->limbAngleY;
            this->handAngle.x -= this->actor.shape.rot.x + this->limbAngleX[0] + this->limbAngleX[1];
        } else {
#if OOT_VERSION < NTSC_1_1
            // Empty
#elif OOT_VERSION < PAL_1_0
            if ((player->stateFlags2 & PLAYER_STATE2_7) && (&this->actor == player->actor.parent)) {
                player->stateFlags2 &= ~PLAYER_STATE2_7;
                player->actor.parent = NULL;
            }
#else
            if ((player->stateFlags2 & PLAYER_STATE2_7) && (&this->actor == player->actor.parent)) {
                player->stateFlags2 &= ~PLAYER_STATE2_7;
                player->actor.parent = NULL;
                player->av2.actionVar2 = 200;
            }
#endif

            if (this->actor.home.rot.z != 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_HAND_AT);
                this->actor.home.rot.z = 0;
            }
        }

        this->actor.shape.rot.y = search_position_angleY(&this->actor.world.pos, &playerPos);

        add_calc(&this->handPos[0].x, playerPos.x, 1.0f, 16.0f, 0.0f);
        add_calc(&this->handPos[0].y, playerPos.y, 1.0f, 16.0f, 0.0f);
        add_calc(&this->handPos[0].z, playerPos.z, 1.0f, 16.0f, 0.0f);

        search_position_angleXY(&this->armPos, &this->handPos[0], &angle, 0);
        Matrix_translate(this->handPos[0].x, this->handPos[0].y, this->handPos[0].z, MTXMODE_NEW);
        Matrix_rotateXYZ(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_Position(&armPosMultiplier2, &this->armPos);
        Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        search_position_angleXY(&this->actor.world.pos, &this->armPos, &angle, 0);
        Matrix_rotateXYZ(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_Position(&armPosMultiplier1, &this->armPos);
        this->limbAngleX[0] = search_position_angleX(&this->actor.world.pos, &this->armPos);
        yaw = search_position_angleY(&this->actor.world.pos, &this->armPos) - this->actor.shape.rot.y;

        if (ABS(yaw) >= 0x4000) {
            this->limbAngleX[0] = -0x8000 - this->limbAngleX[0];
        }

        this->limbAngleX[1] = (search_position_angleX(&this->armPos, &this->handPos[0]) - this->limbAngleX[0]);

        if (this->limbAngleX[1] < 0) {
            this->limbAngleX[0] += this->limbAngleX[1] * 2;
            this->limbAngleX[1] *= -2;
        }
    } else {
        if ((player->stateFlags2 & PLAYER_STATE2_7) && (&this->actor == player->actor.parent)) {
            player->stateFlags2 &= ~PLAYER_STATE2_7;
            player->actor.parent = NULL;
#if OOT_VERSION >= PAL_1_0
            player->av2.actionVar2 = 200;
#endif
        }

        this->actor.home.rot.z = 1;
        add_calc_short_angle2(&this->limbAngleX[1], 0, 1, 0x3E8, 0);
        add_calc_short_angle2(&this->limbAngleX[0], -0x4000, 1, 0x3E8, 0);
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

void En_dha_Actor_mode_damage_init(EnDha* this) {
    this->actionTimer = 15;
    En_dha_actor_set_process(this, En_dha_Actor_mode_damage);
}

void En_dha_Actor_mode_damage(EnDha* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & PLAYER_STATE2_7) && (&this->actor == player->actor.parent)) {
        player->stateFlags2 &= ~PLAYER_STATE2_7;
        player->actor.parent = NULL;
#if OOT_VERSION >= PAL_1_0
        player->av2.actionVar2 = 200;
#endif
    }

    add_calc_short_angle2(&this->limbAngleX[1], 0, 1, 2000, 0);
    add_calc_short_angle2(&this->limbAngleY, 0, 1, 600, 0);
    add_calc_short_angle2(&this->limbAngleX[0], -0x4000, 1, 2000, 0);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actionTimer--;

    if (this->actionTimer == 0) {
        En_dha_Actor_mode_wait_init(this);
    }
}

void En_dha_Actor_mode_down_init(EnDha* this) {
    this->unk_1C0 = 8;
    this->actionTimer = 300;

    if (this->actor.parent != NULL) {
        if (this->actor.parent->params != ENDH_DEATH) {
            Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_HAND_DEAD);
        }
        if (this->actor.parent->params <= ENDH_WAIT_UNDERGROUND) {
            this->actor.parent->params--;
        }
    }

    En_dha_actor_set_process(this, En_dha_Actor_mode_down);
}

void En_dha_Actor_mode_down(EnDha* this, PlayState* play) {
    s16 angle;
    Vec3f vec;
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & PLAYER_STATE2_7) && (&this->actor == player->actor.parent)) {
        player->stateFlags2 &= ~PLAYER_STATE2_7;
        player->actor.parent = NULL;
#if OOT_VERSION >= PAL_1_0
        player->av2.actionVar2 = 200;
#endif
    }

    add_calc_short_angle2(&this->limbAngleX[1], 0, 1, 0x7D0, 0);
    angle = add_calc_short_angle2(&this->limbAngleX[0], -0x4000, 1, 0x7D0, 0);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (angle == 0) {
        vec = this->actor.world.pos;

        if (this->actionTimer != 0) {
            if (-12000.0f < this->actor.shape.yOffset) {
                this->actor.shape.yOffset -= 1000.0f;
                dust_fly_set2(play, &vec, 7.0f, 1, 0x5A, 0x14, 1);
            } else {
                this->actionTimer--;

                if ((this->actor.parent != NULL) && (this->actor.parent->params == ENDH_DEATH)) {
                    Actor_delete(&this->actor);
                }
            }
        } else {
            this->actor.shape.yOffset += 500.0f;
            dust_fly_set2(play, &vec, 7.0f, 1, 0x5A, 0x14, 1);

            if (this->actor.shape.yOffset == 0.0f) {
                En_dha_Actor_mode_wait_init(this);
            }
        }
    }
}

void En_dha_damage_proc(EnDha* this, PlayState* play) {
    if (!((this->unk_1C0 >= 8) || !(this->collider.base.acFlags & AC_HIT))) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.colChkInfo.damageEffect == 0 || this->actor.colChkInfo.damageEffect == 6) {
            return;
        } else {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            if (hp_down(&this->actor) == 0) {
                En_dha_Actor_mode_down_init(this);
                this->actor.colChkInfo.health = 8;
                Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xE0);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_DEADHAND_DAMAGE);
                this->unk_1C0 = 9;
                En_dha_Actor_mode_damage_init(this);
            }
        }
    }

    if ((this->actor.parent != NULL) && (this->actor.parent->params == ENDH_DEATH)) {
        En_dha_Actor_mode_down_init(this);
    }
}

void En_dha_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    if (this->actor.parent == NULL) {
        this->actor.parent = ActorSearch(play, &this->actor, ACTOR_EN_DH, ACTORCAT_ENEMY, 10000.0f);
    }

    En_dha_damage_proc(this, play);
    this->actionFunc(this, play);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 en_dha_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDha* this = (EnDha*)thisx;

    if (limbIndex == 1) {
        rot->y = -(s16)(this->limbAngleX[0] + 0x4000);
        rot->z += this->limbAngleY;
    } else if (limbIndex == 2) {
        rot->z = this->limbAngleX[1];
        rot->y -= this->limbAngleY;
    } else if (limbIndex == 3) {
        rot->y = -this->handAngle.y;
        rot->z = -this->handAngle.x;
    }

    return false;
}

void en_dha_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f handVec = { 1100.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnDha* this = (EnDha*)thisx;

    switch (limbIndex) {
        case 1:
            CollisionCheck_Uty_convJntSphL2G(2, &this->collider);
            CollisionCheck_Uty_convJntSphL2G(3, &this->collider);
            break;
        case 2:
            CollisionCheck_Uty_convJntSphL2G(4, &this->collider);
            CollisionCheck_Uty_convJntSphL2G(5, &this->collider);
            Matrix_Position(&zeroVec, &this->armPos);
            break;
        case 3:
            CollisionCheck_Uty_convJntSphL2G(1, &this->collider);
            Matrix_Position(&handVec, &this->handPos[0]);
            Matrix_Position(&zeroVec, &this->handPos[1]);
            break;
    }
}

void En_dha_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_dha_display1, en_dha_display2, this);
}
