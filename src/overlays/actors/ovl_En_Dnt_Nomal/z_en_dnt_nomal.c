/*
 * File: z_en_dnt_nomal
 * Overlay: ovl_En_Dnt_Nomal
 * Description: Lost Woods minigame scrubs
 */

#include "z_en_dnt_nomal.h"
#include "assets/objects/object_dnk/object_dnk.h"
#include "overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "assets/objects/object_hintnuts/object_hintnuts.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Dnt_Nomal_actor_ct(Actor* thisx, PlayState* play);
void En_Dnt_Nomal_actor_dt(Actor* thisx, PlayState* play);
void En_Dnt_Nomal_actor_move(Actor* thisx, PlayState* play);
void En_Dnt_Nomal_MATO_actor_draw(Actor* thisx, PlayState* play);
void En_Dnt_Nomal_actor_draw(Actor* thisx, PlayState* play);

static void mode_bank_chenge_wait(EnDntNomal* this, PlayState* play);
static void mode_leaf_check(EnDntNomal* this, PlayState* play);

void mode_mato_wait_init(EnDntNomal* this, PlayState* play);
void mode_mato_jump_init(EnDntNomal* this, PlayState* play);
void mode_mato_walk_init(EnDntNomal* this, PlayState* play);
void mode_mato_talk_init(EnDntNomal* this, PlayState* play);
void mode_mato_shoot_init(EnDntNomal* this, PlayState* play);

void mode_mato_wait(EnDntNomal* this, PlayState* play);
void mode_mato_jump(EnDntNomal* this, PlayState* play);
void mode_mato_walk(EnDntNomal* this, PlayState* play);
void mode_mato_rolling_wait(EnDntNomal* this, PlayState* play);
void mode_mato_talk(EnDntNomal* this, PlayState* play);
void mode_mato_shoot(EnDntNomal* this, PlayState* play);
void mode_mato_return(EnDntNomal* this, PlayState* play);
void mode_mato_dead(EnDntNomal* this, PlayState* play);

#if OOT_PAL_N64
void EnDntNomal_DoNothing(EnDntNomal* this, PlayState* play);
#endif

void mode_demo_wait_init(EnDntNomal* this, PlayState* play);
void mode_demo_walk_init(EnDntNomal* this, PlayState* play);
void mode_demo_happy_init(EnDntNomal* this, PlayState* play);
void mode_demo_down_init(EnDntNomal* this, PlayState* play);
void mode_demo_attack_init(EnDntNomal* this, PlayState* play);
void mode_demo_runaway_init(EnDntNomal* this, PlayState* play);

static void mode_demo_wait(EnDntNomal* this, PlayState* play);
void mode_demo_start(EnDntNomal* this, PlayState* play);
void mode_demo_jump(EnDntNomal* this, PlayState* play);
void mode_demo_walk(EnDntNomal* this, PlayState* play);
void mode_demo_happy(EnDntNomal* this, PlayState* play);
void mode_demo_down(EnDntNomal* this, PlayState* play);
void mode_demo_attack_wait(EnDntNomal* this, PlayState* play);
void mode_demo_attack(EnDntNomal* this, PlayState* play);
void mode_demo_runaway(EnDntNomal* this, PlayState* play);

ActorProfile En_Dnt_Nomal_Profile = {
    /**/ ACTOR_EN_DNT_NOMAL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnDntNomal),
    /**/ En_Dnt_Nomal_actor_ct,
    /**/ En_Dnt_Nomal_actor_dt,
    /**/ En_Dnt_Nomal_actor_move,
    /**/ NULL,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 16, 46, 0, { 0, 0, 0 } },
};

static ColliderQuadInit OcInfoData_Swrd = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0001F824, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Color_RGBA8 color_data[] = {
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
    { 255, 255, 255, 255 }, { 210, 255, 0, 255 },   { 255, 195, 175, 255 },
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
};

void En_Dnt_Nomal_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    this->type = this->actor.params;
    if (this->type < ENDNTNOMAL_TARGET) {
        this->type = ENDNTNOMAL_TARGET;
    }
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->objectId = -1;
    if (this->type == ENDNTNOMAL_TARGET) {
        PRINTF("\n\n");
        // "Deku Scrub target"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツ的当て ☆☆☆☆☆ \n" VT_RST);
        ClObjSwrd_ct(play, &this->targetQuad);
        ClObjSwrd_set5(play, &this->targetQuad, &this->actor, &OcInfoData_Swrd);
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->objectId = OBJECT_HINTNUTS;
    } else {
        PRINTF("\n\n");
        // "Deku Scrub mask show audience"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツお面品評会一般人 ☆☆☆☆☆ \n" VT_RST);
        ClObjPipe_ct(play, &this->bodyCyl);
        ClObjPipe_set5(play, &this->bodyCyl, &this->actor, &OcInfoData);
        this->objectId = OBJECT_DNK;
    }
    if (this->objectId >= 0) {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, this->objectId);
        if (this->requiredObjectSlot < 0) {
            Actor_delete(&this->actor);
            // "What?"
            PRINTF(VT_FGCOL(MAGENTA) " なにみの？ %d\n" VT_RST "\n", this->requiredObjectSlot);
            // "Bank is funny"
            PRINTF(VT_FGCOL(CYAN) " バンクおかしいしぞ！%d\n" VT_RST "\n", this->actor.params);
            return;
        }
    } else {
        Actor_delete(&this->actor);
    }
    this->actionFunc = mode_bank_chenge_wait;
}

void En_Dnt_Nomal_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    if (this->type == ENDNTNOMAL_TARGET) {
        ClObjSwrd_dt(play, &this->targetQuad);
    } else {
        ClObjPipe_dt(play, &this->bodyCyl);
    }
}

static void mode_bank_chenge_wait(EnDntNomal* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->requiredObjectSlot].segment);
        this->actor.objectSlot = this->requiredObjectSlot;
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
        this->actor.gravity = -2.0f;
        Actor_set_scale(&this->actor, 0.01f);
        if (this->type == ENDNTNOMAL_TARGET) {
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gHintNutsSkel, &gHintNutsBurrowAnim, this->jointTable,
                           this->morphTable, 10);
            this->actor.draw = En_Dnt_Nomal_MATO_actor_draw;
        } else {
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gDntStageSkel, &gDntStageHideAnim, this->jointTable,
                           this->morphTable, 11);
            this->actor.draw = En_Dnt_Nomal_actor_draw;
        }
        this->actionFunc = mode_leaf_check;
    }
}

static void mode_leaf_check(EnDntNomal* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->flowerPos = this->actor.world.pos;
        if (this->type == ENDNTNOMAL_TARGET) {
            this->actionFunc = mode_mato_wait_init;
        } else {
            this->actionFunc = mode_demo_wait_init;
        }
    }
}

void mode_mato_wait_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsBurrowAnim);
    Skeleton_Info2_init(&this->skelAnime, &gHintNutsBurrowAnim, 0.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    this->skelAnime.curFrame = 8.0f;
    this->actionFunc = mode_mato_wait;
}

void mode_mato_wait(EnDntNomal* this, PlayState* play) {
    Vec3f scorePos;
    f32 targetX = 1340.0f;
    f32 targetY = 50.0f;
    f32 targetZ = -30.0f;
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f scoreAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f scoreVel = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    this->targetVtx[0].x = targetX;
    this->targetVtx[0].y = targetY - 24.0f;
    this->targetVtx[0].z = targetZ + 24.0f;

    this->targetVtx[1].x = targetX;
    this->targetVtx[1].y = targetY - 24.0f;
    this->targetVtx[1].z = targetZ - 24.0f;

    this->targetVtx[2].x = targetX;
    this->targetVtx[2].y = targetY + 24.0f;
    this->targetVtx[2].z = targetZ + 24.0f;

    this->targetVtx[3].x = targetX;
    this->targetVtx[3].y = targetY + 24.0f;
    this->targetVtx[3].z = targetZ - 24.0f;

    Skeleton_Info2_anime_play(&this->skelAnime);
#if OOT_VERSION < PAL_1_0
    if (this->targetQuad.base.acFlags & AC_HIT)
#else
    if ((this->targetQuad.base.acFlags & AC_HIT) || BREG(0))
#endif
    {
        this->targetQuad.base.acFlags &= ~AC_HIT;

        dx = fabsf(targetX - this->targetQuad.elem.acDmgInfo.hitPos.x);
        dy = fabsf(targetY - this->targetQuad.elem.acDmgInfo.hitPos.y);
        dz = fabsf(targetZ - this->targetQuad.elem.acDmgInfo.hitPos.z);

        scoreVel.y = 5.0f;

        if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 8.0f) {
            scorePos.x = this->actor.world.pos.x - 20.0f;
            scorePos.y = this->actor.world.pos.y + 20.0f;
            scorePos.z = this->actor.world.pos.z;
            Effect_SS_Extra_ct(play, &scorePos, &scoreVel, &scoreAccel, 4, 2);
            Nai_StopFx(NA_SE_SY_TRE_BOX_APPEAR);
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            // "Big hit"
            PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 大当り ☆☆☆☆☆ %d\n" VT_RST, this->hitCounter);
            if (!LINK_IS_ADULT && !GET_ITEMGETINF(ITEMGETINF_1D)) {
                this->hitCounter++;
                if (this->hitCounter >= 3) {
                    makeOnepointDemo(play, 4140, -99, &this->actor, CAM_ID_MAIN);
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                    this->timer4 = 50;
                    this->actionFunc = mode_mato_jump_init;
                }
            }
        } else if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 24.0f) {
            scorePos.x = this->actor.world.pos.x;
            scorePos.y = this->actor.world.pos.y + 20.0f;
            scorePos.z = this->actor.world.pos.z;
            Effect_SS_Extra_ct(play, &scorePos, &scoreVel, &scoreAccel, 4, 0);
            this->hitCounter = 0;
        }
    }
}

void mode_mato_jump_init(EnDntNomal* this, PlayState* play) {
    Vec3f spawnPos;

    if (this->timer4 == 0) {
        this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsUnburrowAnim);
        Skeleton_Info2_init(&this->skelAnime, &gHintNutsUnburrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        spawnPos = this->actor.world.pos;
        spawnPos.y = this->actor.world.pos.y + 50.0f;
        Effect_Hahen_Kakusan_ct3(play, &spawnPos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
        this->actionFunc = mode_mato_jump;
    }
}

void mode_mato_jump(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (frame >= this->endFrame) {
        this->actionFunc = mode_mato_walk_init;
    }
}

void mode_mato_walk_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsRunAnim);
    Skeleton_Info2_init(&this->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speed = 1.0f;
    this->actor.colChkInfo.mass = 0;
    this->actionFunc = mode_mato_walk;
}

void mode_mato_walk(EnDntNomal* this, PlayState* play) {
    f32 dx;
    f32 dz;

    Skeleton_Info2_anime_play(&this->skelAnime);
    dx = 1340.0f + 3.0f - this->actor.world.pos.x;
    dz = 0.0f - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 0x32, 0xBB8, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if (this->actor.world.pos.z > -30.0f) {
        this->actor.speed = 0.0f;
        this->actionFunc = mode_mato_rolling_wait;
    }
}

void mode_mato_rolling_wait(EnDntNomal* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if (fabsf(this->actor.shape.rot.y - this->actor.yawTowardsPlayer) < 30.0f) {
        this->actionFunc = mode_mato_talk_init;
    }
}

void mode_mato_talk_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsTalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gHintNutsTalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.textId = 0x10AF;
    message_set(play, this->actor.textId, NULL);
    this->actionFunc = mode_mato_talk;
}

void mode_mato_talk(EnDntNomal* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        restartCameraStoped(GET_ACTIVE_CAM(play));
        GET_ACTIVE_CAM(play)->csId = 0;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_mato_shoot_init;
    }
}

void mode_mato_shoot_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsSpitAnim);
    Skeleton_Info2_init(&this->skelAnime, &gHintNutsSpitAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_mato_shoot;
}

void mode_mato_shoot(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((frame >= 8.0f) && !this->spawnedItem) {
        f32 itemX = this->mouthPos.x - 10.0f;
        f32 itemY = this->mouthPos.y;
        f32 itemZ = this->mouthPos.z;

        if (Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM, itemX, itemY, itemZ, 0, 0, 0,
                               EXITEM_BULLET_BAG) == NULL) {
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
            Actor_delete(&this->actor);
        }
        this->spawnedItem = true;
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
    }
    if (frame >= this->endFrame) {
        this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsRunAnim);
        Skeleton_Info2_init(&this->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
        this->actionFunc = mode_mato_return;
    }
}

void mode_mato_return(EnDntNomal* this, PlayState* play) {
    f32 dx;
    f32 dz;

    Skeleton_Info2_anime_play(&this->skelAnime);
    dx = this->flowerPos.x - this->actor.world.pos.x;
    dz = -180.0f - this->actor.world.pos.z;

    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 3, 0x1388, 0);
    if (fabsf(this->actor.shape.rot.y - RAD_TO_BINANG(fatan2(dx, dz))) < 20.0f) {
        this->actor.speed = 1.0f;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.world.pos.z < -172.0f) {
        this->endFrame = (f32)Si2_anime_end_frame(&gHintNutsBurrowAnim);
        Skeleton_Info2_init(&this->skelAnime, &gHintNutsBurrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->actor.world.pos.z = -173.0f;
        this->actor.speed = 0.0f;
        this->actionFunc = mode_mato_dead;
    }
}

void mode_mato_dead(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (frame >= this->endFrame) {
#if !OOT_PAL_N64
        this->actionFunc = mode_mato_wait_init;
#else
        this->hitCounter = 0;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        xyz_t_move(&this->actor.world.pos, &this->actor.home.pos);
        this->actionFunc = EnDntNomal_DoNothing;
#endif
    }
}

#if OOT_PAL_N64
void EnDntNomal_DoNothing(EnDntNomal* this, PlayState* play) {
}
#endif

void mode_demo_wait_init(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Si2_anime_end_frame(&gDntStageHideAnim);
        Skeleton_Info2_init(&this->skelAnime, &gDntStageHideAnim, 0.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->skelAnime.curFrame = 8.0f;
        this->isSolid = false;
        this->actionFunc = mode_demo_wait;
    }
}

static void mode_demo_wait(EnDntNomal* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void mode_demo_start_init(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Si2_anime_end_frame(&gDntStageUpAnim);
        Skeleton_Info2_init(&this->skelAnime, &gDntStageUpAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        if (this->action != DNT_ACTION_ATTACK) {
            this->rotDirection = -1;
        }
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
        this->isSolid = true;
        this->actionFunc = mode_demo_start;
    }
}

void mode_demo_start(EnDntNomal* this, PlayState* play) {
    s16 rotTarget;
    f32 frame = this->skelAnime.curFrame;
    f32 turnMod;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((frame >= this->endFrame) && (this->action == DNT_ACTION_ATTACK)) {
        this->actionFunc = mode_demo_attack_init;
    } else {
        if (this->timer4 == 0) {
            turnMod = 0.0f;
            if (this->stagePrize == DNT_PRIZE_NONE) {
                add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
            } else {
                f32 dx = this->targetPos.x - this->actor.world.pos.x;
                f32 dz = this->targetPos.z - this->actor.world.pos.z;

                add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 1, 0xBB8, 0);
                turnMod = 90.0f;
            }
            if ((rnd_f(10.0f + turnMod) < 1.0f) && (this->action != DNT_ACTION_ATTACK)) {
                this->timer4 = (s16)rnd_f(30.0f) + 30;
            }
        } else {
            if (this->timer2 == 0) {
                this->rotDirection++;
                if (this->rotDirection > 1) {
                    this->rotDirection = -1;
                }
                this->timer2 = (s16)rnd_f(10.0f) + 10;
            }
            rotTarget = this->actor.yawTowardsPlayer;
            if (this->rotDirection != 0) {
                rotTarget += this->rotDirection * 0x1388;
            }
            add_calc_short_angle2(&this->actor.shape.rot.y, rotTarget, 3, 0x1388, 0);
        }
        if (this->actor.xzDistToPlayer < 70.0f) {
            this->actionFunc = mode_demo_down_init;
        }
    }
}

void mode_demo_jump_init(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Si2_anime_end_frame(&gDntStageUnburrowAnim);
        Skeleton_Info2_init(&this->skelAnime, &gDntStageUnburrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->isSolid = false;
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
        this->actionFunc = mode_demo_jump;
    }
}

void mode_demo_jump(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (frame >= this->endFrame) {
        if (this->action != DNT_ACTION_DANCE) {
            this->timer3 = (s16)rnd_f(2.0f) + (s16)(this->type * 0.5f);
            this->actionFunc = mode_demo_walk_init;
        } else {
            this->timer2 = 300;
            this->actionFunc = mode_demo_happy_init;
        }
    }
}

void mode_demo_walk_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntStageWalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntStageWalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speed = 3.0f;
    this->isSolid = true;
    this->actionFunc = mode_demo_walk;
}

void mode_demo_walk(EnDntNomal* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->timer1 == 0) && (this->timer3 == 0)) {
        f32 dx = this->targetPos.x - this->actor.world.pos.x;
        f32 dz = this->targetPos.z - this->actor.world.pos.z;

        if ((fabsf(dx) < 10.0f) && (fabsf(dz) < 10.0f) && (message_check(&play->msgCtx) != TEXT_STATE_NONE)) {
            this->action = DNT_ACTION_PRIZE;
            this->actionFunc = mode_demo_happy_init;
            this->actor.speed = 0.0f;
            return;
        }
        add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 1, 0xBB8, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else {
        if (this->timer1 == 1) {
            this->timer3 = (s16)rnd_f(20.0f) + 20.0f;
        }
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x14, 0x1388, 0);
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    }
    if (this->timer5 == 0) {
        this->timer5 = 20;
        if ((this->type & 1) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
        }
    } else if ((this->timer5 & 3) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.velocity.y = 7.5f;
    }
}

void mode_demo_happy_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntStageDanceAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntStageDanceAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->isSolid = true;
    this->timer3 = (s16)rnd_f(20.0f) + 20.0f;
    this->rotDirection = -1;
    if (rnd_f(1.99f) < 1.0f) {
        this->rotDirection = 1;
    }
    this->actionFunc = mode_demo_happy;
}

void mode_demo_happy(EnDntNomal* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer2 == 0) {
        if (this->action == DNT_ACTION_DANCE) {
            this->action = DNT_ACTION_HIGH_RUPEES;
            this->actionFunc = mode_demo_down_init;
        } else {
            this->action = DNT_ACTION_NONE;
            this->actionFunc = mode_demo_runaway_init;
        }
    } else if (this->timer3 != 0) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
        if (this->timer3 == 1) {
            this->timer4 = (s16)rnd_f(20.0f) + 20.0f;
            this->rotDirection = -this->rotDirection;
        }
    } else if (this->timer4 != 0) {
        this->actor.shape.rot.y += this->rotDirection * 0x800;
        if (this->timer4 == 1) {
            this->timer3 = (s16)rnd_f(20.0f) + 20.0f;
        }
    }
}

void mode_demo_down_init(EnDntNomal* this, PlayState* play) {
    if (this->timer3 != 0) {
        if ((this->timer3 == 1) && (this->ignore == 1)) {
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
        }
    } else {
        this->endFrame = (f32)Si2_anime_end_frame(&gDntStageHideAnim);
        Skeleton_Info2_init(&this->skelAnime, &gDntStageHideAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->isSolid = false;
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DOWN);
        this->actionFunc = mode_demo_down;
    }
}

void mode_demo_down(EnDntNomal* this, PlayState* play) {
    EnExRuppy* rupee;
    f32 frame = this->skelAnime.curFrame;
    s16 rupeeColor;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (frame >= this->endFrame) {
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
        switch (this->action) {
            case DNT_ACTION_NONE:
                this->actionFunc = mode_demo_wait_init;
                break;
            case DNT_ACTION_ATTACK:
                this->actionFunc = mode_demo_attack_wait;
                break;
            case DNT_ACTION_LOW_RUPEES:
            case DNT_ACTION_HIGH_RUPEES:
                rupee = (EnExRuppy*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_EX_RUPPY, this->actor.world.pos.x,
                                                this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0, 3);
                if (rupee != NULL) {
                    rupeeColor = this->action - DNT_ACTION_LOW_RUPEES;
                    rupee->colorIdx = rupeeColor;
                    if (rnd_f(3.99f) < 1.0f) {
                        rupee->colorIdx = rupeeColor + 1;
                    }
                    rupee->actor.velocity.y = 5.0f;
                    if (rupee->colorIdx == 2) {
                        rupee->actor.velocity.y = 7.0f;
                    }
                    Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                }
                this->action = DNT_ACTION_NONE;
                this->actionFunc = mode_demo_wait_init;
                break;
        }
    }
}

void mode_demo_attack_wait(EnDntNomal* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 70.0f) {
        this->actionFunc = mode_demo_start_init;
    }
}

void mode_demo_attack_init(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Si2_anime_end_frame(&gDntStageSpitAnim);
        Skeleton_Info2_init(&this->skelAnime, &gDntStageSpitAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->isSolid = true;
        this->timer2 = 0;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        this->actionFunc = mode_demo_attack;
    }
}

void mode_demo_attack(EnDntNomal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* nut;
    f32 frame = this->skelAnime.curFrame;
    f32 dz;
    f32 dx;
    f32 dy;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    dx = player->actor.world.pos.x - this->mouthPos.x;
    dy = player->actor.world.pos.y + 30.0f - this->mouthPos.y;
    dz = player->actor.world.pos.z - this->mouthPos.z;
    add_calc_short_angle2(&this->actor.shape.rot.x, -RAD_TO_BINANG(fatan2(dy, sqrtf(SQ(dx) + SQ(dz)))), 3, 0x1388,
                       0);
    if ((frame >= this->endFrame) && (this->timer2 == 0)) {
        this->timer2 = (s16)rnd_f(10.0f) + 10;
    }
    if (this->timer2 == 1) {
        this->spawnedItem = false;
        this->actionFunc = mode_demo_attack_init;
    } else if (this->actor.xzDistToPlayer < 50.0f) {
        this->action = DNT_ACTION_ATTACK;
        this->actionFunc = mode_demo_down_init;
    } else if ((frame >= 8.0f) && (!this->spawnedItem)) {
        Vec3f baseOffset;
        Vec3f spawnOffset;
        f32 spawnX;
        f32 spawnY;
        f32 spawnZ;

        Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
        Matrix_rotateX(BINANG_TO_RAD_ALT(this->actor.shape.rot.x), MTXMODE_APPLY);
        baseOffset.x = 0.0f;
        baseOffset.y = 0.0f;
        baseOffset.z = 5.0f;
        Matrix_Position(&baseOffset, &spawnOffset);
        spawnX = this->mouthPos.x + spawnOffset.x;
        spawnY = this->mouthPos.y + spawnOffset.y;
        spawnZ = this->mouthPos.z + spawnOffset.z;

        nut = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnX, spawnY, spawnZ, this->actor.shape.rot.x,
                          this->actor.shape.rot.y, this->actor.shape.rot.z, 4);
        if (nut != NULL) {
            nut->velocity.y = spawnOffset.y * 0.5f;
        }
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
        this->spawnedItem = true;
    }
}

void mode_demo_runaway_init(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntStageWalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntStageWalkAnim, 1.5f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speed = 4.0f;
    this->isSolid = false;
    this->actionFunc = mode_demo_runaway;
}

void mode_demo_runaway(EnDntNomal* this, PlayState* play) {
    f32 sp2C;
    f32 sp28;

    Skeleton_Info2_anime_play(&this->skelAnime);
    sp2C = this->flowerPos.x - this->actor.world.pos.x;
    sp28 = this->flowerPos.z - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(sp2C, sp28)), 1, 0xBB8, 0);
    if (this->timer5 == 0) {
        this->timer5 = 10;
    } else if (!(this->timer5 & 1)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((fabsf(sp2C) < 7.0f) && (fabsf(sp28) < 7.0f)) {
        this->actor.world.pos.x = this->flowerPos.x;
        this->actor.world.pos.z = this->flowerPos.z;
        this->actor.speed = 0.0f;
        this->actionFunc = mode_demo_down_init;
    }
}

void En_Dnt_Nomal_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    if (this->timer1 != 0) {
        this->timer1--;
    }
    if (this->timer2 != 0) {
        this->timer2--;
    }
    if (this->timer3 != 0) {
        this->timer3--;
    }
    if (this->timer4 != 0) {
        this->timer4--;
    }
    if (this->timer5 != 0) {
        this->timer5--;
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    this->actor.world.rot.x = this->actor.shape.rot.x;
    if (this->actionFunc != mode_demo_walk) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    this->unkCounter++;
    if (this->type != ENDNTNOMAL_TARGET) {
        switch (this->stageSignal) {
            case DNT_SIGNAL_LOOK:
                if (this->stagePrize == DNT_PRIZE_NONE) {
                    this->actionFunc = mode_demo_start_init;
                } else {
                    this->actionFunc = mode_demo_start;
                }
                break;
            case DNT_SIGNAL_CELEBRATE:
                this->action = DNT_ACTION_NONE;
                this->actor.colChkInfo.mass = 0;
                this->timer3 = (s16)rnd_f(3.0f) + (s16)(this->type * 0.5f);
                this->actionFunc = mode_demo_jump_init;
                break;
            case DNT_SIGNAL_DANCE:
                this->action = DNT_ACTION_DANCE;
                this->actionFunc = mode_demo_jump_init;
                break;
            case DNT_SIGNAL_HIDE:
                this->actionFunc = mode_demo_down_init;
                break;
            case DNT_SIGNAL_RETURN:
                this->actionFunc = mode_demo_runaway_init;
                break;
            case DNT_SIGNAL_UNUSED:
                this->actionFunc = mode_demo_happy_init;
                break;
            case DNT_SIGNAL_NONE:
                break;
        }
    }
    if (this->stageSignal != DNT_SIGNAL_NONE) {
        this->stageSignal = DNT_SIGNAL_NONE;
    }
    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState >= 3) {
            this->eyeState = 0;
            this->blinkTimer = (s16)rnd_f(60.0f) + 20;
        }
    }
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    if (this->type == ENDNTNOMAL_TARGET) {
        CollisionCheck_Uty_setSword4Pos(&this->targetQuad, &this->targetVtx[0], &this->targetVtx[1], &this->targetVtx[2],
                                 &this->targetVtx[3]);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->targetQuad.base);
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCyl);
        if (this->isSolid) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCyl.base);
        }
    }
}

s32 En_Dnt_Nomal_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDntNomal* this = (EnDntNomal*)thisx;

    if ((limbIndex == 1) || (limbIndex == 3) || (limbIndex == 4) || (limbIndex == 5) || (limbIndex == 6)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1733);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, color_data[this->type - ENDNTNOMAL_STAGE].r,
                       color_data[this->type - ENDNTNOMAL_STAGE].g, color_data[this->type - ENDNTNOMAL_STAGE].b, 255);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1743);
    }
    return false;
}

void En_Dnt_Nomal_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnDntNomal* this = (EnDntNomal*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (this->type == ENDNTNOMAL_TARGET) {
        if (limbIndex == 5) {
            Matrix_Position(&zeroVec, &this->mouthPos);
        }
    } else if (limbIndex == 7) {
        Matrix_Position(&zeroVec, &this->mouthPos);
    }
}

void En_Dnt_Nomal_actor_draw(Actor* thisx, PlayState* play) {
    static void* dnk_eye_txt[] = { gDntStageEyeOpenTex, gDntStageEyeHalfTex, gDntStageEyeShutTex };
    EnDntNomal* this = (EnDntNomal*)thisx;
    Vec3f dustScale = { 0.25f, 0.25f, 0.25f };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1790);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dnk_eye_txt[this->eyeState]));
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Dnt_Nomal_draw_sub,
                      En_Dnt_Nomal_draw_sub2, this);
    Matrix_translate(this->flowerPos.x, this->flowerPos.y, this->flowerPos.z, MTXMODE_NEW);
    Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, color_data[this->type - ENDNTNOMAL_STAGE].r,
                   color_data[this->type - ENDNTNOMAL_STAGE].g, color_data[this->type - ENDNTNOMAL_STAGE].b, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dnt_nomal.c", 1814);
    gSPDisplayList(POLY_OPA_DISP++, gDntStageFlowerDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1817);
    if (this->actionFunc == mode_demo_walk) {
        Shadow_draw(&this->actor.world.pos, &dustScale, 255, play);
    }
}

void En_Dnt_Nomal_MATO_actor_draw(Actor* thisx, PlayState* play) {
    EnDntNomal* this = (EnDntNomal*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1833);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, En_Dnt_Nomal_draw_sub2, this);
    Matrix_translate(this->flowerPos.x, this->flowerPos.y, this->flowerPos.z, MTXMODE_NEW);
    Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dnt_nomal.c", 1848);
    gSPDisplayList(POLY_OPA_DISP++, gHintNutsFlowerDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dnt_nomal.c", 1851);
}
