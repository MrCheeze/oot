void message_set3(PlayState* play, Actor* actor, u16 textId) {
    message_set2(play, textId);
    actor->textId = textId;
}

/**
 * Tests if event_chk_inf flag is set.
 */
s32 event_check(s32 flag) {
    s32 mask = EVENTCHKINF_MASK(flag);
    s32 index = EVENTCHKINF_INDEX(flag);

    return z_common_data.save.info.eventChkInf[index] & mask;
}

/**
 * Sets event_chk_inf flag.
 */
void event_set(s32 flag) {
    s32 mask = EVENTCHKINF_MASK(flag);
    s32 index = EVENTCHKINF_INDEX(flag);

    z_common_data.save.info.eventChkInf[index] |= mask;
}

/**
 * Tests if inf_table flag is set.
 */
s32 nazo_check(s32 flag) {
    s32 mask = INFTABLE_MASK(flag);
    s32 index = INFTABLE_INDEX(flag);

    return z_common_data.save.info.infTable[index] & mask;
}

/**
 * Sets inf_table flag.
 */
void nazo_set(s32 flag) {
    s32 mask = INFTABLE_MASK(flag);
    s32 index = INFTABLE_INDEX(flag);

    z_common_data.save.info.infTable[index] |= mask;
}

u32 npc_mes_set(PlayState* play, s16 arg1) {
    u16 retTextId = 0;

    switch (arg1) {
        case 0:
            if (event_check(EVENTCHKINF_09)) {
                if (nazo_check(INFTABLE_05)) {
                    retTextId = 0x1048;
                } else {
                    retTextId = 0x1047;
                }
            } else {
                if (event_check(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS)) {
                    if (nazo_check(INFTABLE_03)) {
                        retTextId = 0x1032;
                    } else {
                        retTextId = 0x1031;
                    }
                } else {
                    if (nazo_check(INFTABLE_00)) {
                        if (nazo_check(INFTABLE_01)) {
                            retTextId = 0x1003;
                        } else {
                            retTextId = 0x1002;
                        }
                    } else {
                        retTextId = 0x1001;
                    }
                }
            }
            break;
        case 1:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    if (nazo_check(INFTABLE_10)) {
                        retTextId = 0x1046;
                    } else {
                        retTextId = 0x1045;
                    }
                } else {
                    if (event_check(EVENTCHKINF_03)) {
                        if (nazo_check(INFTABLE_0E)) {
                            retTextId = 0x1034;
                        } else {
                            retTextId = 0x1033;
                        }
                    } else {
                        if (nazo_check(INFTABLE_0C)) {
                            retTextId = 0x1030;
                        } else {
                            retTextId = 0x102F;
                        }
                    }
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    if (nazo_check(INFTABLE_19)) {
                        retTextId = 0x1071;
                    } else {
                        retTextId = 0x1070;
                    }
                } else {
                    if (event_check(EVENTCHKINF_0B)) {
                        if (nazo_check(INFTABLE_17)) {
                            retTextId = 0x1068;
                        } else {
                            retTextId = 0x1067;
                        }
                    } else {
                        if (nazo_check(INFTABLE_15)) {
                            retTextId = 0x1061;
                        } else {
                            retTextId = 0x1060;
                        }
                    }
                }
            }
            break;
        case 2:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1004;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (nazo_check(INFTABLE_41)) {
                    retTextId = 0x1055;
                } else {
                    retTextId = 0x1056;
                }
            }
            break;
        case 3:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else {
                    if (nazo_check(INFTABLE_1E)) {
                        retTextId = 0x1006;
                    } else {
                        retTextId = 0x1005;
                    }
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1073;
                } else {
                    retTextId = 0x105A;
                }
            }
            break;
        case 4:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1007;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (nazo_check(INFTABLE_47)) {
                    retTextId = 0x105E;
                } else {
                    retTextId = 0x105D;
                }
            }
            break;
        case 5:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1044;
                } else if (nazo_check(INFTABLE_22)) {
                    retTextId = 0x1009;
                } else {
                    retTextId = 0x1008;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1075;
                } else {
                    retTextId = 0x105B;
                }
            }
            break;
        case 6:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else if (nazo_check(INFTABLE_24)) {
                    retTextId = 0x100B;
                } else {
                    retTextId = 0x100A;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1056;
                } else {
                    retTextId = 0x105F;
                }
            }
            break;
        case 7:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else if (nazo_check(INFTABLE_26)) {
                    retTextId = 0x100D;
                } else {
                    retTextId = 0x100C;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1057;
                } else {
                    retTextId = 0x1057;
                }
            }
            break;
        case 8:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else if (nazo_check(INFTABLE_28)) {
                    retTextId = 0x1019;
                } else {
                    retTextId = 0x100E;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1077;
                } else if (nazo_check(INFTABLE_51)) {
                    retTextId = 0x1058;
                } else {
                    retTextId = 0x1059;
                }
            }
            break;
        case 9:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x1049;
                } else {
                    retTextId = 0x1035;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else {
                    retTextId = 0x104E;
                }
            }
            break;
        case 10:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x104A;
                } else {
                    retTextId = 0x1038;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else if (nazo_check(INFTABLE_59)) {
                    retTextId = 0x1050;
                } else {
                    retTextId = 0x104F;
                }
            }
            break;
        case 11:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x104B;
                } else {
                    retTextId = 0x103C;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x107B;
                } else {
                    retTextId = 0x1051;
                }
            }
            break;
        case 12:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x104C;
                } else {
                    retTextId = 0x103D;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x107C;
                } else {
                    retTextId = 0x1052;
                }
            }
            break;
        case 13:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_09)) {
                    retTextId = 0x104D;
                } else {
                    retTextId = 0x103E;
                }
            } else {
                if (event_check(EVENTCHKINF_5C)) {
                    retTextId = 0x106E;
                } else if (nazo_check(INFTABLE_61)) {
                    retTextId = 0x1053;
                } else {
                    retTextId = 0x1054;
                }
            }
            break;
        case 15:
            if (event_check(EVENTCHKINF_5C)) {
                retTextId = 0x1078;
            } else if (nazo_check(INFTABLE_66)) {
                retTextId = 0x1066;
            } else {
                retTextId = 0x1062;
            }
            break;
        case 16:
            if (play->sceneId == SCENE_HYRULE_CASTLE) {
                retTextId = 0x7002;
            } else if (nazo_check(INFTABLE_6A)) {
                retTextId = 0x7004;
            } else if ((z_common_data.save.dayTime >= CLOCK_TIME(6, 0)) &&
                       (z_common_data.save.dayTime <= CLOCK_TIME(18, 30))) {
                retTextId = 0x7002;
            } else {
                retTextId = 0x7003;
            }
            break;
        case 17:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                if (nazo_check(INFTABLE_6C)) {
                    retTextId = 0x7008;
                } else {
                    retTextId = 0x7007;
                }
            }
            break;
        case 19:
            retTextId = 0x702D;
            break;
        case 18:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7006;
            } else {
                if (event_check(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
                    if (nazo_check(INFTABLE_71)) {
                        retTextId = 0x7072;
                    } else {
                        retTextId = 0x7071;
                    }
                } else {
                    retTextId = 0x7029;
                }
            }
            break;
        case 20:
        case 21:
            if (event_check(EVENTCHKINF_42)) {
                retTextId = 0x2012;
            } else if (event_check(EVENTCHKINF_41)) {
                if (nazo_check(INFTABLE_76)) {
                    retTextId = 0x2011;
                } else {
                    retTextId = 0x2010;
                }
            } else if (event_check(EVENTCHKINF_40)) {
                retTextId = 0x200F;
            } else {
                retTextId = 0x200E;
            }
            break;
        case 24:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7044;
            } else {
                retTextId = 0x7015;
            }
            break;
        case 25:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7045;
            } else {
                nazo_check(INFTABLE_C2);
                retTextId = 0x7016;
            }
            break;
        case 26:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7046;
            } else {
                nazo_check(INFTABLE_C2);
                retTextId = 0x7018;
            }
            break;
        case 27:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7047;
            } else if (event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x701A;
            } else if (event_check(EVENTCHKINF_11)) {
                if (nazo_check(INFTABLE_C6)) {
                    retTextId = 0x701C;
                } else {
                    retTextId = 0x701B;
                }
            } else {
                retTextId = 0x701A;
            }
            break;
        case 28:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7048;
            } else {
                nazo_check(INFTABLE_CA);
                retTextId = 0x701D;
            }
            break;
        case 29:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x7049;
            } else {
                nazo_check(INFTABLE_CC);
                retTextId = 0x701F;
            }
            break;
        case 30:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x704A;
            } else {
                nazo_check(INFTABLE_CE);
                retTextId = 0x7021;
            }
            break;
        case 31:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x704B;
            } else {
                nazo_check(INFTABLE_D0);
                retTextId = 0x7023;
            }
            break;
        case 32:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x704C;
            } else {
                nazo_check(INFTABLE_D2);
                retTextId = 0x7025;
            }
            break;
        case 33:
            if (event_check(EVENTCHKINF_09) && event_check(EVENTCHKINF_25) &&
                event_check(EVENTCHKINF_37)) {
                retTextId = 0x704D;
            } else {
                nazo_check(INFTABLE_D4);
                retTextId = 0x7027;
            }
            break;
        case 34:
            nazo_check(INFTABLE_D6);
            retTextId = 0x403C;
            break;
        case 35:
            if (nazo_check(INFTABLE_D8)) {
                retTextId = 0x5029;
            } else {
                retTextId = 0x5028;
            }
            break;
        case 37:
            retTextId = 0x5002;
            break;
        case 38:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (event_check(EVENTCHKINF_23)) {
                    retTextId = 0x3021;
                } else if (nazo_check(INFTABLE_E0)) {
                    retTextId = 0x302A;
                } else {
                    retTextId = 0x3008;
                }
            } else {
                if (event_check(EVENTCHKINF_20)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 39:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (event_check(EVENTCHKINF_23)) {
                    retTextId = 0x3026;
                } else {
                    retTextId = 0x3009;
                }
            } else {
                if (event_check(EVENTCHKINF_2A)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 40:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (event_check(EVENTCHKINF_23)) {
                    retTextId = 0x3026;
                } else if (nazo_check(INFTABLE_EB)) {
                    retTextId = 0x302B;
                } else {
                    retTextId = 0x300A;
                }
            } else {
                if (event_check(EVENTCHKINF_2B)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 41:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (nazo_check(INFTABLE_F0)) {
                    retTextId = 0x3015;
                } else {
                    retTextId = 0x3014;
                }
            } else {
                if (event_check(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 42:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (nazo_check(INFTABLE_F4)) {
                    retTextId = 0x3017;
                } else {
                    retTextId = 0x3016;
                }
            } else {
                if (event_check(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 43:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (nazo_check(INFTABLE_F8)) {
                    retTextId = 0x3019;
                } else {
                    retTextId = 0x3018;
                }
            } else {
                if (event_check(EVENTCHKINF_2D)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 48:
            if (event_check(EVENTCHKINF_25)) {
                retTextId = 0x3029;
            } else if (event_check(EVENTCHKINF_20) && event_check(EVENTCHKINF_21)) {
                retTextId = 0x301B;
            } else {
                retTextId = 0x301A;
            }
            break;
        case 49:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (event_check(EVENTCHKINF_30)) {
                retTextId = 0x4007;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 50:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (event_check(EVENTCHKINF_30)) {
                if (nazo_check(INFTABLE_124)) {
                    retTextId = 0x4009;
                } else {
                    retTextId = 0x4008;
                }
            } else {
                retTextId = 0x4006;
            }
            break;
        case 51:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (event_check(EVENTCHKINF_31)) {
                if (nazo_check(INFTABLE_12A)) {
                    retTextId = 0x400B;
                } else {
                    retTextId = 0x402F;
                }
            } else if (event_check(EVENTCHKINF_30)) {
                retTextId = 0x400A;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 52:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (event_check(EVENTCHKINF_30)) {
                retTextId = 0x400C;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 53:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (event_check(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
                retTextId = 0x4010;
            } else if (event_check(EVENTCHKINF_30)) {
                retTextId = 0x400F;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 54:
            if (event_check(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (event_check(EVENTCHKINF_30)) {
                retTextId = 0x4011;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 55:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_37)) {
                    retTextId = 0x402B;
                } else if (event_check(EVENTCHKINF_31)) {
                    if (nazo_check(INFTABLE_138)) {
                        retTextId = 0x401C;
                    } else {
                        retTextId = 0x401B;
                    }
                } else {
                    retTextId = 0x401A;
                }
            }
            break;
        case 58:
            retTextId = 0x500F;
            break;
        case 59:
            retTextId = 0x5010;
            break;
        case 60:
            retTextId = 0x5012;
            break;
        case 61:
            if (nazo_check(INFTABLE_166)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 62:
            retTextId = 0x5012;
            break;
        case 63:
            if (nazo_check(INFTABLE_16A)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 71:
            if (event_check(EVENTCHKINF_CAN_LEARN_EPONAS_SONG)) {
                retTextId = 0x2049;
            } else if (event_check(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH)) {
                retTextId = 0x2048;
            } else if (event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2047;
            } else if (event_check(EVENTCHKINF_RECEIVED_WEIRD_EGG) &&
                       !event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2044;
            } else if (event_check(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME)) {
                if (event_check(EVENTCHKINF_11)) {
                    retTextId = 0x2043;
                } else {
                    retTextId = 0x2042;
                }
            } else {
                retTextId = 0x2041;
            }
            break;
        case 72:
            if (!LINK_IS_ADULT) {
                if (event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    retTextId = 0x2040;
                } else if (nazo_check(INFTABLE_94)) {
                    retTextId = 0x2040;
                } else {
                    retTextId = 0x203F;
                }
            } else {
                if (!event_check(EVENTCHKINF_EPONA_OBTAINED)) {
                    if (!IS_DAY) {
                        retTextId = 0x204E;
                    } else if (nazo_check(INFTABLE_9A)) {
                        retTextId = 0x2031;
                    } else {
                        retTextId = 0x2030;
                    }
                }
            }
            break;
    }

    if (retTextId == 0) {
        retTextId = 1;
    }

    return retTextId;
}

void info_set(u16 textId, s16 arg1) {
    switch (arg1) {
        case 0:
            switch (textId) {
                case 0x1001:
                    nazo_set(INFTABLE_00);
                    return;
                case 0x1002:
                    nazo_set(INFTABLE_01);
                    return;
                case 0x1031:
                    event_set(EVENTCHKINF_03);
                    nazo_set(INFTABLE_03);
                    return;
                case 0x1047:
                    nazo_set(INFTABLE_05);
                    return;
            }
            return;
        case 1:
            switch (textId) {
                case 0x102F:
                    event_set(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS);
                    nazo_set(INFTABLE_0C);
                    return;
                case 0x1033:
                    Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    event_set(EVENTCHKINF_04);
                    nazo_set(INFTABLE_0E);
                    return;
                case 0x1045:
                    nazo_set(INFTABLE_10);
                    return;
                case 0x1060:
                    nazo_set(INFTABLE_15);
                    return;
                case 0x1067:
                    event_set(EVENTCHKINF_0A);
                    nazo_set(INFTABLE_17);
                    return;
                case 0x1070:
                    nazo_set(INFTABLE_19);
                    return;
            }
            return;
        case 2:
            if (textId == 0x1056) {
                nazo_set(INFTABLE_41);
            }
            return;
        case 3:
            if (textId == 0x1005) {
                nazo_set(INFTABLE_1E);
            }
            return;
        case 4:
            if (textId == 0x105D) {
                nazo_set(INFTABLE_47);
            }
            return;
        case 5:
            if (textId == 0x1008) {
                nazo_set(INFTABLE_22);
            }
            return;
        case 6:
            if (textId == 0x100A) {
                nazo_set(INFTABLE_24);
            }
            return;
        case 7:
            if (textId == 0x100C) {
                nazo_set(INFTABLE_26);
            }
            return;
        case 8:
            if (textId == 0x100E) {
                nazo_set(INFTABLE_28);
            }
            if (textId == 0x1059) {
                nazo_set(INFTABLE_51);
            }
            return;
        case 10:
            if (textId == 0x104F) {
                nazo_set(INFTABLE_59);
            }
            return;
        case 13:
            if (textId == 0x1054) {
                nazo_set(INFTABLE_61);
            }
            return;
        case 15:
            if (textId == 0x1062) {
                nazo_set(INFTABLE_66);
            }
            return;
        case 16:
            if (textId == 0x7002) {
                nazo_set(INFTABLE_6A);
            }
            if (textId == 0x7003) {
                nazo_set(INFTABLE_6A);
            }
            return;
        case 17:
            if (textId == 0x7007) {
                nazo_set(INFTABLE_6C);
            }
            return;
        case 18:
            if (textId == 0x7071) {
                nazo_set(INFTABLE_71);
            }
            return;
        case 20:
        case 21:
            if (textId == 0x2010) {
                nazo_set(INFTABLE_76);
            }
            return;
        case 25:
            if (textId == 0x7016) {
                nazo_set(INFTABLE_C2);
            }
            return;
        case 26:
            if (textId == 0x7018) {
                nazo_set(INFTABLE_C4);
            }
            return;
        case 28:
            if (textId == 0x701D) {
                nazo_set(INFTABLE_CA);
            }
            return;
        case 29:
            if (textId == 0x701F) {
                nazo_set(INFTABLE_CC);
            }
            return;
        case 30:
            if (textId == 0x7021) {
                nazo_set(INFTABLE_CE);
            }
            return;
        case 31:
            if (textId == 0x7023) {
                nazo_set(INFTABLE_D0);
            }
            return;
        case 32:
            if (textId == 0x7025) {
                nazo_set(INFTABLE_D2);
            }
            return;
        case 33:
            if (textId == 0x7027) {
                nazo_set(INFTABLE_D4);
            }
            return;
        case 34:
            if (textId == 0x403C) {
                nazo_set(INFTABLE_D6);
            }
            return;
        case 35:
            if (textId == 0x5028) {
                nazo_set(INFTABLE_D8);
            }
            return;
        case 38:
            if (textId == 0x3008) {
                nazo_set(INFTABLE_E0);
            }
            return;
        case 40:
            if (textId == 0x300B) {
                nazo_set(INFTABLE_EB);
            }
            return;
        case 41:
            if (textId == 0x3014) {
                nazo_set(INFTABLE_F0);
            }
            return;
        case 42:
            if (textId == 0x3016) {
                nazo_set(INFTABLE_F4);
            }
            return;
        case 43:
            if (textId == 0x3018) {
                event_set(EVENTCHKINF_20);
                nazo_set(INFTABLE_F8);
            }
            return;
        case 48:
            if (textId == 0x3020) {
                event_set(EVENTCHKINF_22);
                nazo_set(INFTABLE_113);
            }
            return;
        case 49:
        case 52:
        case 53:
        case 54:
            if (textId == 0x4006) {
                event_set(EVENTCHKINF_30);
            }
            return;
        case 50:
            if (textId == 0x4006) {
                event_set(EVENTCHKINF_30);
            }
            if (textId == 0x4008) {
                nazo_set(INFTABLE_124);
            }
            return;
        case 51:
            if (textId == 0x4006) {
                event_set(EVENTCHKINF_30);
            }
            if (textId == 0x400A) {
                event_set(EVENTCHKINF_32);
            }
            if (textId == 0x402F) {
                nazo_set(INFTABLE_12A);
            }
            return;
        case 55:
            if (textId == 0x401B) {
                event_set(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA);
                nazo_set(INFTABLE_138);
            }
            return;
        case 61:
            if (textId == 0x5000) {
                nazo_set(INFTABLE_166);
            }
            return;
        case 63:
            if (textId == 0x5013) {
                nazo_set(INFTABLE_16A);
            }
            return;
        case 71:
            if (textId == 0x2041) {
                event_set(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME);
            }
            if (textId == 0x2044) {
                event_set(EVENTCHKINF_RECEIVED_WEIRD_EGG);
            }
            if (textId == 0x2047) {
                event_set(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH);
            }
            if (textId == 0x2048) {
                event_set(EVENTCHKINF_CAN_LEARN_EPONAS_SONG);
            }
            return;
        case 72:
            return;
    }
}

s32 select_mes(PlayState* play, Actor* actor, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = 1;

    switch (textId) {
        case 0x1035:
            if (msgCtx->choiceIndex == 0) {
                if (nazo_check(INFTABLE_2A)) {
                    message_set3(play, actor, 0x1036);
                } else {
                    message_set3(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (nazo_check(INFTABLE_2B)) {
                    message_set3(play, actor, 0x1037);
                } else {
                    message_set3(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x1038:
            if (msgCtx->choiceIndex == 0) {
                if (nazo_check(INFTABLE_2E)) {
                    message_set3(play, actor, 0x1039);
                } else {
                    message_set3(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (nazo_check(INFTABLE_2F)) {
                    message_set3(play, actor, 0x103A);
                } else {
                    message_set3(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 2) {
                if (nazo_check(INFTABLE_30)) {
                    message_set3(play, actor, 0x103B);
                } else {
                    message_set3(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x103E:
            if (msgCtx->choiceIndex == 0) {
                message_set3(play, actor, 0x103F);
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x1040);
            }
            ret = 0;
            break;
        case 0x1041:
            if (msgCtx->choiceTextId == 0x1035) {
                if (msgCtx->choiceIndex == 0) {
                    message_set3(play, actor, 0x1036);
                    nazo_set(INFTABLE_2A);
                }
                if (msgCtx->choiceIndex == 1) {
                    message_set3(play, actor, 0x1037);
                    nazo_set(INFTABLE_2B);
                }
            }
            if (msgCtx->choiceTextId == 0x1038) {
                if (msgCtx->choiceIndex == 0) {
                    message_set3(play, actor, 0x1039);
                    nazo_set(INFTABLE_2E);
                }
                if (msgCtx->choiceIndex == 1) {
                    message_set3(play, actor, 0x103A);
                    nazo_set(INFTABLE_2F);
                }
                if (msgCtx->choiceIndex == 2) {
                    message_set3(play, actor, 0x103B);
                    nazo_set(INFTABLE_30);
                }
            }
            ret = 0;
            break;
        case 0x1062:
            if (msgCtx->choiceIndex == 0) {
                message_set3(play, actor, 0x1063);
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x1064);
            }
            ret = 0;
            break;
        case 0x2030:
        case 0x2031:
            if (msgCtx->choiceIndex == 0) {
                if (z_common_data.save.info.playerData.rupees >= 10) {
                    message_set3(play, actor, 0x2034);
                    lupy_increase(-10);
                } else {
                    message_set3(play, actor, 0x2032);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x2032);
            }
            nazo_set(INFTABLE_9A);
            ret = 0;
            break;
        case 0x2035:
            break;
        case 0x2036:
        case 0x2037:
            if (msgCtx->choiceIndex == 0) {
                message_set3(play, actor, 0x201F);
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2038:
            if (msgCtx->choiceIndex == 0) {
                break;
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2034:
            if (msgCtx->choiceIndex != 0) {
                break;
            }
            message_set3(play, actor, 0x2035);
            ret = 0;
            break;
        case 0x2043:
            if (event_check(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
                break;
            }
            message_set3(play, actor, 0x2044);
            ret = 0;
            break;
        case 0x205A:
            break;
        case 0x300A:
            if (msgCtx->choiceIndex == 0) {
                if (event_check(EVENTCHKINF_22)) {
                    message_set3(play, actor, 0x300B);
                } else {
                    message_set3(play, actor, 0x300C);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x300D);
            }
            ret = 0;
            break;
        case 0x301B:
            if (msgCtx->choiceIndex == 0) {
                message_set3(play, actor, 0x301D);
            }
            if (msgCtx->choiceIndex == 1) {
                if (nazo_check(INFTABLE_113)) {
                    message_set3(play, actor, 0x301F);
                } else {
                    message_set3(play, actor, 0x301E);
                }
            }
            ret = 0;
            break;
        case 0x301E:
            message_set3(play, actor, 0x3020);
            ret = 0;
            break;
        case 0x400C:
            if (msgCtx->choiceIndex == 0) {
                message_set3(play, actor, 0x400D);
            }
            if (msgCtx->choiceIndex == 1) {
                message_set3(play, actor, 0x400E);
            }
            ret = 0;
            break;
        case 0x7007:
            message_set3(play, actor, 0x703E);
            ret = 0;
            break;
        case 0x703E:
            message_set3(play, actor, 0x703F);
            ret = 0;
            break;
        case 0x703F:
            message_set3(play, actor, 0x7042);
            ret = 0;
            break;
    }

    return ret;
}

u16 set_talk_message(PlayState* play, s16 arg1) {
    return npc_mes_set(play, arg1);
}

s32 set_nazo_and_next_message(PlayState* play, s16 arg1, u16 textId) {
    info_set(textId, arg1);
    return false;
}

s32 special_talk(PlayState* play, Actor* actor, s32 arg2) {
    return select_mes(play, actor, actor->textId);
}

s32 message_end_check(PlayState* play, Actor* actor, s16 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = false;

    switch (message_check(msgCtx)) {
        case TEXT_STATE_CLOSING:
            set_nazo_and_next_message(play, arg2, actor->textId);
            ret = true;
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (pad_on_check(play) && special_talk(play, actor, arg2)) {
                Nai_FxFlagEntry(NA_SE_SY_CANCEL, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
                ret = true;
            }
            break;
    }

    return ret;
}

s32 talk_to_player(PlayState* play, Actor* actor, s32 arg2, s32* arg3) {
    s16 var;
    s16 sp2C;
    s16 sp2A;
    s16 abs_var;

    if (Actor_talk_check(actor, play)) {
        *arg3 = 1;
        return true;
    }

    if (*arg3 == 1) {
        if (message_end_check(play, actor, arg2)) {
            *arg3 = 0;
        }
        return false;
    }

    Actor_display_position_set(play, actor, &sp2C, &sp2A);

    if ((sp2C < 0) || (sp2C > SCREEN_WIDTH) || (sp2A < 0) || (sp2A > SCREEN_HEIGHT)) {
        return false;
    }

    var = actor->yawTowardsPlayer - actor->shape.rot.y;
    abs_var = ABS(var);

    if (abs_var >= 0x4300) {
        return false;
    }

    if ((actor->xyzDistToPlayerSq > SQ(160.0f)) && !actor->isLockedOn) {
        return false;
    }

    if (actor->xyzDistToPlayerSq <= SQ(80.0f)) {
        if (Actor_talk_request2(actor, play, 80.0f)) {
            actor->textId = set_talk_message(play, arg2);
        }
    } else {
        if (Actor_talk_request(actor, play)) {
            actor->textId = set_talk_message(play, arg2);
        }
    }

    return false;
}

s32 chain_angle_reset(Vec3s* headRot, Vec3s* torsoRot) {
    add_calc_short_angle2(&headRot->y, 0, 6, 6200, 100);
    add_calc_short_angle2(&headRot->x, 0, 6, 6200, 100);
    add_calc_short_angle2(&torsoRot->y, 0, 6, 6200, 100);
    add_calc_short_angle2(&torsoRot->x, 0, 6, 6200, 100);
    return true;
}

static s32 chain_angle_set(Actor* actor, Vec3f* target, Vec3s* headRot, Vec3s* torsoRot) {
    s16 pitch;
    s16 yaw;
    s16 yawDiff;

    pitch = search_position_angleX(&actor->focus.pos, target);
    yaw = search_position_angleY(&actor->focus.pos, target) - actor->world.rot.y;

    add_calc_short_angle2(&headRot->x, pitch, 6, 2000, 1);
    headRot->x = CLAMP(headRot->x, -6000, 6000);

    yawDiff = add_calc_short_angle2(&headRot->y, yaw, 6, 2000, 1);
    headRot->y = CLAMP(headRot->y, -8000, 8000);

    if ((yawDiff != 0) && (ABS(headRot->y) < 8000)) {
        return false;
    }

    add_calc_short_angle2(&torsoRot->y, yaw - headRot->y, 4, 2000, 1);
    torsoRot->y = CLAMP(torsoRot->y, -12000, 12000);

    return true;
}

/**
 * Same as eye_move2, except use the actor's world position as the focus point, with the height
 * specified.
 *
 * @param play
 * @param actor
 * @param headRot the computed actor's head's rotation step
 * @param torsoRot the computed actor's torso's rotation step
 * @param focusHeight the height of the focus point relative to their world position
 *
 * @return true if rotated towards player, false if rotations were stepped back to zero.
 *
 * @note same note as eye_move2
 */
s32 eye_move(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, f32 focusHeight) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = actor->world.pos;
    actor->focus.pos.y += focusHeight;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || debug_camera_sw) &&
          (z_common_data.save.entranceIndex == ENTR_KOKIRI_FOREST_0))) {
        yaw = ABS((s16)(actor->yawTowardsPlayer - actor->shape.rot.y));
        if (yaw >= 0x4300) {
            chain_angle_reset(headRot, torsoRot);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || debug_camera_sw) &&
        (z_common_data.save.entranceIndex == ENTR_KOKIRI_FOREST_0)) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    chain_angle_set(actor, &target, headRot, torsoRot);

    return true;
}

/**
 * Computes the necessary HeadRot and TorsoRot steps to be added to the normal rotation to smoothly turn an actors's
 * head and torso towards the player if within a certain yaw, else smoothly returns the rotations back to zero.
 * Also sets the focus position with the specified point.
 *
 * @param play
 * @param actor
 * @param headRot the computed actor's head's rotation step
 * @param torsoRot the computed actor's torso's rotation step
 * @param focusPos the point to set as the actor's focus position
 *
 * @return true if rotated towards player, false if rotations were stepped back to zero.
 *
 * @note if in a cutscene or debug camera is enabled, and the last entrance used was Kokiri Forest spawn 0, the computed
 * rotation will instead turn towards the view eye no matter the yaw.
 */
s32 eye_move2(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, Vec3f focusPos) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = focusPos;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || debug_camera_sw) &&
          (z_common_data.save.entranceIndex == ENTR_KOKIRI_FOREST_0))) {
        yaw = ABS((s16)(actor->yawTowardsPlayer - actor->shape.rot.y));
        if (yaw >= 0x4300) {
            chain_angle_reset(headRot, torsoRot);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || debug_camera_sw) &&
        (z_common_data.save.entranceIndex == ENTR_KOKIRI_FOREST_0)) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    chain_angle_set(actor, &target, headRot, torsoRot);

    return true;
}
