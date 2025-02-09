static void carry_request_set(EnGo2* this, PlayState* play, s32 getItemId) {
    this->getItemId = getItemId;
    Actor_carry_request_set2(&this->actor, play, getItemId, this->actor.xzDistToPlayer + 1.0f,
                       fabsf(this->actor.yDistToPlayer) + 1.0f);
}

static s32 calc_pad_on_cnt(EnGo2* this, PlayState* play) {
    s16 dialogState = message_check(&play->msgCtx);

    if ((this->dialogState == TEXT_STATE_AWAITING_NEXT) || (this->dialogState == TEXT_STATE_EVENT) ||
        (this->dialogState == TEXT_STATE_CLOSING) || (this->dialogState == TEXT_STATE_DONE_HAS_NEXT)) {
        if (dialogState != this->dialogState) {
            this->unk_20C++;
        }
    }

    this->dialogState = dialogState;
    return dialogState;
}

u16 help_go_mes(EnGo2* this) {
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

u16 boso_go_set_message(PlayState* play, EnGo2* this) {
    if (GET_INFTABLE(INFTABLE_11E)) {
        return 0x3013;
    } else if (CUR_CAPACITY(UPG_BOMB_BAG) >= 20 && this->waypoint > 7 && this->waypoint < 12) {
        return 0x3012;
    } else {
        return 0x3011;
    }
}

s16 boso_go_end_message(PlayState* play, EnGo2* this) {
    s32 bombBagUpgrade;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return NPC_TALK_STATE_ACTION;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x3012) {
                    this->actionFunc = go_carry_request;
                    bombBagUpgrade = CUR_CAPACITY(UPG_BOMB_BAG) == 30 ? GI_BOMB_BAG_40 : GI_BOMB_BAG_30;
                    carry_request_set(this, play, bombBagUpgrade);
                    message_close(play);
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

u16 tome_go_set_message(PlayState* play, EnGo2* this) {
    return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3027 : 0x300A;
}

// DMT Goron by Bomb Flower Choice
s16 tome_go_end_message(PlayState* play, EnGo2* this) {
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            if ((this->actor.textId == 0x300B) && !GET_INFTABLE(INFTABLE_EB)) {
                SET_INFTABLE(INFTABLE_EB);
                return NPC_TALK_STATE_ACTION;
            } else {
                return NPC_TALK_STATE_IDLE;
            }
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                // Ask question to DMT Goron by bomb flower
                if (this->actor.textId == 0x300A) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x300B : 0x300C;
                    } else {
                        this->actor.textId = 0x300D;
                    }
                    message_set2(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            FALLTHROUGH;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

u16 b_goron_set_message(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_EVENTCHKINF(EVENTCHKINF_23) ? 0x3026 : 0x3009;
    }
}

s16 b_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 a_goron_set_message(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_EVENTCHKINF(EVENTCHKINF_23) ? 0x3021 : GET_INFTABLE(INFTABLE_E0) ? 0x302A : 0x3008;
    }
}

s16 a_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3008) {
            SET_INFTABLE(INFTABLE_E0);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 d_goron_set_message(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return GET_INFTABLE(INFTABLE_F0) ? 0x3015 : 0x3014;
    }
}

s16 d_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3014) {
            SET_INFTABLE(INFTABLE_F0);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 e_goron_set_message(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3067;
    } else {
        return GET_INFTABLE(INFTABLE_F4) ? 0x3017 : 0x3016;
    }
}

s16 e_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3016) {
            SET_INFTABLE(INFTABLE_F4);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 f_goron_set_message(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x302C
               : !Actor_Environment_sw_Check(play, 0x1B)   ? 0x3017
               : GET_INFTABLE(INFTABLE_F8)      ? 0x3019
                                                : 0x3018;
    }
}

s16 f_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3018) {
            SET_INFTABLE(INFTABLE_F8);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 gorokid_set_message(PlayState* play, EnGo2* this) {
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

s16 gorokid_end_message(PlayState* play, EnGo2* this) {
    switch (calc_pad_on_cnt(this, play)) {
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x3036:
                    carry_request_set(this, play, GI_TUNIC_GORON);
                    this->actionFunc = go_carry_request;
                    return NPC_TALK_STATE_ACTION;
                case 0x3037:
                    SET_INFTABLE(INFTABLE_10E);
                    FALLTHROUGH;
                default:
                    return NPC_TALK_STATE_IDLE;
            }
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x3034) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = GET_INFTABLE(INFTABLE_10B) ? 0x3033 : 0x3035;
                        if (this->actor.textId == 0x3035) {
                            Nai_StopFx(NA_SE_EN_GOLON_CRY);
                        }
                    } else {
                        this->actor.textId = GET_INFTABLE(INFTABLE_10B) ? 0x3036 : 0x3033;
                        if (this->actor.textId == 0x3036) {
                            Nai_StopFx(NA_SE_EN_GOLON_CRY);
                        }
                    }
                    message_set2(play, this->actor.textId);
                    this->unk_20C = 0;
                }
            } else {
                break;
            }
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                switch (this->actor.textId) {
                    case 0x3035:
                        SET_INFTABLE(INFTABLE_10B);
                        FALLTHROUGH;
                    case 0x3032:
                    case 0x3033:
                        this->actor.textId = 0x3034;
                        message_set2(play, this->actor.textId);
                        return NPC_TALK_STATE_TALKING;
                    default:
                        return NPC_TALK_STATE_ACTION;
                }
            }
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

u16 daigoro_set_message(PlayState* play, EnGo2* this) {
    Player* player = GET_PLAYER(play);

    if (z_common_data.save.info.playerData.bgsFlag) {
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

s16 daigoro_end_message(PlayState* play, EnGo2* this) {
    s32 unusedPad;
    u8 dialogState = this->dialogState;

    switch (calc_pad_on_cnt(this, play)) {
#if OOT_VERSION < PAL_1_0
        case TEXT_STATE_CLOSING:
#else
        case TEXT_STATE_DONE:
#endif
            if (this->actor.textId == 0x305E) {
                if (!z_common_data.save.info.playerData.bgsFlag) {
                    carry_request_set(this, play, GI_SWORD_BIGGORON);
                    this->actionFunc = go_carry_request;
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
                    if (Actor_get_item_check(play) != EXCH_ITEM_CLAIM_CHECK) {
                        break;
                    }
                    FALLTHROUGH;
                case 0x3059:
                    if (dialogState == TEXT_STATE_NONE) {
                        Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EN_GOLON_WAKE_UP, 60);
                    }
                    FALLTHROUGH;
                case 0x3054:
                    if (dialogState == TEXT_STATE_NONE) {
                        Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    }
                    break;
            }
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                if ((this->actor.textId == 0x3054) || (this->actor.textId == 0x3055)) {
                    if (play->msgCtx.choiceIndex == 0) {
                        carry_request_set(this, play, GI_PRESCRIPTION);
                        this->actionFunc = go_carry_request;
                        return NPC_TALK_STATE_ACTION;
                    }
                    this->actor.textId = 0x3056;
                    message_set2(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x3059) {
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->actionFunc = go_megusuri_demo;
                }
                return NPC_TALK_STATE_ACTION;
            }
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

u16 help_go_set_message(PlayState* play, EnGo2* this) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_S(this->actor.params, 10, 6))) {
        return 0x3071;
    } else {
        return 0x3051;
    }
}

s16 help_go_end_message(PlayState* play, EnGo2* this) {
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return NPC_TALK_STATE_IDLE;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x3071) {
                    this->actor.textId = help_go_mes(this);
                    message_set2(play, this->actor.textId);
                }
                return NPC_TALK_STATE_TALKING;
            }
            FALLTHROUGH;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

u16 i_goron_set_message(PlayState* play, EnGo2* this) {
    return !LINK_IS_ADULT ? GET_INFTABLE(INFTABLE_E3) ? 0x3022 : 0x300E : 0x3043;
}

s16 i_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x300E) {
            SET_INFTABLE(INFTABLE_E3);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

// Goron in child market bazaar after obtaining Goron Ruby
u16 hiya_go_set_message(PlayState* play, EnGo2* this) {
    return 0x7122;
}

s16 hiya_go_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

u16 j_goron_set_message(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        if (Actor_Environment_sw_Check(play, 0x1C)) {
            return 0x302F;
        } else {
            return GET_INFTABLE(INFTABLE_E6) ? 0x3025 : 0x3024;
        }
    } else {
        return 0x3043;
    }
}

s16 j_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3024) {
            SET_INFTABLE(INFTABLE_E6);
        }
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}

// Goron at base of DMT summit
u16 k_goron_set_message(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3065 : 0x3064;
    } else {
        return 0x3043;
    }
}

s16 k_goron_end_message(PlayState* play, EnGo2* this) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    } else {
        return NPC_TALK_STATE_TALKING;
    }
}
