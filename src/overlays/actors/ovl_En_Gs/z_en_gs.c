/*
 * File: z_en_gs.c
 * Overlay: ovl_En_Gs
 * Description: Gossip Stone
 */

#include "z_en_gs.h"
#include "assets/objects/object_gs/object_gs.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Gs_actor_ct(Actor* thisx, PlayState* play);
void En_Gs_actor_dt(Actor* thisx, PlayState* play);
void En_Gs_actor_move(Actor* thisx, PlayState* play2);
void En_Gs_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(EnGs* this, PlayState* play);
void mode_del(EnGs* this, PlayState* play);

void func_init(EnGs* this);

ActorProfile En_Gs_Profile = {
    /**/ ACTOR_EN_GS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GS,
    /**/ sizeof(EnGs),
    /**/ En_Gs_actor_ct,
    /**/ En_Gs_actor_dt,
    /**/ En_Gs_actor_move,
    /**/ En_Gs_actor_draw,
};

static ColliderCylinderInit atinfodata = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 21, 48, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 statusdata = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable btldata = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0xE),
    /* Explosive     */ DMG_ENTRY(0, 0xC),
    /* Boomerang     */ DMG_ENTRY(0, 0xE),
    /* Normal arrow  */ DMG_ENTRY(0, 0xE),
    /* Hammer swing  */ DMG_ENTRY(0, 0xD),
    /* Hookshot      */ DMG_ENTRY(0, 0xE),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xF),
    /* Master sword  */ DMG_ENTRY(0, 0xF),
    /* Giant's Knife */ DMG_ENTRY(0, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0xB),
    /* Ice arrow     */ DMG_ENTRY(0, 0xB),
    /* Light arrow   */ DMG_ENTRY(0, 0xB),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xB),
    /* Ice magic     */ DMG_ENTRY(0, 0xB),
    /* Light magic   */ DMG_ENTRY(0, 0xB),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void En_Gs_actor_ct(Actor* thisx, PlayState* play) {
    EnGs* this = (EnGs*)thisx;

    ValueSet_process(thisx, value_init);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &atinfodata);
    CollisionCheck_Status_set3(&thisx->colChkInfo, &btldata, &statusdata);

    thisx->attentionRangeType = ATTENTION_RANGE_6;
    this->unk_1D8 = thisx->world.pos;
    this->actionFunc = mode_wait;
    this->unk_1B4[0].x = 1.0f;
    this->unk_1B4[0].y = 1.0f;
    this->unk_1B4[0].z = 1.0f;
    this->unk_1B4[1].x = 1.0f;
    this->unk_1B4[1].y = 1.0f;
    this->unk_1B4[1].z = 1.0f;
}

void En_Gs_actor_dt(Actor* thisx, PlayState* play) {
}

static s32 func_talk_endmsg_chk(EnGs* this, PlayState* play) {
    s32 ret = 2;
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                switch (this->actor.textId) {
                    case 0x2054:
                        this->actor.textId = PARAMS_GET_U(this->actor.params, 0, 8) + 0x400;
                        ret = 1;
                        break;
                    default:
                        ret = 0;
                        break;
                }
            }
            break;
    }
    return ret;
}

void ocarina_check(EnGs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

#if DEBUG_FEATURES
    bREG(15) = 0;
#endif

    if (this->actor.xzDistToPlayer <= 100.0f) {
#if DEBUG_FEATURES
        bREG(15) = 1;
#endif

        if (this->unk_19D == 0) {
            player->stateFlags2 |= PLAYER_STATE2_23;
            if (player->stateFlags2 & PLAYER_STATE2_24) {
                ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
                this->unk_19D |= 1;
            }

        } else if (this->unk_19D & 1) {
            if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
                if ((play->msgCtx.unk_E3F2 == OCARINA_SONG_SARIAS) || (play->msgCtx.unk_E3F2 == OCARINA_SONG_EPONAS) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_LULLABY) || (play->msgCtx.unk_E3F2 == OCARINA_SONG_SUNS) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_TIME)) {
                    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0, FAIRY_HEAL_TIMED);
                    Actor_SE_set(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                } else if (play->msgCtx.unk_E3F2 == OCARINA_SONG_STORMS) {
                    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0, FAIRY_HEAL_BIG);
                    Actor_SE_set(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                }
                this->unk_19D = 0;
                Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));
            } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
                player->stateFlags2 |= PLAYER_STATE2_23;
            }
        }
    }
}

static void func_talk(EnGs* this, PlayState* play) {
    s16 sp26;
    s16 sp24;

    if (this->unk_19C == 1) {
        message_set2(play, this->actor.textId);
        this->unk_19C = 2;
    } else if (this->unk_19C == 2) {
        this->unk_19C = func_talk_endmsg_chk(this, play);
    } else if (Actor_talk_check(&this->actor, play)) {
        this->unk_19C = 2;
    } else {
        Actor_display_position_set(play, &this->actor, &sp26, &sp24);
        if ((sp26 >= 0) && (sp26 <= SCREEN_WIDTH) && (sp24 >= 0) && (sp24 <= SCREEN_HEIGHT) && (this->unk_19C != 3)) {
            if (Actor_talk_request2(&this->actor, play, 40.0f) == 1) {
                if (mask_check(play) == PLAYER_MASK_TRUTH) {
                    this->actor.textId = 0x2054;
                } else {
                    this->actor.textId = 0x2053;
                }
            }
        }
    }
}

f32 func_wave(EnGs* this, PlayState* play, f32* arg2, f32* arg3, u16* arg4, f32 arg5, f32 arg6, f32 arg7, s32 arg8,
                  s32 arg9) {
    f32 sp2C = add_calc(arg2, *arg3, arg5, arg6, arg7);

    if (arg9 == 0) {
        sp2C = add_calc(arg2, *arg3, arg5, arg6, arg7);
        this->unk_1B4[0].x = 1.0f + (sinf(DEG_TO_RAD(((*arg4 % arg8) * (1.0f / arg8)) * 360.0f)) * *arg2);
        this->unk_1B4[0].y = 1.0f - (sinf(DEG_TO_RAD(((*arg4 % arg8) * (1.0f / arg8)) * 360.0f)) * *arg2);
        *arg4 += 1;
    }
    return sp2C;
}

void mode_demo00(EnGs* this, PlayState* play) {
    if (this->unk_19F == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_200 = 0;
        this->unk_19F = 1;
        this->unk_1E8 = 0.5f;
        this->unk_1EC = 0.0f;
    } else if ((this->unk_19F == 1) && (func_wave(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200, 0.8f,
                                                      0.007f, 0.001f, 7, 0) == 0.0f)) {
        if (!Game_play_demo_mode_check(play)) {
            message_set(play, 0x71B1, NULL);
        }
        this->unk_19C = 0;
        this->actionFunc = mode_wait;
    }
}

void mode_demo01(EnGs* this, PlayState* play) {
    if (this->unk_19F == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_1E8 = 0.3f;
        this->unk_1EC = 0.0f;
        this->unk_200 = 0;
        this->unk_19F = 1;
    } else if (this->unk_19F == 1) {
        this->unk_1A0[0].z = DEG_TO_BINANG(((this->unk_200 % 8) / 8.0f) * 360.0f);
        this->unk_1A0[1].z = -this->unk_1A0[0].z;
        if (func_wave(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200, 0.8f, 0.005f, 0.001f, 7, 0) ==
            0.0f) {
            this->unk_19C = 0;
            this->actionFunc = mode_wait;
        }
    }
}

void mode_demo02(EnGs* this, PlayState* play) {
    f32 ret;

    if (this->unk_19F == 0) {
        this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
        this->unk_1EC = -0.8f;
        this->unk_19F++;
    } else if (this->unk_19F == 1) {
        ret = add_calc(&this->unk_1E8, this->unk_1EC, 1.0f, 0.4f, 0.001f);
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (ret == 0.0f) {
            this->unk_200 = 0;
            this->unk_19F++;
        }
    } else if (this->unk_19F == 2) {
        this->unk_200++;
        if (this->unk_200 >= 100) {
            this->unk_19F++;
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 0.0f;
        }
    } else if (this->unk_19F == 3) {
        ret = add_calc(&this->unk_1E8, this->unk_1EC, 1.0f, 0.5f, 0.001f);
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (ret == 0.0f) {
            this->unk_1E8 = 0.5f;
            this->unk_1EC = 0.0f;
            this->unk_200 = 0;
            Actor_SE_set(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->unk_19F++;
        }
    } else if (this->unk_19F == 4) {
        if (func_wave(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200, 1.0f, 0.03f, 0.001f, 5, 0) ==
            0.0f) {
            this->unk_19C = 0;
            this->actionFunc = mode_wait;
        }
    }
}

void mode_demo03(EnGs* this, PlayState* play) {
    if (this->unk_19F == 0) {
        this->unk_200 = 40;
        this->unk_19F++;
    }

    if (this->unk_19F == 1) {
        if ((u32)this->unk_200-- <= 0) {
            this->unk_200 = 80;
            this->unk_19E |= 4;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 2) {
        static Color_RGBA8 color_rgba = { 255, 50, 50, 0 };
        static Color_RGBA8 flashBlue = { 50, 50, 255, 0 };
        static Color_RGBA8 baseWhite = { 255, 255, 255, 0 };

        this->unk_200--;
        rgba_t_move(&this->flashColor, &baseWhite);
        if ((this->unk_200 < 80) && ((this->unk_200 % 20) < 8)) {
            if (this->unk_200 < 20) {
                rgba_t_move(&this->flashColor, &color_rgba);
                if ((this->unk_200 % 20) == 7) {
                    Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_E, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                }
            } else {
                rgba_t_move(&this->flashColor, &flashBlue);
                if ((this->unk_200 % 20) == 7) {
                    Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_N, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                }
            }
        }
        if (this->unk_200 <= 0) {
            this->unk_200 = 0;
            this->unk_19E &= ~4;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 3) {
        u8 i;

        for (i = 0; i < 3; i++) {
            static Vec3f acc = { 0.0f, -0.3f, 0.0f };
            static Color_RGBA8 prim = { 200, 200, 200, 128 };
            static Color_RGBA8 env = { 100, 100, 100, 0 };
            Vec3f dustPos;
            Vec3f dustVelocity;

            dustVelocity.x = rnd_fx(15.0f);
            dustVelocity.y = rnd_f(-1.0f);
            dustVelocity.z = rnd_fx(15.0f);
            dustPos.x = this->actor.world.pos.x + (dustVelocity.x + dustVelocity.x);
            dustPos.y = this->actor.world.pos.y + 7.0f;
            dustPos.z = this->actor.world.pos.z + (dustVelocity.z + dustVelocity.z);
            Effect_SS_Dust_sc_cl_co_ct(play, &dustPos, &dustVelocity, &acc, &prim, &env,
                          (s16)rnd_f(50.0f) + 200, 40, 15);
        }

        Actor_level_SE_set(&this->actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        if (this->unk_200++ >= 40) {
            this->unk_19E |= 0x10;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.cullingVolumeDistance = 12000.0f;

            this->actor.gravity = 0.3f;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 4) {
        Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1);
        if (this->actor.bgCheckFlags & (BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING)) {
            static Vec3f fire_vec = { 0.0f, 0.0f, 0.0f };
            static Vec3f bomb_acc = { 0.0f, 0.0f, 0.0f };
            Vec3f bomb2Pos;

            bomb2Pos.x = this->actor.world.pos.x;
            bomb2Pos.y = this->actor.world.pos.y;
            bomb2Pos.z = this->actor.world.pos.z;
            Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
            Effect_SS_Bomb2_2_ct(play, &bomb2Pos, &fire_vec, &bomb_acc, 100, 20);
            this->unk_200 = 10;
            this->unk_19E |= 8;
            this->actionFunc = mode_del;
        } else {
            Actor_level_SE_set(&this->actor, NA_SE_EV_STONE_LAUNCH - SFX_FLAG);
        }

        Actor_position_moveF(&this->actor);
        if (this->actor.yDistToPlayer < -12000.0f) {
            Actor_delete(&this->actor);
        }
    }
}

void mode_demo04(EnGs* this, PlayState* play) {
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    s32 tmp;
    s16 tmp2 = this->unk_1A0[0].y;

    if (this->unk_19F == 0) {
        this->unk_1E8 = 0.0f;
        this->unk_1EC = 90.0f;
        this->unk_1F0 = 0.1f;
        this->unk_1F4 = 2.0f;
        this->unk_1F8 = 0;
        this->unk_19F = 1;
    }
    if (this->unk_19F == 1) {
        add_calc(&this->unk_1F0, this->unk_1F4, 1.0f, 0.1f, 0.001f);
        tmpf1 = add_calc(&this->unk_1E8, this->unk_1EC, 1.0f, this->unk_1F0, 0.001f);
        this->unk_1A0[0].y += DEG_TO_BINANG2(this->unk_1E8);
        if (tmpf1 == 0.0f) {
            this->unk_200 = 0;
            this->unk_19F = 2;
        }
    }
    if (this->unk_19F == 2) {
        this->unk_1A0[0].y += DEG_TO_BINANG2(this->unk_1E8);
        if (this->unk_200++ > 40) {
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 1.5f;
            this->unk_1F0 = this->unk_1B4[1].y - 1.0f;
            this->unk_1F4 = -0.3f;
            Actor_SE_set(&this->actor, NA_SE_EV_STONE_GROW_UP);
            this->unk_19F = 3;
        }
    }
    if (this->unk_19F == 3) {
        this->unk_1A0[0].y += DEG_TO_BINANG2(90.0f);
        tmpf1 = add_calc(&this->unk_1E8, this->unk_1EC, 0.8f, 0.2f, 0.001f);
        add_calc(&this->unk_1F0, this->unk_1F4, 0.8f, 0.2f, 0.001f);
        this->unk_1B4[0].x = this->unk_1F0 + 1.0f;
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (tmpf1 == 0.0f) {
            this->unk_1E8 = 2.0f * M_PI / 9.0000002;
            this->unk_1EC = M_PI / 9.0000002;
            this->unk_19F = 4;
        }
    }
    if (this->unk_19F == 4) {
        tmpf1 = add_calc(&this->unk_1E8, this->unk_1EC, 0.8f, DEG_TO_BINANG2(90.0f), DEG_TO_BINANG2(20.0f));
        this->unk_1A0[0].y += (s16)this->unk_1E8;
        if (tmpf1 == 0.0f) {

            tmp = this->unk_1A0[0].y;
            if (tmp > 0) {
                tmp -= 0x10000;
            }
            this->unk_1E8 = tmp;

            this->unk_1EC = 0.0f;
            this->unk_19F = 5;
        }
    }
    if (this->unk_19F == 5) {
        tmp = this->unk_1A0[0].y;
        if (tmp > 0) {
            tmp -= 0xFFFF;
        }
        this->unk_1E8 = tmp;
        tmpf1 = add_calc(&this->unk_1E8, this->unk_1EC, 0.8f, DEG_TO_BINANG2(20.0f), 0.001f);
        this->unk_1A0[0].y = this->unk_1E8;
        if (tmpf1 == 0.0f) {
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 0;
            this->unk_1A0[0].y = 0;
            this->unk_200 = 0;
            this->unk_1F0 = this->unk_1B4[0].x - 1.0f;
            this->unk_1F4 = 0;
            this->unk_1F8 = 0.5f;
            this->unk_1FC = 0;
            Actor_SE_set(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->unk_19F = 6;
        }
    }
    if (this->unk_19F == 6) {
        tmpf1 = add_calc(&this->unk_1E8, this->unk_1EC, 0.8f, 0.1f, 0.001f);
        tmpf2 = add_calc(&this->unk_1F0, this->unk_1F4, 0.8f, 0.1f, 0.001f);
        tmpf3 = add_calc(&this->unk_1F8, this->unk_1FC, 0.8f, 0.02f, 0.001f);
        this->unk_1B4[0].x = this->unk_1F0 + 1.0f;
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        this->unk_1B4[0].x += sinf(DEG_TO_RAD(((this->unk_200 % 10) * 0.1f) * 360.0f)) * this->unk_1F8;
        this->unk_1B4[0].y += sinf(DEG_TO_RAD(((this->unk_200 % 10) * 0.1f) * 360.0f)) * this->unk_1F8;
        this->unk_200++;
        if ((tmpf1 == 0.0f) && (tmpf2 == 0.0f) && (tmpf3 == 0.0f)) {
            this->unk_19C = 0;
            this->actionFunc = mode_wait;
        }
    }
    if ((u16)this->unk_1A0[0].y < (u16)tmp2) {
        Actor_SE_set(&this->actor, NA_SE_EV_STONE_ROLLING);
    }
}

void mode_del(EnGs* this, PlayState* play) {
    if (this->unk_200-- <= 0) {
        Actor_delete(&this->actor);
    }
}

static void mode_wait(EnGs* this, PlayState* play) {
    if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        ocarina_check(this, play);
    }
}

void func_init(EnGs* this) {
    static Vec3s init_s_xyz = { 0, 0, 0 };
    static Vec3f init_xyz = { 1.0f, 1.0f, 1.0f };
    s32 i;

    for (i = 0; i < 3; i++) {
        this->unk_1A0[i] = init_s_xyz;
    }

    for (i = 0; i < 2; i++) {
        this->unk_1B4[i] = init_xyz;
    }

    this->unk_19C = 3;
}

void En_Gs_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGs* this = (EnGs*)thisx;

    Actor_world_to_eye(&this->actor, 23.0f);
    if (!(this->unk_19E & 0x10)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->unk_19F = 0;
            this->collider.base.acFlags &= ~AC_HIT;

            switch (this->actor.colChkInfo.damageEffect) {
                case 15:
                    this->unk_19E |= 1;
                    func_init(this);
                    this->actionFunc = mode_demo00;
                    break;
                case 14:
                    this->unk_19E |= 1;
                    func_init(this);
                    this->actionFunc = mode_demo01;
                    break;
                case 13:
                    this->unk_19E |= 1;
                    func_init(this);
                    this->actionFunc = mode_demo02;
                    break;
                case 12:
                    this->unk_19E |= 2;
                    func_init(this);
                    this->actionFunc = mode_demo03;
                    break;
                case 11:
                    this->unk_19E |= 1;
                    func_init(this);
                    this->actionFunc = mode_demo04;
                    break;
            }
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
    this->actionFunc(this, play);
    func_talk(this, play);
}

void En_Gs_actor_draw(Actor* thisx, PlayState* play) {
    EnGs* this = (EnGs*)thisx;
    s32 tmp;
    u32 frames;

    if (!(this->unk_19E & 8)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_gs.c", 1046);

        frames = play->gameplayFrames;
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Matrix_push();
        if (this->unk_19E & 1) {
            Matrix_rotateY(BINANG_TO_RAD(this->unk_1A0[0].y), MTXMODE_APPLY);
            Matrix_rotateX(BINANG_TO_RAD(this->unk_1A0[0].x), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD(this->unk_1A0[0].z), MTXMODE_APPLY);
            Matrix_scale(this->unk_1B4[0].x, this->unk_1B4[0].y, this->unk_1B4[0].z, MTXMODE_APPLY);
            Matrix_rotateY(BINANG_TO_RAD(this->unk_1A0[1].y), MTXMODE_APPLY);
            Matrix_rotateX(BINANG_TO_RAD(this->unk_1A0[1].x), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD(this->unk_1A0[1].z), MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_gs.c", 1064);
        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneMaterialDL);

        if (this->unk_19E & 4) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->flashColor.r, this->flashColor.g, this->flashColor.b,
                            this->flashColor.a);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        }

        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneDL);
        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneSquishedDL);

        Matrix_pull();
        if (this->unk_19E & 2) {
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(0.05f, -0.05f, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_gs.c", 1087);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0, -frames * 0x14,
                                        0x20, 0x80));
            gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_gs.c", 1101);
    }
}
