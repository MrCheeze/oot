void func_80035B18(PlayState* play, Actor* actor, u16 textId) {
    Message_ContinueTextbox(play, textId);
    actor->textId = textId;
}

/**
 * Tests if event_chk_inf flag is set.
 */
s32 Flags_GetEventChkInf(s32 flag) {
    s32 mask = EVENTCHKINF_MASK(flag);
    s32 index = EVENTCHKINF_INDEX(flag);

    return gSaveContext.save.info.eventChkInf[index] & mask;
}

/**
 * Sets event_chk_inf flag.
 */
void Flags_SetEventChkInf(s32 flag) {
    s32 mask = EVENTCHKINF_MASK(flag);
    s32 index = EVENTCHKINF_INDEX(flag);

    gSaveContext.save.info.eventChkInf[index] |= mask;
}

/**
 * Tests if inf_table flag is set.
 */
s32 Flags_GetInfTable(s32 flag) {
    s32 mask = INFTABLE_MASK(flag);
    s32 index = INFTABLE_INDEX(flag);

    return gSaveContext.save.info.infTable[index] & mask;
}

/**
 * Sets inf_table flag.
 */
void Flags_SetInfTable(s32 flag) {
    s32 mask = INFTABLE_MASK(flag);
    s32 index = INFTABLE_INDEX(flag);

    gSaveContext.save.info.infTable[index] |= mask;
}

u32 func_80035BFC(PlayState* play, s16 arg1) {
    u16 retTextId = 0;

    switch (arg1) {
        case 0:
            if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                if (Flags_GetInfTable(INFTABLE_05)) {
                    retTextId = 0x1048;
                } else {
                    retTextId = 0x1047;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS)) {
                    if (Flags_GetInfTable(INFTABLE_03)) {
                        retTextId = 0x1032;
                    } else {
                        retTextId = 0x1031;
                    }
                } else {
                    if (Flags_GetInfTable(INFTABLE_00)) {
                        if (Flags_GetInfTable(INFTABLE_01)) {
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
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    if (Flags_GetInfTable(INFTABLE_10)) {
                        retTextId = 0x1046;
                    } else {
                        retTextId = 0x1045;
                    }
                } else {
                    if (Flags_GetEventChkInf(EVENTCHKINF_03)) {
                        if (Flags_GetInfTable(INFTABLE_0E)) {
                            retTextId = 0x1034;
                        } else {
                            retTextId = 0x1033;
                        }
                    } else {
                        if (Flags_GetInfTable(INFTABLE_0C)) {
                            retTextId = 0x1030;
                        } else {
                            retTextId = 0x102F;
                        }
                    }
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    if (Flags_GetInfTable(INFTABLE_19)) {
                        retTextId = 0x1071;
                    } else {
                        retTextId = 0x1070;
                    }
                } else {
                    if (Flags_GetEventChkInf(EVENTCHKINF_0B)) {
                        if (Flags_GetInfTable(INFTABLE_17)) {
                            retTextId = 0x1068;
                        } else {
                            retTextId = 0x1067;
                        }
                    } else {
                        if (Flags_GetInfTable(INFTABLE_15)) {
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
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1004;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (Flags_GetInfTable(INFTABLE_41)) {
                    retTextId = 0x1055;
                } else {
                    retTextId = 0x1056;
                }
            }
            break;
        case 3:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else {
                    if (Flags_GetInfTable(INFTABLE_1E)) {
                        retTextId = 0x1006;
                    } else {
                        retTextId = 0x1005;
                    }
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1073;
                } else {
                    retTextId = 0x105A;
                }
            }
            break;
        case 4:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1007;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (Flags_GetInfTable(INFTABLE_47)) {
                    retTextId = 0x105E;
                } else {
                    retTextId = 0x105D;
                }
            }
            break;
        case 5:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1044;
                } else if (Flags_GetInfTable(INFTABLE_22)) {
                    retTextId = 0x1009;
                } else {
                    retTextId = 0x1008;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1075;
                } else {
                    retTextId = 0x105B;
                }
            }
            break;
        case 6:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1042;
                } else if (Flags_GetInfTable(INFTABLE_24)) {
                    retTextId = 0x100B;
                } else {
                    retTextId = 0x100A;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1056;
                } else {
                    retTextId = 0x105F;
                }
            }
            break;
        case 7:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else if (Flags_GetInfTable(INFTABLE_26)) {
                    retTextId = 0x100D;
                } else {
                    retTextId = 0x100C;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1057;
                } else {
                    retTextId = 0x1057;
                }
            }
            break;
        case 8:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1043;
                } else if (Flags_GetInfTable(INFTABLE_28)) {
                    retTextId = 0x1019;
                } else {
                    retTextId = 0x100E;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1077;
                } else if (Flags_GetInfTable(INFTABLE_51)) {
                    retTextId = 0x1058;
                } else {
                    retTextId = 0x1059;
                }
            }
            break;
        case 9:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x1049;
                } else {
                    retTextId = 0x1035;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else {
                    retTextId = 0x104E;
                }
            }
            break;
        case 10:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x104A;
                } else {
                    retTextId = 0x1038;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else if (Flags_GetInfTable(INFTABLE_59)) {
                    retTextId = 0x1050;
                } else {
                    retTextId = 0x104F;
                }
            }
            break;
        case 11:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x104B;
                } else {
                    retTextId = 0x103C;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x107B;
                } else {
                    retTextId = 0x1051;
                }
            }
            break;
        case 12:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x104C;
                } else {
                    retTextId = 0x103D;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x107C;
                } else {
                    retTextId = 0x1052;
                }
            }
            break;
        case 13:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_09)) {
                    retTextId = 0x104D;
                } else {
                    retTextId = 0x103E;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x106E;
                } else if (Flags_GetInfTable(INFTABLE_61)) {
                    retTextId = 0x1053;
                } else {
                    retTextId = 0x1054;
                }
            }
            break;
        case 15:
            if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                retTextId = 0x1078;
            } else if (Flags_GetInfTable(INFTABLE_66)) {
                retTextId = 0x1066;
            } else {
                retTextId = 0x1062;
            }
            break;
        case 16:
            if (play->sceneId == SCENE_HYRULE_CASTLE) {
                retTextId = 0x7002;
            } else if (Flags_GetInfTable(INFTABLE_6A)) {
                retTextId = 0x7004;
            } else if ((gSaveContext.save.dayTime >= CLOCK_TIME(6, 0)) &&
                       (gSaveContext.save.dayTime <= CLOCK_TIME(18, 30))) {
                retTextId = 0x7002;
            } else {
                retTextId = 0x7003;
            }
            break;
        case 17:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                if (Flags_GetInfTable(INFTABLE_6C)) {
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
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7006;
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
                    if (Flags_GetInfTable(INFTABLE_71)) {
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
            if (Flags_GetEventChkInf(EVENTCHKINF_42)) {
                retTextId = 0x2012;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_41)) {
                if (Flags_GetInfTable(INFTABLE_76)) {
                    retTextId = 0x2011;
                } else {
                    retTextId = 0x2010;
                }
            } else if (Flags_GetEventChkInf(EVENTCHKINF_40)) {
                retTextId = 0x200F;
            } else {
                retTextId = 0x200E;
            }
            break;
        case 24:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7044;
            } else {
                retTextId = 0x7015;
            }
            break;
        case 25:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7045;
            } else {
                Flags_GetInfTable(INFTABLE_C2);
                retTextId = 0x7016;
            }
            break;
        case 26:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7046;
            } else {
                Flags_GetInfTable(INFTABLE_C2);
                retTextId = 0x7018;
            }
            break;
        case 27:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7047;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x701A;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_11)) {
                if (Flags_GetInfTable(INFTABLE_C6)) {
                    retTextId = 0x701C;
                } else {
                    retTextId = 0x701B;
                }
            } else {
                retTextId = 0x701A;
            }
            break;
        case 28:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7048;
            } else {
                Flags_GetInfTable(INFTABLE_CA);
                retTextId = 0x701D;
            }
            break;
        case 29:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x7049;
            } else {
                Flags_GetInfTable(INFTABLE_CC);
                retTextId = 0x701F;
            }
            break;
        case 30:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x704A;
            } else {
                Flags_GetInfTable(INFTABLE_CE);
                retTextId = 0x7021;
            }
            break;
        case 31:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x704B;
            } else {
                Flags_GetInfTable(INFTABLE_D0);
                retTextId = 0x7023;
            }
            break;
        case 32:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x704C;
            } else {
                Flags_GetInfTable(INFTABLE_D2);
                retTextId = 0x7025;
            }
            break;
        case 33:
            if (Flags_GetEventChkInf(EVENTCHKINF_09) && Flags_GetEventChkInf(EVENTCHKINF_25) &&
                Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x704D;
            } else {
                Flags_GetInfTable(INFTABLE_D4);
                retTextId = 0x7027;
            }
            break;
        case 34:
            Flags_GetInfTable(INFTABLE_D6);
            retTextId = 0x403C;
            break;
        case 35:
            if (Flags_GetInfTable(INFTABLE_D8)) {
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
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_23)) {
                    retTextId = 0x3021;
                } else if (Flags_GetInfTable(INFTABLE_E0)) {
                    retTextId = 0x302A;
                } else {
                    retTextId = 0x3008;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_20)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 39:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_23)) {
                    retTextId = 0x3026;
                } else {
                    retTextId = 0x3009;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2A)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 40:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_23)) {
                    retTextId = 0x3026;
                } else if (Flags_GetInfTable(INFTABLE_EB)) {
                    retTextId = 0x302B;
                } else {
                    retTextId = 0x300A;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2B)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 41:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F0)) {
                    retTextId = 0x3015;
                } else {
                    retTextId = 0x3014;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 42:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F4)) {
                    retTextId = 0x3017;
                } else {
                    retTextId = 0x3016;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 43:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F8)) {
                    retTextId = 0x3019;
                } else {
                    retTextId = 0x3018;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2D)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 48:
            if (Flags_GetEventChkInf(EVENTCHKINF_25)) {
                retTextId = 0x3029;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_20) && Flags_GetEventChkInf(EVENTCHKINF_21)) {
                retTextId = 0x301B;
            } else {
                retTextId = 0x301A;
            }
            break;
        case 49:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                retTextId = 0x4007;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 50:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                if (Flags_GetInfTable(INFTABLE_124)) {
                    retTextId = 0x4009;
                } else {
                    retTextId = 0x4008;
                }
            } else {
                retTextId = 0x4006;
            }
            break;
        case 51:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_31)) {
                if (Flags_GetInfTable(INFTABLE_12A)) {
                    retTextId = 0x400B;
                } else {
                    retTextId = 0x402F;
                }
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                retTextId = 0x400A;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 52:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                retTextId = 0x400C;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 53:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
                retTextId = 0x4010;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                retTextId = 0x400F;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 54:
            if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_30)) {
                retTextId = 0x4011;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 55:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_37)) {
                    retTextId = 0x402B;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_31)) {
                    if (Flags_GetInfTable(INFTABLE_138)) {
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
            if (Flags_GetInfTable(INFTABLE_166)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 62:
            retTextId = 0x5012;
            break;
        case 63:
            if (Flags_GetInfTable(INFTABLE_16A)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 71:
            if (Flags_GetEventChkInf(EVENTCHKINF_CAN_LEARN_EPONAS_SONG)) {
                retTextId = 0x2049;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH)) {
                retTextId = 0x2048;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2047;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_RECEIVED_WEIRD_EGG) &&
                       !Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2044;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME)) {
                if (Flags_GetEventChkInf(EVENTCHKINF_11)) {
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
                if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    retTextId = 0x2040;
                } else if (Flags_GetInfTable(INFTABLE_94)) {
                    retTextId = 0x2040;
                } else {
                    retTextId = 0x203F;
                }
            } else {
                if (!Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED)) {
                    if (!IS_DAY) {
                        retTextId = 0x204E;
                    } else if (Flags_GetInfTable(INFTABLE_9A)) {
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

void func_80036E50(u16 textId, s16 arg1) {
    switch (arg1) {
        case 0:
            switch (textId) {
                case 0x1001:
                    Flags_SetInfTable(INFTABLE_00);
                    return;
                case 0x1002:
                    Flags_SetInfTable(INFTABLE_01);
                    return;
                case 0x1031:
                    Flags_SetEventChkInf(EVENTCHKINF_03);
                    Flags_SetInfTable(INFTABLE_03);
                    return;
                case 0x1047:
                    Flags_SetInfTable(INFTABLE_05);
                    return;
            }
            return;
        case 1:
            switch (textId) {
                case 0x102F:
                    Flags_SetEventChkInf(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS);
                    Flags_SetInfTable(INFTABLE_0C);
                    return;
                case 0x1033:
                    Audio_PlaySfxGeneral(NA_SE_SY_CORRECT_CHIME, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    Flags_SetEventChkInf(EVENTCHKINF_04);
                    Flags_SetInfTable(INFTABLE_0E);
                    return;
                case 0x1045:
                    Flags_SetInfTable(INFTABLE_10);
                    return;
                case 0x1060:
                    Flags_SetInfTable(INFTABLE_15);
                    return;
                case 0x1067:
                    Flags_SetEventChkInf(EVENTCHKINF_0A);
                    Flags_SetInfTable(INFTABLE_17);
                    return;
                case 0x1070:
                    Flags_SetInfTable(INFTABLE_19);
                    return;
            }
            return;
        case 2:
            if (textId == 0x1056) {
                Flags_SetInfTable(INFTABLE_41);
            }
            return;
        case 3:
            if (textId == 0x1005) {
                Flags_SetInfTable(INFTABLE_1E);
            }
            return;
        case 4:
            if (textId == 0x105D) {
                Flags_SetInfTable(INFTABLE_47);
            }
            return;
        case 5:
            if (textId == 0x1008) {
                Flags_SetInfTable(INFTABLE_22);
            }
            return;
        case 6:
            if (textId == 0x100A) {
                Flags_SetInfTable(INFTABLE_24);
            }
            return;
        case 7:
            if (textId == 0x100C) {
                Flags_SetInfTable(INFTABLE_26);
            }
            return;
        case 8:
            if (textId == 0x100E) {
                Flags_SetInfTable(INFTABLE_28);
            }
            if (textId == 0x1059) {
                Flags_SetInfTable(INFTABLE_51);
            }
            return;
        case 10:
            if (textId == 0x104F) {
                Flags_SetInfTable(INFTABLE_59);
            }
            return;
        case 13:
            if (textId == 0x1054) {
                Flags_SetInfTable(INFTABLE_61);
            }
            return;
        case 15:
            if (textId == 0x1062) {
                Flags_SetInfTable(INFTABLE_66);
            }
            return;
        case 16:
            if (textId == 0x7002) {
                Flags_SetInfTable(INFTABLE_6A);
            }
            if (textId == 0x7003) {
                Flags_SetInfTable(INFTABLE_6A);
            }
            return;
        case 17:
            if (textId == 0x7007) {
                Flags_SetInfTable(INFTABLE_6C);
            }
            return;
        case 18:
            if (textId == 0x7071) {
                Flags_SetInfTable(INFTABLE_71);
            }
            return;
        case 20:
        case 21:
            if (textId == 0x2010) {
                Flags_SetInfTable(INFTABLE_76);
            }
            return;
        case 25:
            if (textId == 0x7016) {
                Flags_SetInfTable(INFTABLE_C2);
            }
            return;
        case 26:
            if (textId == 0x7018) {
                Flags_SetInfTable(INFTABLE_C4);
            }
            return;
        case 28:
            if (textId == 0x701D) {
                Flags_SetInfTable(INFTABLE_CA);
            }
            return;
        case 29:
            if (textId == 0x701F) {
                Flags_SetInfTable(INFTABLE_CC);
            }
            return;
        case 30:
            if (textId == 0x7021) {
                Flags_SetInfTable(INFTABLE_CE);
            }
            return;
        case 31:
            if (textId == 0x7023) {
                Flags_SetInfTable(INFTABLE_D0);
            }
            return;
        case 32:
            if (textId == 0x7025) {
                Flags_SetInfTable(INFTABLE_D2);
            }
            return;
        case 33:
            if (textId == 0x7027) {
                Flags_SetInfTable(INFTABLE_D4);
            }
            return;
        case 34:
            if (textId == 0x403C) {
                Flags_SetInfTable(INFTABLE_D6);
            }
            return;
        case 35:
            if (textId == 0x5028) {
                Flags_SetInfTable(INFTABLE_D8);
            }
            return;
        case 38:
            if (textId == 0x3008) {
                Flags_SetInfTable(INFTABLE_E0);
            }
            return;
        case 40:
            if (textId == 0x300B) {
                Flags_SetInfTable(INFTABLE_EB);
            }
            return;
        case 41:
            if (textId == 0x3014) {
                Flags_SetInfTable(INFTABLE_F0);
            }
            return;
        case 42:
            if (textId == 0x3016) {
                Flags_SetInfTable(INFTABLE_F4);
            }
            return;
        case 43:
            if (textId == 0x3018) {
                Flags_SetEventChkInf(EVENTCHKINF_20);
                Flags_SetInfTable(INFTABLE_F8);
            }
            return;
        case 48:
            if (textId == 0x3020) {
                Flags_SetEventChkInf(EVENTCHKINF_22);
                Flags_SetInfTable(INFTABLE_113);
            }
            return;
        case 49:
        case 52:
        case 53:
        case 54:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_30);
            }
            return;
        case 50:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_30);
            }
            if (textId == 0x4008) {
                Flags_SetInfTable(INFTABLE_124);
            }
            return;
        case 51:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_30);
            }
            if (textId == 0x400A) {
                Flags_SetEventChkInf(EVENTCHKINF_32);
            }
            if (textId == 0x402F) {
                Flags_SetInfTable(INFTABLE_12A);
            }
            return;
        case 55:
            if (textId == 0x401B) {
                Flags_SetEventChkInf(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA);
                Flags_SetInfTable(INFTABLE_138);
            }
            return;
        case 61:
            if (textId == 0x5000) {
                Flags_SetInfTable(INFTABLE_166);
            }
            return;
        case 63:
            if (textId == 0x5013) {
                Flags_SetInfTable(INFTABLE_16A);
            }
            return;
        case 71:
            if (textId == 0x2041) {
                Flags_SetEventChkInf(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME);
            }
            if (textId == 0x2044) {
                Flags_SetEventChkInf(EVENTCHKINF_RECEIVED_WEIRD_EGG);
            }
            if (textId == 0x2047) {
                Flags_SetEventChkInf(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH);
            }
            if (textId == 0x2048) {
                Flags_SetEventChkInf(EVENTCHKINF_CAN_LEARN_EPONAS_SONG);
            }
            return;
        case 72:
            return;
    }
}

s32 func_800374E0(PlayState* play, Actor* actor, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = 1;

    switch (textId) {
        case 0x1035:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetInfTable(INFTABLE_2A)) {
                    func_80035B18(play, actor, 0x1036);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_2B)) {
                    func_80035B18(play, actor, 0x1037);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x1038:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetInfTable(INFTABLE_2E)) {
                    func_80035B18(play, actor, 0x1039);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_2F)) {
                    func_80035B18(play, actor, 0x103A);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 2) {
                if (Flags_GetInfTable(INFTABLE_30)) {
                    func_80035B18(play, actor, 0x103B);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x103E:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x103F);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x1040);
            }
            ret = 0;
            break;
        case 0x1041:
            if (msgCtx->choiceTextId == 0x1035) {
                if (msgCtx->choiceIndex == 0) {
                    func_80035B18(play, actor, 0x1036);
                    Flags_SetInfTable(INFTABLE_2A);
                }
                if (msgCtx->choiceIndex == 1) {
                    func_80035B18(play, actor, 0x1037);
                    Flags_SetInfTable(INFTABLE_2B);
                }
            }
            if (msgCtx->choiceTextId == 0x1038) {
                if (msgCtx->choiceIndex == 0) {
                    func_80035B18(play, actor, 0x1039);
                    Flags_SetInfTable(INFTABLE_2E);
                }
                if (msgCtx->choiceIndex == 1) {
                    func_80035B18(play, actor, 0x103A);
                    Flags_SetInfTable(INFTABLE_2F);
                }
                if (msgCtx->choiceIndex == 2) {
                    func_80035B18(play, actor, 0x103B);
                    Flags_SetInfTable(INFTABLE_30);
                }
            }
            ret = 0;
            break;
        case 0x1062:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x1063);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x1064);
            }
            ret = 0;
            break;
        case 0x2030:
        case 0x2031:
            if (msgCtx->choiceIndex == 0) {
                if (gSaveContext.save.info.playerData.rupees >= 10) {
                    func_80035B18(play, actor, 0x2034);
                    Rupees_ChangeBy(-10);
                } else {
                    func_80035B18(play, actor, 0x2032);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x2032);
            }
            Flags_SetInfTable(INFTABLE_9A);
            ret = 0;
            break;
        case 0x2035:
            break;
        case 0x2036:
        case 0x2037:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x201F);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2038:
            if (msgCtx->choiceIndex == 0) {
                break;
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2034:
            if (msgCtx->choiceIndex != 0) {
                break;
            }
            func_80035B18(play, actor, 0x2035);
            ret = 0;
            break;
        case 0x2043:
            if (Flags_GetEventChkInf(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
                break;
            }
            func_80035B18(play, actor, 0x2044);
            ret = 0;
            break;
        case 0x205A:
            break;
        case 0x300A:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetEventChkInf(EVENTCHKINF_22)) {
                    func_80035B18(play, actor, 0x300B);
                } else {
                    func_80035B18(play, actor, 0x300C);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x300D);
            }
            ret = 0;
            break;
        case 0x301B:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x301D);
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_113)) {
                    func_80035B18(play, actor, 0x301F);
                } else {
                    func_80035B18(play, actor, 0x301E);
                }
            }
            ret = 0;
            break;
        case 0x301E:
            func_80035B18(play, actor, 0x3020);
            ret = 0;
            break;
        case 0x400C:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x400D);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x400E);
            }
            ret = 0;
            break;
        case 0x7007:
            func_80035B18(play, actor, 0x703E);
            ret = 0;
            break;
        case 0x703E:
            func_80035B18(play, actor, 0x703F);
            ret = 0;
            break;
        case 0x703F:
            func_80035B18(play, actor, 0x7042);
            ret = 0;
            break;
    }

    return ret;
}

u16 func_80037C30(PlayState* play, s16 arg1) {
    return func_80035BFC(play, arg1);
}

s32 func_80037C5C(PlayState* play, s16 arg1, u16 textId) {
    func_80036E50(textId, arg1);
    return false;
}

s32 func_80037C94(PlayState* play, Actor* actor, s32 arg2) {
    return func_800374E0(play, actor, actor->textId);
}

s32 func_80037CB8(PlayState* play, Actor* actor, s16 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = false;

    switch (Message_GetState(msgCtx)) {
        case TEXT_STATE_CLOSING:
            func_80037C5C(play, arg2, actor->textId);
            ret = true;
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play) && func_80037C94(play, actor, arg2)) {
                Audio_PlaySfxGeneral(NA_SE_SY_CANCEL, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
                ret = true;
            }
            break;
    }

    return ret;
}

s32 func_80037D98(PlayState* play, Actor* actor, s32 arg2, s32* arg3) {
    s16 var;
    s16 sp2C;
    s16 sp2A;
    s16 abs_var;

    if (Actor_TalkOfferAccepted(actor, play)) {
        *arg3 = 1;
        return true;
    }

    if (*arg3 == 1) {
        if (func_80037CB8(play, actor, arg2)) {
            *arg3 = 0;
        }
        return false;
    }

    Actor_GetScreenPos(play, actor, &sp2C, &sp2A);

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
        if (Actor_OfferTalk(actor, play, 80.0f)) {
            actor->textId = func_80037C30(play, arg2);
        }
    } else {
        if (Actor_OfferTalkNearColChkInfoCylinder(actor, play)) {
            actor->textId = func_80037C30(play, arg2);
        }
    }

    return false;
}

s32 Actor_TrackNone(Vec3s* headRot, Vec3s* torsoRot) {
    Math_SmoothStepToS(&headRot->y, 0, 6, 6200, 100);
    Math_SmoothStepToS(&headRot->x, 0, 6, 6200, 100);
    Math_SmoothStepToS(&torsoRot->y, 0, 6, 6200, 100);
    Math_SmoothStepToS(&torsoRot->x, 0, 6, 6200, 100);
    return true;
}

s32 Actor_TrackPoint(Actor* actor, Vec3f* target, Vec3s* headRot, Vec3s* torsoRot) {
    s16 pitch;
    s16 yaw;
    s16 yawDiff;

    pitch = Math_Vec3f_Pitch(&actor->focus.pos, target);
    yaw = Math_Vec3f_Yaw(&actor->focus.pos, target) - actor->world.rot.y;

    Math_SmoothStepToS(&headRot->x, pitch, 6, 2000, 1);
    headRot->x = CLAMP(headRot->x, -6000, 6000);

    yawDiff = Math_SmoothStepToS(&headRot->y, yaw, 6, 2000, 1);
    headRot->y = CLAMP(headRot->y, -8000, 8000);

    if ((yawDiff != 0) && (ABS(headRot->y) < 8000)) {
        return false;
    }

    Math_SmoothStepToS(&torsoRot->y, yaw - headRot->y, 4, 2000, 1);
    torsoRot->y = CLAMP(torsoRot->y, -12000, 12000);

    return true;
}

/**
 * Same as Actor_TrackPlayer, except use the actor's world position as the focus point, with the height
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
 * @note same note as Actor_TrackPlayer
 */
s32 Actor_TrackPlayerSetFocusHeight(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, f32 focusHeight) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = actor->world.pos;
    actor->focus.pos.y += focusHeight;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || gDebugCamEnabled) &&
          (gSaveContext.save.entranceIndex == ENTR_KOKIRI_FOREST_0))) {
        yaw = ABS((s16)(actor->yawTowardsPlayer - actor->shape.rot.y));
        if (yaw >= 0x4300) {
            Actor_TrackNone(headRot, torsoRot);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || gDebugCamEnabled) &&
        (gSaveContext.save.entranceIndex == ENTR_KOKIRI_FOREST_0)) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    Actor_TrackPoint(actor, &target, headRot, torsoRot);

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
s32 Actor_TrackPlayer(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, Vec3f focusPos) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = focusPos;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || gDebugCamEnabled) &&
          (gSaveContext.save.entranceIndex == ENTR_KOKIRI_FOREST_0))) {
        yaw = ABS((s16)(actor->yawTowardsPlayer - actor->shape.rot.y));
        if (yaw >= 0x4300) {
            Actor_TrackNone(headRot, torsoRot);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || gDebugCamEnabled) &&
        (gSaveContext.save.entranceIndex == ENTR_KOKIRI_FOREST_0)) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    Actor_TrackPoint(actor, &target, headRot, torsoRot);

    return true;
}
