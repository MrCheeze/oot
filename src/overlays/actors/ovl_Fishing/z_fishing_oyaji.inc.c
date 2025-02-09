static u16 sPondOwnerTextIds[] = { 0x4096, 0x408D, 0x408E, 0x408F, 0x4094, 0x4095 };

void Fishing_HandleOwnerDialog(Fishing* this, PlayState* play) {
    switch (this->stateAndTimer) {
        case 0:
            if (sFishingPlayingState == 0) {
                if (sLinkAge != LINK_AGE_CHILD) {
                    if ((HIGH_SCORE(HS_FISHING) & HS_FISH_PLAYED_CHILD) &&
                        !(HIGH_SCORE(HS_FISHING) & HS_FISH_PLAYED_ADULT)) {
                        this->actor.textId = 0x4093;
                    } else {
                        this->actor.textId = 0x407B;
                    }
                } else {
                    this->actor.textId = 0x407B;
                }
            } else if (!sIsOwnersHatHooked) {
                this->actor.textId = 0x4084;
            } else {
                this->actor.textId = 0x4097;
            }

            if (Actor_TalkOfferAccepted(&this->actor, play)) {
                if (sFishingPlayingState == 0) {
                    this->stateAndTimer = 1;
                    if (sLinkAge != LINK_AGE_CHILD) {
                        HIGH_SCORE(HS_FISHING) |= HS_FISH_PLAYED_ADULT;
                    } else {
                        HIGH_SCORE(HS_FISHING) |= HS_FISH_PLAYED_CHILD;
                    }
                } else {
                    this->stateAndTimer = 10;
                }
            } else {
                Actor_OfferTalk(&this->actor, play, 100.0f);
            }
            break;

        case 1:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        if (gSaveContext.save.info.playerData.rupees >= 20) {
                            Rupees_ChangeBy(-20);
                            if (!Rumble_Controller1HasRumblePak()) {
                                this->actor.textId = 0x407C;
                            } else {
                                this->actor.textId = 0x407D;
                            }
                            Message_ContinueTextbox(play, this->actor.textId);
                            this->stateAndTimer = 2;
                        } else {
                            Message_ContinueTextbox(play, 0x407E);
                            this->stateAndTimer = 3;
                        }
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x2D);
                        this->stateAndTimer = 3;
                        break;
                }
            }
            break;

        case 2:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Message_ContinueTextbox(play, 0x407F);
                this->stateAndTimer = 4;
            }
            break;

        case 3:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->stateAndTimer = 0;
            }
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
                this->stateAndTimer = 0;
            }
            break;

        case 4:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        sFishLengthToWeigh = sFishingRecordLength;
                        Message_ContinueTextbox(play, 0x4080);
                        this->stateAndTimer = 5;
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x407F);
                        break;
                }
            }
            break;

        case 5:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                play->interfaceCtx.unk_260 = 1;
                play->startPlayerFishing(play);
                sFishingPlayingState = 1;
                sOwnerTheftTimer = 20;
                this->stateAndTimer = 0;

                if ((HIGH_SCORE(HS_FISHING) & 0xFF0000) < 0xFF0000) {
                    HIGH_SCORE(HS_FISHING) += HS_FISH_PLAYED;
                }
            }
            break;

        case 10:
            if (sIsOwnersHatHooked) { // owner asks for hat back
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            Message_ContinueTextbox(play, 0x40B2);
                            sOwnerHair = FS_OWNER_CAPPED;
                            sIsOwnersHatHooked = false;
                            this->stateAndTimer = 20;
                            break;
                        case 1:
                            this->stateAndTimer = 0;
                            break;
                    }
                }
            } else {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            if (sFishOnHandLength == 0.0f) {
                                this->actor.textId = 0x408C;
                                this->stateAndTimer = 20;
                            } else if (sFishOnHandIsLoach == 0) {
                                sFishLengthToWeigh = sFishOnHandLength;
                                if ((s16)sFishingRecordLength < (s16)sFishOnHandLength) {
                                    if (sLureCaughtWith == FS_LURE_SINKING) {
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
                            Message_ContinueTextbox(play, this->actor.textId);
                            break;
                        case 1:
                            if (sFishingTimePlayed > 36000) {
                                sFishingTimePlayed = 30000;
                                Message_ContinueTextbox(play, 0x4088);
                            } else {
                                if (sFishingFoggy == 0) {
                                    if (sPondOwnerTextIdIndex == 0) {
                                        sPondOwnerTextIdIndex++;
                                    }
                                }

                                if ((sLureEquipped == FS_LURE_SINKING) &&
                                    (sPondOwnerTextIds[sPondOwnerTextIdIndex] == 0x408D)) {
                                    Message_ContinueTextbox(play, 0x40AF);
                                } else {
                                    Message_ContinueTextbox(play, sPondOwnerTextIds[sPondOwnerTextIdIndex]);
                                }

                                sPondOwnerTextIdIndex++;

                                if (sLinkAge != LINK_AGE_CHILD) {
                                    if (sPondOwnerTextIdIndex >= 6) {
                                        sPondOwnerTextIdIndex = 0;
                                    }
                                } else {
                                    if (sPondOwnerTextIdIndex >= 4) {
                                        sPondOwnerTextIdIndex = 0;
                                    }
                                }
                            }
                            this->stateAndTimer = 0;
                            break;
                        case 2:
                            if (sFishesCaught == 0) {
                                Message_ContinueTextbox(play, 0x4085);
                            } else if (sLinkAge == LINK_AGE_CHILD) {
                                Message_ContinueTextbox(play, 0x4092);
                            }
                            this->stateAndTimer = 22;
                            break;
                    }
                }
            }
            break;

        case 11: // collect prize, update record.
            if (((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) ||
                 (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) &&
                Message_ShouldAdvance(play)) {
                s32 getItemId;

                Message_CloseTextbox(play);

                if (sFishOnHandIsLoach == 0) {
                    sFishingRecordLength = sFishOnHandLength;
                    sFishOnHandLength = 0.0f;

                    if (sLinkAge == LINK_AGE_CHILD) {
                        f32 temp;

                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF00;
                        HIGH_SCORE(HS_FISHING) |= (s32)sFishingRecordLength & HS_FISH_LENGTH_CHILD;

                        temp = (HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_ADULT) >> 0x18;
                        if (temp < sFishingRecordLength) {
                            HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                            HIGH_SCORE(HS_FISHING) |= ((s32)sFishingRecordLength & HS_FISH_LENGTH_CHILD) << 0x18;

                            if (sLureCaughtWith == FS_LURE_SINKING) {
                                HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_ADULT;
                            }
                        }

                        if (sLureCaughtWith == FS_LURE_SINKING) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_CHILD;
                            this->stateAndTimer = 0;
                            break;
                        }
                    } else {
                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                        HIGH_SCORE(HS_FISHING) |= ((s32)sFishingRecordLength & HS_FISH_LENGTH_CHILD) << 0x18;

                        if (sLureCaughtWith == FS_LURE_SINKING) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_CHEAT_ADULT;
                            this->stateAndTimer = 0;
                            break;
                        }
                    }

                    if (sFishingRecordLength >= 60.0f) { // 13 lbs
                        getItemId = GI_RUPEE_PURPLE;
                    } else if (sFishingRecordLength >= 50.0f) { // 9 lbs
                        getItemId = GI_RUPEE_RED;
                    } else if (sFishingRecordLength >= 40.0f) { // 6 lbs
                        getItemId = GI_RUPEE_BLUE;
                    } else {
                        getItemId = GI_RUPEE_GREEN;
                    }

                    if (sLinkAge == LINK_AGE_CHILD) { // 9 lbs
                        if ((sFishingRecordLength >= 50.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD)) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_CHILD;
                            getItemId = GI_HEART_PIECE;
                            sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
                        }
                    } else { // 13 lbs
                        if ((sFishingRecordLength >= 60.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT)) {
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_ADULT;
                            getItemId = GI_SCALE_GOLDEN;
                            sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
                        }
                    }
                } else {
                    sFishOnHandLength = 0.0f; // doesn't record loach
                    getItemId = GI_RUPEE_PURPLE;
                }

                this->actor.parent = NULL;

                Actor_OfferGetItem(&this->actor, play, getItemId, 2000.0f, 1000.0f);
                this->stateAndTimer = 23;
            }
            break;

        case 20:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->stateAndTimer = 0;
            }
            break;

        case 21:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        this->stateAndTimer = 0;
                        break;
                    case 1:
                        if (sFishesCaught == 0) {
                            Message_ContinueTextbox(play, 0x4085);
                        } else if (sLinkAge == LINK_AGE_CHILD) {
                            Message_ContinueTextbox(play, 0x4092);
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

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                this->stateAndTimer = 0;
                if (sIsOwnersHatHooked) {
                    sOwnerHair = FS_OWNER_CAPPED;
                    sIsOwnersHatHooked = false;
                }
                sFishingPlayingState = 0;
                play->interfaceCtx.unk_260 = 0;
            }
            break;

        case 23:
            sIsRodVisible = false;
            if (Actor_HasParent(&this->actor, play)) {
                this->stateAndTimer = 24;
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_SCALE_GOLDEN, 2000.0f, 1000.0f);
            }
            break;

        case 24:
            sIsRodVisible = false;
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
                if (sFishOnHandIsLoach == 0) {
                    this->stateAndTimer = 0;
                } else {
                    Message_StartTextbox(play, 0x409C, NULL);
                    this->stateAndTimer = 20;
                }
            }
            break;
    }
}
