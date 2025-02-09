void EnGo2_GetItem(EnGo2* this, PlayState* play, s32 getItemId) {
    this->getItemId = getItemId;
    Actor_OfferGetItem(&this->actor, play, getItemId, this->actor.xzDistToPlayer + 1.0f,
                       fabsf(this->actor.yDistToPlayer) + 1.0f);
}

s32 EnGo2_GetDialogState(EnGo2* this, PlayState* play) {
    s16 dialogState = Message_GetState(&play->msgCtx);

    if ((this->dialogState == TEXT_STATE_AWAITING_NEXT) || (this->dialogState == TEXT_STATE_EVENT) ||
        (this->dialogState == TEXT_STATE_CLOSING) || (this->dialogState == TEXT_STATE_DONE_HAS_NEXT)) {
        if (dialogState != this->dialogState) {
            this->unk_20C++;
        }
    }

    this->dialogState = dialogState;
    return dialogState;
}

u16 EnGo2_GoronFireGenericGetTextId(EnGo2* this) {
    switch (PARAMS_GET_S(this->actor.params, 10, 6)) {
        case 3:
            return 0x3069;
        case 5:
            return 0x306A;
        case 4:
            return 0x306B;
        case 2:
            return 0x306C;
        case 10:
            return 0x306D;
        case 8:
            return 0x306E;
        case 11:
            return 0x306F;
        case 1:
            return 0x3070;
        default:
            return 0x3052;
    }
}

u16 EnGo2_GetTextIdGoronCityRollingBig(PlayState* play, EnGo2* this) {
    if (GET_INFTABLE(INFTABLE_11E)) {
        return 0x3013;
    } else if (CUR_CAPACITY(UPG_BOMB_BAG) >= 20 && this->waypoint > 7 && this->waypoint < 12) {
        return 0x3012;
    } else {
        return 0x3011;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityRollingBig(PlayState* play, EnGo2* this) {
    s32 bombBagUpgrade;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return NPC_TALK_STATE_ACTION;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3012) {
                    this->actionFunc = EnGo2_SetupGetItem;
                    bombBagUpgrade = CUR_CAPACITY(UPG_BOMB_BAG) == 30 ? GI_BOMB_BAG_40 : GI_BOMB_BAG_30;
                    EnGo2_GetItem(this, play, bombBagUpgrade);
                    Message_CloseTextbox(play);
                    SET_INFTABLE(INFTABLE_11E);
                    return NPC_TALK_STATE_ACTION;
                } else {
                    return NPC_TALK_STATE_ACTION;
                }
            }
            FALLTHROUGH;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronDmtBombFlower(PlayState* play, EnGo2* this) {
    return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3027 : 0x300A;
}

// DMT Goron by Bomb Flower Choice
s16 EnGo2_UpdateTalkStateGoronDmtBombFlower(PlayState* play, EnGo2* this) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            if ((this->actor.textId == 0x300B) && !GET_INFTABLE(INFTABLE_EB)) {
                SET_INFTABLE(INFTABLE_EB);
                return NPC_TALK_STATE_ACTION;
            } else {
                return NPC_TALK_STATE_IDLE;
            }
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                // Ask question to DMT Goron by bomb flower
                if (this->actor.textId == 0x300A) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x300B : 0x300C;
                    } else {
                        this->actor.textId = 0x300D;
                    }
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            FALLTHROUGH;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronDmtRollingSmall(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_EVENTCHKINF(EVENTCHKINF_23) ? 0x3026 : 0x3009;
    }
}

s16 EnGo2_UpdateTalkStateGoronDmtRollingSmall(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronDmtDcEntrance(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_EVENTCHKINF(EVENTCHKINF_23) ? 0x3021 : GET_INFTABLE(INFTABLE_E0) ? 0x302A : 0x3008;
    }
}

s16 EnGo2_UpdateTalkStateGoronDmtDcEntrance(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3008) {
            SET_INFTABLE(INFTABLE_E0);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityEntrance(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_INFTABLE(INFTABLE_F0) ? 0x3015 : 0x3014;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityEntrance(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3014) {
            SET_INFTABLE(INFTABLE_F0);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityIsland(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3067;
    } else {
        return GET_INFTABLE(INFTABLE_F4) ? 0x3017 : 0x3016;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityIsland(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3016) {
            SET_INFTABLE(INFTABLE_F4);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityLowestFloor(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x302C
               : !Flags_GetSwitch(play, 0x1B)   ? 0x3017
               : GET_INFTABLE(INFTABLE_F8)      ? 0x3019
                                                : 0x3018;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityLowestFloor(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3018) {
            SET_INFTABLE(INFTABLE_F8);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityLink(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        return GET_INFTABLE(INFTABLE_10F) ? 0x3042 : 0x3041;
    } else if (CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
        return GET_INFTABLE(INFTABLE_10E) ? 0x3038 : 0x3037;
    } else if (GET_INFTABLE(INFTABLE_10C)) {
        this->unk_20C = 0;
        this->dialogState = TEXT_STATE_NONE;
        return GET_INFTABLE(INFTABLE_10A) ? 0x3033 : 0x3032;
    } else {
        return 0x3030;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityLink(PlayState* play, EnGo2* this) {
    switch (EnGo2_GetDialogState(this, play)) {
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x3036:
                    EnGo2_GetItem(this, play, GI_TUNIC_GORON);
                    this->actionFunc = EnGo2_SetupGetItem;
                    return NPC_TALK_STATE_ACTION;
                case 0x3037:
                    SET_INFTABLE(INFTABLE_10E);
                    FALLTHROUGH;
                default:
                    return NPC_TALK_STATE_IDLE;
            }
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3034) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = GET_INFTABLE(INFTABLE_10B) ? 0x3033 : 0x3035;
                        if (this->actor.textId == 0x3035) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    } else {
                        this->actor.textId = GET_INFTABLE(INFTABLE_10B) ? 0x3036 : 0x3033;
                        if (this->actor.textId == 0x3036) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    }
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->unk_20C = 0;
                }
            } else {
                break;
            }
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (this->actor.textId) {
                    case 0x3035:
                        SET_INFTABLE(INFTABLE_10B);
                        FALLTHROUGH;
                    case 0x3032:
                    case 0x3033:
                        this->actor.textId = 0x3034;
                        Message_ContinueTextbox(play, this->actor.textId);
                        return NPC_TALK_STATE_TALKING;
                    default:
                        return NPC_TALK_STATE_ACTION;
                }
            }
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

u16 EnGo2_GetTextIdGoronDmtBiggoron(PlayState* play, EnGo2* this) {
    Player* player = GET_PLAYER(play);

    if (gSaveContext.save.info.playerData.bgsFlag) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_CLAIM_CHECK) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) {
        player->exchangeItemId = EXCH_ITEM_EYE_DROPS;
        return 0x3058;
    } else {
        player->exchangeItemId = EXCH_ITEM_BROKEN_GORONS_SWORD;
        return 0x3053;
    }
}

s16 EnGo2_UpdateTalkStateGoronDmtBiggoron(PlayState* play, EnGo2* this) {
    s32 unusedPad;
    u8 dialogState = this->dialogState;

    switch (EnGo2_GetDialogState(this, play)) {
#if OOT_VERSION < PAL_1_0
        case TEXT_STATE_CLOSING:
#else
        case TEXT_STATE_DONE:
#endif
            if (this->actor.textId == 0x305E) {
                if (!gSaveContext.save.info.playerData.bgsFlag) {
                    EnGo2_GetItem(this, play, GI_SWORD_BIGGORON);
                    this->actionFunc = EnGo2_SetupGetItem;
                    return NPC_TALK_STATE_ACTION;
                } else {
                    return NPC_TALK_STATE_IDLE;
                }
            } else {
                return NPC_TALK_STATE_IDLE;
            }
        case TEXT_STATE_DONE_FADING:
            switch (this->actor.textId) {
                case 0x305E:
                    if (Actor_GetPlayerExchangeItemId(play) != EXCH_ITEM_CLAIM_CHECK) {
                        break;
                    }
                    FALLTHROUGH;
                case 0x3059:
                    if (dialogState == TEXT_STATE_NONE) {
                        func_800F4524(&gSfxDefaultPos, NA_SE_EN_GOLON_WAKE_UP, 60);
                    }
                    FALLTHROUGH;
                case 0x3054:
                    if (dialogState == TEXT_STATE_NONE) {
                        Audio_PlaySfxGeneral(NA_SE_SY_TRE_BOX_APPEAR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    }
                    break;
            }
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if ((this->actor.textId == 0x3054) || (this->actor.textId == 0x3055)) {
                    if (play->msgCtx.choiceIndex == 0) {
                        EnGo2_GetItem(this, play, GI_PRESCRIPTION);
                        this->actionFunc = EnGo2_SetupGetItem;
                        return NPC_TALK_STATE_ACTION;
                    }
                    this->actor.textId = 0x3056;
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3059) {
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->actionFunc = EnGo2_BiggoronEyedrops;
                }
                return NPC_TALK_STATE_ACTION;
            }
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

u16 EnGo2_GetTextIdGoronFireGeneric(PlayState* play, EnGo2* this) {
    if (Flags_GetSwitch(play, PARAMS_GET_S(this->actor.params, 10, 6))) {
        return 0x3071;
    } else {
        return 0x3051;
    }
}

s16 EnGo2_UpdateTalkStateGoronFireGeneric(PlayState* play, EnGo2* this) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return NPC_TALK_STATE_IDLE;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3071) {
                    this->actor.textId = EnGo2_GoronFireGenericGetTextId(this);
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            FALLTHROUGH;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityStairwell(PlayState* play, EnGo2* this) {
    return !LINK_IS_ADULT ? GET_INFTABLE(INFTABLE_E3) ? 0x3022 : 0x300E : 0x3043;
}

s16 EnGo2_UpdateTalkStateGoronCityStairwell(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x300E) {
            SET_INFTABLE(INFTABLE_E3);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

// Goron in child market bazaar after obtaining Goron Ruby
u16 EnGo2_GetTextIdGoronMarketBazaar(PlayState* play, EnGo2* this) {
    return 0x7122;
}

s16 EnGo2_UpdateTalkStateGoronMarketBazaar(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 EnGo2_GetTextIdGoronCityLostWoods(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        if (Flags_GetSwitch(play, 0x1C)) {
            return 0x302F;
        } else {
            return GET_INFTABLE(INFTABLE_E6) ? 0x3025 : 0x3024;
        }
    } else {
        return 0x3043;
    }
}

s16 EnGo2_UpdateTalkStateGoronCityLostWoods(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3024) {
            SET_INFTABLE(INFTABLE_E6);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

// Goron at base of DMT summit
u16 EnGo2_GetTextIdGoronDmtFairyHint(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3065 : 0x3064;
    } else {
        return 0x3043;
    }
}

s16 EnGo2_UpdateTalkStateGoronDmtFairyHint(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}
