#include "z_en_bom_bowl_pit.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"

#include "regs.h"
#include "sfx.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Bom_Bowl_Pit_actor_ct(Actor* thisx, PlayState* play);
void En_Bom_Bowl_Pit_actor_dt(Actor* thisx, PlayState* play);
void En_Bom_Bowl_Pit_actor_move(Actor* thisx, PlayState* play);

static void mode_wait(EnBomBowlPit* this, PlayState* play);
static void mode_hit_check(EnBomBowlPit* this, PlayState* play);
void mode_big_hit_move(EnBomBowlPit* this, PlayState* play);
void mode_item_get_wait(EnBomBowlPit* this, PlayState* play);
void mode_item_get_demo(EnBomBowlPit* this, PlayState* play);
void mode_item_get_init(EnBomBowlPit* this, PlayState* play);
static void mode_player_item_request(EnBomBowlPit* this, PlayState* play);
static void mode_player_item_up(EnBomBowlPit* this, PlayState* play);

static s32 Bowl_Item_No_Data[] = { GI_BOMB_BAG_30, GI_HEART_PIECE, GI_BOMBCHUS_10, GI_BOMBS_1, GI_RUPEE_PURPLE };

ActorProfile En_Bom_Bowl_Pit_Profile = {
    /**/ ACTOR_EN_BOM_BOWL_PIT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnBomBowlPit),
    /**/ En_Bom_Bowl_Pit_actor_ct,
    /**/ En_Bom_Bowl_Pit_actor_dt,
    /**/ En_Bom_Bowl_Pit_actor_move,
    /**/ NULL,
};

void En_Bom_Bowl_Pit_actor_ct(Actor* thisx, PlayState* play) {
    EnBomBowlPit* this = (EnBomBowlPit*)thisx;

    this->actionFunc = mode_wait;
}

void En_Bom_Bowl_Pit_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_wait(EnBomBowlPit* this, PlayState* play) {
    if (this->start != 0) {
        this->start = this->status = 0;
        this->actionFunc = mode_hit_check;
    }
}

static void mode_hit_check(EnBomBowlPit* this, PlayState* play) {
    EnBomChu* chu;
    Vec3f chuPosDiff;

    if (play->cameraPtrs[CAM_ID_MAIN]->setting == CAM_SET_CHU_BOWLING) {
        chu = (EnBomChu*)play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

        while (chu != NULL) {
            if ((&chu->actor == &this->actor) || (chu->actor.id != ACTOR_EN_BOM_CHU)) {
                chu = (EnBomChu*)chu->actor.next;
                continue;
            }

            chuPosDiff.x = chu->actor.world.pos.x - this->actor.world.pos.x;
            chuPosDiff.y = chu->actor.world.pos.y - this->actor.world.pos.y;
            chuPosDiff.z = chu->actor.world.pos.z - this->actor.world.pos.z;

            if (((fabsf(chuPosDiff.x) < 40.0f) || (BREG(2))) && ((fabsf(chuPosDiff.y) < 40.0f) || (BREG(2))) &&
                ((fabsf(chuPosDiff.z) < 40.0f) || (BREG(2)))) {
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
                chu->timer = 1;

                this->subCamId = Gama_play_make_camera(play);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);

                this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.1f;
                this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z = 0.1f;

                this->subCamAt.x = this->viewAt.x = play->view.at.x;
                this->subCamAt.y = this->viewAt.y = play->view.at.y;
                this->subCamAt.z = this->viewAt.z = play->view.at.z;

                this->subCamEye.x = this->viewEye.x = play->view.eye.x;
                this->subCamEye.y = this->viewEye.y = play->view.eye.y;
                this->subCamEye.z = this->viewEye.z = play->view.eye.z;

                this->subCamAtNext.x = 20.0f;
                this->subCamAtNext.y = 100.0f;
                this->subCamAtNext.z = -800.0f;

                this->subCamEyeNext.x = 20.0f;
                this->subCamEyeNext.y = 50.0f;
                this->subCamEyeNext.z = -485.0f;

                this->subCamEyeVel.x = fabsf(this->subCamEye.x - this->subCamEyeNext.x) * 0.02f;
                this->subCamEyeVel.y = fabsf(this->subCamEye.y - this->subCamEyeNext.y) * 0.02f;
                this->subCamEyeVel.z = fabsf(this->subCamEye.z - this->subCamEyeNext.z) * 0.02f;

                this->subCamAtVel.x = fabsf(this->subCamAt.x - this->subCamAtNext.x) * 0.02f;
                this->subCamAtVel.y = fabsf(this->subCamAt.y - this->subCamAtNext.y) * 0.02f;
                this->subCamAtVel.z = fabsf(this->subCamAt.z - this->subCamAtNext.z) * 0.02f;

                Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
                this->actor.textId = 0xF;
                message_set(play, this->actor.textId, NULL);
                this->unk_154 = TEXT_STATE_EVENT;
                Na_StartSystemSe_F(NA_SE_EV_HIT_SOUND);
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
                this->status = 1;
                this->actionFunc = mode_big_hit_move;
                break;
            } else {
                chu = (EnBomChu*)chu->actor.next;
            }
        }
    }
}

void mode_big_hit_move(EnBomBowlPit* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        add_calc2(&this->subCamAt.x, this->subCamAtNext.x, this->subCamAtMaxVelFrac.x, this->subCamAtVel.x);
        add_calc2(&this->subCamAt.y, this->subCamAtNext.y, this->subCamAtMaxVelFrac.y, this->subCamAtVel.y);
        add_calc2(&this->subCamAt.z, this->subCamAtNext.z, this->subCamAtMaxVelFrac.z, this->subCamAtVel.z);
        add_calc2(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamEyeMaxVelFrac.x, this->subCamEyeVel.x);
        add_calc2(&this->subCamEye.y, this->subCamEyeNext.y, this->subCamEyeMaxVelFrac.y, this->subCamEyeVel.y);
        add_calc2(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamEyeMaxVelFrac.z, this->subCamEyeVel.z);
    }

    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);

    if ((this->unk_154 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        message_close(play);
    }

    if ((fabsf(this->subCamEye.x - this->subCamEyeNext.x) < 5.0f) &&
        (fabsf(this->subCamEye.y - this->subCamEyeNext.y) < 5.0f) &&
        (fabsf(this->subCamEye.z - this->subCamEyeNext.z) < 5.0f) &&
        (fabsf(this->subCamAt.x - this->subCamAtNext.x) < 5.0f) &&
        (fabsf(this->subCamAt.y - this->subCamAtNext.y) < 5.0f) &&
        (fabsf(this->subCamAt.z - this->subCamAtNext.z) < 5.0f)) {
        message_close(play);
        this->timer = 30;
        this->actionFunc = mode_item_get_wait;
    }
}

void mode_item_get_wait(EnBomBowlPit* this, PlayState* play) {
    if (this->timer == 0) {
        this->exItem = (EnExItem*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM,
                                                     this->actor.world.pos.x, this->actor.world.pos.y,
                                                     this->actor.world.pos.z - 70.0f, 0, 0, 0, this->prizeIndex);
        if (this->exItem != NULL) {
            this->actionFunc = mode_item_get_demo;
        }
    }
}

void mode_item_get_demo(EnBomBowlPit* this, PlayState* play) {
    if (this->exItemDone != 0) {
        switch (this->prizeIndex) {
            case EXITEM_BOMB_BAG_BOWLING:
                SET_ITEMGETINF(ITEMGETINF_11);
                break;
            case EXITEM_HEART_PIECE_BOWLING:
                SET_ITEMGETINF(ITEMGETINF_12);
                break;
        }

        Gama_play_clear_camera(play, this->subCamId);
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_item_get_init;
    }
}

void mode_item_get_init(EnBomBowlPit* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
    this->getItemId = Bowl_Item_No_Data[this->prizeIndex];

    if ((this->getItemId == GI_BOMB_BAG_30) && (CUR_CAPACITY(UPG_BOMB_BAG) == 30)) {
        this->getItemId = GI_BOMB_BAG_40;
    }

    player->stateFlags1 &= ~PLAYER_STATE1_29;
    this->actor.parent = NULL;
    Actor_carry_request_set2(&this->actor, play, this->getItemId, 2000.0f, 1000.0f);
    player->stateFlags1 |= PLAYER_STATE1_29;
    this->actionFunc = mode_player_item_request;
}

static void mode_player_item_request(EnBomBowlPit* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_player_item_up;
    } else {
        Actor_carry_request_set2(&this->actor, play, this->getItemId, 2000.0f, 1000.0f);
    }
}

static void mode_player_item_up(EnBomBowlPit* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        // "Normal termination"/"completion"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        if (this->getItemId == GI_HEART_PIECE) {
            z_common_data.healthAccumulator = 0x140;
            // "Ah recovery!" (?)
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ あぁ回復！ ☆☆☆☆☆ \n" VT_RST);
        }
        this->exItemDone = 0;
        this->status = 2;
        this->actionFunc = mode_wait;
    }
}

void En_Bom_Bowl_Pit_actor_move(Actor* thisx, PlayState* play) {
    EnBomBowlPit* this = (EnBomBowlPit*)thisx;

    this->actionFunc(this, play);

    if (this->timer != 0) {
        this->timer--;
    }
}
