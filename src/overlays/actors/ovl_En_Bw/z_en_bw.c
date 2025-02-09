/*
 * File: z_en_bw.c
 * Overlay: ovl_En_Bw
 * Description: Torch slug
 */

#include "z_en_bw.h"
#include "versions.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_bw/object_bw.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Bw_Actor_ct(Actor* thisx, PlayState* play);
void En_Bw_Actor_dt(Actor* thisx, PlayState* play);
void En_Bw_Actor_move(Actor* thisx, PlayState* play2);
void En_Bw_Actor_draw(Actor* thisx, PlayState* play2);

void En_Bw_Actor_landing(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_wait_init(EnBw* this);
void En_Bw_Actor_mode_wait(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_find_init(EnBw* this);
void En_Bw_Actor_mode_find(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_jump_forward_init(EnBw* this);
void En_Bw_Actor_mode_jump_forward(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_rev_d_init(EnBw* this);
void En_Bw_Actor_mode_rev_d(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_rev_r_init(EnBw* this);
void En_Bw_Actor_mode_rev_r(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_down_init(EnBw* this);
void En_Bw_Actor_mode_down(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_damage_init(EnBw* this);
void En_Bw_Actor_mode_damage(EnBw* this, PlayState* play);
void En_Bw_Actor_mode_paralyze_init(EnBw* this);
void En_Bw_Actor_mode_paralyze(EnBw* this, PlayState* play);

ActorProfile En_Bw_Profile = {
    /**/ ACTOR_EN_BW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BW,
    /**/ sizeof(EnBw),
    /**/ En_Bw_Actor_ct,
    /**/ En_Bw_Actor_dt,
    /**/ En_Bw_Actor_move,
    /**/ En_Bw_Actor_draw,
};

static ColliderCylinderInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 30, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
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
    { 30, 35, 0, { 0, 0, 0 } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(2, 0xF),
    /* Ice arrow     */ DMG_ENTRY(4, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xE),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 check_time = 0;

void En_Bw_actor_set_process(EnBw* this, EnBwActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Bw_Actor_ct(Actor* thisx, PlayState* play) {
    EnBw* this = (EnBw*)thisx;

    Actor_set_scale(&this->actor, 0.012999999f);
    this->actor.naviEnemyId = NAVI_ENEMY_TORCH_SLUG;
    this->actor.gravity = -2.0f;
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gTorchSlugSkel, &gTorchSlugEyestalkWaveAnim, this->jointTable,
                   this->morphTable, TORCH_SLUG_LIMB_MAX);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 40.0f);
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.colChkInfo.health = 6;
#if OOT_VERSION >= PAL_1_0
    this->actor.colChkInfo.mass = MASS_HEAVY;
#endif
    this->actor.focus.pos = this->actor.world.pos;
    En_Bw_Actor_mode_wait_init(this);
    this->color1.a = this->color1.r = 255;
    this->color1.g = this->color1.b = 0;
    this->unk_248 = 0.6f;
    this->unk_221 = 3;
    ClObjPipe_ct(play, &this->collider1);
    //! this->collider2 should have Init called on it, but it doesn't matter since the heap is zeroed before use.
    ClObjPipe_set5(play, &this->collider1, &this->actor, &AtInfoData);
    ClObjPipe_set5(play, &this->collider2, &this->actor, &OcInfoData);
    this->unk_236 = this->actor.world.rot.y;
    this->actor.params = check_time;
    check_time = (check_time + 1) & 3;
}

void En_Bw_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBw* this = (EnBw*)thisx;

    ClObjPipe_dt(play, &this->collider1);
    ClObjPipe_dt(play, &this->collider2);
}

void En_Bw_Actor_landing(EnBw* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    this->unk_222 -= 250;
    this->actor.scale.x = 0.013f + sinf_table(this->unk_222 * 0.001f) * 0.0069999998f;
    this->actor.scale.y = 0.013f - sinf_table(this->unk_222 * 0.001f) * 0.0069999998f;
    this->actor.scale.z = 0.013f + sinf_table(this->unk_222 * 0.001f) * 0.0069999998f;
    if (this->unk_222 == 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        En_Bw_Actor_mode_wait_init(this);
    }
}

void En_Bw_Actor_mode_wait_init(EnBw* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTorchSlugEyestalkWaveAnim, -2.0f);
    this->unk_220 = 2;
    this->unk_222 = fqrand() * 200.0f + 200.0f;
    this->unk_232 = 0;
    this->actor.speed = 0.0f;
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_wait);
}

void En_Bw_Actor_mode_wait(EnBw* this, PlayState* play) {
    CollisionPoly* sp74 = NULL;
    Vec3f sp68;
    u32 sp64 = 0;
    s16 sp62;
    s16 sp60;
    f32 sp5C;
    f32 sp58;
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->unk_244 = this->unk_250 + 0.1f;
    sp58 = cosf_table(this->unk_240);
    this->unk_240 += this->unk_244;
    if (this->unk_24C < 0.8f) {
        this->unk_24C += 0.1f;
    }
    this->actor.scale.x = 0.013f - sinf_table(this->unk_240) * (this->unk_24C * 0.004f);
    this->actor.scale.y = 0.013f - sinf_table(this->unk_240) * (this->unk_24C * 0.004f);
    this->actor.scale.z = 0.013f + sinf_table(this->unk_240) * (this->unk_24C * 0.004f);
    sp5C = cosf_table(this->unk_240);
    if (this->unk_232 == 0) {
        if (ABS(sp58) < ABS(sp5C)) {
            this->unk_232++;
        }
    } else {
        if (ABS(sp58) > ABS(sp5C)) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_WALK);
            this->unk_232 = 0;
        }
    }
    sp5C *= this->unk_24C * (10.0f * this->unk_244);
    this->actor.speed = ABS(sp5C);
    if (this->unk_221 != 1) {
        sp58 = sinf_table(this->unk_240);
        sp60 = ABS(sp58) * 85.0f;
        this->color1.g = sp60;
    }
    if ((((play->gameplayFrames % 4) == (u32)this->actor.params) && (this->actor.speed != 0.0f) &&
         (sp64 =
              T_BGCheck_LineCheck_poly_chgrp(&play->colCtx, &this->actor.world.pos, &this->unk_264, &sp68, &sp74, 1, 0, 0, 1))) ||
        (this->unk_222 == 0)) {
        if (sp74 != NULL) {
            sp74 = SEGMENTED_TO_VIRTUAL(sp74);
            sp62 = RAD_TO_BINANG(fatan2(sp74->normal.x, sp74->normal.z));
        } else {
            sp62 = this->actor.world.rot.y + 0x8000;
        }
        if ((this->unk_236 != sp62) || (sp64 == 0)) {
            if (T_BGCheck_LineCheck_poly_chgrp(&play->colCtx, &this->unk_270, &this->unk_288, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 2;
            }
            if (T_BGCheck_LineCheck_poly_chgrp(&play->colCtx, &this->unk_270, &this->unk_27C, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 4;
            }
            switch (sp64) {
                case 0:
                    this->unk_236 += this->unk_238;
                    FALLTHROUGH;
                case 1:
                    if (this->unk_221 == 3) {
                        if (play->gameplayFrames & 2) {
                            this->unk_238 = 0x4000;
                        } else {
                            this->unk_238 = -0x4000;
                        }
                    } else {
                        if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0.0f) {
                            this->unk_238 = 0x4000;
                        } else {
                            this->unk_238 = -0x4000;
                        }
                        if (this->unk_221 == 1) {
                            this->unk_238 = -this->unk_238;
                        }
                    }
                    break;
                case 2:
                    this->unk_236 += this->unk_238;
                    FALLTHROUGH;
                case 3:
                    this->unk_238 = 0x4000;
                    break;
                case 4:
                    this->unk_236 += this->unk_238;
                    FALLTHROUGH;
                case 5:
                    this->unk_238 = -0x4000;
                    break;
                case 7:
                    this->unk_238 = 0;
                    break;
            }
            if (sp64 != 6) {
                this->unk_236 = sp62;
            }
            this->unk_222 = (fqrand() * 200.0f) + 200.0f;
        }
    } else if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        if (this->unk_236 != this->actor.wallYaw) {
            sp64 = 1;
            this->unk_236 = this->actor.wallYaw;
            if (this->unk_221 == 3) {
                if (play->gameplayFrames & 0x20) {
                    this->unk_238 = 0x4000;
                } else {
                    this->unk_238 = -0x4000;
                }
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
                this->unk_222 = (fqrand() * 20.0f) + 160.0f;
            } else {
                if ((s16)(this->actor.yawTowardsPlayer - this->unk_236) >= 0) {
                    this->unk_238 = 0x4000;
                } else {
                    this->unk_238 = -0x4000;
                }
                if (this->unk_221 == 1) {
                    this->unk_238 = -this->unk_238;
                }
            }
        } else if (this->unk_221 == 0) {
            sp64 = T_BGCheck_LineCheck_poly_chgrp(&play->colCtx, &this->actor.world.pos, &player->actor.world.pos, &sp68, &sp74,
                                        1, 0, 0, 1);
            if (sp64 != 0) {
                sp74 = SEGMENTED_TO_VIRTUAL(sp74);
                sp60 = RAD_TO_BINANG(fatan2(sp74->normal.x, sp74->normal.z));
                if (this->unk_236 != sp60) {
                    if ((s16)(this->actor.yawTowardsPlayer - sp60) >= 0) {
                        this->unk_238 = 0x4000;
                    } else {
                        this->unk_238 = -0x4000;
                    }
                    this->unk_236 = sp60;
                }
            }
        }
    }
    this->unk_222--;
    if (this->unk_224 != 0) {
        this->unk_224--;
    }
    if ((this->unk_234 == 0) && !BG_point_check(&this->actor, play, 50.0f, this->unk_236 + this->unk_238)) {
        if (this->unk_238 != 0x4000) {
            this->unk_238 = 0x4000;
        } else {
            this->unk_238 = -0x4000;
        }
    }
    switch (this->unk_221) {
        case 3:
            add_calc(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if ((this->unk_224 == 0) && (this->actor.xzDistToPlayer < 200.0f) &&
                (ABS(this->actor.yDistToPlayer) < 50.0f) && Actor_player_direction_check(&this->actor, 0x1C70)) {
                En_Bw_Actor_mode_find_init(this);
            } else {
                add_calc_short_angle2(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speed * 1000.0f, 0);
            }
            break;
        case 0:
            add_calc(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if (sp64 == 0) {
                add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1,
                                   this->actor.speed * 1000.0f, 0);
                if ((this->actor.xzDistToPlayer < 90.0f) && (this->actor.yDistToPlayer < 50.0f) &&
                    Actor_player_direction_check(&this->actor, 0x1554) &&
                    BG_point_check(&this->actor, play, 71.24802f, this->actor.yawTowardsPlayer)) {
                    En_Bw_Actor_mode_jump_forward_init(this);
                }
            } else {
                add_calc_short_angle2(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speed * 1000.0f, 0);
            }
            if ((this->unk_224 == 0) || (ABS(this->actor.yDistToPlayer) > 60.0f) ||
                (player2->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14))) {
                this->unk_221 = 3;
                this->unk_224 = 150;
                this->unk_250 = 0.0f;
            }
            break;
        case 1:
            if (((sp64 == 0) && !(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) ||
                Actor_player_direction_check(&this->actor, 0x1C70)) {
                if (Actor_player_direction_check(&this->actor, 0x1C70)) {
                    this->unk_238 = -this->unk_238;
                }
                add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x8000, 1,
                                   this->actor.speed * 1000.0f, 0);
            } else {
                add_calc_short_angle2(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speed * 1000.0f, 0);
            }
            if (this->unk_224 <= 200) {
                sp60 = sin_s(this->unk_224 * (0x960 - this->unk_224)) * 55.0f;
                this->color1.r = 255 - ABS(sp60);
                sp60 = sin_s(this->unk_224 * (0x960 - this->unk_224)) * 115.0f;
                this->color1.g = ABS(sp60) + 85;
                sp60 = sin_s(0x960 - this->unk_224) * 255.0f;
                this->color1.b = ABS(sp60);
                if (this->unk_224 == 0) {
                    this->unk_221 = 3;
                    this->unk_250 = 0.0f;
                }
            }
            break;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Bw_Actor_mode_find_init(EnBw* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTorchSlugEyestalkRaiseAnim, -2.0f);
    this->unk_220 = 3;
    this->unk_221 = 0;
    this->unk_250 = 0.6f;
    this->unk_222 = 20;
    this->unk_224 = 0xBB8;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_AIM);
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_find);
}

void En_Bw_Actor_mode_find(EnBw* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->unk_222 > 0) {
            this->unk_222--;
        } else {
            this->unk_222 = 100;
        }
    }
    if (this->unk_222 >= 20) {
        this->unk_224 -= 250;
    }
    this->actor.scale.x = 0.013f - sinf_table(this->unk_224 * 0.001f) * 0.0034999999f;
    this->actor.scale.y = 0.013f + sinf_table(this->unk_224 * 0.001f) * 0.0245f;
    this->actor.scale.z = 0.013f - sinf_table(this->unk_224 * 0.001f) * 0.0034999999f;
    if (this->unk_224 == 0) {
        En_Bw_Actor_mode_wait_init(this);
        this->unk_224 = 200;
    }
}

void En_Bw_Actor_mode_jump_forward_init(EnBw* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTorchSlugEyestalkFlailAnim, -1.0f);
    this->actor.speed = 7.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->unk_220 = 4;
    this->unk_222 = 1000;
    this->actor.velocity.y = 11.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_jump_forward);
}

void En_Bw_Actor_mode_jump_forward(EnBw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 floorPolyType;

    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    this->unk_222 += 250;
    this->actor.scale.x = 0.013f - sinf_table(this->unk_222 * 0.001f) * 0.0034999999f;
    this->actor.scale.y = 0.013f + sinf_table(this->unk_222 * 0.001f) * 0.0245f;
    this->actor.scale.z = 0.013f - sinf_table(this->unk_222 * 0.001f) * 0.0034999999f;
    if (this->collider1.base.atFlags & AT_HIT) {
        this->collider1.base.atFlags &= ~AT_HIT;
        this->actor.speed = -6.0f;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if ((&player->actor == this->collider1.base.at) && !(this->collider1.base.atFlags & AT_BOUNCED)) {
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        floorPolyType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        if ((floorPolyType == FLOOR_TYPE_2) || (floorPolyType == FLOOR_TYPE_3) || (floorPolyType == FLOOR_TYPE_9)) {
            Actor_delete(&this->actor);
            return;
        }
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        this->unk_222 = 3000;
        this->actor.flags &= ~ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
        this->actor.speed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        En_Bw_actor_set_process(this, En_Bw_Actor_landing);
    }
}

void En_Bw_Actor_mode_rev_d_init(EnBw* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTorchSlugEyestalkFlailAnim, -1.0f);
    this->unk_220 = 5;
    this->unk_222 = 1000;
    this->unk_260 = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 11.0f;
    this->unk_25C = fqrand() * 0.25f + 1.0f;
    this->unk_224 = 0xBB8;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_rev_d);
}

void En_Bw_Actor_mode_rev_d(EnBw* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.z, 0x7FFF, 1, 0xFA0, 0);
    add_calc(&this->unk_248, 0.0f, 1.0f, 0.05f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        if ((play->gameplayFrames % 0x80) == 0) {
            this->unk_25C = (fqrand() * 0.25f) + 0.7f;
        }
        this->unk_221 = 4;
        this->unk_258 += this->unk_25C;
        add_calc(&this->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        }
        if (this->unk_224 != 0) {
            this->unk_224 -= 250;
            this->actor.scale.x = 0.013f + sinf_table(this->unk_224 * 0.001f) * 0.0069999998f;
            this->actor.scale.y = 0.013f - sinf_table(this->unk_224 * 0.001f) * 0.0069999998f;
            this->actor.scale.z = 0.013f + sinf_table(this->unk_224 * 0.001f) * 0.0069999998f;
        }
        this->unk_222--;
        if (this->unk_222 == 0) {
            En_Bw_Actor_mode_rev_r_init(this);
        }
    } else {
        this->color1.r -= 8;
        this->color1.g += 32;
        if (this->color1.r < 200) {
            this->color1.r = 200;
        }
        if (this->color1.g > 200) {
            this->color1.g = 200;
        }
        if (this->color1.b > 235) {
            this->color1.b = 255;
        } else {
            this->color1.b += 40;
        }
        if (this->actor.shape.yOffset < 1000.0f) {
            this->actor.shape.yOffset += 200.0f;
        }
    }
}

void En_Bw_Actor_mode_rev_r_init(EnBw* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTorchSlugEyestalkFlailAnim, -1.0f);
    this->unk_220 = 6;
    this->unk_222 = 1000;
    this->unk_221 = 3;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 11.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_rev_r);
}

void En_Bw_Actor_mode_rev_r(EnBw* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.z, 0, 1, 0xFA0, 0);
    add_calc(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        this->unk_222 = 0xBB8;
        this->unk_250 = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        En_Bw_actor_set_process(this, En_Bw_Actor_landing);
    }
    if (this->color1.r < 247) {
        this->color1.r += 8;
    } else {
        this->color1.r = 255;
    }
    if (this->color1.g < 32) {
        this->color1.g = 0;
    } else {
        this->color1.g -= 32;
    }
    if (this->color1.b < 40) {
        this->color1.b = 0;
    } else {
        this->color1.b -= 40;
    }
    if (this->actor.shape.yOffset > 0.0f) {
        this->actor.shape.yOffset -= 200.0f;
    }
}

void En_Bw_Actor_mode_down_init(EnBw* this) {
    this->unk_220 = 0;
    this->unk_222 = 40;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_DEAD);
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_down);
}

void En_Bw_Actor_mode_down(EnBw* this, PlayState* play) {
    if (this->unk_230) {
        this->actor.scale.x += 0.0002f;
        this->actor.scale.y -= 0.0002f;
        this->actor.scale.z += 0.0002f;
    }
    this->actor.shape.shadowAlpha = this->color1.a -= 6;
    this->unk_222--;
    if (this->unk_222 <= 0) {
        Actor_delete(&this->actor);
    }
}

void En_Bw_Actor_mode_damage_init(EnBw* this) {
    this->unk_220 = 1;
    this->actor.speed = 0.0f;
    this->unk_25C = (fqrand() * 0.25f) + 1.0f;
    this->unk_260 = 0.0f;
    if (this->damageEffect == 0xE) {
        this->iceTimer = 0x50;
    }
    this->unk_222 = (this->actor.colorFilterParams & 0x4000) ? 25 : 80;
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_damage);
}

void En_Bw_Actor_mode_damage(EnBw* this, PlayState* play) {
    this->color1.r -= 1;
    this->color1.g += 4;
    this->color1.b += 5;
    if (this->color1.r < 200) {
        this->color1.r = 200;
    }
    if (this->color1.g > 200) {
        this->color1.g = 200;
    }
    if (this->color1.b > 230) {
        this->color1.b = 230;
    }
    if (this->actor.colorFilterParams & 0x4000) {
        if ((play->gameplayFrames % 0x80) == 0) {
            this->unk_25C = 0.5f + fqrand() * 0.25f;
        }
        this->unk_258 += this->unk_25C;
        add_calc(&this->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
    }
    this->unk_222--;
    if (this->unk_222 == 0) {
        En_Bw_Actor_mode_wait_init(this);
        this->color1.r = this->color1.g = 200;
        this->color1.b = 255;
        this->unk_224 = 0x258;
        this->unk_221 = 1;
        this->unk_250 = 0.7f;
        this->unk_236++;
    }
}

void En_Bw_Actor_mode_paralyze_init(EnBw* this) {
    this->actor.speed = 0.0f;
    if (this->damageEffect == 0xE) {
        this->iceTimer = 32;
    }
    this->unk_23C = this->actor.colorFilterTimer;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    En_Bw_actor_set_process(this, En_Bw_Actor_mode_paralyze);
}

void En_Bw_Actor_mode_paralyze(EnBw* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        this->unk_23C = 0;
        if (this->actor.colChkInfo.health != 0) {
            if ((this->unk_220 != 5) && (this->unk_220 != 6)) {
                En_Bw_Actor_mode_wait_init(this);
                this->color1.r = this->color1.g = 200;
                this->color1.b = 255;
                this->unk_224 = 0x258;
                this->unk_221 = 1;
                this->unk_250 = 0.7f;
                this->unk_236++;
            } else if (this->unk_220 != 5) {
                En_Bw_actor_set_process(this, En_Bw_Actor_mode_rev_r);
            } else {
                En_Bw_Actor_mode_rev_r_init(this);
            }
        } else {
            if (HammerSwingCheck(play, &this->collider2.base)) {
                this->unk_230 = 0;
                this->actor.scale.y -= 0.009f;
                _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
            } else {
                this->unk_230 = 1;
            }
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x90);
            En_Bw_Actor_mode_down_init(this);
        }
    }
}

void En_Bw_damage_proc(EnBw* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->unk_230 = 0;
        this->actor.scale.y -= 0.009f;
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        En_Bw_Actor_mode_down_init(this);
    } else {
        if (this->collider2.base.acFlags & AC_HIT) {
            this->collider2.base.acFlags &= ~AC_HIT;
            if ((this->actor.colChkInfo.damageEffect == 0) || (this->unk_220 == 6)) {
                return;
            }
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            Hit_bit_set(&this->actor, &this->collider2.elem, false);
            if ((this->damageEffect == 1) || (this->damageEffect == 0xE)) {
                if (this->unk_23C == 0) {
                    hp_down(&this->actor);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);
                    En_Bw_Actor_mode_paralyze_init(this);
                    this->unk_248 = 0.0f;
                }
                return;
            }
            if (this->unk_248 == 0.0f) {
                hp_down(&this->actor);
            }
            if (((this->unk_221 == 1) || (this->unk_221 == 4)) && (this->actor.colChkInfo.health == 0)) {
                if (this->unk_220 != 0) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                    if (HammerSwingCheck(play, &this->collider2.base)) {
                        this->unk_230 = 0;
                        this->actor.scale.y -= 0.009f;
                        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0,
                                                 false);
                    } else {
                        this->unk_230 = 1;
                    }
                    Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x90);
                    En_Bw_Actor_mode_down_init(this);
                }
            } else if ((this->unk_220 != 1) && (this->unk_220 != 6)) {
                Actor_SE_set(&this->actor, NA_SE_EN_BUBLEWALK_DAMAGE);
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                if (this->unk_220 != 5) {
                    En_Bw_Actor_mode_damage_init(this);
                }
                this->unk_248 = 0.0f;
            }
        }
        if ((play->actorCtx.unk_02 != 0) && (this->actor.xzDistToPlayer <= 400.0f) &&
            (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            if (this->unk_220 == 5) {
                this->unk_23C = 0;
                En_Bw_Actor_mode_rev_r_init(this);
            } else if (this->unk_220 != 0) {
                this->unk_23C = 0;
                En_Bw_Actor_mode_rev_d_init(this);
            }
        }
    }
}

void En_Bw_Actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBw* this = (EnBw*)thisx;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s32 pad[3]; // probably an unused Vec3f
    Color_RGBA8 sp50 = { 255, 200, 0, 255 };
    Color_RGBA8 sp4C = { 255, 80, 0, 255 };
    Color_RGBA8 sp48 = { 0, 0, 0, 255 };
    Color_RGBA8 sp44 = { 0, 0, 0, 220 };

    En_Bw_damage_proc(this, play);
    if (thisx->colChkInfo.damageEffect != 6) {
        this->actionFunc(this, play);
        if (this->unk_23C == 0) {
            this->unk_23A = (this->unk_23A + 4) & 0x7F;
        }
        if ((play->gameplayFrames & this->unk_221) == 0) {
            accel.y = -3.0f;
            velocity.x = rnd_fx(this->unk_248 * 24.0f);
            velocity.y = this->unk_248 * 30.0f;
            velocity.z = rnd_fx(this->unk_248 * 24.0f);
            accel.x = velocity.x * -0.075f;
            accel.z = velocity.z * -0.075f;
            Effect_SS_Dust_sc_cl_co_ct(play, &thisx->world.pos, &velocity, &accel, &sp50, &sp4C, 0x3C, 0, 0x14);
        }
        if (this->unk_248 <= 0.4f) {
            this->collider1.elem.atDmgInfo.effect = 0;
            if (((play->gameplayFrames & 1) == 0) && (this->unk_220 < 5) && (this->unk_23C == 0)) {
                accel.y = -0.1f;
                velocity.x = rnd_fx(4.0f);
                velocity.y = rnd_fx(2.0f) + 6.0f;
                velocity.z = rnd_fx(4.0f);
                accel.x = velocity.x * -0.1f;
                accel.z = velocity.z * -0.1f;
                rnd_fx(4.0f);
                rnd_fx(4.0f);
                sp48.a = this->color1.a;
                if (sp48.a >= 30) {
                    sp44.a = sp48.a - 30;
                } else {
                    sp44.a = 0;
                }
                Effect_SS_Dust_sc_cl_co_ct(play, &thisx->world.pos, &velocity, &accel, &sp48, &sp44, 0xB4, 0x28,
                              20.0f - (this->unk_248 * 40.0f));
            }
        } else {
            this->collider1.elem.atDmgInfo.effect = 1;
        }

        this->unk_234 = BG_point_check(thisx, play, 50.0f, thisx->world.rot.y);
        if ((this->unk_220 == 4) || (this->unk_220 == 6) || (this->unk_220 == 5) || (this->unk_220 == 1) ||
            (this->unk_234 != 0)) {
            Actor_position_moveF(thisx);
        }
        Actor_BGcheck2(play, thisx, 20.0f, 30.0f, 21.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 |
                                    UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider2);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider2.base);
    if ((this->unk_220 != 0) && ((thisx->colorFilterTimer == 0) || !(thisx->colorFilterParams & 0x4000))) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider2.base);
    }
    if ((this->unk_221 != 1) && (this->unk_220 < 5) && (this->unk_248 > 0.4f)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider1);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider1.base);
    }
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 5.0f;
}

s32 en_bw_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnBw* this = (EnBw*)thisx;

    if (limbIndex == TORCH_SLUG_LIMB_BODY) {
        gSPSegment(
            (*gfx)++, 0x09,
            two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, 0, this->unk_23A, 0x20, 0x20));
        if ((this->unk_220 == 1) || (this->unk_220 == 5)) {
            Matrix_push();
            Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            Matrix_rotateX(this->unk_258 * 0.115f, MTXMODE_APPLY);
            Matrix_rotateY(this->unk_258 * 0.13f, MTXMODE_APPLY);
            Matrix_rotateZ(this->unk_258 * 0.1f, MTXMODE_APPLY);
            Matrix_scale(1.0f - this->unk_260, 1.0f + this->unk_260, 1.0f - this->unk_260, MTXMODE_APPLY);
            Matrix_rotateZ(-(this->unk_258 * 0.1f), MTXMODE_APPLY);
            Matrix_rotateY(-(this->unk_258 * 0.13f), MTXMODE_APPLY);
            Matrix_rotateX(-(this->unk_258 * 0.115f), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_bw.c", 1388);
            gSPDisplayList((*gfx)++, *dList);
            Matrix_pull();
            return 1;
        }
    }
    return 0;
}

static Vec3f ice_pos[] = {
    { 20.0f, 10.0f, 0.0f },  { -20.0f, 10.0f, 0.0f }, { 0.0f, 10.0f, -25.0f }, { 10.0f, 0.0f, 15.0f },
    { 10.0f, 0.0f, -15.0f }, { 0.0f, 10.0f, 25.0f },  { -10.0f, 0.0f, 15.0f }, { -10.0f, 0.0f, -15.0f },
};

void En_Bw_Actor_draw(Actor* thisx, PlayState* play2) {
    Vec3f spAC = { 0.0f, 0.0f, 0.0f };
    PlayState* play = play2;
    EnBw* this = (EnBw*)thisx;
    Vec3f icePos;
    s32 iceIndex;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bw.c", 1423);

    if (this->color1.a == 0xFF) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, this->color1.r, this->color1.g, this->color1.b, this->color1.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        POLY_OPA_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_bw_display1, NULL, this, POLY_OPA_DISP);
    } else {
        _texture_z_light_fog_prim_xlu(play2->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, this->color1.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->color1.r, this->color1.g, this->color1.b, this->color1.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, &Actor_change_render_mode[0]);
        POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_bw_display1, NULL, this, POLY_XLU_DISP);
    }

    if (((play->gameplayFrames + 1) % 4) == (u32)thisx->params) {
        spAC.z = thisx->scale.z * 375000.0f;
        Matrix_Position(&spAC, &this->unk_264);
        spAC.z = thisx->scale.z * 150000.0f;
        Matrix_Position(&spAC, &this->unk_270);
        spAC.x = thisx->scale.x * 550000.0f;
        Matrix_Position(&spAC, &this->unk_288);
        spAC.x = -spAC.x;
        Matrix_Position(&spAC, &this->unk_27C);
    }

    Matrix_translate(thisx->world.pos.x, thisx->world.pos.y + ((thisx->scale.y - 0.013f) * 1000.0f), thisx->world.pos.z,
                     MTXMODE_NEW);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                (play->gameplayFrames * -20) % 0x200, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
    Matrix_scale(this->unk_248 * 0.01f, this->unk_248 * 0.01f, this->unk_248 * 0.01f, MTXMODE_APPLY);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_bw.c", 1500);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    if (this->iceTimer != 0) {
        thisx->colorFilterTimer++;
        this->iceTimer--;
        if ((this->iceTimer & 3) == 0) {
            iceIndex = this->iceTimer >> 2;

            icePos.x = ice_pos[iceIndex].x + thisx->world.pos.x;
            icePos.y = ice_pos[iceIndex].y + thisx->world.pos.y;
            icePos.z = ice_pos[iceIndex].z + thisx->world.pos.z;
            Effect_En_Ice_ct0(play, thisx, &icePos, 0x96, 0x96, 0x96, 0xFA, 0xEB, 0xF5, 0xFF, 1.3f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bw.c", 1521);
}
