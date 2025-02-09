/*
 * File: z_en_sth.c
 * Overlay: ovl_En_Sth
 * Description: Uncursed House of Skulltula People
 */

#include "terminal.h"
#include "z_en_sth.h"
#include "assets/objects/object_ahg/object_ahg.h"
#include "assets/objects/object_boj/object_boj.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Sth_Actor_ct(Actor* thisx, PlayState* play);
void En_Sth_Actor_dt(Actor* thisx, PlayState* play);
void En_Sth_Actor_wait(Actor* thisx, PlayState* play);
void En_Sth_Actor_move(Actor* thisx, PlayState* play);
void En_Sth_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait2(EnSth* this, PlayState* play);
void sth_0_wait(EnSth* this, PlayState* play);
void sth_c_orei0(EnSth* this, PlayState* play);
void sth_1_wait(EnSth* this, PlayState* play);

ActorProfile En_Sth_Profile = {
    /**/ ACTOR_EN_STH,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnSth),
    /**/ En_Sth_Actor_ct,
    /**/ En_Sth_Actor_dt,
    /**/ En_Sth_Actor_wait,
    /**/ NULL,
};

#include "assets/overlays/ovl_En_Sth/z_en_sth.c"

static ColliderCylinderInit EnSthOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
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
    { 30, 40, 0, { 0, 0, 0 } },
};

static s16 en_sth_shape_data[6] = {
    OBJECT_AHG, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ,
};

static FlexSkeletonHeader* mdl_info[6] = {
    &gHylianMan1Skel, &gHylianMan2Skel, &gHylianMan2Skel, &gHylianMan2Skel, &gHylianMan2Skel, &gHylianMan2Skel,
};

static AnimationHeader* soft_anime_info[6] = {
    &sth_oya_matsu_soft_anim_tbl_info, &sth_ko_matsu_soft_anim_tbl_info, &sth_ko_matsu_soft_anim_tbl_info, &sth_ko_matsu_soft_anim_tbl_info, &sth_ko_matsu_soft_anim_tbl_info, &sth_ko_matsu_soft_anim_tbl_info,
};

static EnSthActionFunc prc0_tbl[6] = {
    sth_0_wait, sth_1_wait, sth_1_wait,
    sth_1_wait,  sth_1_wait, sth_1_wait,
};

static u16 event_tlb[6] = {
    0,
    EVENTCHKINF_MASK(EVENTCHKINF_SKULLTULA_REWARD_10),
    EVENTCHKINF_MASK(EVENTCHKINF_SKULLTULA_REWARD_20),
    EVENTCHKINF_MASK(EVENTCHKINF_SKULLTULA_REWARD_30),
    EVENTCHKINF_MASK(EVENTCHKINF_SKULLTULA_REWARD_40),
    EVENTCHKINF_MASK(EVENTCHKINF_SKULLTULA_REWARD_50),
};

static s16 item_tbl[6] = {
    GI_RUPEE_GOLD, GI_WALLET_ADULT, GI_STONE_OF_AGONY, GI_WALLET_GIANT, GI_BOMBCHUS_10, GI_HEART_PIECE,
};

void En_Sth_actor_set_process(EnSth* this, EnSthActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Sth_Actor_ct(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;

    s16 objectId;
    s32 params = this->actor.params;
    s32 objectSlot;

    PRINTF(VT_FGCOL(BLUE) "金スタル屋 no = %d\n" VT_RST, params); // "Gold Skulltula Shop"
    if (this->actor.params == 0) {
        if (z_common_data.save.info.inventory.gsTokens < 100) {
            Actor_delete(&this->actor);
            // "Gold Skulltula Shop I still can't be a human"
            PRINTF("金スタル屋 まだ 人間に戻れない \n");
            return;
        }
    } else if (z_common_data.save.info.inventory.gsTokens < (this->actor.params * 10)) {
        Actor_delete(&this->actor);
        // "Gold Skulltula Shop I still can't be a human"
        PRINTF(VT_FGCOL(BLUE) "金スタル屋 まだ 人間に戻れない \n" VT_RST);
        return;
    }

    objectId = en_sth_shape_data[params];
    if (objectId != OBJECT_GAMEPLAY_KEEP) {
        objectSlot = Object_Exchange_bank_check(&play->objectCtx, objectId);
    } else {
        objectSlot = 0;
    }

    PRINTF("bank_ID = %d\n", objectSlot);
    if (objectSlot < 0) {
        ASSERT(0, "0", "../z_en_sth.c", 1564);
    }
    this->requiredObjectSlot = objectSlot;
    this->drawFunc = En_Sth_Actor_draw;
    Actor_set_scale(&this->actor, 0.01f);
    En_Sth_actor_set_process(this, move_wait2);
    this->actor.draw = NULL;
    this->unk_2B2 = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
}

void sth_common_ct(EnSth* this, PlayState* play) {
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnSthOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.update = En_Sth_Actor_move;
    this->actor.draw = this->drawFunc;
}

void sth_0_ct(EnSth* this, PlayState* play) {
    s32 pad;
    s16* params;

    sth_common_ct(this, play);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->requiredObjectSlot].segment);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, mdl_info[this->actor.params], NULL, this->jointTable, this->morphTable,
                       16);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, soft_anime_info[this->actor.params]);

    params = &this->actor.params;
    this->eventFlag = event_tlb[*params];
    if (z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_SKULLTULA_REWARD] & this->eventFlag) {
        En_Sth_actor_set_process(this, prc0_tbl[*params]);
    } else {
        En_Sth_actor_set_process(this, sth_c_orei0);
    }
}

void En_Sth_Actor_dt(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void move_wait2(EnSth* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actionFunc = sth_0_ct;
    }
}

void sth_search_furimuki(EnSth* this, PlayState* play) {
    s32 pad;
    s16 diffRot = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(diffRot) <= 0x4000) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 0xFA0, 0x64);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2AC, this->actor.focus.pos);
    } else {
        if (diffRot < 0) {
            add_calc_short_angle2(&this->headRot.y, -0x2000, 6, 0x1838, 0x100);
        } else {
            add_calc_short_angle2(&this->headRot.y, 0x2000, 6, 0x1838, 0x100);
        }
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xC, 0x3E8, 0x64);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void sth_search_normal(EnSth* this, PlayState* play) {
    s16 diffRot = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(diffRot) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2AC, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRot.x, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->headRot.y, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->unk_2AC.x, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->unk_2AC.y, 0, 6, 0x1838, 0x64);
    }
}

void sth_c_talk(EnSth* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        if (this->actor.params == 0) {
            En_Sth_actor_set_process(this, sth_0_wait);
        } else {
            En_Sth_actor_set_process(this, sth_1_wait);
        }
    }
    sth_search_furimuki(this, play);
}

void sth_0_wait(EnSth* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        En_Sth_actor_set_process(this, sth_c_talk);
    } else {
        this->actor.textId = 0x23;
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
    sth_search_normal(this, play);
}

void sth_c_orei2_request(EnSth* this, PlayState* play) {
    u16 getItemId = item_tbl[this->actor.params];

    switch (this->actor.params) {
        case 1:
        case 3:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    getItemId = GI_WALLET_ADULT;
                    break;

                case 1:
                    getItemId = GI_WALLET_GIANT;
                    break;
            }
            break;
    }

    Actor_carry_request_set2(&this->actor, play, getItemId, 10000.0f, 50.0f);
}

void sth_c_orei2(EnSth* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        En_Sth_actor_set_process(this, sth_c_talk);
        z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_SKULLTULA_REWARD] |= this->eventFlag;
    } else {
        sth_c_orei2_request(this, play);
    }
    sth_search_furimuki(this, play);
}

void sth_c_orei1(EnSth* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        En_Sth_actor_set_process(this, sth_c_orei2);
        sth_c_orei2_request(this, play);
    }
    sth_search_furimuki(this, play);
}

void sth_c_orei0(EnSth* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        En_Sth_actor_set_process(this, sth_c_orei1);
    } else {
        if (this->actor.params == 0) {
            this->actor.textId = 0x28;
        } else {
            this->actor.textId = 0x21;
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
    sth_search_normal(this, play);
}

void sth_1_wait(EnSth* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        En_Sth_actor_set_process(this, sth_c_talk);
    } else {
        if (z_common_data.save.info.inventory.gsTokens < 50) {
            this->actor.textId = 0x20;
        } else {
            this->actor.textId = 0x1F;
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
    sth_search_normal(this, play);
}

void En_Sth_Actor_wait(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;

    this->actionFunc(this, play);
}

void En_Sth_Actor_move(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, play);

    // Likely an unused blink timer and eye index
    if (DECR(this->unk_2B6) == 0) {
        this->unk_2B6 = get_random_timer(0x3C, 0x3C);
    }
    this->unk_2B4 = this->unk_2B6;
    if (this->unk_2B4 >= 3) {
        this->unk_2B4 = 0;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSth* this = (EnSth*)thisx;

    s32 fidgetFrequency;

    if (limbIndex == 15) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
        *dList = sth_atama_model;
    }

    if (this->unk_2B2 & 2) {
        this->unk_2B2 &= ~2;
        return 0;
    }

    if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
        fidgetFrequency = limbIndex * FIDGET_FREQ_LIMB;
        rot->y += sin_s(play->state.frames * (fidgetFrequency + FIDGET_FREQ_Y)) * FIDGET_AMPLITUDE;
        rot->z += cos_s(play->state.frames * (fidgetFrequency + FIDGET_FREQ_Z)) * FIDGET_AMPLITUDE;
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 700.0f, 400.0f, 0.0f };
    EnSth* this = (EnSth*)thisx;

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
        if (this->actor.params != 0) { // Children
            OPEN_DISPS(play->state.gfxCtx, "../z_en_sth.c", 2079);

            gSPDisplayList(POLY_OPA_DISP++, sth_zura_model);

            CLOSE_DISPS(play->state.gfxCtx, "../z_en_sth.c", 2081);
        }
    }
}

static Gfx* pa(GraphicsContext* play, u8 envR, u8 envG, u8 envB, u8 envA) {
    Gfx* dList;

    dList = GRAPH_ALLOC(play, 2 * sizeof(Gfx));
    gDPSetEnvColor(dList, envR, envG, envB, envA);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void En_Sth_Actor_draw(Actor* thisx, PlayState* play) {
    static Color_RGB8 color_table[6] = {
        { 190, 110, 0 }, { 0, 180, 110 }, { 0, 255, 80 }, { 255, 160, 60 }, { 190, 230, 250 }, { 240, 230, 120 },
    };
    EnSth* this = (EnSth*)thisx;
    Color_RGB8* envColor1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_sth.c", 2133);

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->requiredObjectSlot].segment);
    _polygon_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               pa(play->state.gfxCtx, color_table[this->actor.params].r,
                                     color_table[this->actor.params].g, color_table[this->actor.params].b, 255));

    if (this->actor.params == 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, pa(play->state.gfxCtx, 190, 110, 0, 255));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09, pa(play->state.gfxCtx, 90, 110, 130, 255));
    }
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_sth.c", 2176);
}
