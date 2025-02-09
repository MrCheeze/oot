static u16 seken_mes_no[] = { 0x4096, 0x408D, 0x408E, 0x408F, 0x4094, 0x4095 };

void bm_message_check(Fishing* this, PlayState* play) {
    switch (this->stateAndTimer) {
        case 0:
            if (fishing_game_mode == 0) {
                if (AGE != LINK_AGE_CHILD) {
                    if ((HIGH_SCORE(HS_FISHING) & HS_FISH_PLAYED_CHILD) &&
                        !(HIGH_SCORE(HS_FISHING) & HS_FISH_PLAYED_ADULT)) {
                        this->actor.textId = 0x4093;
                    } else {
                        this->actor.textId = 0x407B;
                    }
                } else {
                    this->actor.textId = 0x407B;
                }
            } else if (!lure_with_head) {
                this->actor.textId = 0x4084;
            } else {
                this->actor.textId = 0x4097;
            }

            if (Actor_talk_check(&this->actor, play)) {
                if (fishing_game_mode == 0) {
                    this->stateAndTimer = 1;
                    if (AGE != LINK_AGE_CHILD) {
                        HIGH_SCORE(HS_FISHING) |= HS_FISH_PLAYED_ADULT;
                    } else {
                        HIGH_SCORE(HS_FISHING) |= HS_FISH_PLAYED_CHILD;
                    }
                } else {
                    this->stateAndTimer = 10;
                }
            } else {
                Actor_talk_request2(&this->actor, play, 100.0f);
            }
            break;

        case 1:
            if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                message_close(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        if (z_common_data.save.info.playerData.rupees >= 20) {
                            lupy_increase(-20);
                            if (!z_vibctl2_RumblePackIsConnected()) {
                                this->actor.textId = 0x407C;
                            } else {
                                this->actor.textId = 0x407D;
                            }
                            message_set2(play, this->actor.textId);
                            this->stateAndTimer = 2;
                        } else {
                            message_set2(play, 0x407E);
                            this->stateAndTimer = 3;
                        }
                        break;
                    case 1:
                        message_set2(play, 0x2D);
                        this->stateAndTimer = 3;
                        break;
                }
            }
            break;

        case 2:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_close(play);
                message_set2(play, 0x407F);
                this->stateAndTimer = 4;
            }
            break;

        case 3:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_close(play);
                this->stateAndTimer = 0;
            }
            if (message_check(&play->msgCtx) == TEXT_STATE_DONE) {
                this->stateAndTimer = 0;
            }
            break;

        case 4:
            if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                message_close(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        FishRangeForMessage = fish_record;
                        message_set2(play, 0x4080);
                        this->stateAndTimer = 5;
                        break;
                    case 1:
                        message_set2(play, 0x407F);
                        break;
                }
            }
            break;

        case 5:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_close(play);

                play->interfaceCtx.unk_260 = 1;
                play->startPlayerFishing(play);
                fishing_game_mode = 1;
                head_check = 20;
                this->stateAndTimer = 0;

                if ((HIGH_SCORE(HS_FISHING) & 0xFF0000) < 0xFF0000) {
                    HIGH_SCORE(HS_FISHING) += HS_FISH_PLAYED;
                }
            }
            break;

        case 10:
            if (lure_with_head) { // owner asks for hat back
                if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                    message_close(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            message_set2(play, 0x40B2);
                            bm_head_no = FS_OWNER_CAPPED;
                            lure_with_head = false;
                            this->stateAndTimer = 20;
                            break;
                        case 1:
                            this->stateAndTimer = 0;
                            break;
                    }
                }
            } else {
                if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                    message_close(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            if (my_record == 0.0f) {
                                this->actor.textId = 0x408C;
                                this->stateAndTimer = 20;
                            } else if (my_fish_kind == 0) {
                                FishRangeForMessage = my_record;
                                if ((s16)fish_record < (s16)my_record) {
                                    if (use_lure == FS_LURE_SINKING) {
                                        this->actor.textId = 0x40B0;
                                    } else {
                                        this->actor.textId = 0x4086;
                                    }
                                    this->stateAndTimer = 11;
                                } else {
                                    this->actor.textId = 0x408B;
                                    this->stateAndTimer = 20;
                                }
                            } else {
                                this->actor.textId = 0x409B;
                                this->stateAndTimer = 11;
                            }
                            message_set2(play, this->actor.textId);
                            break;
                        case 1:
                            if (fishing_time > 36000) {
                                fishing_time = 30000;
                                message_set2(play, 0x4088);
                            } else {
                                if (mad_water == 0) {
                                    if (seken_mes_ct == 0) {
                                        seken_mes_ct++;
                                    }
                                }

                                if ((lure_type == FS_LURE_SINKING) &&
                                    (seken_mes_no[seken_mes_ct] == 0x408D)) {
                                    message_set2(play, 0x40AF);
                                } else {
                                    message_set2(play, seken_mes_no[seken_mes_ct]);
                                }

                                seken_mes_ct++;

                                if (AGE != LINK_AGE_CHILD) {
                                    if (seken_mes_ct >= 6) {
                                        seken_mes_ct = 0;
                                    }
                                } else {
                                    if (seken_mes_ct >= 4) {
                                        seken_mes_ct = 0;
                                    }
                                }
                            }
                            this->stateAndTimer = 0;
                            break;
                        case 2:
                            if (fish_catch_count == 0) {
                                message_set2(play, 0x4085);
                            } else if (AGE == LINK_AGE_CHILD) {
                                message_set2(play, 0x4092);
                            }
                            this->stateAndTimer = 22;
                            break;
                    }
                }
            }
            break;

        case 11: // collect prize, update record.
            if (((message_check(&play->msgCtx) == TEXT_STATE_EVENT) ||
                 (message_check(&play->msgCtx) == TEXT_STATE_NONE)) &&
                pad_on_check(play)) {
                s32 getItemId;

                message_close(play);

                if (my_fish_kind == 0) {
                    fish_record = my_record;
                    my_record = 0.0f;

                    if (AGE == LINK_AGE_CHILD) {
                        f32 temp;

                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF00;
                        HIGH_SCORE(HS_FISHING) |= (s32)fish_record & HS_FISH_LENGTH_CHILD;

                        temp = (HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_ADULT) >> 0x18;
                        if (temp < fish_record) {
                            HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                            HIGH_SCORE(HS_FISHING) |= ((s32)fish_record & HS_FISH_LENGTH_CHILD) << 0x18;

                            if (use_lure == FS_LURE_SINKING) {
                                HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_ADULT;
                            }
                        }

                        if (use_lure == FS_LURE_SINKING) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_CHILD;
                            this->stateAndTimer = 0;
                            break;
                        }
                    } else {
                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                        HIGH_SCORE(HS_FISHING) |= ((s32)fish_record & HS_FISH_LENGTH_CHILD) << 0x18;

                        if (use_lure == FS_LURE_SINKING) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_ADULT;
                            this->stateAndTimer = 0;
                            break;
                        }
                    }

                    if (fish_record >= 60.0f) { // 13 lbs
                        getItemId = GI_RUPEE_PURPLE;
                    } else if (fish_record >= 50.0f) { // 9 lbs
                        getItemId = GI_RUPEE_RED;
                    } else if (fish_record >= 40.0f) { // 6 lbs
                        getItemId = GI_RUPEE_BLUE;
                    } else {
                        getItemId = GI_RUPEE_GREEN;
                    }

                    if (AGE == LINK_AGE_CHILD) { // 9 lbs
                        if ((fish_record >= 50.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD)) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_CHILD;
                            getItemId = GI_HEART_PIECE;
                            worm_set_P = (u8)rnd_f(3.999f) + 1;
                        }
                    } else { // 13 lbs
                        if ((fish_record >= 60.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT)) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_ADULT;
                            getItemId = GI_SCALE_GOLDEN;
                            worm_set_P = (u8)rnd_f(3.999f) + 1;
                        }
                    }
                } else {
                    my_record = 0.0f; // doesn't record loach
                    getItemId = GI_RUPEE_PURPLE;
                }

                this->actor.parent = NULL;

                Actor_carry_request_set2(&this->actor, play, getItemId, 2000.0f, 1000.0f);
                this->stateAndTimer = 23;
            }
            break;

        case 20:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_close(play);
                this->stateAndTimer = 0;
            }
            break;

        case 21:
            if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                message_close(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        this->stateAndTimer = 0;
                        break;
                    case 1:
                        if (fish_catch_count == 0) {
                            message_set2(play, 0x4085);
                        } else if (AGE == LINK_AGE_CHILD) {
                            message_set2(play, 0x4092);
                        }
                        this->stateAndTimer = 22;
                        break;
                }
            }
            break;

        case 22:
#if DEBUG_FEATURES
            if (play) {}
#endif

            if (message_check(&play->msgCtx) == TEXT_STATE_NONE) {
                this->stateAndTimer = 0;
                if (lure_with_head) {
                    bm_head_no = FS_OWNER_CAPPED;
                    lure_with_head = false;
                }
                fishing_game_mode = 0;
                play->interfaceCtx.unk_260 = 0;
            }
            break;

        case 23:
            rod_disp = false;
            if (Actor_carry_check(&this->actor, play)) {
                this->stateAndTimer = 24;
            } else {
                Actor_carry_request_set2(&this->actor, play, GI_SCALE_GOLDEN, 2000.0f, 1000.0f);
            }
            break;

        case 24:
            rod_disp = false;
            if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
                if (my_fish_kind == 0) {
                    this->stateAndTimer = 0;
                } else {
                    message_set(play, 0x409C, NULL);
                    this->stateAndTimer = 20;
                }
            }
            break;
    }
}
