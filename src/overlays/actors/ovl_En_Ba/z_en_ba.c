/*
 * File: z_en_ba.c
 * Overlay: ovl_En_Ba
 * Description: Tentacle from inside Lord Jabu-Jabu
 */

#include "z_en_ba.h"
#include "assets/objects/object_bxa/object_bxa.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_ba_actor_ct(Actor* thisx, PlayState* play);
void En_ba_actor_dt(Actor* thisx, PlayState* play);
void En_ba_actor_move(Actor* thisx, PlayState* play);
void En_ba_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnBa* this);
static void mode_break_init(EnBa* this);
static void mode_wait(EnBa* this, PlayState* play);
static void mode_break(EnBa* this, PlayState* play);
static void mode_attack(EnBa* this, PlayState* play);
static void mode_dam(EnBa* this, PlayState* play);
static void mode_dead(EnBa* this, PlayState* play);
static void mode_attack_init(EnBa* this);

ActorProfile En_Ba_Profile = {
    /**/ ACTOR_EN_BA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BXA,
    /**/ sizeof(EnBa),
    /**/ En_ba_actor_ct,
    /**/ En_ba_actor_dt,
    /**/ En_ba_actor_move,
    /**/ En_ba_actor_draw,
};

static Vec3f arm_reng = { 0.0f, 0.0f, 32.0f };

static ColliderJntSphElementInit JntSphElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000010, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 8, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 13, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    2,
    JntSphElemData,
};

void En_ba_actor_set_process(EnBa* this, EnBaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static Vec3f D_809B80E4 = { 0.01f, 0.01f, 0.01f };

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_PARASITIC_TENTACLE, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2500, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void En_ba_actor_ct(Actor* thisx, PlayState* play) {
    EnBa* this = (EnBa*)thisx;
    Vec3f sp38 = D_809B80E4;
    s32 pad;
    s16 i;

    ValueSet_process(&this->actor, value_init);
    this->actor.world.pos.y = this->actor.home.pos.y + 100.0f;
    for (i = 13; i >= 0; i--) {
        this->unk_200[i] = sp38;
        this->unk_2A8[i].x = -0x4000;
        this->unk_158[i] = this->actor.world.pos;
        this->unk_158[i].y = this->actor.world.pos.y - (i + 1) * 32.0f;
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_4;
    this->upperParams = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;

    if (this->actor.params < EN_BA_DEAD_BLOB) {
        if (Actor_Environment_sw_Check(play, this->upperParams)) {
            Actor_delete(&this->actor);
            return;
        }
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 48.0f);
        Actor_set_scale(&this->actor, 0.01f);
        mode_wait_init(this);
        this->actor.colChkInfo.health = 4;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        ClObjJntSph_ct(play, &this->collider);
        ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &JntSphData, this->colliderItems);
    } else {
        Actor_set_scale(&this->actor, 0.021f);
        mode_break_init(this);
    }
}

void En_ba_actor_dt(Actor* thisx, PlayState* play) {
    EnBa* this = (EnBa*)thisx;
    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mode_wait_init(EnBa* this) {
    this->unk_14C = 2;
    this->unk_31C = 1500;
    this->actor.speed = 10.0f;
    En_ba_actor_set_process(this, mode_wait);
}

static void mode_wait(EnBa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 pad;
    Vec3s sp5C;

    if ((this->actor.colChkInfo.mass == MASS_IMMOVABLE) && (this->actor.xzDistToPlayer > 175.0f)) {
        add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 330.0f, 1.0f, 7.0f, 0.0f);
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 100.0f, 1.0f, 10.0f, 0.0f);
    }
    this->unk_2FC = this->actor.world.pos;
    if (play->gameplayFrames % 16 == 0) {
        this->unk_308.z += rnd_fx(180.0f);
        this->unk_314 += rnd_fx(180.0f);
        this->unk_308.x = sinf_table(this->unk_308.z) * 80.0f;
        this->unk_308.y = cosf_table(this->unk_314) * 80.0f;
    }
    this->unk_2FC.y -= 448.0f;
    this->unk_2FC.x += this->unk_308.x;
    this->unk_2FC.z += this->unk_308.y;
    chase_pos(&this->unk_2FC, &this->unk_158[13], 1.0f, this->actor.speed, 0.0f, 0.0f);
    for (i = 12; i >= 0; i--) {
        search_position_angleXY(&this->unk_158[i + 1], &this->unk_158[i], &sp5C, 0);
        Matrix_translate(this->unk_158[i + 1].x, this->unk_158[i + 1].y, this->unk_158[i + 1].z, MTXMODE_NEW);
        Matrix_rotateXYZ(sp5C.x, sp5C.y, 0, MTXMODE_APPLY);
        Matrix_Position(&arm_reng, &this->unk_158[i]);
    }
    search_position_angleXY(&this->unk_158[0], &this->unk_2FC, &sp5C, 0);
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->unk_2A8[0].y, 3, this->unk_31C, 182);
    add_calc_short_angle2(&this->actor.shape.rot.x, this->unk_2A8[0].x, 3, this->unk_31C, 182);
    Matrix_rotateXYZ(this->actor.shape.rot.x - 0x8000, this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Position(&arm_reng, &this->unk_158[0]);
    this->unk_2A8[13].y = sp5C.y;
    this->unk_2A8[13].x = sp5C.x + 0x8000;

    for (i = 0; i < 13; i++) {
        Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
        add_calc_short_angle2(&this->unk_2A8[i].y, this->unk_2A8[i + 1].y, 3, this->unk_31C, 182);
        add_calc_short_angle2(&this->unk_2A8[i].x, this->unk_2A8[i + 1].x, 3, this->unk_31C, 182);
        Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_Position(&arm_reng, &this->unk_158[i + 1]);
    }
    this->unk_2A8[13].x = this->unk_2A8[12].x;
    this->unk_2A8[13].y = this->unk_2A8[12].y;
    if (!(player->stateFlags1 & PLAYER_STATE1_26) && (this->actor.xzDistToPlayer <= 175.0f) &&
        (this->actor.world.pos.y == this->actor.home.pos.y + 100.0f)) {
        mode_attack_init(this);
    }
}

static void mode_break_init(EnBa* this) {
    this->unk_14C = 0;
    this->actor.speed = rnd_fx(8.0f);
    this->actor.world.rot.y = rnd_fx(65535.0f);
    this->unk_318 = 20;
    this->actor.gravity = -2.0f;
    En_ba_actor_set_process(this, mode_break);
}

/**
 * Action function of the pink fleshy blobs that spawn and fall to the floor when a tentacle dies
 */
static void mode_break(EnBa* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.scale.y -= 0.001f;
        this->actor.scale.x += 0.0005f;
        this->actor.scale.z += 0.0005f;
        this->unk_318--;
        if (this->unk_318 == 0) {
            Actor_delete(&this->actor);
        }
    } else {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 30.0f, 28.0f, 80.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    }
}

static void mode_attack_init(EnBa* this) {
    this->unk_14C = 3;
    this->unk_318 = 20;
    this->unk_31A = 0;
    this->unk_31C = 1500;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.speed = 20.0f;
    En_ba_actor_set_process(this, mode_attack);
}

static void mode_attack(EnBa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp;
    s16 i;
    Vec3s sp58;
    s16 phi_fp;

    add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 60.0f, 1.0f, 10.0f, 0.0f);

    if ((this->actor.xzDistToPlayer <= 175.0f) || (this->unk_31A != 0)) {
        if (this->unk_318 == 20) {
            Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_HAND_UP);
            this->unk_31C = 1500;
        }
        if (this->unk_318 != 0) {
            this->unk_31A = 10;
            this->unk_318--;
            if (this->unk_318 >= 11) {
                this->unk_2FC = player->actor.world.pos;
                this->unk_2FC.y += 30.0f;
                phi_fp = this->actor.yawTowardsPlayer;
            } else {
                phi_fp = search_position_angleY(&this->actor.world.pos, &this->unk_2FC);
            }
            add_calc_short_angle2(&this->unk_31C, 1500, 1, 30, 0);
            search_position_angleXY(&this->actor.world.pos, &this->unk_158[0], &sp58, 0);
            add_calc_short_angle2(&this->actor.shape.rot.y, sp58.y, 1, this->unk_31C, 0);
            add_calc_short_angle2(&this->actor.shape.rot.x, (sp58.x + 0x8000), 1, this->unk_31C, 0);
            Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
            Matrix_rotateXYZ((this->actor.shape.rot.x - 0x8000), this->actor.shape.rot.y, 0, MTXMODE_APPLY);
            Matrix_Position(&arm_reng, &this->unk_158[0]);

            for (i = 0; i < 13; i++) {
                add_calc_short_angle2(&this->unk_2A8[i].x, (i * 1200) - 0x4000, 1, this->unk_31C, 0);
                add_calc_short_angle2(&this->unk_2A8[i].y, phi_fp, 1, this->unk_31C, 0);
                Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
                Matrix_rotateXYZ((this->unk_2A8[i].x - 0x8000), this->unk_2A8[i].y, 0, MTXMODE_APPLY);
                Matrix_Position(&arm_reng, &this->unk_158[i + 1]);
            }
        } else {
            if (this->unk_31A == 10) {
                Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_HAND_DOWN);
            }
            if (this->unk_31A != 0) {
                this->unk_31C = 8000;
                this->actor.speed = 30.0f;
                phi_fp = search_position_angleY(&this->actor.world.pos, &this->unk_2FC);
                temp = search_position_angleX(&this->actor.world.pos, &this->unk_158[0]) + 0x8000;
                add_calc_short_angle2(&this->actor.shape.rot.y, phi_fp, 1, this->unk_31C, 0);
                add_calc_short_angle2(&this->actor.shape.rot.x, temp, 1, this->unk_31C, 0);
                Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                 MTXMODE_NEW);
                Matrix_rotateXYZ(this->actor.shape.rot.x - 0x8000, this->actor.shape.rot.y, 0, MTXMODE_APPLY);
                Matrix_Position(&arm_reng, this->unk_158);

                for (i = 0; i < 13; i++) {
                    temp = -cos_s(this->unk_31A * 0xCCC) * (i * 1200);
                    add_calc_short_angle2(&this->unk_2A8[i].x, temp - 0x4000, 1, this->unk_31C, 0);
                    add_calc_short_angle2(&this->unk_2A8[i].y, phi_fp, 1, this->unk_31C, 0);
                    Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
                    Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
                    Matrix_Position(&arm_reng, &this->unk_158[i + 1]);
                }
                this->unk_31A--;
            } else if ((this->actor.xzDistToPlayer > 175.0f) || (player->stateFlags1 & PLAYER_STATE1_26)) {
                mode_wait_init(this);
            } else {
                mode_attack_init(this);
                this->unk_318 = 27;
                this->unk_31C = 750;
            }
        }
        this->unk_2A8[13].x = this->unk_2A8[12].x;
        this->unk_2A8[13].y = this->unk_2A8[12].y;

        //! @bug This code being located here gives multiple opportunities for the current action to change
        //! before damage handling can be done.
        //! By, for example, taking damage on the same frame the collider contacts Player, a different action
        //! will run and `AT_HIT` will remain set. Then when returning back to this action, Player
        //! will get knocked back instantly, even though there was no apparent collision.
        //! Handling `AT_HIT` directly in Update, where it can run every frame, would help catch these edge cases.
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            if (this->collider.base.at == &player->actor) {
                Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            }
        }

        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    } else if ((this->actor.xzDistToPlayer > 175.0f) || (player->stateFlags1 & PLAYER_STATE1_26)) {
        mode_wait_init(this);
    } else {
        mode_attack_init(this);
        this->unk_318 = 27;
        this->unk_31C = 750;
    }
}

static void mode_dam_init(EnBa* this) {
    this->unk_14C = 1;
    this->unk_31C = 1500;
    this->unk_318 = 20;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.speed = 10.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_HAND_DAMAGE);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
    En_ba_actor_set_process(this, mode_dam);
}

static void mode_dam(EnBa* this, PlayState* play) {
    s32 i;
    Vec3s sp6C;

    add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 330.0f, 1.0f, 30.0f, 0.0f);
    this->unk_2FC = this->actor.world.pos;
    if (play->gameplayFrames % 16 == 0) {
        this->unk_308.z += rnd_fx(180.0f);
        this->unk_314 += rnd_fx(180.0f);
        this->unk_308.x = sinf_table(this->unk_308.z) * 80.0f;
        this->unk_308.y = cosf_table(this->unk_314) * 80.0f;
    }
    this->unk_2FC.y -= 448.0f;
    this->unk_2FC.x += this->unk_308.x;
    this->unk_2FC.z += this->unk_308.y;
    chase_pos(&this->unk_2FC, &this->unk_158[13], 1.0f, this->actor.speed, 0.0f, 0.0f);
    for (i = 12; i >= 0; i--) {
        search_position_angleXY(&this->unk_158[i + 1], &this->unk_158[i], &sp6C, 0);
        Matrix_translate(this->unk_158[i + 1].x, this->unk_158[i + 1].y, this->unk_158[i + 1].z, MTXMODE_NEW);
        Matrix_rotateXYZ(sp6C.x, sp6C.y, 0, MTXMODE_APPLY);
        Matrix_Position(&arm_reng, &this->unk_158[i]);
    }
    search_position_angleXY(&this->actor.world.pos, &this->unk_158[0], &sp6C, 0);
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    add_calc_short_angle2(&this->actor.shape.rot.y, sp6C.y, 3, this->unk_31C, 182);
    add_calc_short_angle2(&this->actor.shape.rot.x, sp6C.x + 0x8000, 3, this->unk_31C, 182);
    Matrix_rotateXYZ(this->actor.shape.rot.x - 0x8000, this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Position(&arm_reng, &this->unk_158[0]);

    for (i = 0; i < 13; i++) {
        search_position_angleXY(&this->unk_158[i], &this->unk_158[i + 1], &sp6C, 0);
        Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
        add_calc_short_angle2(&this->unk_2A8[i].y, sp6C.y, 3, this->unk_31C, 182);
        add_calc_short_angle2(&this->unk_2A8[i].x, sp6C.x + 0x8000, 3, this->unk_31C, 182);
        Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_Position(&arm_reng, &this->unk_158[i + 1]);
    }

    this->unk_2A8[13].x = this->unk_2A8[12].x;
    this->unk_2A8[13].y = this->unk_2A8[12].y;
    this->unk_318--;
    if (this->unk_318 == 0) {
        mode_wait_init(this);
    }
}

static void mode_dead_init(EnBa* this, PlayState* play2) {
    s16 unk_temp;
    s32 i;
    Vec3f sp74 = { 0.0f, 0.0f, 0.0f };
    PlayState* play = play2;

    this->unk_31C = 2500;
    Effect_sound_ct(play, &this->actor.projectedPos, NA_SE_EN_BALINADE_HAND_DEAD, 1, 1, 40);
    this->unk_14C = 0;

    for (i = 7; i < 14; i++) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BA, this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, 0,
                    0, 0, EN_BA_DEAD_BLOB);
    }
    unk_temp = search_position_angleX(&this->actor.world.pos, &this->unk_158[0]) + 0x8000;
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, this->unk_31C, 0);
    add_calc_short_angle2(&this->actor.shape.rot.x, unk_temp, 1, this->unk_31C, 0);
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.shape.rot.x - 0x8000, this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Position(&arm_reng, &this->unk_158[0]);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    for (i = 5; i < 13; i++) {
        add_calc_short_angle2(&this->unk_2A8[i].x, this->unk_2A8[5].x, 1, this->unk_31C, 0);
        add_calc_short_angle2(&this->unk_2A8[i].y, this->unk_2A8[5].y, 1, this->unk_31C, 0);
        Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
        Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_Position(&sp74, &this->unk_158[i + 1]);
    }
    this->unk_31A = 15;
    En_ba_actor_set_process(this, mode_dead);
}

static void mode_dead(EnBa* this, PlayState* play) {
    Vec3f sp6C = { 0.0f, 0.0f, 0.0f };
    s16 temp;
    s32 i;

    if (this->unk_31A != 0) {
        this->actor.speed = 30.0f;
        this->unk_31C = 8000;
        this->actor.world.pos.y += 8.0f;
        temp = search_position_angleX(&this->actor.world.pos, &this->unk_158[0]) + 0x8000;
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, this->unk_31C, 0);
        add_calc_short_angle2(&this->actor.shape.rot.x, temp, 1, this->unk_31C, 0);
        Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_rotateXYZ(this->actor.shape.rot.x - 0x8000, this->actor.shape.rot.y, 0, MTXMODE_APPLY);
        Matrix_Position(&arm_reng, &this->unk_158[0]);
        for (i = 0; i < 5; i++) {
            temp = -cos_s(this->unk_31A * 0x444) * (i * 400);
            add_calc_short_angle2(&this->unk_2A8[i].x, temp - 0x4000, 1, this->unk_31C, 0);
            add_calc_short_angle2(&this->unk_2A8[i].y, this->actor.yawTowardsPlayer, 1, this->unk_31C, 0);
            Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
            Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
            Matrix_Position(&arm_reng, &this->unk_158[i + 1]);
        }
        for (i = 5; i < 13; i++) {
            add_calc_short_angle2(&this->unk_2A8[i].x, this->unk_2A8[5].x, 1, this->unk_31C, 0);
            add_calc_short_angle2(&this->unk_2A8[i].y, this->unk_2A8[5].y, 1, this->unk_31C, 0);
            Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
            Matrix_rotateXYZ(this->unk_2A8[i].x - 0x8000, this->unk_2A8[i].y, 0, MTXMODE_APPLY);
            Matrix_Position(&sp6C, &this->unk_158[i + 1]);
        }
        this->unk_31A--;
    } else {
        Actor_Environment_sw_On(play, this->upperParams);
        Actor_delete(&this->actor);
    }
}

void En_ba_actor_move(Actor* thisx, PlayState* play) {
    EnBa* this = (EnBa*)thisx;

    if ((this->actor.params < EN_BA_DEAD_BLOB) && (this->collider.base.acFlags & AC_HIT)) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->actor.colChkInfo.health--;
        if (this->actor.colChkInfo.health == 0) {
            mode_dead_init(this, play);
        } else {
            mode_dam_init(this);
        }
    }
    this->actionFunc(this, play);
    if (this->actor.params < EN_BA_DEAD_BLOB) {
        this->actor.focus.pos = this->unk_158[6];
    }
    if (this->unk_14C >= 2) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

static void* txt_dat[] = {
    object_bxa_Tex_0024F0,
    object_bxa_Tex_0027F0,
    object_bxa_Tex_0029F0,
};

void En_ba_actor_draw(Actor* thisx, PlayState* play) {
    EnBa* this = (EnBa*)thisx;
    s32 pad;
    s16 i;
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 14);
    Vec3f unused = { 0.0f, 0.0f, 448.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ba.c", 933);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    if (this->actor.params < EN_BA_DEAD_BLOB) {
        Matrix_push();
        gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(txt_dat[this->actor.params]));
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 16, 16, 1, 0,
                                    (play->gameplayFrames * -10) % 128, 32, 32));
        for (i = 0; i < 14; i++, mtx++) {
            Matrix_translate(this->unk_158[i].x, this->unk_158[i].y, this->unk_158[i].z, MTXMODE_NEW);
            Matrix_rotateXYZ(this->unk_2A8[i].x, this->unk_2A8[i].y, this->unk_2A8[i].z, MTXMODE_APPLY);
            Matrix_scale(this->unk_200[i].x, this->unk_200[i].y, this->unk_200[i].z, MTXMODE_APPLY);
            if ((i == 6) || (i == 13)) {
                if (i == 13) {
                    CollisionCheck_Uty_convJntSphL2G(i, &this->collider);
                } else {
                    Matrix_scale(0.5f, 0.5f, 1.0f, MTXMODE_APPLY);
                    CollisionCheck_Uty_convJntSphL2G(8, &this->collider);
                }
            }
            MATRIX_TO_MTX(mtx, "../z_en_ba.c", 970);
        }
        Matrix_pull();
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ba.c", 973);
        gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_000890);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->gameplayFrames * 2) % 128,
                                    (play->gameplayFrames * 2) % 128, 32, 32, 1, (play->gameplayFrames * -5) % 128,
                                    (play->gameplayFrames * -5) % 128, 32, 32));
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 125, 100, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ba.c", 991);
        gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_001D80);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ba.c", 995);
}
