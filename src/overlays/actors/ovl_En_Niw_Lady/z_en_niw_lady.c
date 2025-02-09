#include "z_en_niw_lady.h"
#include "assets/objects/object_ane/object_ane.h"
#include "assets/objects/object_os_anime/object_os_anime.h"
#include "overlays/actors/ovl_En_Niw/z_en_niw.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Niw_Lady_actor_ct(Actor* thisx, PlayState* play);
void En_Niw_Lady_actor_dt(Actor* thisx, PlayState* play);
void En_Niw_Lady_actor_move(Actor* thisx, PlayState* play);

static void mode_dma_wait(EnNiwLady* this, PlayState* play);
void En_Niw_Lady_actor_draw(Actor* thisx, PlayState* play2);
static void mode_wait_init(EnNiwLady* this, PlayState* play);
void mode_message_init(EnNiwLady* this, PlayState* play);
void mode_ad_message_init(EnNiwLady* this, PlayState* play);
void mode_ad_message_check(EnNiwLady* this, PlayState* play);
void mode_ad_message_select_check(EnNiwLady* this, PlayState* play);
void mode_ad_item_niw_select(EnNiwLady* this, PlayState* play);
static void mode_player_item_request(EnNiwLady* this, PlayState* play);
void mode_ad_message_end(EnNiwLady* this, PlayState* play);
void mode_item_get_up(EnNiwLady* this, PlayState* play);
void mode_niw_check(EnNiwLady* this, PlayState* play);
void mode_item_up_init(EnNiwLady* this, PlayState* play);
static void mode_message_check(EnNiwLady* this, PlayState* play);

ActorProfile En_Niw_Lady_Profile = {
    /**/ ACTOR_EN_NIW_LADY,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ANE,
    /**/ sizeof(EnNiwLady),
    /**/ En_Niw_Lady_actor_ct,
    /**/ En_Niw_Lady_actor_dt,
    /**/ En_Niw_Lady_actor_move,
    /**/ NULL,
};

static s16 Message_data[] = {
    0x5036, 0x5070, 0x5072, 0x5037, 0x5038, 0x5039, 0x503A, 0x503B, 0x503D, 0x503C,
};

static s16 niw_birth_bit_check_data[] = {
    INFTABLE_MASK(INFTABLE_199), INFTABLE_MASK(INFTABLE_19A), INFTABLE_MASK(INFTABLE_19B), INFTABLE_MASK(INFTABLE_19C),
    INFTABLE_MASK(INFTABLE_19D), INFTABLE_MASK(INFTABLE_19E), INFTABLE_MASK(INFTABLE_19F),
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

void En_Niw_Lady_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNiwLady* this = (EnNiwLady*)thisx;

    this->aneObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_ANE);
    this->osAnimeObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_OS_ANIME);
    if ((this->osAnimeObjectSlot < 0) || (this->aneObjectSlot < 0)) {
        Actor_delete(thisx);
        return;
    }
    this->unk_278 = 0;
    if (play->sceneId == SCENE_IMPAS_HOUSE) {
        this->unk_278 = 1;
    }
    if ((this->unk_278 != 0) && IS_DAY) {
        Actor_delete(thisx);
        return;
    }
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ねぇちゃんうっふん ☆☆☆☆☆ %d\n" VT_RST, this->unk_278);
    PRINTF("\n\n");
    this->actionFunc = mode_dma_wait;
    thisx->cullingVolumeDistance = 600.0f;
}

void En_Niw_Lady_actor_dt(Actor* thisx, PlayState* play) {
    EnNiwLady* this = (EnNiwLady*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void anime_change_rtn(EnNiwLady* this, PlayState* play, s32 arg2) {
    f32 frames;

    if (get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY) != 0) {
        arg2 = 8;
    }
    if (arg2 != this->unk_270) {
        this->unk_275 = 0;
        this->unk_276 = 1;
        this->unk_270 = arg2;
        switch (arg2) {
            case 10:
                this->unk_275 = 1;
                FALLTHROUGH;
            case 9:
                frames = Si2_anime_end_frame(&gObjOsAnim_07D0);
                Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_07D0, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
                break;
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
            case 21:
            case 22:
            case 24:
            case 29:
                frames = Si2_anime_end_frame(&gObjOsAnim_9F94);
                Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_9F94, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
                break;
            case 7:
            case 20:
            case 23:
            case 25:
            case 26:
            case 27:
            case 28:
                frames = Si2_anime_end_frame(&gObjOsAnim_0718);
                Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_0718, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
                break;
            case 100:
                frames = Si2_anime_end_frame(&gObjOsAnim_A630);
                Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_A630, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
                this->unk_276 = 0;
                break;
        }
        return;
    }
}

static void mode_dma_wait(EnNiwLady* this, PlayState* play) {
    f32 frames;
    s32 pad;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->aneObjectSlot) &&
        Object_Exchange_bank_dma_check(&play->objectCtx, this->osAnimeObjectSlot)) {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->aneObjectSlot].segment);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gCuccoLadySkel, NULL, this->jointTable, this->morphTable, 16);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->osAnimeObjectSlot].segment);
        this->unk_27E = 1;
        this->actor.gravity = -3.0f;
        Actor_set_scale(&this->actor, 0.01f);
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
        this->unk_272 = 0;
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
        this->actor.draw = En_Niw_Lady_actor_draw;
        switch (this->unk_278) {
            case 0:
                if (!GET_ITEMGETINF(ITEMGETINF_0C) && !LINK_IS_ADULT) {
                    frames = Si2_anime_end_frame(&gObjOsAnim_A630);
                    Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_A630, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, 0.0f);
                } else {
                    frames = Si2_anime_end_frame(&gObjOsAnim_07D0);
                    Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_07D0, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, 0.0f);
                }
                if (LINK_IS_ADULT) {
                    this->actionFunc = mode_ad_message_init;
                } else {
                    this->actionFunc = mode_wait_init;
                }
                return;
            case 1:
                frames = Si2_anime_end_frame(&gObjOsAnim_07D0);
                Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_07D0, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, 0.0f);
                this->actionFunc = mode_message_init;
                return;
        }
    }
}

static void mode_wait_init(EnNiwLady* this, PlayState* play) {
    this->actor.textId = Message_data[0];
    this->unk_262 = TEXT_STATE_DONE;
    this->actionFunc = mode_niw_check;
}

void mode_niw_check(EnNiwLady* this, PlayState* play) {
    EnNiw* currentCucco;
    s32 pad[2];
    s32 phi_s1;

    this->cuccosInPen = 0;
    currentCucco = (EnNiw*)play->actorCtx.actorLists[ACTORCAT_PROP].head;
    while (currentCucco != NULL) {
        if (currentCucco->actor.id == ACTOR_EN_NIW) {
            if ((fabsf(currentCucco->actor.world.pos.x - 330.0f) < 90.0f) &&
                (fabsf(currentCucco->actor.world.pos.z - 1610.0f) < 190.0f)) {
                if (this->unk_26C == 0) {
                    z_common_data.save.info.infTable[INFTABLE_INDEX_199_19A_19B_19C_19D_19E_19F] |=
                        niw_birth_bit_check_data[currentCucco->unk_2AA];
                    if (BREG(1) != 0) {
                        // "GET inside the chicken fence!"
                        PRINTF(VT_FGCOL(GREEN) "☆ 鶏柵内ＧＥＴ！☆ %x\n" VT_RST, niw_birth_bit_check_data[currentCucco->unk_2AA]);
                    }
                }
                this->cuccosInPen++;
            } else if (this->unk_26C == 0) {
                z_common_data.save.info.infTable[INFTABLE_INDEX_199_19A_19B_19C_19D_19E_19F] &=
                    ~niw_birth_bit_check_data[currentCucco->unk_2AA];
            }
        }
        currentCucco = (EnNiw*)currentCucco->actor.next;
    }
    if (DEBUG_FEATURES && BREG(7) != 0) {
        this->cuccosInPen = BREG(7) - 1;
    }
    phi_s1 = this->cuccosInPen;
    if ((message_check(&play->msgCtx) == TEXT_STATE_NONE) || (message_check(&play->msgCtx) == TEXT_STATE_DONE)) {
        this->unk_26E = 101;
    }
    if (this->cuccosInPen >= 7) {
        phi_s1 = 8;
        if ((this->unk_26C < 2) && (this->unk_26C == 0)) {
            phi_s1 = 7;
        }
    }
    if ((this->unk_26C != 0) && (phi_s1 < 7)) {
        phi_s1 = 9;
    }
    this->actor.textId = Message_data[phi_s1];
    if (get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY) != 0) {
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY);
        this->unk_262 = TEXT_STATE_DONE;
    }
    if ((this->unk_26C != 0) && (phi_s1 != 9)) {
        phi_s1 = 10;
        this->unk_26E = 11;
    }
    if (Actor_talk_check(&this->actor, play)) {
        PRINTF("\n\n");
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ねぇちゃん選択\t ☆☆☆☆ %d\n" VT_RST, phi_s1);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ねぇちゃんハート     ☆☆☆☆ %d\n" VT_RST, this->unk_26C);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ねぇちゃん保存       ☆☆☆☆ %d\n" VT_RST, this->unk_26A);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ねぇちゃん今\t ☆☆☆☆ %d\n" VT_RST, this->cuccosInPen);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ this->actor.talk_message ☆☆ %x\n" VT_RST, this->actor.textId);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ this->message_end_code   ☆☆ %d\n" VT_RST, this->unk_262);
        PRINTF("\n\n");
        if (get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY) == 0) {
#if OOT_VERSION >= NTSC_1_1
            if (this->actor.textId == 0x503C) {
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
                this->unk_26C = 2;
                this->unk_262 = TEXT_STATE_EVENT;
                this->actionFunc = mode_item_up_init;
                return;
            }
#endif
            this->unk_26E = phi_s1 + 1;
            if (phi_s1 == 7) {
                Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                this->unk_26C = 1;
                this->unk_262 = TEXT_STATE_EVENT;
                this->unk_26A = this->cuccosInPen;
                PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 柵内BIT変更前 ☆☆ %x\n" VT_RST,
                       z_common_data.save.info.infTable[INFTABLE_INDEX_199_19A_19B_19C_19D_19E_19F]);
                z_common_data.save.info.infTable[INFTABLE_INDEX_199_19A_19B_19C_19D_19E_19F] &=
                    (u16) ~(INFTABLE_MASK(INFTABLE_199) | INFTABLE_MASK(INFTABLE_19A) | INFTABLE_MASK(INFTABLE_19B) |
                            INFTABLE_MASK(INFTABLE_19C) | INFTABLE_MASK(INFTABLE_19D) | INFTABLE_MASK(INFTABLE_19E) |
                            INFTABLE_MASK(INFTABLE_19F));
                PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 柵内BIT変更後 ☆☆ %x\n" VT_RST,
                       z_common_data.save.info.infTable[INFTABLE_INDEX_199_19A_19B_19C_19D_19E_19F]);
                PRINTF("\n\n");
                this->actionFunc = mode_item_up_init;
                return;
            }
            if (this->unk_26A != this->cuccosInPen) {
                if (this->cuccosInPen < this->unk_26A) {
                    Na_StartSystemSe_F(NA_SE_SY_ERROR);
#if OOT_VERSION < NTSC_1_1
                    if (phi_s1 == 9) {
                        this->unk_26C = 2;
                        this->unk_262 = TEXT_STATE_EVENT;
                        this->actionFunc = mode_item_up_init;
                    }
#endif
                } else if (phi_s1 + 1 < 9) {
                    Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
            if (this->unk_26A < this->cuccosInPen) {
                this->unk_26A = this->cuccosInPen;
                return;
            }
        }
    } else {
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
}

void mode_item_up_init(EnNiwLady* this, PlayState* play) {
    if (this->unk_262 == message_check(&play->msgCtx) && pad_on_check(play)) {
        message_close(play);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ハート ☆☆☆☆☆ %d\n" VT_RST, this->unk_26C);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 爆弾   ☆☆☆☆☆ %d\n" VT_RST, this->unk_272);
        PRINTF("\n\n");
        this->unk_26E = 0xB;
        if (!GET_ITEMGETINF(ITEMGETINF_0C)) {
            this->actor.parent = NULL;
            this->getItemId = GI_BOTTLE_EMPTY;
            Actor_carry_request_set2(&this->actor, play, GI_BOTTLE_EMPTY, 100.0f, 50.0f);
            this->actionFunc = mode_player_item_request;
            return;
        }
        if (this->unk_26C == 1) {
            this->getItemId = GI_RUPEE_PURPLE;
            Actor_carry_request_set2(&this->actor, play, GI_RUPEE_PURPLE, 100.0f, 50.0f);
            this->actionFunc = mode_player_item_request;
        }
        this->actionFunc = mode_niw_check;
    }
}

static s16 Message_Adult_Data[] = { 0x503E, 0x503F, 0x5047, 0x5040, 0x5042, 0x5043,
                                   0x5044, 0x00CF, 0x5045, 0x5042, 0x5027 };

void mode_ad_message_init(EnNiwLady* this, PlayState* play) {
    // "☆☆☆☆☆ Adult message check ☆☆☆☆☆"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ アダルトメッセージチェック ☆☆☆☆☆ \n" VT_RST);
    this->unk_262 = TEXT_STATE_DONE;
    this->unk_273 = 0;
    if (!GET_ITEMGETINF(ITEMGETINF_2C)) {
        if (this->unk_274 != 0) {
            this->unk_27A = 1;
        } else {
            this->unk_27A = 0;
        }
        this->unk_273 = 1;
        this->unk_262 = TEXT_STATE_CHOICE;
    } else {
        this->unk_27A = 2;
        if (!GET_ITEMGETINF(ITEMGETINF_2E)) {
            this->unk_27A = 3;
            if (GET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO)) {
                this->unk_27A = 9;
                if (this->unk_277 != 0) {
                    this->unk_27A = 10;
                }
            } else {
                this->unk_27A = 4;
            }
        }
    }
    this->actor.textId = Message_Adult_Data[this->unk_27A];
    this->actionFunc = mode_ad_message_check;
}

void mode_ad_message_check(EnNiwLady* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_NONE) || (message_check(&play->msgCtx) == TEXT_STATE_DONE)) {
        this->unk_26E = 11;
    }
    if (Actor_talk_check(&this->actor, play)) {
        s8 playerExchangeItemId = Actor_get_item_check(play);

        if ((playerExchangeItemId == EXCH_ITEM_POCKET_CUCCO) && GET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO)) {
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            player->actor.textId = Message_Adult_Data[5];
            this->unk_26E = this->unk_27A + 21;
            this->unk_262 = TEXT_STATE_CHOICE;
            this->actionFunc = mode_ad_item_niw_select;
        } else if (playerExchangeItemId != EXCH_ITEM_NONE) {
            player->actor.textId = Message_Adult_Data[7];
            this->unk_26E = this->unk_27A + 21;
        } else {
            this->unk_274 = 1;
            this->unk_26E = this->unk_27A + 21;
            this->actionFunc = !this->unk_273 ? mode_ad_message_init : mode_ad_message_select_check;
        }
        return;
    }

    Actor_talk_request_get_item(&this->actor, play, 50.0f, EXCH_ITEM_POCKET_CUCCO);
}

void mode_ad_message_select_check(EnNiwLady* this, PlayState* play) {
    if ((this->unk_262 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                message_close(play);
                this->actor.parent = NULL;
                Actor_carry_request_set2(&this->actor, play, GI_POCKET_EGG, 200.0f, 100.0f);
                this->actionFunc = mode_player_item_request;
                break;
            case 1:
                this->actor.textId = Message_Adult_Data[3];
                this->unk_26E = this->unk_27A + 21;
                message_set2(play, this->actor.textId);
                this->unk_262 = TEXT_STATE_EVENT;
                this->actionFunc = mode_ad_message_end;
                break;
        }
    }
}

void mode_ad_message_end(EnNiwLady* this, PlayState* play) {
    this->unk_26E = 11;
    if ((this->unk_262 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = mode_ad_message_init;
    }
}

void mode_ad_item_niw_select(EnNiwLady* this, PlayState* play) {
    if ((this->unk_262 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                message_close(play);
                this->actor.parent = NULL;
                Actor_carry_request_set2(&this->actor, play, GI_COJIRO, 200.0f, 100.0f);
                this->actionFunc = mode_player_item_request;
                break;
            case 1:
                message_close(play);
                this->unk_277 = 1;
                this->actor.textId = Message_Adult_Data[8];
                this->unk_26E = this->unk_27A + 21;
                message_set2(play, this->actor.textId);
                this->unk_262 = TEXT_STATE_EVENT;
                this->actionFunc = mode_ad_message_end;
                break;
        }
    }
}

static void mode_player_item_request(EnNiwLady* this, PlayState* play) {
    s32 getItemId;

    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_item_get_up;
    } else {
        getItemId = this->getItemId;
        if (LINK_IS_ADULT) {
            getItemId = !GET_ITEMGETINF(ITEMGETINF_2C) ? GI_POCKET_EGG : GI_COJIRO;
        }
        Actor_carry_request_set2(&this->actor, play, getItemId, 200.0f, 100.0f);
    }
}

void mode_item_get_up(EnNiwLady* this, PlayState* play) {
    if ((message_check(&play->msgCtx) != TEXT_STATE_DONE) || !pad_on_check(play)) {
        return;
    }
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
    if (LINK_IS_ADULT) {
        if (!GET_ITEMGETINF(ITEMGETINF_2C)) {
            SET_ITEMGETINF(ITEMGETINF_2C);
        } else {
            SET_ITEMGETINF(ITEMGETINF_2E);
        }
        this->actionFunc = mode_ad_message_init;
    } else {
        SET_ITEMGETINF(ITEMGETINF_0C);
        this->unk_262 = TEXT_STATE_DONE;
        this->actionFunc = mode_niw_check;
    }
}

void mode_message_init(EnNiwLady* this, PlayState* play) {
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 通常メッセージチェック ☆☆☆☆☆ \n" VT_RST);
    this->unk_262 = TEXT_STATE_DONE;
    this->actionFunc = mode_message_check;
}

static void mode_message_check(EnNiwLady* this, PlayState* play) {
    this->actor.textId = 0x503D;
    if (get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY) != 0) {
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_CUCCO_LADY);
    }
    if ((message_check(&play->msgCtx) == TEXT_STATE_NONE) || (message_check(&play->msgCtx) == TEXT_STATE_DONE)) {
        this->unk_26E = 8;
    }
    if (Actor_talk_check(&this->actor, play)) {
        this->unk_274 = 1;
        this->unk_26E = this->unk_27A + 9;
        this->actionFunc = mode_message_init;
    } else {
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
}

void En_Niw_Lady_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNiwLady* this = (EnNiwLady*)thisx;
    Player* player = GET_PLAYER(play);

    Actor_world_to_eye(thisx, 60.0f);
    this->interactInfo.trackPos = player->actor.world.pos;
    if (!LINK_IS_ADULT) {
        this->interactInfo.trackPos.y = player->actor.world.pos.y - 10.0f;
    }
    eye_moveM(thisx, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
    this->headRot = this->interactInfo.headRot;
    this->torsoRot = this->interactInfo.torsoRot;
    if (this->unk_276 == 0) {
        add_calc_short_angle2(&this->headRot.y, 0, 5, 3000, 0);
    }
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->osAnimeObjectSlot].segment);
    if (this->osAnimeObjectSlot >= 0) {
        if (this->unk_27E != 0) {
            if (this->unk_26E != 0) {
                this->unk_26E--;
                anime_change_rtn(this, play, this->unk_26E);
                this->unk_26E = 0;
            }
            Skeleton_Info2_anime_play(&this->skelAnime);
        }
        this->aneObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_ANE);
        if (this->aneObjectSlot < 0) {
            return;
        }

        this->actionFunc(this, play);

        if (this->unusedTimer2 != 0) {
            this->unusedTimer2--;
        }
        if (this->unusedRandomTimer != 0) {
            this->unusedRandomTimer--;
        }
        this->unusedTimer++;
        if (this->unusedRandomTimer == 0) {
            this->faceState++;
            if (this->faceState >= 3) {
                this->faceState = 0;
                this->unusedRandomTimer = ((s16)rnd_f(60.0f) + 0x14);
            }
        }
        Actor_BGcheck2(play, thisx, 20.0f, 20.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

static Gfx* npc_nothing(GraphicsContext* gfxCtx) {
    Gfx* dList;

    dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    gSPEndDisplayList(dList);
    return dList;
}

s32 En_Niw_Lady_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNiwLady* this = (EnNiwLady*)thisx;
    s32 pad;

    if (limbIndex == 15) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    if (limbIndex == 8) {
        rot->x += this->torsoRot.y;
    }
    if (this->unk_275 != 0) {
        if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
            // clang-format off
            rot->y += sin_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Y))) * FIDGET_AMPLITUDE;
            rot->z += cos_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Z))) * FIDGET_AMPLITUDE;
            // clang-format on
        }
    }
    return false;
}

void En_Niw_Lady_actor_draw(Actor* thisx, PlayState* play2) {
    static void* eye_txt[] = { gCuccoLadyEyeOpenTex, gCuccoLadyEyeHalfTex, gCuccoLadyEyeClosedTex };
    EnNiwLady* this = (EnNiwLady*)thisx;
    PlayState* play = (PlayState*)play2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_niw_lady.c", 1347);
    if (this->unk_27E != 0) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->faceState]));
        gSPSegment(POLY_OPA_DISP++, 0x0C, npc_nothing(play->state.gfxCtx));
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              En_Niw_Lady_draw_sub, NULL, this);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_niw_lady.c", 1370);
}
