#include "z_en_m_thunder.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void En_M_Thunder_actor_ct(Actor* thisx, PlayState* play2);
void En_M_Thunder_actor_dt(Actor* thisx, PlayState* play);
void En_M_Thunder_actor_rolling_move(Actor* thisx, PlayState* play);
void En_M_Thunder_actor_rolling_draw(Actor* thisx, PlayState* play2);

void set_add_light_sword_tame(PlayState* play, f32 arg1);
static void move_wait(EnMThunder* this, PlayState* play);
void move_attack(EnMThunder* this, PlayState* play);

ActorProfile En_M_Thunder_Profile = {
    /**/ ACTOR_EN_M_THUNDER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnMThunder),
    /**/ En_M_Thunder_actor_ct,
    /**/ En_M_Thunder_actor_dt,
    /**/ En_M_Thunder_actor_rolling_move,
    /**/ En_M_Thunder_actor_rolling_draw,
};

static ColliderCylinderInit AcOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000001, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 200, 200, 0, { 0, 0, 0 } },
};

static u32 at_type_data[] = { DMG_SPIN_MASTER, DMG_SPIN_KOKIRI, DMG_SPIN_GIANT };
static u32 D_80AA0458[] = { DMG_JUMP_MASTER, DMG_JUMP_KOKIRI, DMG_JUMP_GIANT };

// Setup action
void En_M_Thunder_actor_set_process(EnMThunder* this, EnMThunderActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_M_Thunder_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &AcOcInfoData);
    this->unk_1C7 = PARAMS_GET_S(this->actor.params, 0, 8) - 1;
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 255, 255, 0);
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    this->collider.dim.radius = 0;
    this->collider.dim.height = 40;
    this->collider.dim.yShift = -20;
    this->unk_1C4 = 8;
    this->unk_1B4 = 0.0f;
    this->actor.world.pos = player->bodyPartsPos[PLAYER_BODYPART_WAIST];
    this->unk_1AC = 0.0f;
    this->unk_1BC = 0.0f;
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;
    this->actor.room = -1;
    Actor_set_scale(&this->actor, 0.1f);
    this->unk_1CA = 0;

    if (player->stateFlags2 & PLAYER_STATE2_17) {
        if (!z_common_data.save.info.playerData.isMagicAcquired || (z_common_data.magicState != MAGIC_STATE_IDLE) ||
            (PARAMS_GET_S(this->actor.params, 8, 8) &&
             !(magic_meter_check(play, PARAMS_GET_S(this->actor.params, 8, 8), MAGIC_CONSUME_NOW)))) {
            Nai_FxFlagEntry(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            Nai_FxFlagEntry(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            Actor_delete(&this->actor);
            return;
        }

        player->stateFlags2 &= ~PLAYER_STATE2_17;
        this->unk_1CA = 1;
        this->collider.elem.atDmgInfo.dmgFlags = at_type_data[this->unk_1C7];
        this->unk_1C6 = 1;
        this->unk_1C9 = ((this->unk_1C7 == 1) ? 2 : 4);
        En_M_Thunder_actor_set_process(this, move_attack);
        this->unk_1C4 = 8;
        Nai_FxFlagEntry(NA_SE_IT_ROLLING_CUT_LV1, &player->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->unk_1AC = 1.0f;
    } else {
        En_M_Thunder_actor_set_process(this, move_wait);
    }
    this->actor.child = NULL;
}

void En_M_Thunder_actor_dt(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;

    if (this->unk_1CA != 0) {
        magic_cancel_check(play);
    }

    ClObjPipe_dt(play, &this->collider);
    set_add_light_sword_tame(play, 0.0f);
    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
}

void set_add_light_sword_tame(PlayState* play, f32 arg1) {
    set_add_light_global(play, arg1, 850.0f, 0.2f, 0.0f);
}

static void move_wait2(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_17) {
        if (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            Nai_FxFlagEntry(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            Nai_FxFlagEntry(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        Actor_delete(&this->actor);
        return;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK)) {
        Actor_delete(&this->actor);
    }
}

static void move_wait(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* child = this->actor.child;

    this->unk_1B8 = player->unk_858;
    this->actor.world.pos = player->bodyPartsPos[PLAYER_BODYPART_WAIST];
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;

    if (this->unk_1CA == 0) {
        if (player->unk_858 >= 0.1f) {
            if ((z_common_data.magicState != MAGIC_STATE_IDLE) ||
                (PARAMS_GET_S(this->actor.params, 8, 8) &&
                 !(magic_meter_check(play, PARAMS_GET_S(this->actor.params, 8, 8), MAGIC_CONSUME_WAIT_PREVIEW)))) {
                move_wait2(this, play);
                En_M_Thunder_actor_set_process(this, move_wait2);
                this->unk_1C8 = 0;
                this->unk_1BC = 0.0;
                this->unk_1AC = 0.0f;
                return;
            }

            this->unk_1CA = 1;
        }
    }

    if (player->unk_858 >= 0.1f) {
        z_vibctl2_vib_setQ(0.0f, (s32)(player->unk_858 * 150.0f), 2, (s32)(player->unk_858 * 150.0f));
    }

    if (player->stateFlags2 & PLAYER_STATE2_17) {
        if ((child != NULL) && (child->update != NULL)) {
            child->parent = NULL;
        }

        if (player->unk_858 <= 0.15f) {
            if ((player->unk_858 >= 0.1f) && (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H)) {
                Nai_FxFlagEntry(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                Nai_FxFlagEntry(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
            Actor_delete(&this->actor);
            return;
        } else {
            player->stateFlags2 &= ~PLAYER_STATE2_17;
            if (PARAMS_GET_S(this->actor.params, 8, 8)) {
                z_common_data.magicState = MAGIC_STATE_CONSUME_SETUP;
            }
            if (player->unk_858 < 0.85f) {
                this->collider.elem.atDmgInfo.dmgFlags = at_type_data[this->unk_1C7];
                this->unk_1C6 = 1;
                this->unk_1C9 = ((this->unk_1C7 == 1) ? 2 : 4);
            } else {
                this->collider.elem.atDmgInfo.dmgFlags = D_80AA0458[this->unk_1C7];
                this->unk_1C6 = 0;
                this->unk_1C9 = ((this->unk_1C7 == 1) ? 4 : 8);
            }

            En_M_Thunder_actor_set_process(this, move_attack);
            this->unk_1C4 = 8;

            {
                static u16 SE_flag[] = {
                    NA_SE_IT_ROLLING_CUT_LV2,
                    NA_SE_IT_ROLLING_CUT_LV1,
                    NA_SE_IT_ROLLING_CUT_LV2,
                    NA_SE_IT_ROLLING_CUT_LV1,
                };

                Nai_FxFlagEntry(SE_flag[this->unk_1C6], &player->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }

            this->unk_1AC = 1.0f;
            return;
        }
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK)) {
        if (this->actor.child != NULL) {
            this->actor.child->parent = NULL;
        }
        Actor_delete(&this->actor);
        return;
    }

    if (player->unk_858 > 0.15f) {
        this->unk_1C8 = 255;
        if (this->actor.child == NULL) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EFF_DUST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                               this->unk_1C7 + 2);
        }
        this->unk_1BC += ((((player->unk_858 - 0.15f) * 1.5f) - this->unk_1BC) * 0.5f);

    } else if (player->unk_858 > .1f) {
        this->unk_1C8 = (s32)((player->unk_858 - .1f) * 255.0f * 20.0f);
        this->unk_1AC = (player->unk_858 - .1f) * 10.0f;
    } else {
        this->unk_1C8 = 0;
    }

    if (player->unk_858 > 0.85f) {
        Na_SetSwordChargeSe(&player->actor.projectedPos, 2);
    } else if (player->unk_858 > 0.15f) {
        Na_SetSwordChargeSe(&player->actor.projectedPos, 1);
    } else if (player->unk_858 > 0.1f) {
        Na_SetSwordChargeSe(&player->actor.projectedPos, 0);
    }

    if (Game_play_demo_mode_check(play)) {
        Actor_delete(&this->actor);
    }
}

void move_attack_common(EnMThunder* this, PlayState* play) {
    if (this->unk_1C4 < 2) {
        if (this->unk_1C8 < 40) {
            this->unk_1C8 = 0;
        } else {
            this->unk_1C8 -= 40;
        }
    }

    this->unk_1B4 += 2.0f * this->unk_1B0;

    if (this->unk_1BC < this->unk_1AC) {
        this->unk_1BC += ((this->unk_1AC - this->unk_1BC) * 0.1f);
    } else {
        this->unk_1BC = this->unk_1AC;
    }
}

void move_attack(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (chase_f(&this->unk_1AC, 0.0f, 1 / 16.0f)) {
        Actor_delete(&this->actor);
    } else {
        add_calc(&this->actor.scale.x, (s32)this->unk_1C9, 0.6f, 0.8f, 0.0f);
        Actor_set_scale(&this->actor, this->actor.scale.x);
        this->collider.dim.radius = (this->actor.scale.x * 25.0f);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_1C4 > 0) {
        this->actor.world.pos.x = player->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        this->actor.world.pos.z = player->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        this->unk_1C4--;
    }

    if (this->unk_1AC > 0.6f) {
        this->unk_1B0 = 1.0f;
    } else {
        this->unk_1B0 = this->unk_1AC * (5.0f / 3.0f);
    }

    move_attack_common(this, play);

    if (Game_play_demo_mode_check(play)) {
        Actor_delete(&this->actor);
    }
}

void En_M_Thunder_actor_rolling_move(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;
    f32 blueRadius;
    s32 redGreen;

    this->actionFunc(this, play);
    set_add_light_sword_tame(play, this->unk_1BC);
    blueRadius = this->unk_1AC;
    redGreen = (u32)(blueRadius * 255.0f) & 0xFF;
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, redGreen, redGreen, (u32)(blueRadius * 100.0f),
                              (s32)(blueRadius * 800.0f));
}

void En_M_Thunder_actor_rolling_draw(Actor* thisx, PlayState* play2) {
    static f32 yuragi_table[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.25f, 0.2f, 0.15f };
    PlayState* play = play2;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);
    f32 phi_f14;
    s32 phi_t1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_m_thunder.c", 844);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_m_thunder.c", 853);

    switch (this->unk_1C6) {
        case 0:
        case 1:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE,
                                        0xFF - ((u8)(s32)(this->unk_1B4 * 30) & 0xFF), 0, 0x40, 0x20, 1,
                                        0xFF - ((u8)(s32)(this->unk_1B4 * 20) & 0xFF), 0, 8, 8));
            break;
    }

    switch (this->unk_1C6) {
        case 0:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, (u8)(this->unk_1B0 * 255));
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack3DL);
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack4DL);
            break;
        case 1:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, (u8)(this->unk_1B0 * 255));
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack1DL);
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack2DL);
            break;
    }

    Matrix_mult(&player->mf_9E0, MTXMODE_NEW);

    switch (this->unk_1C7) {
        case 1:
            Matrix_translate(0.0f, 220.0f, 0.0f, MTXMODE_APPLY);
            Matrix_scale(-0.7f, -0.6f, -0.4f, MTXMODE_APPLY);
            Matrix_rotateX(16384.0f, MTXMODE_APPLY);
            break;
        case 0:
            Matrix_translate(0.0f, 300.0f, -100.0f, MTXMODE_APPLY);
            Matrix_scale(-1.2f, -1.0f, -0.7f, MTXMODE_APPLY);
            Matrix_rotateX(16384.0f, MTXMODE_APPLY);
            break;
        case 2:
            Matrix_translate(200.0f, 350.0f, 0.0f, MTXMODE_APPLY);
            Matrix_scale(-1.8f, -1.4f, -0.7f, MTXMODE_APPLY);
            Matrix_rotateX(16384.0f, MTXMODE_APPLY);
            break;
    }

    if (this->unk_1B8 >= 0.85f) {
        phi_f14 = (yuragi_table[(play->gameplayFrames & 7)] * 6.0f) + 1.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, this->unk_1C8);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 128);
        phi_t1 = 0x28;
    } else {
        phi_f14 = (yuragi_table[play->gameplayFrames & 7] * 2.0f) + 1.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, this->unk_1C8);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
        phi_t1 = 0x14;
    }
    Matrix_scale(1.0f, phi_f14, phi_f14, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_m_thunder.c", 960);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->gameplayFrames * 5) & 0xFF, 0, 0x20, 0x20,
                                1, (play->gameplayFrames * 20) & 0xFF, (play->gameplayFrames * phi_t1) & 0xFF, 8, 8));

    gSPDisplayList(POLY_XLU_DISP++, gSpinAttackChargingDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_m_thunder.c", 1031);
}
