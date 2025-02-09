/*
 * File: z_en_torch2.c
 * Overlay: ovl_En_Torch2
 * Description: Dark Link
 */

#include "z_en_torch2.h"
#include "assets/objects/object_torch2/object_torch2.h"
#include "libu64/pad.h"
#include "versions.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum EnTorch2ActionStates {
    /* 0 */ ENTORCH2_WAIT,
    /* 1 */ ENTORCH2_ATTACK,
    /* 2 */ ENTORCH2_DEATH,
    /* 3 */ ENTORCH2_DAMAGE
} EnTorch2ActionStates;

void En_Torch2_Actor_ct(Actor* thisx, PlayState* play2);
void En_Torch2_Actor_dt(Actor* thisx, PlayState* play);
void En_Torch2_Actor_move(Actor* thisx, PlayState* play2);
void En_Torch2_Actor_draw(Actor* thisx, PlayState* play2);

ActorProfile En_Torch2_Profile = {
    /**/ ACTOR_EN_TORCH2,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_TORCH2,
    /**/ sizeof(Player),
    /**/ En_Torch2_Actor_ct,
    /**/ En_Torch2_Actor_dt,
    /**/ En_Torch2_Actor_move,
    /**/ En_Torch2_Actor_draw,
};

static f32 stick_pow = 0.0f;
static s16 stick_y = 0;
static f32 plus_y = 0.0f;
static s32 shield_timer = 0;
static u8 z_flg = false;
static u8 L_LIFE = false;

static Input action_pad;
static u8 sp_flg;
static Vec3f start_pos;
static u8 no_rock_timer;
static u8 at_flg;
static u8 bl_mode;
static u8 bl_timer0;
static u8 bl_defswing;
static u8 bl_roll;
static u8 ds_count;
static u8 ds_timer;
static s8 ktype_b;
static u8 alpha;

static DamageTable btl_data = {
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
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(2, 0xE),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(3, 0xD),
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

void En_Torch2_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    action_pad.cur.button = action_pad.press.button = action_pad.rel.button = 0;
    action_pad.cur.stick_x = action_pad.cur.stick_y = 0;
    this->currentShield = PLAYER_SHIELD_HYLIAN;
    this->heldItemAction = this->heldItemId = PLAYER_IA_SWORD_MASTER;
    shape_type_set(this, PLAYER_MODELGROUP_SWORD_AND_SHIELD);
    play->playerInit(this, play, &gDarkLinkSkel);
    this->actor.naviEnemyId = NAVI_ENEMY_DARK_LINK;
    this->cylinder.base.acFlags = AC_ON | AC_TYPE_PLAYER;
    this->meleeWeaponQuads[0].base.atFlags = this->meleeWeaponQuads[1].base.atFlags = AT_ON | AT_TYPE_ENEMY;
    this->meleeWeaponQuads[0].base.acFlags = this->meleeWeaponQuads[1].base.acFlags = AC_ON | AC_HARD | AC_TYPE_PLAYER;
    this->meleeWeaponQuads[0].base.colMaterial = this->meleeWeaponQuads[1].base.colMaterial = COL_MATERIAL_METAL;
    this->meleeWeaponQuads[0].elem.atDmgInfo.damage = this->meleeWeaponQuads[1].elem.atDmgInfo.damage = 8;
    this->meleeWeaponQuads[0].elem.acElemFlags = this->meleeWeaponQuads[1].elem.acElemFlags = ACELEM_ON;
    this->shieldQuad.base.atFlags = AT_ON | AT_TYPE_ENEMY;
    this->shieldQuad.base.acFlags = AC_ON | AC_HARD | AC_TYPE_PLAYER;
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.colChkInfo.health = z_common_data.save.info.playerData.healthCapacity >> 3;
    this->actor.colChkInfo.cylRadius = 60;
    this->actor.colChkInfo.cylHeight = 100;
    play->func_11D54(this, play);

    bl_mode = ENTORCH2_WAIT;
    bl_roll = 0;
    plus_y = 0.0f;
    sp_flg = 0;
    no_rock_timer = 0;
    at_flg = false;
    bl_defswing = ds_timer = ds_count = 0;
    ktype_b = 0;
    alpha = 95;
    start_pos = this->actor.home.pos;
}

void En_Torch2_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    Player* this = (Player*)thisx;

    EffectFreeIndex(play, this->meleeWeaponEffectIndex);
    Na_StopMiddleBossBgm();
    ClObjPipe_dt(play, &this->cylinder);
    ClObjSwrd_dt(play, &this->meleeWeaponQuads[0]);
    ClObjSwrd_dt(play, &this->meleeWeaponQuads[1]);
    ClObjSwrd_dt(play, &this->shieldQuad);
}

static Actor* Shot_def(PlayState* play, Player* this) {
    Actor* rangedItem = ShotVsMyCheck(play, &this->actor, 4000.0f);

    if (rangedItem != NULL) {
        return rangedItem;
    } else {
        return BlastVsMyCheck_c(play, &this->actor);
    }
}

s32 kiru_sub(PlayState* play, Input* input, Player* this) {
    f32 noAttackChance = 0.0f;
    s32 attackDelay = 7;
    Player* player = GET_PLAYER(play);

    if ((this->speedXZ < 0.0f) || (player->speedXZ < 0.0f)) {
        return 0;
    }
    if (z_common_data.save.info.playerData.health < 0x50) {
        attackDelay = 15;
        noAttackChance += 0.3f;
    }
    if (alpha != 255) {
        noAttackChance += 2.0f;
    }
    if ((((play->gameplayFrames & attackDelay) == 0) || (sp_flg != 0)) && (noAttackChance <= fqrand())) {
        if (sp_flg == 0) {
            switch ((s32)(fqrand() * 7.0f)) {
                case 1:
                case 5:
                    stick_y += 0x4000;
                    stick_pow = 127.0f;
                    break;
                case 2:
                case 6:
                    stick_y -= 0x4000;
                    stick_pow = 127.0f;
                    break;
            }
        }
        input->cur.button = BTN_B;
        return 1;
    }
    return 0;
}

void back_jump_set(Player* this, Input* input, Actor* thisx) {
    thisx->world.rot.y = thisx->shape.rot.y = thisx->yawTowardsPlayer;
    stick_y = thisx->yawTowardsPlayer + 0x8000;
    stick_pow = 127.0f;
    z_flg = true;
    input->cur.button = BTN_A;
    this->invincibilityTimer = 10;
    bl_defswing = 0;
}

void En_Torch2_Actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* player2 = GET_PLAYER(play2);
    Player* player = player2;
    Player* this = (Player*)thisx;
    Input* input = &action_pad;
    Camera* mainCam;
    s16 sp66;
    s8 stickY;
    u32 pad54;
    Actor* attackItem;
    s16 sp5A;

    sp5A = player->actor.shape.rot.y - this->actor.shape.rot.y;
    input->cur.button = 0;
    mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
    attackItem = Shot_def(play, this);
    switch (bl_mode) {
        case ENTORCH2_WAIT:
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            this->skelAnime.curFrame = 0.0f;
            this->skelAnime.playSpeed = 0.0f;
            this->actor.world.pos.x = (sin_s(this->actor.world.rot.y) * 25.0f) + start_pos.x;
            this->actor.world.pos.z = (cos_s(this->actor.world.rot.y) * 25.0f) + start_pos.z;
            if ((this->actor.xzDistToPlayer <= 120.0f) || Anc_Fight_My_Check(play, &this->actor) ||
                (attackItem != NULL)) {
                if (attackItem != NULL) {
                    bl_roll = 1;
                    stick_y = this->actor.yawTowardsPlayer;
                    stick_pow = 127.0f;
                    input->cur.button = BTN_A;
                    z_flg = false;
                    sp66 = mainCam->camDir.y - stick_y;
                    action_pad.cur.stick_x = stick_pow * sin_s(sp66);
                    stickY = stick_pow * cos_s(sp66);
                    if (stickY) {}
                    action_pad.cur.stick_y = stickY;
                }
                Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
                bl_mode = ENTORCH2_ATTACK;
            }
            break;

        case ENTORCH2_ATTACK:
            stick_pow = 0.0f;

            // Handles Dark Link's sword clanking on Link's sword

            if ((this->meleeWeaponQuads[0].base.acFlags & AC_BOUNCED) ||
                (this->meleeWeaponQuads[1].base.acFlags & AC_BOUNCED)) {
                this->meleeWeaponQuads[0].base.acFlags &= ~AC_BOUNCED;
                this->meleeWeaponQuads[1].base.acFlags &= ~AC_BOUNCED;
                this->meleeWeaponQuads[0].base.atFlags |= AT_BOUNCED;
                this->meleeWeaponQuads[1].base.atFlags |= AT_BOUNCED;
                this->cylinder.base.acFlags &= ~AC_HIT;

                if (ktype_b != this->meleeWeaponAnimation) {
                    ds_count++;
                    ktype_b = this->meleeWeaponAnimation;
                }
                /*! @bug
                 *  This code is needed to reset bl_defswing, and should run regardless
                 *  of how much health Link has. Without it, bl_defswing stays at 2 until
                 *  something else resets it, preventing Dark Link from using his shield and
                 *  creating a hole in his defenses. This also makes Dark Link harder at low
                 *  health, while the other health checks are intended to make him easier.
                 */
                if ((z_common_data.save.info.playerData.health < 0x50) && (bl_defswing != 0)) {
                    bl_defswing = 0;
                    ds_timer = 50;
                }
            }
            if ((bl_defswing != 0) && (this->meleeWeaponState != 0)) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->meleeWeaponQuads[0].base);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->meleeWeaponQuads[1].base);
            }

            // Ignores hits when jumping on Link's sword
            if ((this->invincibilityTimer < 0) && (bl_mode != ENTORCH2_DAMAGE) &&
                (this->cylinder.base.acFlags & AC_HIT)) {
                this->cylinder.base.acFlags &= ~AC_HIT;
            }

            // Handles Dark Link rolling to dodge item attacks

            if (bl_roll != 0) {
                stick_pow = 127.0f;
            } else if (attackItem != NULL) {
                bl_roll = 1;
                stick_y = this->actor.yawTowardsPlayer;
                stick_pow = 127.0f;
                input->cur.button = BTN_A;
            } else if (no_rock_timer == 0) {

                // Handles Dark Link's initial reaction to jumpslashes

                if (((player->meleeWeaponState != 0) || (player->actor.velocity.y > -3.0f)) &&
                    (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START)) {
                    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;

                    if (play->gameplayFrames % 2) {
                        stick_y = this->actor.yawTowardsPlayer + 0x4000;
                    } else {
                        stick_y = this->actor.yawTowardsPlayer - 0x4000;
                    }
                    stick_pow = 127.0f;
                    no_rock_timer = 15;
                    at_flg = false;
                    input->cur.button |= BTN_A;

                    // Handles jumping on Link's sword

                } else if (sp_flg != 0) {
                    stick_pow = 0.0f;
                    player->stateFlags3 |= PLAYER_STATE3_2;
                    add_calc(&this->actor.world.pos.x,
                                       (sin_s(player->actor.shape.rot.y - 0x3E8) * 45.0f) +
                                           player->actor.world.pos.x,
                                       1.0f, 5.0f, 0.0f);
                    add_calc(&this->actor.world.pos.z,
                                       (cos_s(player->actor.shape.rot.y - 0x3E8) * 45.0f) +
                                           player->actor.world.pos.z,
                                       1.0f, 5.0f, 0.0f);
                    bl_timer0--;
                    if (((u32)bl_timer0 == 0) ||
                        ((player->invincibilityTimer > 0) && (this->meleeWeaponState == 0))) {
                        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                        input->cur.button = BTN_A;
                        player->stateFlags3 &= ~PLAYER_STATE3_2;
                        stick_pow = 127.0f;
                        player->skelAnime.curFrame = 3.0f;
                        stick_y = this->actor.yawTowardsPlayer + 0x8000;
                        bl_timer0 = sp_flg = 0;
                        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    } else if (sp_flg == 1) {
                        if (bl_timer0 < 16) {
                            kiru_sub(play, input, this);
                            sp_flg++;
                        } else if (bl_timer0 == 19) {
                            Na_SetBlackLinkVoice(&this->actor.projectedPos, NA_SE_VO_LI_AUTO_JUMP);
                        }
                    }
                } else {
                    // This does nothing, as shield_timer is never set.
                    if (shield_timer != 0) {
                        shield_timer--;
                        input->cur.button = BTN_R;
                    }

                    // Handles Dark Link's reaction to sword attack other than jumpslashes

                    if (PlayerSwingCheck(play, &this->actor, 120.0f, 0x7FFF, 0x7FFF, this->actor.world.rot.y)) {
                        if ((player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H) &&
                            (this->actor.xzDistToPlayer < 90.0f)) {

                            // Handles the reaction to a one-handed stab. If the conditions are satisfied,
                            // Dark Link jumps on Link's sword. Otherwise he backflips away.

                            if ((this->meleeWeaponState == 0) && (bl_defswing == 0) &&
                                (player->invincibilityTimer == 0) &&
                                (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H) &&
                                (this->actor.xzDistToPlayer <= 85.0f) && Anc_Fight_My_Check(play, &this->actor)) {

                                stick_pow = 0.0f;
                                sp_flg = 1;
                                player->stateFlags3 |= PLAYER_STATE3_2;
                                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                                bl_timer0 = 27;
                                player->meleeWeaponState = 0;
                                player->speedXZ = 0.0f;
                                this->invincibilityTimer = -7;
                                this->speedXZ = 0.0f;
                                player->skelAnime.curFrame = 2.0f;
                                Skeleton_Info_Rom_anime_play(play, &player->skelAnime);
                                shield_timer = 0;
                                input->cur.button = BTN_A;
                            } else {
                                back_jump_set(this, input, &this->actor);
                            }
                        } else {

                            // Handles reactions to all other sword attacks

                            stick_y = thisx->yawTowardsPlayer;
                            input->cur.button = BTN_B;

                            if (player->meleeWeaponAnimation <= PLAYER_MWA_FORWARD_COMBO_2H) {
                                stick_pow = 0.0f;
                            } else if (player->meleeWeaponAnimation <= PLAYER_MWA_RIGHT_COMBO_2H) {
                                stick_pow = 127.0f;
                                stick_y += 0x4000;
                            } else if (player->meleeWeaponAnimation <= PLAYER_MWA_LEFT_COMBO_2H) {
                                stick_pow = 127.0f;
                                stick_y -= 0x4000;
                            } else if (player->meleeWeaponAnimation <= PLAYER_MWA_HAMMER_SIDE) {
                                input->cur.button = BTN_R;
                            } else if (player->meleeWeaponAnimation <= PLAYER_MWA_BIG_SPIN_2H) {
                                back_jump_set(this, input, &this->actor);
                            } else {
                                back_jump_set(this, input, &this->actor);
                            }
                            if (!CHECK_BTN_ANY(input->cur.button, BTN_A | BTN_R) && (this->meleeWeaponState == 0) &&
                                (player->meleeWeaponState != 0)) {
                                bl_defswing = 1;
                            }
                        }
                    } else {

                        // Handles movement and attacks when not reacting to Link's actions

                        stick_y = thisx->yawTowardsPlayer;
                        if ((90.0f >= this->actor.xzDistToPlayer) && (this->actor.xzDistToPlayer > 70.0f) &&
                            (ABS(sp5A) >= 0x7800) &&
                            (this->actor.isLockedOn || !(player->stateFlags1 & PLAYER_STATE1_SHIELDING))) {
                            kiru_sub(play, input, this);
                        } else {
                            f32 sp50 = 0.0f;

                            if (((this->actor.xzDistToPlayer <= 70.0f) ||
                                 ((this->actor.xzDistToPlayer <= 80.0f + sp50) && (player->meleeWeaponState != 0))) &&
                                (this->meleeWeaponState == 0)) {
                                if (!kiru_sub(play, input, this) && (this->meleeWeaponState == 0) &&
                                    (bl_defswing == 0)) {
                                    back_jump_set(this, input, &this->actor);
                                }
                            } else if (this->actor.xzDistToPlayer <= 50 + sp50) {
                                stick_pow = 127.0f;
                                stick_y = this->actor.yawTowardsPlayer;
                                if (!this->actor.isLockedOn) {
                                    add_calc_short_angle2(&stick_y, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328, 0);
                                }
                            } else if (this->actor.xzDistToPlayer > 100.0f + sp50) {
                                if ((player->meleeWeaponState == 0) ||
                                    !((player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) &&
                                      (player->meleeWeaponAnimation <= PLAYER_MWA_BIG_SPIN_2H)) ||
                                    (this->actor.xzDistToPlayer >= 280.0f)) {
                                    stick_pow = 127.0f;
                                    stick_y = this->actor.yawTowardsPlayer;
                                    if (!this->actor.isLockedOn) {
                                        add_calc_short_angle2(&stick_y, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328,
                                                           0);
                                    }
                                } else {
                                    back_jump_set(this, input, &this->actor);
                                }
                            } else if (((ABS(sp5A) < 0x7800) && (ABS(sp5A) >= 0x3000)) ||
                                       !kiru_sub(play, input, this)) {
                                stick_y = this->actor.yawTowardsPlayer;
                                stick_pow = 127.0f;
                                if (!this->actor.isLockedOn) {
                                    add_calc_short_angle2(&stick_y, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328, 0);
                                }
                            }
                        }
                    }
                }

                // Handles Dark Link's counterattack to jumpslashes

            } else if (at_flg && (alpha == 255) && (this->actor.velocity.y > 0)) {
                input->cur.button |= BTN_B;
            } else if (!at_flg && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                stick_y = this->actor.yawTowardsPlayer;
                if (alpha != 255) {
                    stick_y += 0x8000;
                    stick_pow = 127.0f;
                    z_flg = true;
                }
                input->cur.button |= BTN_A;
                at_flg = true;
                this->invincibilityTimer = 10;
            }

            // Rotates Dark Link's stick angle from Link-relative to camera-relative.

            sp66 = mainCam->camDir.y - stick_y;
            action_pad.cur.stick_x = stick_pow * sin_s(sp66);
            stickY = stick_pow * cos_s(sp66);
            if (alpha) {}
            action_pad.cur.stick_y = stickY;

            if ((alpha != 255) && ((play->gameplayFrames % 8) == 0)) {
                alpha++;
            }
            break;

        case ENTORCH2_DAMAGE:
            this->meleeWeaponState = 0;
            input->cur.stick_x = input->cur.stick_y = 0;
            if ((this->invincibilityTimer > 0) && (this->actor.world.pos.y < (this->actor.floorHeight - 160.0f))) {
                this->stateFlags3 &= ~PLAYER_STATE3_0;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->invincibilityTimer = 0;
                this->actor.velocity.y = 0.0f;
                this->actor.world.pos.y = start_pos.y + 40.0f;
                this->actor.world.pos.x = (sin_s(player->actor.shape.rot.y) * -120.0f) + player->actor.world.pos.x;
                this->actor.world.pos.z = (cos_s(player->actor.shape.rot.y) * -120.0f) + player->actor.world.pos.z;
#if OOT_VERSION < NTSC_1_2
                if (Actor_search_position_distance(&this->actor, &start_pos) > 1000.0f)
#else
                if (Actor_search_position_distance(&this->actor, &start_pos) > 800.0f)
#endif
                {
                    f32 sp50 = fqrand() * 20.0f;
                    s16 sp4E = rnd_fx(4000.0f);

                    this->actor.shape.rot.y = this->actor.world.rot.y =
                        search_position_angleY(&start_pos, &player->actor.world.pos);
                    this->actor.world.pos.x =
                        (sin_s(this->actor.world.rot.y + sp4E) * (25.0f + sp50)) + start_pos.x;
                    this->actor.world.pos.z =
                        (cos_s(this->actor.world.rot.y + sp4E) * (25.0f + sp50)) + start_pos.z;
                    this->actor.world.pos.y = start_pos.y;
                } else {
                    this->actor.world.pos.y = this->actor.floorHeight;
                }
#if OOT_VERSION >= NTSC_1_2
                xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
#endif
                play->func_11D54(this, play);
                bl_mode = ENTORCH2_ATTACK;
                stick_pow = 0.0f;
                if (alpha != 255) {
                    ds_count = 0;
                    ds_timer = 0;
                }
            }
            break;

        case ENTORCH2_DEATH:
            if (alpha - 13 <= 0) {
                alpha = 0;
                Actor_delete(&this->actor);
                return;
            }
            alpha -= 13;
            this->actor.shape.shadowAlpha -= 13;
            break;
    }

    // Causes Dark Link to shield in place when Link is using magic attacks other than the spin attack

    if ((z_common_data.magicState == MAGIC_STATE_METER_FLASH_1) &&
        (player->meleeWeaponState == 0 || !((player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) &&
                                            (player->meleeWeaponAnimation <= PLAYER_MWA_BIG_SPIN_2H)))) {
        stick_pow = 0.0f;
        input->cur.stick_x = 0;
        input->cur.stick_y = 0;
        input->cur.button = BTN_R;
    }

    if ((bl_mode == ENTORCH2_ATTACK) && (this->actor.xzDistToPlayer <= 610.0f) && z_flg) {
        input->cur.button |= BTN_Z;
    }

    // Updates Dark Link's "controller". The conditional seems to cause him to
    // stop targeting and hold shield if he's been holding it long enough.

    pad54 = input->prev.button ^ input->cur.button;
    input->press.button = input->cur.button & pad54;
    if (CHECK_BTN_ANY(input->cur.button, BTN_R)) {
        input->cur.button = ((bl_defswing == 0) && (this->meleeWeaponState == 0)) ? BTN_R : input->cur.button ^ BTN_R;
    }
    input->rel.button = input->prev.button & pad54;
    input->prev.button = input->cur.button & (u16) ~(BTN_A | BTN_B);
    pad_correct_stick(input);

    input->press.stick_x += (s8)(input->cur.stick_x - input->prev.stick_x);
    input->press.stick_y += (s8)(input->cur.stick_y - input->prev.stick_y);

    // Handles Dark Link being damaged

    if ((this->actor.colChkInfo.health == 0) && L_LIFE) {
        this->csAction = PLAYER_CSACTION_24;
        this->csActor = &player->actor;
        this->cv.haltActorsDuringCsAction = true;
        L_LIFE = false;
    }
    if ((this->invincibilityTimer == 0) && (this->actor.colChkInfo.health != 0) &&
        (this->cylinder.base.acFlags & AC_HIT) && !(this->stateFlags1 & PLAYER_STATE1_26) &&
        !(this->meleeWeaponQuads[0].base.atFlags & AT_HIT) && !(this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {

        if (!hp_down(&this->actor)) {
            Na_StopMiddleBossBgm();
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            this->knockbackType = PLAYER_KNOCKBACK_LARGE;
            this->knockbackSpeed = 6.0f;
            this->knockbackYVelocity = 6.0f;
            this->knockbackDamage = this->actor.colChkInfo.damage;
            this->knockbackRot = this->actor.yawTowardsPlayer + 0x8000;
            L_LIFE++;
            bl_mode = ENTORCH2_DEATH;
            Actor_info_finish(play, &this->actor);
            Item_Set_Std(play, &this->actor, &thisx->world.pos, 0xC0);
            this->stateFlags3 &= ~PLAYER_STATE3_2;
        } else {
            Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
            if (this->actor.colChkInfo.damageEffect == 1) {
                if (alpha == 255) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
                } else {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, 80);
                }
            } else {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->knockbackDamage = this->actor.colChkInfo.damage;
                this->knockbackType = PLAYER_KNOCKBACK_SMALL;
                this->knockbackYVelocity = 6.0f;
                this->knockbackSpeed = 8.0f;
                this->knockbackRot = this->actor.yawTowardsPlayer + 0x8000;
                Hit_bit_set(&this->actor, &this->cylinder.elem, true);
                this->stateFlags3 &= ~PLAYER_STATE3_2;
                this->stateFlags3 |= PLAYER_STATE3_0;
                bl_mode = ENTORCH2_DAMAGE;
                if (alpha == 255) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
                } else {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 12);
                }
            }
        }
        this->actor.colChkInfo.damage = 0;
        this->knockbackDamage = 0;
    }

    // Handles being frozen by a deku nut

    if ((this->actor.colorFilterTimer == 0) || (this->actor.colorFilterParams & 0x4000)) {
        this->stateFlags3 &= ~PLAYER_STATE3_2;
    } else {
        this->stateFlags3 |= PLAYER_STATE3_2;
        this->stateFlags1 &= ~PLAYER_STATE1_26;
        this->invincibilityTimer = 0;
        input->press.stick_x = input->press.stick_y = 0;
        /*! @bug
         *  Setting cur.button to 0 clears the Z-trigger, causing Dark Link to break his
         *  lock on Link. If he presses A while not locked on, he'll put his sword away.
         *  This clears his held item param permanently and makes him unable to attack.
         */
        input->cur.button = 0;
        input->press.button = 0;
        this->speedXZ = 0.0f;
    }

    play->playerUpdate(this, play, input);

    /*
     * Handles sword clanks and removes their recoil for both Links. Dark Link staggers
     * if he's had to counter with enough different sword animations in a row.
     */
    if (this->speedXZ == -18.0f) {
        u8 staggerThreshold = (u32)rnd_fx(2.0f) + 6;

        if (z_common_data.save.info.playerData.health < 0x50) {
            staggerThreshold = (u32)rnd_fx(2.0f) + 3;
        }
        if (this->actor.xzDistToPlayer > 80.0f) {
            this->speedXZ = 1.2f;
        } else if (this->actor.xzDistToPlayer < 70.0f) {
            this->speedXZ = -1.5f;
        } else {
            this->speedXZ = 1.0f;
        }
        if (staggerThreshold < ds_count) {
            this->skelAnime.playSpeed *= 0.6f;
            Na_SetBlackLinkVoice(&this->actor.projectedPos, NA_SE_PL_DAMAGE);
            ds_timer = 0;
            ds_count = 0;
        }
    }
    if (player->speedXZ == -18.0f) {
        if (this->actor.xzDistToPlayer > 80.0f) {
            player->speedXZ = 1.2f;
        } else if (this->actor.xzDistToPlayer < 70.0f) {
            player->speedXZ = -1.5f;
        } else {
            player->speedXZ = 1.0f;
        }
    }
    /*
     * This ensures Dark Link's counter animation mirrors Link's exactly.
     */
    if ((bl_defswing != 0) && (bl_defswing == 1)) {
        if (this->meleeWeaponState == 0) {
            bl_defswing = 0;
        } else {
            bl_defswing = 2;
            this->meleeWeaponState = 1;
            this->skelAnime.curFrame = player->skelAnime.curFrame - player->skelAnime.playSpeed;
            this->skelAnime.playSpeed = player->skelAnime.playSpeed;
            Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
            ClObjSwrd_ATClear(play, &this->meleeWeaponQuads[0].base);
            ClObjSwrd_ATClear(play, &this->meleeWeaponQuads[1].base);
        }
    }
    if (ds_timer != 0) {
        ds_timer--;
        if (ds_timer == 0) {
            bl_defswing = 0;
            ds_count = 0;
        }
    }
    if (bl_roll != 0) {
        if (bl_roll == 1) {
            this->invincibilityTimer = 20;
        }
        bl_roll = (this->invincibilityTimer > 0) ? 2 : 0;
    }
    if (this->invincibilityTimer != 0) {
        this->cylinder.base.colMaterial = COL_MATERIAL_NONE;
        this->cylinder.elem.elemMaterial = ELEM_MATERIAL_UNK5;
    } else {
        this->cylinder.base.colMaterial = COL_MATERIAL_HIT5;
        this->cylinder.elem.elemMaterial = ELEM_MATERIAL_UNK1;
    }
    /*
     * Handles the jump movement onto Link's sword. Dark Link doesn't move during the
     * sword jump. Instead, his shape y-offset is increased (see below). Once the sword
     * jump is finished, the offset is added to his position to fix the discrepancy.
     */
    if (sp_flg != 0) {
        add_calc(&plus_y, 2630.0f, 1.0f, 2000.0f, 0.0f);
        this->actor.velocity.y -= 0.6f;
    } else if (plus_y != 0) {
        this->actor.world.pos.y += plus_y * 0.01f;
        plus_y = 0;
    }
    if ((bl_mode == ENTORCH2_WAIT) || (this->invincibilityTimer < 0)) {
        z_flg = false;
    } else {
        z_flg = true;
    }
    if (no_rock_timer != 0) {
        no_rock_timer--;
    }
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 20.0f;
    this->actor.shape.yOffset = plus_y;
}

s32 en_b_link_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                              Gfx** gfx) {
    Player* this = (Player*)thisx;

    return player_before_draw_joint_move(play, limbIndex, dList, pos, rot, &this->actor);
}

void en_b_link_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Player* this = (Player*)thisx;

    player_after_draw(play, limbIndex, dList, rot, &this->actor);
}

void En_Torch2_Actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_torch2.c", 1050);
    _texture_z_light_fog_prim2(play);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (alpha == 255) {
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 0, 0, alpha);
        gSPSegment(POLY_OPA_DISP++, 0x0C, Actor_change_render_mode + 2);
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
        POLY_OPA_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_b_link_display1, en_b_link_display2, this, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, alpha);
        gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
        POLY_XLU_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_b_link_display1, en_b_link_display2, this, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_torch2.c", 1114);
}
