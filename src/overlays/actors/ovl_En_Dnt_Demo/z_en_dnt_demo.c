/*
 * File: z_en_dnt_demo.c
 * Overlay: ovl_En_Dnt_Demo
 * Description: Forest Stage minigame
 */

#include "z_en_dnt_demo.h"

#include "overlays/actors/ovl_En_Dnt_Jiji/z_en_dnt_jiji.h"
#include "overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.h"
#include "terminal.h"

#define FLAGS 0

typedef enum EnDntDemoResults {
    /* 0 */ DNT_LIKE,
    /* 1 */ DNT_HATE,
    /* 2 */ DNT_LOVE
} EnDntDemoResults;

void En_Dnt_Demo_actor_ct(Actor* thisx, PlayState* play2);
void En_Dnt_Demo_actor_dt(Actor* thisx, PlayState* play);
void En_Dnt_Demo_actor_move(Actor* thisx, PlayState* play);

static void mode_check(EnDntDemo* this, PlayState* play);
void mode_help_check(EnDntDemo* this, PlayState* play);
static void mode_no_move(EnDntDemo* this, PlayState* play);

ActorProfile En_Dnt_Demo_Profile = {
    /**/ ACTOR_EN_DNT_DEMO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnDntDemo),
    /**/ En_Dnt_Demo_actor_ct,
    /**/ En_Dnt_Demo_actor_dt,
    /**/ En_Dnt_Demo_actor_move,
    /**/ NULL,
};

//! @bug
//! This table is missing a column for the Mask of Truth, so it reads the first value of the next row. In the last row,
//! it reads the first entry of mode_data (4), which is an invalid result. The scrubs have no reaction in this case.
static s16 demo_type_table_data[8][7] = {
    /* Keaton    Skull     Spooky    Bunny     Goron      Zora     Gerudo         Truth   */
    { DNT_LIKE, DNT_HATE, DNT_LIKE, DNT_HATE, DNT_LOVE, DNT_LIKE, DNT_HATE }, /* DNT_HATE */
    { DNT_HATE, DNT_LIKE, DNT_HATE, DNT_LIKE, DNT_HATE, DNT_HATE, DNT_LOVE }, /* DNT_LOVE */
    { DNT_LOVE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_LIKE, DNT_LIKE, DNT_LIKE }, /* DNT_HATE */
    { DNT_HATE, DNT_LOVE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_LIKE }, /* DNT_LIKE */
    { DNT_LIKE, DNT_LIKE, DNT_LOVE, DNT_LIKE, DNT_LIKE, DNT_LIKE, DNT_HATE }, /* DNT_LIKE */
    { DNT_LIKE, DNT_LIKE, DNT_LIKE, DNT_LOVE, DNT_HATE, DNT_LOVE, DNT_LIKE }, /* DNT_HATE */
    { DNT_HATE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_HATE, DNT_HATE }, /* DNT_LOVE */
    { DNT_LOVE, DNT_LOVE, DNT_LOVE, DNT_LOVE, DNT_LOVE, DNT_LOVE, DNT_LOVE }, /* INVALID  */
};

static s16 mode_data[3][2] = {
    /* DNT_LIKE */ { DNT_SIGNAL_HIDE, DNT_ACTION_LOW_RUPEES },
    /* DNT_HATE */ { DNT_SIGNAL_HIDE, DNT_ACTION_ATTACK },
    /* DNT_LOVE */ { DNT_SIGNAL_DANCE, DNT_ACTION_DANCE },
};

static Vec3f nomal_pos_Data[] = {
    { 3810.0f, -20.0f, 1010.0f }, { 3890.0f, -20.0f, 990.0f }, { 3730.0f, -20.0f, 950.0f },
    { 3840.0f, -20.0f, 930.0f },  { 3910.0f, -20.0f, 870.0f }, { 3780.0f, -20.0f, 860.0f },
    { 3710.0f, -20.0f, 840.0f },  { 3860.0f, -20.0f, 790.0f }, { 3750.0f, -20.0f, 750.0f },
};

void En_Dnt_Demo_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Dnt_Demo_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDntDemo* this = (EnDntDemo*)thisx;
    s32 i;
    s32 pad;

    PRINTF("\n\n");
    // "Deku Scrub mask show start"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツお面品評会開始 ☆☆☆☆☆ \n" VT_RST);
    for (i = 0; i < 9; i++) {
        this->scrubPos[i] = nomal_pos_Data[i];
        this->scrubs[i] = (EnDntNomal*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_DNT_NOMAL,
                                                          this->scrubPos[i].x, this->scrubPos[i].y, this->scrubPos[i].z,
                                                          0, 0, 0, i + ENDNTNOMAL_STAGE);
        if (this->scrubs[i] != NULL) {
            // "zako zako" [small fries]
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ザコザコ ☆☆☆☆☆ %x\n" VT_RST, this->scrubs[i]);
        }
    }

    this->leaderPos.x = 4050.0f;
    this->leaderPos.y = -20.0f;
    this->leaderPos.z = 1000.0f;
    this->leader = (EnDntJiji*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_DNT_JIJI,
                                                  this->leaderPos.x, this->leaderPos.y, this->leaderPos.z, 0, 0, 0, 0);
    if (this->leader != NULL) {
        // "jiji jiji jiji jiji jiji" [onomatopoeia for the scrub sound?]
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ じじじじじじじじじじい ☆☆☆☆☆ %x\n" VT_RST, this->leader);
    }
    this->subCamId = SUB_CAM_ID_DONE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_check;
}

static void mode_check(EnDntDemo* this, PlayState* play) {
    s16 delay;
    s16 reaction;
    s16 rand9;
    s16 maskIdx;
    s16 resultIdx;
    u8 ignore;
    s32 i;

    if (this->leaderSignal != DNT_SIGNAL_NONE) {
        for (i = 0; i < 9; i++) {
            this->scrubs[i]->stageSignal = this->leaderSignal;
            this->scrubs[i]->action = this->action;
            this->scrubs[i]->stagePrize = DNT_PRIZE_NONE;
        }
        if (this->leader->isSolid) {
            this->leader->stageSignal = DNT_LEADER_SIGNAL_BURROW;
        }
        this->leaderSignal = DNT_SIGNAL_NONE;
        this->actionFunc = mode_help_check;
    } else if ((this->actor.xzDistToPlayer > 30.0f) || (mask_check(play) == 0)) {
        this->debugArrowTimer++;
        if (this->subCamId != SUB_CAM_ID_DONE) {
            this->subCamId = SUB_CAM_ID_DONE;
        }
        if (this->judgeTimer != 0) {
            for (i = 0; i < 9; i++) {
                this->scrubs[i]->stageSignal = DNT_SIGNAL_HIDE;
            }
            this->judgeTimer = 0;
        }
    } else {
        if ((mask_check(play) != 0) && (this->subCamId == SUB_CAM_ID_DONE)) {
            this->subCamId = makeOnepointDemo(play, 2220, -99, &this->scrubs[3]->actor, CAM_ID_MAIN);
        }
        this->debugArrowTimer = 0;
        if (this->judgeTimer == 40) {
            for (i = 0; i < 9; i++) {
                this->scrubs[i]->stageSignal = DNT_SIGNAL_LOOK;
            }
        }
        if (this->judgeTimer > 40) {
            // "gera gera" [onomatopoeia for loud giggling]
            PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ げらげら ☆☆☆☆☆ \n" VT_RST);
            Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EV_CROWD - SFX_FLAG, 2.0f);
        }
        if (this->judgeTimer < 120) {
            this->judgeTimer++;
        } else {
            ignore = false;
            reaction = DNT_SIGNAL_NONE;
            delay = 0;
            switch (mask_check(play)) {
                case PLAYER_MASK_SKULL:
                    if (!GET_ITEMGETINF(ITEMGETINF_FOREST_STAGE_STICK_UPGRADE)) {
                        reaction = DNT_SIGNAL_CELEBRATE;
                        this->prize = DNT_PRIZE_STICK;
                        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_SARIA_THEME);
                        break;
                    }
                    FALLTHROUGH;
                case PLAYER_MASK_TRUTH:
                    if (!GET_ITEMGETINF(ITEMGETINF_FOREST_STAGE_NUT_UPGRADE) &&
                        (mask_check(play) != PLAYER_MASK_SKULL)) {
                        Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                        this->prize = DNT_PRIZE_NUTS;
                        this->leader->stageSignal = DNT_LEADER_SIGNAL_UP;
                        reaction = DNT_SIGNAL_LOOK;
                        if (this->subCamId != SUB_CAM_ID_DONE) {
                            this->subCamId = SUB_CAM_ID_DONE;
                            reaction = DNT_SIGNAL_LOOK;
                            makeOnepointDemo(play, 2340, -99, &this->leader->actor, CAM_ID_MAIN);
                        }
                        break;
                    }
                    FALLTHROUGH;
                case PLAYER_MASK_KEATON:
                case PLAYER_MASK_SPOOKY:
                case PLAYER_MASK_BUNNY:
                case PLAYER_MASK_GORON:
                case PLAYER_MASK_ZORA:
                case PLAYER_MASK_GERUDO:
                    rand9 = rnd_f(8.99f);
                    // fake match, possible alternative is `maskIdx = mask_check(play); maskIdx--;` on one line
                    maskIdx = (s16)mask_check(play) - 1;
                    if (rand9 == 8) {
                        ignore = true;
                        delay = 8;
                        reaction = DNT_SIGNAL_HIDE;
                        // "Special!"
                        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 特別！ ☆☆☆☆☆ \n" VT_RST);
                    } else {
                        if (maskIdx >= PLAYER_MASK_MAX - 1) {
                            // "This is dangerous!"
                            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ヤバいよこれ！ ☆☆☆☆☆ \n" VT_RST);
                            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ヤバいよこれ！ ☆☆☆☆☆ \n" VT_RST);
                            PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ ヤバいよこれ！ ☆☆☆☆☆ \n" VT_RST);
                            PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ ヤバいよこれ！ ☆☆☆☆☆ \n" VT_RST);
                            maskIdx = rnd_f(7.99f);
                        }

                        resultIdx = demo_type_table_data[rand9][maskIdx];
                        reaction = mode_data[resultIdx][0];
                        this->action = mode_data[resultIdx][1];
                        switch (this->action) {
                            case DNT_ACTION_LOW_RUPEES:
                                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_COURTYARD);
                                break;
                            case DNT_ACTION_ATTACK:
                                if (this->subCamId != SUB_CAM_ID_DONE) {
                                    this->subCamId = SUB_CAM_ID_DONE;
                                    makeOnepointDemo(play, 2350, -99, &this->scrubs[3]->actor, CAM_ID_MAIN);
                                }
                                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 8, NA_BGM_ENEMY);
                                break;
                            case DNT_ACTION_DANCE:
                                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_SHOP);
                                break;
                        }
                        PRINTF("\n\n");
                        // "Each index 1"
                        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 各インデックス１ ☆☆☆☆☆ %d\n" VT_RST, rand9);
                        // "Each index 2"
                        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 各インデックス２ ☆☆☆☆☆ %d\n" VT_RST, maskIdx);
                        // "Each index 3"
                        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 各インデックス３ ☆☆☆☆☆ %d\n" VT_RST, resultIdx);
                        PRINTF("\n");
                        // "What kind of evaluation?"
                        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ どういう評価？  ☆☆☆☆☆☆ %d\n" VT_RST, reaction);
                        // "What kind of action?"
                        PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ どういうアクション？  ☆☆☆ %d\n" VT_RST, this->action);
                        PRINTF("\n\n");
                        break;
                    }
            }
            if (reaction != DNT_SIGNAL_NONE) {
                for (i = 0; i < 9; i++) {
                    if (delay != 0) {
                        this->scrubs[i]->timer3 = delay * i;
                    }
                    this->scrubs[i]->action = this->action;
                    this->scrubs[i]->stageSignal = reaction;
                    this->scrubs[i]->ignore = ignore;
                    if (this->prize != DNT_PRIZE_NONE) {
                        this->scrubs[i]->timer1 = 300;
                        this->scrubs[i]->stagePrize = this->prize;
                        this->scrubs[i]->targetPos = this->leader->actor.world.pos;
                        if (this->prize == DNT_PRIZE_NUTS) {
                            this->leader->stageSignal = DNT_LEADER_SIGNAL_UP;
                        }
                        if (this->prize == DNT_PRIZE_STICK) {
                            this->leader->timer = 300;
                        }
                    }
                }
                this->actionFunc = mode_help_check;
            }
        }
    }
}

void mode_help_check(EnDntDemo* this, PlayState* play) {
    s32 i;
    s16 offsetAngle;
    Vec3f leaderPos;
    f32 offsetDist;

    if (this->leaderSignal != DNT_SIGNAL_NONE) {
        for (i = 0; i < 9; i++) {
            this->scrubs[i]->action = this->action;
            this->scrubs[i]->stageSignal = this->leaderSignal;
            this->scrubs[i]->stagePrize = DNT_PRIZE_NONE;
        }
        if (this->leader->action == DNT_LEADER_ACTION_UP) {
            this->leader->stageSignal = DNT_LEADER_SIGNAL_BURROW;
        } else if (this->leader->unburrow) {
            this->leader->stageSignal = DNT_LEADER_SIGNAL_RETURN;
        } else {
            this->leader->action = DNT_LEADER_ACTION_ATTACK;
        }
        this->leader->timer = 0;
        this->leaderSignal = this->action = DNT_SIGNAL_NONE;
        this->actionFunc = mode_no_move;
    } else if (this->prize == DNT_PRIZE_STICK) {
        for (i = 0; i < 9; i++) {
            offsetAngle = -this->leader->actor.shape.rot.y;
            leaderPos = this->leader->actor.world.pos;

            if (!(i & 1)) {
                offsetAngle -= 0x59D8;
            }
            offsetDist = ((i + 1) * 20.0f) + 20.0f;
            this->scrubs[i]->timer2 = 10;
            this->scrubs[i]->targetPos.x = leaderPos.x + sin_s(offsetAngle) * offsetDist;
            this->scrubs[i]->targetPos.y = leaderPos.y;
            this->scrubs[i]->targetPos.z = leaderPos.z + cos_s(offsetAngle) * offsetDist;
        }
    }
}

static void mode_no_move(EnDntDemo* this, PlayState* play) {
    s32 i;

    if (this->leaderSignal != DNT_SIGNAL_NONE) {
        for (i = 0; i < 9; i++) {
            this->scrubs[i]->action = this->action;
            this->scrubs[i]->stageSignal = this->leaderSignal;
            this->scrubs[i]->stagePrize = DNT_PRIZE_NONE;
        }
        this->leaderSignal = this->action = DNT_SIGNAL_NONE;
    }
}

void En_Dnt_Demo_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntDemo* this = (EnDntDemo*)thisx;

    if (this->unkTimer2 != 0) {
        this->unkTimer2--;
    }
    if (this->unkTimer1 != 0) {
        this->unkTimer1--;
    }
    this->actionFunc(this, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        if (this->debugArrowTimer != 0) {
            if (!(this->debugArrowTimer & 1)) {
                Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 0, 255, 4, play->state.gfxCtx);
            }
        } else {
            Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                   this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 255, 255, 255, 4, play->state.gfxCtx);
        }
    }
}
