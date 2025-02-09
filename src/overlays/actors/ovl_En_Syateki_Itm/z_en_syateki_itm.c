#include "z_en_syateki_itm.h"
#include "terminal.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum EnSyatekItemRound {
    SYATEKI_ROUND_GREEN_APPEAR,
    SYATEKI_ROUND_BLUE_SEQUENTIAL,
    SYATEKI_ROUND_GREEN_THROW,
    SYATEKI_ROUND_BLUE_SIMUL,
    SYATEKI_ROUND_RED_LEFT,
    SYATEKI_ROUND_RED_RIGHT,
    SYATEKI_ROUND_MAX
} EnSyatekItemRound;

void En_Syateki_Itm_actor_ct(Actor* thisx, PlayState* play2);
void En_Syateki_Itm_actor_dt(Actor* thisx, PlayState* play);
void En_Syateki_Itm_actor_move(Actor* thisx, PlayState* play);

void mode_start_wait(EnSyatekiItm* this, PlayState* play);
void mode_game_select_init(EnSyatekiItm* this, PlayState* play);
void mode_game_move(EnSyatekiItm* this, PlayState* play);
void mode_next_game_wait(EnSyatekiItm* this, PlayState* play);
void mode_game_ende_check_init(EnSyatekiItm* this, PlayState* play);
static void mode_game_ende_check(EnSyatekiItm* this, PlayState* play);

ActorProfile En_Syateki_Itm_Profile = {
    /**/ ACTOR_EN_SYATEKI_ITM,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnSyatekiItm),
    /**/ En_Syateki_Itm_actor_ct,
    /**/ En_Syateki_Itm_actor_dt,
    /**/ En_Syateki_Itm_actor_move,
    /**/ NULL,
};

static Vec3f initial_set_pos = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSeqHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSeqHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sGreenThrowHome = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSimulHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSimulHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sRedLeftHome1 = { 260.0f, 100.0f, -320.0f };
static Vec3f sRedLeftHome2 = { 360.0f, 100.0f, -320.0f };
static Vec3f sRedRightHome1 = { -230.0f, 94.0f, -360.0f };
static Vec3f sRedRightHome2 = { -400.0f, 94.0f, -360.0f };
static Vec3f sh_pos_data = { 0.0f, 53.0f, -270.0f };
static Vec3f sBlueSeqFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSeqFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sGreenThrowFinal = { 0.0f, 0.0f, 0.0f };
static Vec3f sBlueSimulFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSimulFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sRedLeftFinal1 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedLeftFinal2 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal1 = { 260.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal2 = { 260.0f, 0.0f, 0.0f };
static s16 color_data[] = { 0, 1, 0, 1, 2, 2 };
static s16 color2_data[] = { 0, 1, 1, 0, 1, 1, 4, 4, 4, 4 };
static Vec3f HIT_ruppy_pos_data[] = {
    { -40.0f, 0.0f, -90.0f }, { -20.0f, 0.0f, -90.0f }, { 0.0f, 0.0f, -90.0f },   { 20.0f, 0.0f, -90.0f },
    { 40.0f, 0.0f, -90.0f },  { -40.0f, 0.0f, -60.0f }, { -20.0f, 0.0f, -60.0f }, { 0.0f, 0.0f, -60.0f },
    { 20.0f, 0.0f, -60.0f },  { 40.0f, 0.0f, -60.0f },
};

void En_Syateki_Itm_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnSyatekiItm* this = (EnSyatekiItm*)thisx;
    s32 i;

    this->man = (EnSyatekiMan*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_SYATEKI_MAN, 140.0f,
                                                  0.0f, 255.0f, 0, -0x4000, 0, 0);
    if (this->man == NULL) {
        // "Spawn error"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ エラー原 ☆☆☆☆ \n" VT_RST);
        Actor_delete(&this->actor);
        return;
    }
    for (i = 0; i < 10; i++) {
        this->markers[i] = (EnExRuppy*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_RUPPY,
                                                          HIT_ruppy_pos_data[i].x, HIT_ruppy_pos_data[i].y, HIT_ruppy_pos_data[i].z, 0, 0, 0, 4);
        if (this->markers[i] == NULL) {
            // "Second spawn error"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ エラー原セカンド ☆☆☆☆ \n" VT_RST);
            Actor_delete(&this->actor);
            return;
        }
        this->markers[i]->colorIdx = color2_data[i];
    }
    this->actionFunc = mode_start_wait;
}

void En_Syateki_Itm_actor_dt(Actor* thisx, PlayState* play) {
}

void mode_start_wait(EnSyatekiItm* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);

    if (this->signal == ENSYATEKI_START) {
        player->actor.world.pos.x = -12.0f;
        player->actor.world.pos.y = 20.0f;
        player->actor.world.pos.z = 182.0f;
        player->yaw = player->actor.world.rot.y = player->actor.shape.rot.y = 0x7F03;
        player->actor.world.rot.x = player->actor.shape.rot.x = player->actor.world.rot.z = player->actor.shape.rot.z =
            0;
        to_bow_game_set(play, 15);
        this->roundNum = this->hitCount = 0;
        for (i = 0; i < 6; i++) {
            this->roundFlags[i] = false;
        }
        for (i = 0; i < 10; i++) {
            this->markers[i]->galleryFlag = false;
        }
        this->actionFunc = mode_game_select_init;
    }
}

void mode_game_select_init(EnSyatekiItm* this, PlayState* play) {
    s32 i;
    s32 j;
    Player* player = GET_PLAYER(play);

    if (this->unkTimer == 0) {
        if (LINK_IS_ADULT) {
            for (i = 0, j = 0; i < SYATEKI_ROUND_MAX; i++) {
                if (this->roundFlags[i]) {
                    j++;
                }
            }
            if (j >= SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                this->signal = ENSYATEKI_END;
                this->actionFunc = mode_game_ende_check_init;
                return;
            }
            i = rnd_f(5.99f);
            while (this->roundFlags[i]) {
                i = rnd_f(5.99f);
                if (1) {}
            }
            this->roundNum = i + 1;
            this->roundFlags[i] = true;
        } else {
            this->roundNum++;
            if (this->roundNum > SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                this->signal = ENSYATEKI_END;
                this->actionFunc = mode_game_ende_check_init;
                return;
            }
        }

        this->timer = (this->roundNum == 1) ? 50 : 30;

        Na_StartSystemSe_F(NA_SE_SY_FOUND);
        this->actionFunc = mode_game_move;
    }
}

void mode_game_move(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 roundIdx;

    if (play->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        this->signal = ENSYATEKI_END;
        this->actionFunc = mode_game_ende_check_init;
        return;
    }
    if (this->timer == 0) {
        for (i = 0; i < 2; i++) {
            xyz_t_move(&this->targetHome[i], &zeroVec);
            xyz_t_move(&this->targetFinal[i], &zeroVec);
            this->targets[i] = NULL;
        }
        this->numTargets = 2;
        this->curMarkers[0] = this->curMarkers[1] = NULL;
        roundIdx = this->roundNum - 1;

        switch (roundIdx) {
            case SYATEKI_ROUND_GREEN_APPEAR:
                xyz_t_move(&this->targetHome[0], &initial_set_pos);
                xyz_t_move(&this->targetFinal[0], &sh_pos_data);
                this->curMarkers[0] = this->markers[0];
                this->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                xyz_t_move(&this->targetHome[0], &sBlueSeqHome1);
                xyz_t_move(&this->targetHome[1], &sBlueSeqHome2);
                xyz_t_move(&this->targetFinal[0], &sBlueSeqFinal1);
                xyz_t_move(&this->targetFinal[1], &sBlueSeqFinal2);
                this->curMarkers[0] = this->markers[1];
                this->curMarkers[1] = this->markers[2];
                break;
            case SYATEKI_ROUND_GREEN_THROW:
                xyz_t_move(&this->targetHome[0], &sGreenThrowHome);
                xyz_t_move(&this->targetFinal[0], &sGreenThrowFinal);
                this->curMarkers[0] = this->markers[3];
                this->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SIMUL:
                xyz_t_move(&this->targetHome[0], &sBlueSimulHome1);
                xyz_t_move(&this->targetHome[1], &sBlueSimulHome2);
                xyz_t_move(&this->targetFinal[0], &sBlueSimulFinal1);
                xyz_t_move(&this->targetFinal[1], &sBlueSimulFinal2);
                this->curMarkers[0] = this->markers[4];
                this->curMarkers[1] = this->markers[5];
                break;
            case SYATEKI_ROUND_RED_LEFT:
                xyz_t_move(&this->targetHome[0], &sRedLeftHome1);
                xyz_t_move(&this->targetHome[1], &sRedLeftHome2);
                xyz_t_move(&this->targetFinal[0], &sRedLeftFinal1);
                xyz_t_move(&this->targetFinal[1], &sRedLeftFinal2);
                this->curMarkers[0] = this->markers[6];
                this->curMarkers[1] = this->markers[7];
                break;
            case SYATEKI_ROUND_RED_RIGHT:
                xyz_t_move(&this->targetHome[0], &sRedRightHome1);
                xyz_t_move(&this->targetHome[1], &sRedRightHome2);
                xyz_t_move(&this->targetFinal[0], &sRedRightFinal1);
                xyz_t_move(&this->targetFinal[1], &sRedRightFinal2);
                this->curMarkers[0] = this->markers[8];
                this->curMarkers[1] = this->markers[9];
                break;
        }

        for (i = 0; i < this->numTargets; i++) {
            this->targets[i] = (EnGSwitch*)Actor_info_make_child_actor(
                &play->actorCtx, &this->actor, play, ACTOR_EN_G_SWITCH, this->targetHome[i].x, this->targetHome[i].y,
                this->targetHome[i].z, 0, 0, 0, (ENGSWITCH_TARGET_RUPEE << 0xC) | 0x3F);
            if (this->targets[i] == NULL) {
                // "Rupee spawn error"
                PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ルピーでエラー原 ☆☆☆☆ \n" VT_RST);
                Actor_delete(&this->actor);
                return;
            }
            this->targets[i]->index = i;
            this->targets[i]->colorIdx = color_data[roundIdx];
            xyz_t_move(&this->targets[i]->targetPos, &this->targetFinal[i]);
            switch (roundIdx) {
                case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                    if (i == 1) {
                        this->targets[i]->delayTimer = 60;
                    }
                    break;
                case SYATEKI_ROUND_GREEN_THROW:
                    this->targets[i]->actor.velocity.y = 15.0f;
                    this->targets[i]->actor.gravity = -1.0f;
                    this->targets[i]->moveMode = GSWITCH_THROW;
                    break;
                case SYATEKI_ROUND_RED_LEFT:
                    this->targets[i]->actor.velocity.x = -5.0f;
                    this->targets[i]->moveMode = GSWITCH_LEFT;
                    break;
                case SYATEKI_ROUND_RED_RIGHT:
                    this->targets[i]->actor.velocity.x = 7.0f;
                    this->targets[i]->moveMode = GSWITCH_RIGHT;
                    break;
            }
        }
        this->targetState[0] = this->targetState[1] = ENSYATEKIHIT_NONE;
        this->actionFunc = mode_next_game_wait;
    }
}

void mode_next_game_wait(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 j;

    if (play->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        this->signal = ENSYATEKI_END;
        this->actionFunc = mode_game_ende_check_init;
    } else {
        for (i = 0, j = 0; i < 2; i++) {
            if (this->targetState[i] != ENSYATEKIHIT_NONE) {
                if (this->targetState[i] == ENSYATEKIHIT_HIT) {
                    this->curMarkers[i]->galleryFlag = true;
                }
                j++;
            }
        }
        if (j == this->numTargets) {
            this->actionFunc = mode_game_select_init;
        }
    }
}

void mode_game_ende_check_init(EnSyatekiItm* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if ((this->targetState[i] == ENSYATEKIHIT_NONE) && (this->targets[i] != NULL)) {
            Actor_delete(&this->targets[i]->actor);
        }
    }
    this->actionFunc = mode_game_ende_check;
}

static void mode_game_ende_check(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 10;
    if (this->signal == ENSYATEKI_RESULTS) {
        this->signal = ENSYATEKI_NONE;
        this->actionFunc = mode_start_wait;
    }
    if (this->signal == ENSYATEKI_START) {
        // "1 frame attack and defense!"
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        this->signal = ENSYATEKI_NONE;
        this->actionFunc = mode_start_wait;
    }
}

void En_Syateki_Itm_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSyatekiItm* this = (EnSyatekiItm*)thisx;

    this->actionFunc(this, play);

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->unkTimer != 0) {
        this->unkTimer--;
    }

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}
