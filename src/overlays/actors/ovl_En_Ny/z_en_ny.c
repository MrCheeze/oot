#include "z_en_ny.h"
#include "assets/objects/object_ny/object_ny.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Ny_actor_ct(Actor* thisx, PlayState* play);
void En_Ny_actor_dt(Actor* thisx, PlayState* play);
void En_Ny_actor_move(Actor* thisx, PlayState* play);
void En_Ny_actor_draw(Actor* thisx, PlayState* play);

void En_Ny_actor_move2(Actor* thisx, PlayState* play2);
void Ny_go(EnNy* this, PlayState* play);
void Ny_eff_crush(EnNy* this, PlayState* play);
void Ny_MaxSpeedSet(EnNy* this);
void CHG_Ny_go(EnNy* this);
void Ny_syu(EnNy* this, PlayState* play);
void Ny_fadeOUT(EnNy* this, PlayState* play);
void Ny_wait(EnNy* this, PlayState* play);
void Ny_fadeIN(EnNy* this, PlayState* play);
void Ny_syakin(EnNy* this, PlayState* play);
void En_Ny_eff_move(Actor* thisx, PlayState* play);
void Ny_eff_wait(EnNy* this, PlayState* play);
void En_Ny_eff_draw(Actor* thisx, PlayState* play);
void Ny_speed_y_set(EnNy* this, f32, f32);

ActorProfile En_Ny_Profile = {
    /**/ ACTOR_EN_NY,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_NY,
    /**/ sizeof(EnNy),
    /**/ En_Ny_actor_ct,
    /**/ En_Ny_actor_dt,
    /**/ En_Ny_actor_move,
    /**/ En_Ny_actor_draw,
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x04, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_SPIKE, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void En_Ny_actor_ct(Actor* thisx, PlayState* play) {
    EnNy* this = (EnNy*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.colChkInfo.health = 2;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &JntSphData, this->elements);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
    this->unk_1CA = 0;
    this->unk_1D0 = 0;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.speed = 0.0f;
    this->actor.shape.rot.y = 0;
    this->actor.gravity = -0.4f;
    this->hitPlayer = 0;
    this->unk_1CE = 2;
    this->actor.velocity.y = 0.0f;
    this->unk_1D4 = 0xFF;
    this->unk_1D8 = 0;
    this->unk_1E8 = 0.0f;
    this->unk_1E0 = 0.25f;
    if (this->actor.params == 0) {
        // "New initials"
        PRINTF("ニュウ イニシャル[ %d ] ！！\n", this->actor.params);
        this->actor.colChkInfo.mass = 0;
        this->unk_1D4 = 0;
        this->unk_1D8 = 0xFF;
        this->unk_1E0 = 1.0f;
        CHG_Ny_go(this);
    } else {
        // This mode is unused in the final game
        // "Dummy new initials"
        PRINTF("ダミーニュウ イニシャル[ %d ] ！！\n", this->actor.params);
        PRINTF("En_Ny_actor_move2[ %x ] ！！\n", En_Ny_actor_move2);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.update = En_Ny_actor_move2;
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
    }
}

void En_Ny_actor_dt(Actor* thisx, PlayState* play) {
    EnNy* this = (EnNy*)thisx;
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void Ny_MaxSpeedSet(EnNy* this) {
    f32 temp;

    temp = (this->actor.depthInWater > 0.0f) ? 0.7f : 1.0f;
    this->unk_1E8 = 2.8f * temp;
}

void CHG_Ny_wait(EnNy* this) {
    this->actionFunc = Ny_wait;
}

void CHG_Ny_fadeIN(EnNy* this) {
    this->stoneTimer = 0x14;
    this->actionFunc = Ny_fadeIN;
}

void CHG_Ny_syakin(EnNy* this) {
    this->actionFunc = Ny_syakin;
}

void CHG_Ny_go(EnNy* this) {
    this->unk_1F4 = 0.0f;
    Ny_MaxSpeedSet(this);
    this->stoneTimer = 180;
    this->actionFunc = Ny_go;
}

void CHG_Ny_syu(EnNy* this) {
    Actor_SE_set(&this->actor, NA_SE_EN_NYU_HIT_STOP);
    this->actionFunc = Ny_syu;
    this->unk_1E8 = 0.0f;
}

void CHG_Ny_fadeOUT(EnNy* this) {
    this->stoneTimer = 0x3C;
    this->actionFunc = Ny_fadeOUT;
}

void Ny_wait(EnNy* this, PlayState* play) {
    if (this->actor.xyzDistToPlayerSq <= SQ(160.0f)) {
        CHG_Ny_fadeIN(this);
    }
}

void Ny_fadeIN(EnNy* this, PlayState* play) {
    s32 phi_v1;
    s32 phi_v0;

    phi_v1 = this->unk_1D4 - 0x40;
    phi_v0 = this->unk_1D8 + 0x40;
    if (phi_v0 >= 0xFF) {
        phi_v1 = 0;
        phi_v0 = 0xFF;
        CHG_Ny_syakin(this);
    }
    this->unk_1D4 = phi_v1;
    this->unk_1D8 = phi_v0;
}

void Ny_syakin(EnNy* this, PlayState* play) {
    f32 phi_f0;

    phi_f0 = this->unk_1E0;
    phi_f0 += 2.0f;
    if (phi_f0 >= 1.0f) {
        phi_f0 = 1.0f;
        CHG_Ny_go(this);
    }
    this->unk_1E0 = phi_f0;
}

void Ny_go(EnNy* this, PlayState* play) {
    f32 yawDiff;
    s32 stoneTimer;

    if (!(this->unk_1F0 < this->actor.depthInWater)) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_NYU_MOVE - SFX_FLAG);
    }
    Ny_MaxSpeedSet(this);
    stoneTimer = this->stoneTimer;
    this->stoneTimer--;
    if ((stoneTimer <= 0) || this->hitPlayer) {
        CHG_Ny_syu(this);
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, this->unk_1F4, 0);
        add_calc2(&this->unk_1F4, 2000.0f, 1.0f, 100.0f);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        yawDiff = fatan2(this->actor.yDistToPlayer, this->actor.xzDistToPlayer);
        this->actor.speed = fabsf(cosf(yawDiff) * this->unk_1E8);
        if (this->unk_1F0 < this->actor.depthInWater) {
            this->unk_1EC = sinf(yawDiff) * this->unk_1E8;
        }
    }
}

void Ny_syu(EnNy* this, PlayState* play) {
    f32 phi_f0;

    phi_f0 = this->unk_1E0;
    phi_f0 -= 2.0f;
    if (phi_f0 <= 0.25f) {
        phi_f0 = 0.25f;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            if (!(this->unk_1F0 < this->actor.depthInWater)) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
            }
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
            this->actor.speed = 0.0f;
            this->actor.world.rot.y = this->actor.shape.rot.y;
            CHG_Ny_fadeOUT(this);
        }
    }
    this->unk_1E0 = phi_f0;
}

void Ny_fadeOUT(EnNy* this, PlayState* play) {
    s32 phi_v0;
    s32 phi_v1;

    phi_v0 = this->unk_1D4;
    phi_v0 += 0x40;
    phi_v1 = this->unk_1D8;
    phi_v1 -= 0x40;
    if (phi_v0 >= 0xFF) {
        phi_v0 = 0xFF;
        phi_v1 = 0;
        if (this->stoneTimer != 0) {
            this->stoneTimer--;
        } else {
            CHG_Ny_wait(this);
        }
    }
    this->unk_1D4 = phi_v0;
    this->unk_1D8 = phi_v1;
}

s32 Ny_Cross_Check(EnNy* this, PlayState* play) {
    u8 sp3F;
    Vec3f effectPos;

    sp3F = 0;
    this->hitPlayer = 0;
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;
        this->hitPlayer = 1;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.speed = -4.0f;
        return 0;
    }
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->hitPlayer = 1;
        return 0;
    } else {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            effectPos.x = this->collider.elements[0].base.acDmgInfo.hitPos.x;
            effectPos.y = this->collider.elements[0].base.acDmgInfo.hitPos.y;
            effectPos.z = this->collider.elements[0].base.acDmgInfo.hitPos.z;
            if ((this->unk_1E0 == 0.25f) && (this->unk_1D4 == 0xFF)) {
                switch (this->actor.colChkInfo.damageEffect) {
                    case 0xE:
                        sp3F = 1;
                        FALLTHROUGH;
                    case 0xF:
                        hp_down(&this->actor);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                        break;
                    case 1:
                        hp_down(&this->actor);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                        break;
                    case 2:
                        this->unk_1CA = 4;
                        hp_down(&this->actor);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                        break;
                }
            }
            this->stoneTimer = 0;
            if (this->actor.colChkInfo.health == 0) {
                this->actor.shape.shadowAlpha = 0;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->unk_1D0 = sp3F;
                Actor_info_finish(play, &this->actor);
                return 1;
            }
            Effect_SS_HitMark_ct(play, 0, &effectPos);
            return 0;
        }
    }
    return 0;
}

void Ny_speed_y_set(EnNy* this, f32 arg1, f32 arg2) {
    if (this->unk_1E8 == 0.0f) {
        this->actor.gravity = -0.4f;
    } else if (!(arg1 < this->actor.depthInWater)) {
        this->actor.gravity = -0.4f;
    } else if (arg2 < this->actor.depthInWater) {
        this->actor.gravity = 0.0;
        if (this->unk_1EC < this->actor.velocity.y) {
            this->actor.velocity.y -= 0.4f;
            if (this->actor.velocity.y < this->unk_1EC) {
                this->actor.velocity.y = this->unk_1EC;
            }
        } else if (this->actor.velocity.y < this->unk_1EC) {
            this->actor.velocity.y += 0.4f;
            if (this->unk_1EC < this->actor.velocity.y) {
                this->actor.velocity.y = this->unk_1EC;
            }
        }
    }
}

void En_Ny_actor_move(Actor* thisx, PlayState* play) {
    EnNy* this = (EnNy*)thisx;
    f32 temp_f20;
    f32 temp_f22;

    this->timer++;
    temp_f20 = this->unk_1E0 - 0.25f;
    if (this->unk_1CA != 0) {
        this->unk_1CA--;
    }
    Actor_world_to_eye(&this->actor, 0.0f);
    Actor_set_scale(&this->actor, 0.01f);
    this->collider.elements[0].dim.scale = 1.33f * temp_f20 + 1.0f;
    temp_f22 = (24.0f * temp_f20) + 12.0f;
    this->actor.shape.rot.x += (s16)(this->unk_1E8 * 1000.0f);
    Ny_speed_y_set(this, temp_f22 + 10.0f, temp_f22 - 10.0f);
    Actor_position_moveF(&this->actor);
    chase_f(&this->unk_1E4, this->unk_1E8, 0.1f);
    this->actionFunc(this, play);
    this->actor.prevPos.y -= temp_f22;
    this->actor.world.pos.y -= temp_f22;
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    this->unk_1F0 = temp_f22;
    this->actor.world.pos.y += temp_f22;
    if (Ny_Cross_Check(this, play) != 0) {
        s32 i;

        for (i = 0; i < 8; i++) {
            this->unk_1F8[i].x = (rnd_fx(20.0f) + this->actor.world.pos.x);
            this->unk_1F8[i].y = (rnd_fx(20.0f) + this->actor.world.pos.y);
            this->unk_1F8[i].z = (rnd_fx(20.0f) + this->actor.world.pos.z);
        }
        this->timer = 0;
        this->actor.update = En_Ny_eff_move;
        this->actor.draw = En_Ny_eff_draw;
        this->actionFunc = Ny_eff_wait;
        return;
    }
    if (this->unk_1E0 > 0.25f) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void Ny_eff_wait(EnNy* this, PlayState* play) {
    s32 effectScale;
    s32 i;
    Vec3f effectPos;
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effectAccel = { 0.0f, 0.1f, 0.0f };

    if (this->timer >= 2) {
        if (this->actor.depthInWater > 0.0f) {
            for (i = 0; i < 10; i++) {
                effectPos.x = rnd_fx(30.0f) + this->actor.world.pos.x;
                effectPos.y = rnd_fx(30.0f) + this->actor.world.pos.y;
                effectPos.z = rnd_fx(30.0f) + this->actor.world.pos.z;
                effectScale = get_random_timer(0x50, 0x64);
                Effect_SS_Dt_Bubble_sc_co_ct(play, &effectPos, &effectVelocity, &effectAccel, effectScale, 25, 0,
                                                   1);
            }
            for (i = 0; i < 0x14; i++) {
                effectPos.x = rnd_fx(30.0f) + this->actor.world.pos.x;
                effectPos.y = rnd_fx(30.0f) + this->actor.world.pos.y;
                effectPos.z = rnd_fx(30.0f) + this->actor.world.pos.z;
                Effect_SS_Bubble_ct(play, &effectPos, 10.0f, 10.0f, 30.0f, 0.25f);
            }
        }
        for (i = 0; i < 8; i++) {
            this->unk_1F8[i + 8].x = rnd_fx(10.0f);
            this->unk_1F8[i + 8].z = rnd_fx(10.0f);
            this->unk_1F8[i + 8].y = rnd_f(4.0f) + 4.0f;
        }
        this->timer = 0;
        if (this->unk_1D0 == 0) {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xA0);
        } else {
            Item_set0(play, &this->actor.world.pos, ITEM00_ARROWS_SMALL);
        }
        Actor_SE_set(&this->actor, NA_SE_EN_NYU_DEAD);
        this->actionFunc = Ny_eff_crush;
    }
}

void Ny_eff_crush(EnNy* this, PlayState* play) {
    s32 i;

    if (this->actor.depthInWater > 0.0f) {
        for (i = 0; i < 8; i += 1) {
            this->unk_1F8[i].x += this->unk_1F8[i + 8].x;
            this->unk_1F8[i].y += this->unk_1F8[i + 8].y;
            this->unk_1F8[i].z += this->unk_1F8[i + 8].z;
            chase_f(&this->unk_1F8[i + 8].x, 0.0f, 0.1f);
            chase_f(&this->unk_1F8[i + 8].y, -1.0f, 0.4f);
            chase_f(&this->unk_1F8[i + 8].z, 0.0f, 0.1f);
        }
        if (this->timer >= 0x1F) {
            Actor_delete(&this->actor);
            return;
        }
    } else {
        for (i = 0; i < 8; i += 1) {
            this->unk_1F8[i].x += this->unk_1F8[i + 8].x;
            this->unk_1F8[i].y += this->unk_1F8[i + 8].y;
            this->unk_1F8[i].z += this->unk_1F8[i + 8].z;
            chase_f(&this->unk_1F8[i + 8].x, 0.0f, 0.15f);
            chase_f(&this->unk_1F8[i + 8].y, -1.0f, 0.6f);
            chase_f(&this->unk_1F8[i + 8].z, 0.0f, 0.15f);
        }
        if (this->timer >= 0x10) {
            Actor_delete(&this->actor);
            return;
        }
    }
}

void En_Ny_eff_move(Actor* thisx, PlayState* play) {
    EnNy* this = (EnNy*)thisx;

    this->timer++;
    if (this->unk_1CA != 0) {
        this->unk_1CA--;
    }
    this->actionFunc(this, play);
}

void En_Ny_actor_move2(Actor* thisx, PlayState* play2) {
    EnNy* this = (EnNy*)thisx;
    PlayState* play = play2;
    f32 sp3C;
    f32 temp_f0;

    sp3C = this->unk_1E0 - 0.25f;
    this->timer++;
    Actor_world_to_eye(&this->actor, 0.0f);
    Actor_set_scale(&this->actor, 0.01f);
    temp_f0 = (24.0f * sp3C) + 12.0f;
    this->actor.prevPos.y -= temp_f0;
    this->actor.world.pos.y -= temp_f0;

    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    this->unk_1F0 = temp_f0;
    this->actor.world.pos.y += temp_f0;

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    chase_f(&this->unk_1E4, this->unk_1E8, 0.1f);
}
static Vec3f fire_pos[] = {
    { 5.0f, 0.0f, 0.0f },
    { -5.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 5.0f },
    { 0.0f, 0.0f, -5.0f },
};

void En_Ny_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNy* this = (EnNy*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ny.c", 837);
    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 1);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ny.c", 845);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_1D8);
    gSPDisplayList(POLY_XLU_DISP++, gEnNyMetalBodyDL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_1D4);
    gSPDisplayList(POLY_XLU_DISP++, gEnNyRockBodyDL);
    if (this->unk_1E0 > 0.25f) {
        Matrix_scale(this->unk_1E0, this->unk_1E0, this->unk_1E0, MTXMODE_APPLY);
        Actor_HiliteReflect_set_init(&this->actor, play, 1);
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ny.c", 868);
        gSPDisplayList(POLY_OPA_DISP++, gEnNySpikeDL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ny.c", 872);
    if (this->unk_1CA != 0) {
        Vec3f tempVec;
        Vec3f* fireOffset;
        s16 temp;

        temp = this->unk_1CA - 1;
        this->actor.colorFilterTimer++;
        if (temp == 0) {
            fireOffset = &fire_pos[temp & 3];
            tempVec.x = rnd_fx(5.0f) + (this->actor.world.pos.x + fireOffset->x);
            tempVec.y = rnd_fx(5.0f) + (this->actor.world.pos.y + fireOffset->y);
            tempVec.z = rnd_fx(5.0f) + (this->actor.world.pos.z + fireOffset->z);
            Effect_En_Fire_ct(play, &this->actor, &tempVec, 100, 0, 0, -1);
        }
    }
}

void En_Ny_eff_draw(Actor* thisx, PlayState* play) {
    EnNy* this = (EnNy*)thisx;
    Vec3f* temp;
    f32 scale;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ny.c", 900);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gDPPipeSync(POLY_OPA_DISP++);
    for (i = 0; i < 8; i++) {
        if (this->timer < (i + 22)) {
            temp = &this->unk_1F8[i];
            Matrix_translate(temp->x, temp->y, temp->z, MTXMODE_NEW);
            scale = this->actor.scale.x * 0.4f * (1.0f + (i * 0.04f));
            Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ny.c", 912);
            gSPDisplayList(POLY_OPA_DISP++, gEnNyRockBodyDL);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ny.c", 919);
    if (this->unk_1CA != 0) {
        Vec3f tempVec;
        Vec3f* fireOffset;
        s16 fireOffsetIndex;

        fireOffsetIndex = this->unk_1CA - 1;
        this->actor.colorFilterTimer++;
        if ((fireOffsetIndex & 1) == 0) {
            fireOffset = &fire_pos[fireOffsetIndex & 3];
            tempVec.x = rnd_fx(5.0f) + (this->actor.world.pos.x + fireOffset->x);
            tempVec.y = rnd_fx(5.0f) + (this->actor.world.pos.y + fireOffset->y);
            tempVec.z = rnd_fx(5.0f) + (this->actor.world.pos.z + fireOffset->z);
            Effect_En_Fire_ct(play, &this->actor, &tempVec, 100, 0, 0, -1);
        }
    }
}
