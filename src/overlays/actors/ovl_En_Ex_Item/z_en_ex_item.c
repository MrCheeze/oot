/*
 * File: z_en_ex_item.c
 * Overlay: ovl_En_Ex_Item
 * Description: Minigame prize items
 */

#include "z_en_ex_item.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"

#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "segmented_address.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64draw.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Ex_Item_actor_ct(Actor* thisx, PlayState* play);
void En_Ex_Item_actor_dt(Actor* thisx, PlayState* play);
void En_Ex_Item_actor_move(Actor* thisx, PlayState* play);
void En_Ex_Item_actor_disp(Actor* thisx, PlayState* play);

void polygon_nomal_draw(EnExItem* this, PlayState* play);
void haert_piece_draw(EnExItem* this, PlayState* play);
void ruppy_draw(EnExItem* this, PlayState* play);
void ex_txt_draw(EnExItem* this, PlayState* play, s32 index);
void magic_stone_draw(EnExItem* this, PlayState* play, s16 magicIndex);

static void mode_bank_chenge_wait(EnExItem* this, PlayState* play);
void mode_get_item_of_bowl(EnExItem* this, PlayState* play);
void mode_show_up_of_bowl_init(EnExItem* this, PlayState* play);
void mode_show_up_of_bowl(EnExItem* this, PlayState* play);
void mode_takara_up_item(EnExItem* this, PlayState* play);
static void mode_no_move(EnExItem* this, PlayState* play);
void mode_get_item_of_mato(EnExItem* this, PlayState* play);
static void mode_player_item_request(EnExItem* this, PlayState* play);
static void mode_player_item_up(EnExItem* this, PlayState* play);

ActorProfile En_Ex_Item_Profile = {
    /**/ ACTOR_EN_EX_ITEM,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnExItem),
    /**/ En_Ex_Item_actor_ct,
    /**/ En_Ex_Item_actor_dt,
    /**/ En_Ex_Item_actor_move,
    /**/ En_Ex_Item_actor_disp,
};

void En_Ex_Item_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Ex_Item_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->type = PARAMS_GET_U(this->actor.params, 0, 8);
    this->unusedParam = PARAMS_GET_U(this->actor.params, 8, 8);
    PRINTF("\n\n");
    // "What will come out?"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, this->type);
    // "What will come out?"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, this->unusedParam);
    this->initPos = this->actor.world.pos;
    this->getItemObjectId = -1;
    switch (this->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
            this->getItemObjectId = OBJECT_GI_BOMBPOUCH;
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            this->getItemObjectId = OBJECT_GI_HEARTS;
            break;
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBCHUS_COUNTER:
            this->getItemObjectId = OBJECT_GI_BOMB_2;
            break;
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMBS_COUNTER:
            this->getItemObjectId = OBJECT_GI_BOMB_1;
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            this->getItemObjectId = OBJECT_GI_RUPY;
            break;
        case EXITEM_SMALL_KEY_CHEST:
            this->scale = 0.05f;
            this->actor.velocity.y = 10.0f;
            this->timer = 7;
            this->actionFunc = mode_takara_up_item;
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            this->getItemObjectId = OBJECT_GI_GODDESS;
            break;
        case EXITEM_BULLET_BAG:
            this->getItemObjectId = OBJECT_GI_DEKUPOUCH;
    }

    if (this->getItemObjectId >= 0) {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, this->getItemObjectId);
        this->actor.draw = NULL;
        if (this->requiredObjectSlot < 0) {
            Actor_delete(&this->actor);
            // "What?"
            PRINTF("なにみの？ %d\n", this->actor.params);
            // "bank is funny"
            PRINTF(VT_FGCOL(MAGENTA) " バンクおかしいしぞ！%d\n" VT_RST "\n", this->actor.params);
            return;
        }
        this->actionFunc = mode_bank_chenge_wait;
    }
}

static void mode_bank_chenge_wait(EnExItem* this, PlayState* play) {
    s32 onCounter;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        // "End of transfer"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(BLUE) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n\n" VT_RST, this->actor.params);
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.draw = En_Ex_Item_actor_disp;
        this->stopRotate = false;
        onCounter = false;
        switch (this->type) {
            case EXITEM_BOMB_BAG_COUNTER:
                onCounter = true;
                FALLTHROUGH;
            case EXITEM_BOMB_BAG_BOWLING:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->giDrawId = GID_BOMB_BAG_30;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (onCounter == 0) {
                    this->actionFunc = mode_get_item_of_bowl;
                } else {
                    this->actionFunc = mode_show_up_of_bowl_init;
                    this->actor.shape.yOffset = -18.0f;
                }
                break;
            case EXITEM_HEART_PIECE_COUNTER:
                onCounter = true;
                FALLTHROUGH;
            case EXITEM_HEART_PIECE_BOWLING:
                this->unk_17C = Actor_HiliteReflect_xlu_set_init;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (!onCounter) {
                    Na_StartSystemSe_F(NA_SE_SY_PIECE_OF_HEART);
                    this->actionFunc = mode_get_item_of_bowl;
                } else {
                    this->actionFunc = mode_show_up_of_bowl_init;
                    this->actor.shape.yOffset = -10.0f;
                }
                break;
            case EXITEM_BOMBCHUS_COUNTER:
                onCounter = true;
                FALLTHROUGH;
            case EXITEM_BOMBCHUS_BOWLING:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->giDrawId = GID_BOMBCHU;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (!onCounter) {
                    this->actionFunc = mode_get_item_of_bowl;
                } else {
                    this->actionFunc = mode_show_up_of_bowl_init;
                }
                break;
            case EXITEM_BOMBS_BOWLING:
            case EXITEM_BOMBS_COUNTER:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->giDrawId = GID_BOMB;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                if (this->type == EXITEM_BOMBS_BOWLING) {
                    this->actionFunc = mode_get_item_of_bowl;
                } else {
                    this->actionFunc = mode_show_up_of_bowl_init;
                    this->actor.shape.yOffset = 10.0f;
                }
                break;
            case EXITEM_PURPLE_RUPEE_BOWLING:
            case EXITEM_PURPLE_RUPEE_COUNTER:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->unk_180 = Actor_HiliteReflect_xlu_set_init;
                this->giDrawId = GID_RUPEE_PURPLE;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                if (this->type == EXITEM_PURPLE_RUPEE_BOWLING) {
                    this->actionFunc = mode_get_item_of_bowl;
                } else {
                    this->actionFunc = mode_show_up_of_bowl_init;
                    this->actor.shape.yOffset = 40.0f;
                }
                break;
            case EXITEM_GREEN_RUPEE_CHEST:
            case EXITEM_BLUE_RUPEE_CHEST:
            case EXITEM_RED_RUPEE_CHEST:
            case EXITEM_13:
            case EXITEM_14:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->unk_180 = Actor_HiliteReflect_xlu_set_init;
                this->timer = 7;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                this->actor.velocity.y = 10.0f;
                switch (this->type) {
                    case EXITEM_GREEN_RUPEE_CHEST:
                        this->giDrawId = GID_RUPEE_GREEN;
                        break;
                    case EXITEM_BLUE_RUPEE_CHEST:
                        this->giDrawId = GID_RUPEE_BLUE;
                        break;
                    case EXITEM_RED_RUPEE_CHEST:
                        this->giDrawId = GID_RUPEE_RED;
                        break;
                    case EXITEM_14:
                        this->giDrawId = GID_RUPEE_PURPLE;
                        break;
                }
                this->actionFunc = mode_takara_up_item;
                break;
            case EXITEM_MAGIC_FIRE:
            case EXITEM_MAGIC_WIND:
            case EXITEM_MAGIC_DARK:
                this->scale = 0.35f;
                this->actionFunc = mode_no_move;
                break;
            case EXITEM_BULLET_BAG:
                this->unk_17C = Actor_HiliteReflect_set_init;
                this->giDrawId = GID_BULLET_BAG;
                this->scale = 0.1f;
                this->timer = 80;
                this->prizeRotateTimer = 35;
                this->actionFunc = mode_get_item_of_mato;
                break;
            case EXITEM_SMALL_KEY_CHEST:
                break;
        }
    }
}

void mode_get_item_of_bowl(EnExItem* this, PlayState* play) {
    s32 pad;
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;
    f32 sp3C;

    if (!this->stopRotate) {
        this->actor.shape.rot.y += 0x1000;
        if ((this->prizeRotateTimer == 0) && ((this->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            this->stopRotate++;
        }
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, 0, 5, 0x1000, 0);
    }
    if (this->timer != 0) {
        if (this->prizeRotateTimer != 0) {
            sp3C = 250.0f;
            if (this->type == EXITEM_BOMBCHUS_BOWLING) {
                sp3C = 220.0f;
            }
            tmpf1 = play->view.at.x - play->view.eye.x;
            tmpf2 = play->view.at.y - play->view.eye.y;
            tmpf3 = play->view.at.z + sp3C - play->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = play->view.eye.x + tmpf5 - this->actor.world.pos.x;
            tmpf2 = play->view.eye.y + tmpf6 - this->actor.world.pos.y;
            tmpf3 = play->view.eye.z + tmpf7 - this->actor.world.pos.z;

            this->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            this->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            this->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        // "parent"
        PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 母親ー？     ☆☆☆☆☆ %x\n" VT_RST, this->actor.parent);
        // "Can it move?"
        PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 動いてねー？ ☆☆☆☆☆ %x\n" VT_RST, this->actor.parent->update);
        if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL)) {
            ((EnBomBowlPit*)this->actor.parent)->exItemDone = 1;
            // "It can't move!"
            PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ さぁきえるぞ！ ☆☆☆☆☆ \n" VT_RST);
        }
        Actor_delete(&this->actor);
    }
}

void mode_show_up_of_bowl_init(EnExItem* this, PlayState* play) {
    this->actor.world.rot.y = this->actor.shape.rot.y = 0x4268;
    this->actionFunc = mode_show_up_of_bowl;
}

void mode_show_up_of_bowl(EnExItem* this, PlayState* play) {
    this->actor.shape.rot.y += 0x800;
    if (this->killItem) {
        Actor_delete(&this->actor);
    }
}

void mode_takara_up_item(EnExItem* this, PlayState* play) {
    this->actor.shape.rot.y += 0x1000;
    if (this->timer != 0) {
        if (this->timer == 1) {
            this->chestKillTimer = 20;
        }
    } else {
        this->actor.velocity.y = 0.0f;
        if (this->chestKillTimer == 0) {
            Actor_delete(&this->actor);
        }
    }
    Actor_position_moveF(&this->actor);
}

static void mode_no_move(EnExItem* this, PlayState* play) {
    this->actor.shape.rot.y += 0x800;
}

void mode_get_item_of_mato(EnExItem* this, PlayState* play) {
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;

    add_calc2(&this->scale, 0.8f, 0.1f, 0.02f);
    if (!this->stopRotate) {
        this->actor.shape.rot.y += 0x1000;
        if ((this->prizeRotateTimer == 0) && ((this->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            this->stopRotate++;
        }
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, -0x4000, 5, 0x1000, 0);
    }

    if (this->timer != 0) {
        if (this->prizeRotateTimer != 0) {
            tmpf1 = play->view.at.x - play->view.eye.x;
            tmpf2 = play->view.at.y - 10.0f - play->view.eye.y;
            tmpf3 = play->view.at.z + 10.0f - play->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = play->view.eye.x + tmpf5 - this->actor.world.pos.x;
            tmpf2 = play->view.eye.y - 10.0f + tmpf6 - this->actor.world.pos.y;
            tmpf3 = play->view.eye.z + 10.0f + tmpf7 - this->actor.world.pos.z;

            this->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            this->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            this->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        s32 getItemId;

        this->actor.draw = NULL;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actor.parent = NULL;
        if (CUR_UPG_VALUE(UPG_BULLET_BAG) == 1) {
            getItemId = GI_BULLET_BAG_40;
        } else {
            getItemId = GI_BULLET_BAG_50;
        }
        Actor_carry_request_set2(&this->actor, play, getItemId, 2000.0f, 1000.0f);
        this->actionFunc = mode_player_item_request;
    }
}

static void mode_player_item_request(EnExItem* this, PlayState* play) {
    s32 getItemId;

    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_player_item_up;
    } else {
        getItemId = (CUR_UPG_VALUE(UPG_BULLET_BAG) == 2) ? GI_BULLET_BAG_50 : GI_BULLET_BAG_40;

        Actor_carry_request_set2(&this->actor, play, getItemId, 2000.0f, 1000.0f);
    }
}

static void mode_player_item_up(EnExItem* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        // "Successful completion"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        SET_ITEMGETINF(ITEMGETINF_1D);
        Actor_delete(&this->actor);
    }
}

void En_Ex_Item_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->chestKillTimer != 0) {
        this->chestKillTimer--;
    }
    if (this->prizeRotateTimer != 0) {
        this->prizeRotateTimer--;
    }
    this->actionFunc(this, play);
}

void En_Ex_Item_actor_disp(Actor* thisx, PlayState* play) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;
    s32 magicType;

    Actor_set_scale(&this->actor, this->scale);
    switch (this->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
        case EXITEM_BOMBCHUS_COUNTER:
        case EXITEM_BOMBS_COUNTER:
        case EXITEM_BULLET_BAG:
            polygon_nomal_draw(this, play);
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            haert_piece_draw(this, play);
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            ruppy_draw(this, play);
            break;
        case EXITEM_SMALL_KEY_CHEST:
            ex_txt_draw(this, play, 0);
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            magicType = this->type - EXITEM_MAGIC_FIRE;
            magic_stone_draw(this, play, magicType);
            break;
    }
}

void polygon_nomal_draw(EnExItem* this, PlayState* play) {
    if (this->unk_17C != NULL) {
        this->unk_17C(&this->actor, play, 0);
    }
    if (this) {}
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Draw_GetItemType(play, this->giDrawId);
}

void haert_piece_draw(EnExItem* this, PlayState* play) {
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Draw_GetItemType(play, GID_HEART_PIECE);
}

void magic_stone_draw(EnExItem* this, PlayState* play, s16 magicIndex) {
    static s16 stone_data[] = { GID_DINS_FIRE, GID_FARORES_WIND, GID_NAYRUS_LOVE };

    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Draw_GetItemType(play, stone_data[magicIndex]);
}

void ex_txt_draw(EnExItem* this, PlayState* play, s32 index) {
    static void* ex_txt_model[] = { gDropKeySmallTex };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ex_item.c", 880);

    softsprite(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ex_item.c", 887);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(ex_txt_model[index]));
    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ex_item.c", 893);
}

void ruppy_draw(EnExItem* this, PlayState* play) {
    if (this->unk_17C != NULL) {
        this->unk_17C(&this->actor, play, 0);
    }
    if (this->unk_180 != NULL) {
        this->unk_180(&this->actor, play, 0);
    }
    Draw_GetItemType(play, this->giDrawId);
}
