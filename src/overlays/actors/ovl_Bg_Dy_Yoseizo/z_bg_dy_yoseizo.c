/*
 * File: z_bg_dy_yoseizo.c
 * Overlay: ovl_Bg_Dy_Yoseizo
 * Description: Great Fairy
 */

#include "z_bg_dy_yoseizo.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "regs.h"
#include "segmented_address.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "versions.h"
#include "z_lib.h"
#include "z64ocarina.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_dy_obj/object_dy_obj.h"
#include "assets/scenes/indoors/yousei_izumi_yoko/yousei_izumi_yoko_scene.h"
#include "assets/scenes/indoors/daiyousei_izumi/daiyousei_izumi_scene.h"

#if OOT_VERSION < NTSC_1_1
#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)
#else
#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)
#endif

typedef enum BgDyYoseizoRewardType {
    /* 0 */ FAIRY_UPGRADE_MAGIC,
    /* 1 */ FAIRY_UPGRADE_DOUBLE_MAGIC,
    /* 2 */ FAIRY_UPGRADE_DOUBLE_DEFENSE
} BgDyYoseizoRewardType;

typedef enum BgDyYoseizoSpellType {
    /* 0 */ FAIRY_SPELL_FARORES_WIND,
    /* 1 */ FAIRY_SPELL_DINS_FIRE,
    /* 2 */ FAIRY_SPELL_NAYRUS_LOVE
} BgDyYoseizoSpellType;

void Bg_Dy_Yoseizo_actor_ct(Actor* thisx, PlayState* play2);
void Bg_Dy_Yoseizo_actor_dt(Actor* thisx, PlayState* play);
void Bg_Dy_Yoseizo_actor_move(Actor* thisx, PlayState* play2);
void Bg_Dy_Yoseizo_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgDyYoseizo* this, PlayState* play);
void mode_wait_wait(BgDyYoseizo* this, PlayState* play);
static void mode_start_init(BgDyYoseizo* this, PlayState* play);
static void mode_start(BgDyYoseizo* this, PlayState* play);
void mode_start_anime_ende_check(BgDyYoseizo* this, PlayState* play);
void mode_mes_init(BgDyYoseizo* this, PlayState* play);
static void mode_message_wait(BgDyYoseizo* this, PlayState* play);
void mode_power_up_init(BgDyYoseizo* this, PlayState* play);
void mode_power_up(BgDyYoseizo* this, PlayState* play);
void mode_mes_end(BgDyYoseizo* this, PlayState* play);
static void mode_return_init(BgDyYoseizo* this, PlayState* play);
static void mode_return(BgDyYoseizo* this, PlayState* play);
void mode_last_wait(BgDyYoseizo* this, PlayState* play);
void demo_mode_wait(BgDyYoseizo* this, PlayState* play);
void demo_mode_start(BgDyYoseizo* this, PlayState* play);
void demo_mode_power_up(BgDyYoseizo* this, PlayState* play);

void yoseizo_eff_set(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                             Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type);
void yoseizo_eff_move(BgDyYoseizo* this, PlayState* play);
void yoseizo_eff_disp(BgDyYoseizo* this, PlayState* play);

static s32 Stone_Item_No_Data[] = { GI_FARORES_WIND, GI_NAYRUS_LOVE, GI_DINS_FIRE };

ActorProfile Bg_Dy_Yoseizo_Profile = {
    /**/ ACTOR_BG_DY_YOSEIZO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DY_OBJ,
    /**/ sizeof(BgDyYoseizo),
    /**/ Bg_Dy_Yoseizo_actor_ct,
    /**/ Bg_Dy_Yoseizo_actor_dt,
    /**/ Bg_Dy_Yoseizo_actor_move,
    /**/ NULL,
};

void Bg_Dy_Yoseizo_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    this->fountainType = play->spawn;

    if (this->fountainType < 0) {
        this->fountainType = 0;
    }

    this->vanishHeight = this->actor.world.pos.y;
    this->grownHeight = this->vanishHeight + 40.0f;
    this->actor.focus.pos = this->actor.world.pos;

    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        // "Great Fairy Fountain"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 大妖精の泉 ☆☆☆☆☆ %d\n" VT_RST, play->spawn);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGreatFairySkel, &gGreatFairySittingTransitionAnim,
                           this->jointTable, this->morphTable, 28);
#if OOT_VERSION < NTSC_1_1
        if (!z_common_data.save.info.playerData.isMagicAcquired && (this->fountainType != FAIRY_UPGRADE_MAGIC)) {
            Actor_delete(&this->actor);
            return;
        }
#endif
    } else {
        // "Stone/Jewel Fairy Fountain"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 石妖精の泉 ☆☆☆☆☆ %d\n" VT_RST, play->spawn);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGreatFairySkel, &gGreatFairyLayingDownTransitionAnim,
                           this->jointTable, this->morphTable, 28);
#if OOT_VERSION < NTSC_1_1
        if (!z_common_data.save.info.playerData.isMagicAcquired) {
            Actor_delete(&this->actor);
            return;
        }
#endif
    }

#if OOT_VERSION < NTSC_1_1
    this->actionFunc = mode_wait_wait;
#else
    this->actionFunc = mode_wait;
#endif
}

void Bg_Dy_Yoseizo_actor_dt(Actor* thisx, PlayState* play) {
}

static Color_RGB8 kirakira_prim_data[] = {
    { 255, 255, 255 }, { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 255, 170 },
    { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 255, 170 },
};

static Color_RGB8 kirakira_env_data[] = {
    { 155, 255, 255 }, { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 100, 255 },
    { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 100, 255, 255 },
};

void kirakira_eff_set(BgDyYoseizo* this, PlayState* play, s16 type) {
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel;
    Vec3f pos;
    Color_RGB8 primColor;
    Color_RGB8 envColor;
    f32 spawnPosVariation;
    s32 effectType;
    f32 scale;
    s32 i;
    s16 life;

    if (!(this->scale < 0.01f)) {
        spawnPosVariation = this->scale * 3500.0f;
        accel.x = fqrand() - 0.5f;
        accel.y = fqrand() - 0.5f;
        accel.z = fqrand() - 0.5f;
        for (i = 0; i < 2; i++) {
            if (type == 0) {
                effectType = 0;
                scale = 0.4f;
                life = 90;
                pos.x = this->actor.world.pos.x;
                pos.y = this->actor.world.pos.y + spawnPosVariation +
                        ((fqrand() - 0.5f) * (spawnPosVariation * 0.5f));
                pos.z = this->actor.world.pos.z + 30.0f;
            } else {
                life = 50;
                effectType = type;
                scale = 0.2f;
                pos.x = this->actor.world.pos.x + rnd_fx(10.0f);

                if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
                    pos.y = this->actor.world.pos.y + spawnPosVariation + 50.0f +
                            ((fqrand() - 0.5f) * (spawnPosVariation * 0.1f));
                    pos.z = this->actor.world.pos.z + 30.0f;
                } else {
                    pos.y = this->actor.world.pos.y + spawnPosVariation - 30.0f +
                            ((fqrand() - 0.5f) * (spawnPosVariation * 0.1f));
                    pos.z = this->actor.world.pos.z + 60.0f;
                }

                if (LINK_IS_ADULT) {
                    pos.y += 20.0f;
                }
            }

            primColor.r = kirakira_prim_data[effectType].r;
            primColor.g = kirakira_prim_data[effectType].g;
            primColor.b = kirakira_prim_data[effectType].b;
            envColor.r = kirakira_env_data[effectType].r;
            envColor.g = kirakira_env_data[effectType].g;
            envColor.b = kirakira_env_data[effectType].b;
            yoseizo_eff_set(this, &pos, &vel, &accel, &primColor, &envColor, scale, life, effectType);
        }
    }
}

void yosei_huwa_set(BgDyYoseizo* this, PlayState* play) {
    this->targetHeight = this->grownHeight + this->bobOffset;
    add_calc2(&this->actor.world.pos.y, this->targetHeight, 0.1f, 10.0f);
    add_calc2(&this->bobOffset, 10.0f, 0.1f, 0.5f);

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actor.velocity.y = sin_s(this->bobTimer);
    } else {
        this->actor.velocity.y = sin_s(this->bobTimer) * 0.4f;
    }
}

#if OOT_VERSION >= NTSC_1_1
static void mode_wait(BgDyYoseizo* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, 0x38)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
            if (!z_common_data.save.info.playerData.isMagicAcquired && (this->fountainType != FAIRY_UPGRADE_MAGIC)) {
                Actor_delete(&this->actor);
                return;
            }
        } else {
            if (!z_common_data.save.info.playerData.isMagicAcquired) {
                Actor_delete(&this->actor);
                return;
            }
        }
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
        this->actionFunc = mode_wait_wait;
    }
}
#endif

void mode_wait_wait(BgDyYoseizo* this, PlayState* play) {
    s32 givingReward;

#if OOT_VERSION < NTSC_1_1
    if (!Actor_Environment_sw_Check(play, 0x38)) {
        return;
    }

    if (play->msgCtx.ocarinaMode != OCARINA_MODE_04) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
        return;
    }
#endif

    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
    // "Mode"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ もうど ☆☆☆☆☆ %d\n" VT_RST, play->msgCtx.ocarinaMode);
    givingReward = false;

    if (play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        switch (this->fountainType) {
            case FAIRY_SPELL_FARORES_WIND:
                if (!GET_ITEMGETINF(ITEMGETINF_18)) {
                    givingReward = true;
                }
                break;
            case FAIRY_SPELL_DINS_FIRE:
                if (!GET_ITEMGETINF(ITEMGETINF_19)) {
                    givingReward = true;
                }
                break;
            case FAIRY_SPELL_NAYRUS_LOVE:
                if (!GET_ITEMGETINF(ITEMGETINF_1A)) {
                    givingReward = true;
                }
                break;
        }
    } else {
        switch (this->fountainType) {
            case FAIRY_UPGRADE_MAGIC:
                if (!z_common_data.save.info.playerData.isMagicAcquired || BREG(2)) {
                    // "Spin Attack speed UP"
                    PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 回転切り速度ＵＰ ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
            case FAIRY_UPGRADE_DOUBLE_MAGIC:
                if (!z_common_data.save.info.playerData.isDoubleMagicAcquired) {
                    // "Magic Meter doubled"
                    PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 魔法ゲージメーター倍増 ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
            case FAIRY_UPGRADE_DOUBLE_DEFENSE:
                if (!z_common_data.save.info.playerData.isDoubleDefenseAcquired) {
                    // "Damage halved"
                    PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ ダメージ半減 ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
        }
    }

    if (givingReward) {
        if (!IS_CUTSCENE_LAYER || !DEBUG_FEATURES) {
            if (play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
                switch (this->fountainType) {
                    case FAIRY_SPELL_FARORES_WIND:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyFaroresWindCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                    case FAIRY_SPELL_DINS_FIRE:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyDinsFireCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                    case FAIRY_SPELL_NAYRUS_LOVE:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyNayrusLoveCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                }
            } else {
                switch (this->fountainType) {
                    case FAIRY_UPGRADE_MAGIC:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyMagicCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                    case FAIRY_UPGRADE_DOUBLE_MAGIC:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyDoubleMagicCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                    case FAIRY_UPGRADE_DOUBLE_DEFENSE:
                        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGreatFairyDoubleDefenseCs);
                        z_common_data.cutsceneTrigger = 1;
                        break;
                }
            }
        }
        this->actionFunc = demo_mode_wait;
        return;
    }

    play->envCtx.lightSettingOverride = 2;

    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        makeOnepointDemo(play, 8603, -99, NULL, CAM_ID_MAIN);
    } else {
        makeOnepointDemo(play, 8604, -99, NULL, CAM_ID_MAIN);
    };

    Actor_SE_set(&this->actor, NA_SE_EV_GREAT_FAIRY_APPEAR);
    this->actor.draw = Bg_Dy_Yoseizo_actor_draw;
    this->actionFunc = mode_start_init;
}

// Sets animations for spingrow
static void mode_start_init(BgDyYoseizo* this, PlayState* play) {
    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        this->frameCount = Si2_anime_end_frame(&gGreatFairySittingTransitionAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairySittingTransitionAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    } else {
        this->frameCount = Si2_anime_end_frame(&gGreatFairyLayingDownTransitionAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairyLayingDownTransitionAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    }

    Actor_SE_set(&this->actor, NA_SE_VO_FR_LAUGH_0);
#if OOT_VERSION >= NTSC_1_1
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#endif
    this->actionFunc = mode_start;
}

static void mode_start(BgDyYoseizo* this, PlayState* play) {
#if OOT_VERSION >= NTSC_1_1
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#endif
    add_calc2(&this->actor.world.pos.y, this->grownHeight, this->heightFraction, 100.0f);
    add_calc2(&this->scale, 0.035f, this->scaleFraction, 0.005f);
    add_calc2(&this->heightFraction, 0.8f, 0.1f, 0.02f);
    add_calc2(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
    // Finished growing
    if (this->scale >= 0.034f) {
        if ((this->actor.shape.rot.y > -8000) && (this->actor.shape.rot.y < 1000)) {
            Skeleton_Info2_anime_play(&this->skelAnime);
            // Turn to front
            add_calc_short_angle2(&this->actor.shape.rot.y, 0, 5, 1000, 0);
            if (fabsf(this->actor.shape.rot.y) < 50.0f) {
                this->actionFunc = mode_start_anime_ende_check;
            }
        } else {
            this->actor.shape.rot.y += 3000;
        }
    } else {
        this->actor.shape.rot.y += 3000;
    }
    kirakira_eff_set(this, play, 0);
}

void mode_start_anime_ende_check(BgDyYoseizo* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

#if OOT_VERSION >= NTSC_1_1
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#endif

    if ((this->frameCount * 1273.0f) <= this->bobTimer) {
        this->bobTimer = 0.0f;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->frameCount <= curFrame) && !this->animationChanged) {
        this->actionFunc = mode_mes_init;
    }
}

void mode_mes_init(BgDyYoseizo* this, PlayState* play) {
#if OOT_VERSION >= NTSC_1_1
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#endif

    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        this->frameCount = Si2_anime_end_frame(&gGreatFairySittingAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairySittingAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
    } else {
        this->frameCount = Si2_anime_end_frame(&gGreatFairyLayingSidewaysAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairyLayingSidewaysAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
    }

    this->actor.textId = 0xDB;
    this->dialogState = TEXT_STATE_EVENT;
    message_set(play, this->actor.textId, NULL);
    kirakira_eff_set(this, play, 0);
    this->actionFunc = mode_message_wait;
}

static void mode_message_wait(BgDyYoseizo* this, PlayState* play) {
#if OOT_VERSION >= NTSC_1_1
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#endif
    this->bobTimer = this->skelAnime.curFrame * 1273.0f;

    if ((this->frameCount * 1273.0f) <= this->bobTimer) {
        this->bobTimer = 0.0f;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->dialogState == message_check(&play->msgCtx)) && pad_on_check(play)) {
        message_close(play);
        alpha_change(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE);
        this->actionFunc = mode_power_up_init;
    }

    yosei_huwa_set(this, play);
    kirakira_eff_set(this, play, 0);
}

void mode_power_up_init(BgDyYoseizo* this, PlayState* play) {
    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        this->frameCount = Si2_anime_end_frame(&gGreatFairyGivingUpgradeAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairyGivingUpgradeAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                         -10.0f);
    } else {
        this->frameCount = Si2_anime_end_frame(&gGreatFairyAnim_005810);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAnim_005810, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                         -10.0f);
    }

    Actor_SE_set(&this->actor, NA_SE_VO_FR_SMILE_0);
    this->mouthState = 1;
    this->actionFunc = mode_power_up;
}

void mode_power_up(BgDyYoseizo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f beamPos;
    s16 beamParams;

    if (this->animationChanged) {
        this->bobTimer = this->skelAnime.curFrame * 1300.0f;
        if ((this->frameCount * 1300.0f) <= this->bobTimer) {
            this->bobTimer = 0.0f;
        }
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->frameCount <= curFrame) && !(this->animationChanged)) {
        if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
            this->frameCount = Si2_anime_end_frame(&gGreatFairyAfterUpgradeAnim);
            Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAfterUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                             ANIMMODE_LOOP, -10.0f);
        } else {
            this->frameCount = Si2_anime_end_frame(&gGreatFairyAfterSpellAnim);
            Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAfterSpellAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                             -10.0f);
        }
        this->healingTimer = 150;
        this->animationChanged = true;
        if (!this->givingSpell) {
            beamPos.x = player->actor.world.pos.x;
            beamPos.y = player->actor.world.pos.y + 200.0f;
            beamPos.z = player->actor.world.pos.z;

            beamParams = ((play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) ? 0 : 1);

            this->beam = (EnDyExtra*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_DY_EXTRA,
                                                        beamPos.x, beamPos.y, beamPos.z, 0, 0, 0, beamParams);
        }
    }
    if (this->refillTimer > 1) {
        this->refillTimer--;
    }

    if (this->healingTimer >= 110) {
        this->healingTimer--;
    }

    if (this->healingTimer == 110) {
        z_common_data.healthAccumulator = 0x140;
        magic_mode_check(play);
        this->refillTimer = 200;
    }

    if (((z_common_data.save.info.playerData.healthCapacity == z_common_data.save.info.playerData.health) &&
         (z_common_data.save.info.playerData.magic == z_common_data.magicCapacity)) ||
        (this->refillTimer == 1)) {
        this->healingTimer--;
        if (this->healingTimer == 90) {
            if (!this->givingSpell) {
                this->beam->trigger = 1;
            }
            this->givingSpell = false;
        }
    }

    if (this->healingTimer == 1) {
        this->actor.textId = 0xDA;
        this->dialogState = TEXT_STATE_EVENT;
        message_set2(play, this->actor.textId);
        this->actionFunc = mode_mes_end;
        return;
    }
    yosei_huwa_set(this, play);
}

void mode_mes_end(BgDyYoseizo* this, PlayState* play) {
    this->bobTimer = this->skelAnime.curFrame * 1400.0f;

    if (this->bobTimer >= (this->frameCount * 1400.0f)) {
        this->bobTimer = 0.0f;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->dialogState == message_check(&play->msgCtx)) && pad_on_check(play)) {
        message_close(play);
        this->mouthState = 0;
        this->actionFunc = mode_return_init;
        restartCameraStoped(GET_ACTIVE_CAM(play));
    }

    yosei_huwa_set(this, play);
    kirakira_eff_set(this, play, 0);
}

static void mode_return_init(BgDyYoseizo* this, PlayState* play) {
    if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
        this->frameCount = Si2_anime_end_frame(&gGreatFairyJewelFountainSpinShrinkAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairyJewelFountainSpinShrinkAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    } else {
        this->frameCount = Si2_anime_end_frame(&gGreatFairySpellFountainSpinShrinkAnim);
        Skeleton_Info2_init(&this->skelAnime, &gGreatFairySpellFountainSpinShrinkAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    }

    this->vanishTimer = 5;
    this->scaleFraction = 0.0f;
    this->heightFraction = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_VO_FR_LAUGH_0);
    Actor_SE_set(&this->actor, NA_SE_EV_GREAT_FAIRY_VANISH);
    this->actionFunc = mode_return;
}

static void mode_return(BgDyYoseizo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->vanishTimer == 0) {
        if (this->scale < 0.003f) {
            this->vanishTimer = 30;
            this->actionFunc = mode_last_wait;
        } else {
            add_calc2(&this->actor.world.pos.y, this->vanishHeight, this->heightFraction, 100.0f);
            add_calc0(&this->scale, this->scaleFraction, 0.005f);
            add_calc2(&this->heightFraction, 0.8f, 0.1f, 0.02f);
            add_calc2(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
            this->actor.shape.rot.y += 3000;
            kirakira_eff_set(this, play, 0);
        }
    }
}

void mode_last_wait(BgDyYoseizo* this, PlayState* play) {
    Actor* findOcarinaSpot;

    if (this->vanishTimer == 0) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        play->envCtx.lightSettingOverride = 0;
        findOcarinaSpot = play->actorCtx.actorLists[ACTORCAT_PROP].head;

        while (findOcarinaSpot != NULL) {
            if (findOcarinaSpot->id != ACTOR_EN_OKARINA_TAG) {
                findOcarinaSpot = findOcarinaSpot->next;
                continue;
            }
            Actor_delete(findOcarinaSpot);
            break;
        }

        Actor_Environment_sw_Off(play, 0x38);
        Actor_delete(&this->actor);
    }
}

void demo_mode_wait(BgDyYoseizo* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.actorCues[0] != NULL) && (play->csCtx.actorCues[0]->id == 2)) {
            this->actor.draw = Bg_Dy_Yoseizo_actor_draw;
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            this->finishedSpinGrow = false;

            if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
                this->frameCount = Si2_anime_end_frame(&gGreatFairySittingTransitionAnim);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairySittingTransitionAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            } else {
                this->frameCount = Si2_anime_end_frame(&gGreatFairyLayingDownTransitionAnim);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairyLayingDownTransitionAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            }

            Actor_SE_set(&this->actor, NA_SE_EV_GREAT_FAIRY_APPEAR);
            this->actionFunc = demo_mode_start;
        }
    }
}

void demo_mode_start(BgDyYoseizo* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (!this->finishedSpinGrow) {
        add_calc2(&this->actor.world.pos.y, this->grownHeight, this->heightFraction, 100.0f);
        add_calc2(&this->scale, 0.035f, this->scaleFraction, 0.005f);
        add_calc2(&this->heightFraction, 0.8f, 0.1f, 0.02f);
        add_calc2(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
        // Finished growing
        if (this->scale >= 0.034f) {
            if ((this->actor.shape.rot.y > -8000) && (this->actor.shape.rot.y < 1000)) {
                Skeleton_Info2_anime_play(&this->skelAnime);
                // Spin until facing front
                adds(&this->actor.shape.rot.y, 0, 5, 1000);
                if (fabsf(this->actor.shape.rot.y) < 50.0f) {
                    this->finishedSpinGrow = true;
                }
            } else {
                this->actor.shape.rot.y += 3000;
            }
        } else {
            this->actor.shape.rot.y += 3000;
        }
    } else {
        Skeleton_Info2_anime_play(&this->skelAnime);

        if ((this->frameCount <= curFrame) && !this->animationChanged) {
            if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
                this->frameCount = Si2_anime_end_frame(&gGreatFairySittingAnim);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairySittingAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                                 -10.0f);
            } else {
                this->frameCount = Si2_anime_end_frame(&gGreatFairyLayingSidewaysAnim);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairyLayingSidewaysAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_LOOP, -10.0f);
            }
            this->animationChanged = true;
        }

        if ((play->csCtx.state != CS_STATE_IDLE) &&
            ((play->csCtx.actorCues[0] != NULL) && (play->csCtx.actorCues[0]->id == 3))) {
            this->finishedSpinGrow = this->animationChanged = false;
            if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
                this->frameCount = Si2_anime_end_frame(&gGreatFairyGivingUpgradeAnim);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairyGivingUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            } else {
                this->frameCount = Si2_anime_end_frame(&gGreatFairyAnim_005810);
                Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAnim_005810, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                                 -10.0f);
            }
            this->mouthState = 1;
            this->actionFunc = demo_mode_power_up;
        }
    }
    kirakira_eff_set(this, play, 0);
}

static s16 Light_color_data[] = { DEMO_EFFECT_LIGHT_GREEN, DEMO_EFFECT_LIGHT_RED, DEMO_EFFECT_LIGHT_BLUE };

static s16 UP_stone_data[] = { EXITEM_MAGIC_WIND, EXITEM_MAGIC_FIRE, EXITEM_MAGIC_DARK };

static s16 UP_stone_save_data[] = { ITEMGETINF_MASK(ITEMGETINF_18), ITEMGETINF_MASK(ITEMGETINF_19),
                                   ITEMGETINF_MASK(ITEMGETINF_1A) };

static u8 UP_stone_get_data[] = { ITEM_FARORES_WIND, ITEM_DINS_FIRE, ITEM_NAYRUS_LOVE };

void demo_mode_power_up(BgDyYoseizo* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);
    s16 cueIdTemp;
    s16 demoEffectParams;
    Vec3f itemPos;

    if (this->animationChanged) {
        this->bobTimer = this->skelAnime.curFrame * 1400.0f;
        if ((this->frameCount * 1400.0f) <= this->bobTimer) {
            this->bobTimer = 0.0f;
        }
    }
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->frameCount <= curFrame) && !this->animationChanged) {
        if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
            this->frameCount = Si2_anime_end_frame(&gGreatFairyAfterUpgradeAnim);
            Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAfterUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                             ANIMMODE_LOOP, -10.0f);
        } else {
            this->frameCount = Si2_anime_end_frame(&gGreatFairyAfterSpellAnim);
            Skeleton_Info2_init(&this->skelAnime, &gGreatFairyAfterSpellAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                             -10.0f);
        }
        this->animationChanged = true;
    }

    if (play->csCtx.actorCues[0]->id == 13) {
        this->actionFunc = mode_return_init;
        return;
    }

    if ((play->csCtx.actorCues[0]->id >= 4) && (play->csCtx.actorCues[0]->id < 7)) {
        cueIdTemp = play->csCtx.actorCues[0]->id - 4;

        if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
            cueIdTemp++;
            kirakira_eff_set(this, play, cueIdTemp);

        } else if (!this->lightBallSpawned) {
            demoEffectParams = ((s16)(Light_color_data[cueIdTemp] << 0xC) | DEMO_EFFECT_LIGHT);
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, (s32)demoEffectParams);
            this->lightBallSpawned = true;
        }
    } else {
        kirakira_eff_set(this, play, 0);
    }

    if ((play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) && (play->csCtx.actorCues[0]->id >= 10) &&
        (play->csCtx.actorCues[0]->id < 13)) {
        cueIdTemp = play->csCtx.actorCues[0]->id - 10;

        switch (cueIdTemp) {
            case FAIRY_UPGRADE_MAGIC:
                z_common_data.save.info.playerData.isMagicAcquired = true;
                z_common_data.magicFillTarget = MAGIC_NORMAL_METER;
                // magicLevel is already 0, setting isMagicAcquired to true triggers magicCapacity to grow
                alpha_change(HUD_VISIBILITY_HEARTS_MAGIC);
                break;
            case FAIRY_UPGRADE_DOUBLE_MAGIC:
                if (!z_common_data.save.info.playerData.isMagicAcquired) {
                    z_common_data.save.info.playerData.isMagicAcquired = true;
                }
                z_common_data.save.info.playerData.isDoubleMagicAcquired = true;
                z_common_data.magicFillTarget = MAGIC_DOUBLE_METER;
                // Setting magicLevel to 0 triggers magicCapacity to grow
                z_common_data.save.info.playerData.magicLevel = 0;
                alpha_change(HUD_VISIBILITY_HEARTS_MAGIC);
                break;
            case FAIRY_UPGRADE_DOUBLE_DEFENSE:
                z_common_data.save.info.playerData.isDoubleDefenseAcquired = true;
                alpha_change(HUD_VISIBILITY_HEARTS_MAGIC);
                break;
        }

        if (!this->healing) {
            z_common_data.healthAccumulator = 0x140;
            this->healing = true;
            if (cueIdTemp == 2) {
                magic_mode_check(play);
            }
        }
    }

    if ((play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) && (play->csCtx.actorCues[0]->id >= 14) &&
        (play->csCtx.actorCues[0]->id < 17)) {
        cueIdTemp = play->csCtx.actorCues[0]->id - 14;

        if (!this->itemSpawned) {
            itemPos.x = player->actor.world.pos.x;
            itemPos.y = (LINK_IS_ADULT ? player->actor.world.pos.y + 73.0f : player->actor.world.pos.y + 53.0f);
            itemPos.z = player->actor.world.pos.z;

            this->item = (EnExItem*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM, itemPos.x,
                                                       itemPos.y, itemPos.z, 0, 0, 0, UP_stone_data[cueIdTemp]);

            if (this->item != NULL) {
                if (!z_common_data.save.info.playerData.isMagicAcquired) {
                    z_common_data.save.info.playerData.isMagicAcquired = true;
                } else {
                    magic_mode_check(play);
                }

                this->itemSpawned = true;
                z_common_data.healthAccumulator = 0x140;
                alpha_change(HUD_VISIBILITY_HEARTS_MAGIC);
                z_common_data.save.info.itemGetInf[ITEMGETINF_INDEX_18_19_1A] |= UP_stone_save_data[cueIdTemp];
                item_get_setting(play, UP_stone_get_data[cueIdTemp]);
            }
        } else {
            this->item->actor.world.pos.x = player->actor.world.pos.x;
            this->item->actor.world.pos.y =
                (LINK_IS_ADULT ? player->actor.world.pos.y + 73.0f : player->actor.world.pos.y + 53.0f);
            this->item->actor.world.pos.z = player->actor.world.pos.z;
            this->item->scale = 0.3f;
        }
    }

    if ((play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) && (play->csCtx.actorCues[0]->id == 17) &&
        (this->item != NULL)) {
        Actor_delete(&this->item->actor);
        this->item = NULL;
    }

    if ((play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) && (play->csCtx.actorCues[0]->id == 18)) {
        this->giveDefenseHearts = true;
    }

    if (this->giveDefenseHearts) {
        if (z_common_data.save.info.inventory.defenseHearts < 20) {
            z_common_data.save.info.inventory.defenseHearts++;
        }
    }

    if ((play->csCtx.actorCues[0]->id >= 19) && (play->csCtx.actorCues[0]->id < 22) && !this->warpEffectSpawned) {
        cueIdTemp = play->csCtx.actorCues[0]->id - 11;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, player->actor.world.pos.x, player->actor.world.pos.y,
                    player->actor.world.pos.z, 0, 0, 0, cueIdTemp);
        this->warpEffectSpawned = true;
    }
    yosei_huwa_set(this, play);
}

void Bg_Dy_Yoseizo_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;
    s32 sfx;

    this->absoluteTimer++;

    if (this->vanishTimer != 0) {
        this->vanishTimer--;
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->unusedTimer != 0) {
        this->unusedTimer--;
    }

    this->actionFunc(this, play);

    if (play->csCtx.state != CS_STATE_IDLE) {
        sfx = 0;
        if (play->sceneId == SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) {
            if ((play->csCtx.curFrame == 32) || (play->csCtx.curFrame == 291) || (play->csCtx.curFrame == 426) ||
                (play->csCtx.curFrame == 851)) {
                sfx = 1;
            }
            if (play->csCtx.curFrame == 101) {
                sfx = 2;
            }
        } else {
            if ((play->csCtx.curFrame == 35) || (play->csCtx.curFrame == 181) || (play->csCtx.curFrame == 462) ||
                (play->csCtx.curFrame == 795)) {
                sfx = 1;
            }
            if (play->csCtx.curFrame == 90) {
                sfx = 2;
            }
        }

        if (sfx == 1) {
            Actor_SE_set(&this->actor, NA_SE_VO_FR_SMILE_0);
        }
        if (sfx == 2) {
            Actor_SE_set(&this->actor, NA_SE_VO_FR_LAUGH_0);
        }
    }

    if ((this->blinkTimer == 0) && (this->actionFunc != mode_power_up)) {
        this->eyeState++;
        this->eyeState2++;
        if (this->eyeState >= 3) {
            this->eyeState = this->eyeState2 = 0;
            this->blinkTimer = (s16)rnd_f(60.0f) + 20;
        }
    }

    Actor_position_moveF(&this->actor);
    this->heightOffset = this->scale * 7500.0f;
    Actor_world_to_eye(&this->actor, this->heightOffset);
    this->actor.focus.pos.y = this->heightOffset;
    eye_move2(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    yoseizo_eff_move(this, play);
    Actor_set_scale(&this->actor, this->scale);
}

s32 Bg_Dy_Yoseizo_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    if (limbIndex == 8) { // Torso
        rot->x += this->torsoRot.y;
    }
    if (limbIndex == 15) { // Head
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }
    return 0;
}

static void* dy_eye_txt[] = {
    gGreatFairyEyeOpenTex,   // Open
    gGreatFairyEyeHalfTex,   // Half
    gGreatFairyEyeClosedTex, // Closed
};

static void* dy_mouth_txt[] = {
    gGreatFairyMouthClosedTex, // Closed
    gGreatFairyMouthOpenTex,   // Open
};

void Bg_Dy_Yoseizo_actor_draw(Actor* thisx, PlayState* play) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_dy_yoseizo.c", 1609);
    if (this->actionFunc != mode_last_wait) {
        _texture_z_light_fog_prim(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dy_eye_txt[this->eyeState]));

        // This was probably intended to allow this actor to wink, but segment 09 is not used in the dList for the head,
        // so it can only blink
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(dy_eye_txt[this->eyeState2]));

        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(dy_mouth_txt[this->mouthState]));

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              Bg_Dy_Yoseizo_draw_sub, NULL, this);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_dy_yoseizo.c", 1629);
    yoseizo_eff_disp(this, play);
}

void yoseizo_eff_set(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                             Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type) {
    BgDyYoseizoEffect* effect;
    s16 i;

    effect = this->effects;

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (effect->alive == 0) {
            effect->alive = 1;
            effect->pos = *initPos;
            effect->velocity = *initVelocity;
            effect->accel = *accel;
            effect->primColor = *primColor;
            effect->alpha = 0;
            effect->envColor = *envColor;
            effect->scale = scale;
            effect->timer = life;
            effect->type = type;
            effect->pitch = 0.0f;
            effect->yaw = rnd_fx(30000.0f);
            effect->roll = 0.0f;
            return;
        }
    }
}

void yoseizo_eff_move(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizoEffect* effect = this->effects;
    Player* player = GET_PLAYER(play);
    Vec3f sp94;
    Vec3f sp88;
    f32 goalPitch;
    f32 goalYaw;
    s16 i = 0;

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (effect->alive != 0) {
            effect->roll += 3000.0f;

            if (effect->type == 0) {
                effect->pos.x += effect->velocity.x;
                effect->pos.y += effect->velocity.y;
                effect->pos.z += effect->velocity.z;
                effect->velocity.x += effect->accel.x;
                effect->velocity.y += effect->accel.y;
                effect->velocity.z += effect->accel.z;
            } else {
                Actor_SE_set(&this->actor, NA_SE_EV_HEALING - SFX_FLAG);

                sp94 = player->actor.world.pos;
                sp94.y = player->actor.world.pos.y - 150.0f;
                sp94.z = player->actor.world.pos.z - 50.0f;

                goalPitch = search_position_angleX(&effect->pos, &sp94);
                goalYaw = search_position_angleY(&effect->pos, &sp94);

                add_calc2(&effect->pitch, goalPitch, 0.9f, 5000.0f);
                add_calc2(&effect->yaw, goalYaw, 0.9f, 5000.0f);
                Matrix_push();
                Matrix_rotateY(BINANG_TO_RAD_ALT(effect->yaw), MTXMODE_NEW);
                Matrix_rotateX(BINANG_TO_RAD_ALT(effect->pitch), MTXMODE_APPLY);

                sp94.x = sp94.y = sp94.z = 3.0f;

                Matrix_Position(&sp94, &sp88);
                Matrix_pull();
                effect->pos.x += sp88.x;
                effect->pos.y += sp88.y;
                effect->pos.z += sp88.z;
            }
        }

        // fade up, fade down, vanish and reset
        if (effect->timer != 0) {
            effect->timer--;
            effect->alpha += 30;

            if (effect->alpha > 255) {
                effect->alpha = 255;
            }
        } else {
            effect->alpha -= 30;

            if (effect->alpha <= 0) {
                effect->alpha = effect->alive = 0;
            }
        }
    }
}

void yoseizo_eff_disp(BgDyYoseizo* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 materialFlag = 0;
    BgDyYoseizoEffect* effect = this->effects;
    s16 i;

    OPEN_DISPS(gfxCtx, "../z_bg_dy_yoseizo.c", 1767);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (effect->alive == 1) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGreatFairyParticleMaterialDL));
                gDPPipeSync(POLY_XLU_DISP++);

                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->primColor.r, effect->primColor.g, effect->primColor.b,
                            effect->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, effect->envColor.r, effect->envColor.g, effect->envColor.b, 0);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            Matrix_rotateZ(effect->roll, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_bg_dy_yoseizo.c", 1810);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGreatFairyParticleModelDL));
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_bg_dy_yoseizo.c", 1819);
}
