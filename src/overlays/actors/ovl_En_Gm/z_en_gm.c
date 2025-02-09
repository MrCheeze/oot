/*
 * File: z_en_gm.c
 * Overlay: ovl_En_Gm
 * Description: Medi-Goron
 */

#include "z_en_gm.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"
#include "assets/objects/object_gm/object_gm.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Gm_actor_ct(Actor* thisx, PlayState* play);
void En_Gm_actor_dt(Actor* thisx, PlayState* play);
void En_Gm_actor_move(Actor* thisx, PlayState* play);
void En_Gm_actor_draw(Actor* thisx, PlayState* play);

void Gm_init(EnGm* this, PlayState* play);
void Gm_move(EnGm* this, PlayState* play);
void Gm_wall_wait(EnGm* this, PlayState* play);
void Gm_talk_wait(EnGm* this, PlayState* play);
void Gm_wall_talk_start(EnGm* this, PlayState* play);
void Gm_talk_start(EnGm* this, PlayState* play);
void Gm_talk2_start(EnGm* this, PlayState* play);
void Gm_carry_wait(EnGm* this, PlayState* play);
void Gm_carry_start(EnGm* this, PlayState* play);

ActorProfile En_Gm_Profile = {
    /**/ ACTOR_EN_GM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnGm),
    /**/ En_Gm_actor_ct,
    /**/ En_Gm_actor_dt,
    /**/ En_Gm_actor_move,
    /**/ NULL,
};

static ColliderCylinderInitType1 GmPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 100, 120, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void En_Gm_actor_ct(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    ValueSet_process(&this->actor, value_init);

    // "Medi Goron"
    PRINTF(VT_FGCOL(GREEN) "%s[%d] : 中ゴロン[%d]" VT_RST "\n", "../z_en_gm.c", 133, this->actor.params);

    this->gmObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GM);

    if (this->gmObjectSlot < 0) {
        PRINTF_COLOR_ERROR();
        // "There is no model bank! !! (Medi Goron)"
        PRINTF("モデル バンクが無いよ！！（中ゴロン）\n");
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_gm.c", 145);
    }

    this->updateFunc = Gm_init;
}

void En_Gm_actor_dt(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 Gm_Player_Check(void) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        return 0;
    } else if (!CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON)) {
        return 1;
    } else if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)) {
        return 2;
    } else {
        return 3;
    }
}

void Gm_init(EnGm* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->gmObjectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable, 18);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->gmObjectSlot].segment);
        Skeleton_Info2_init(&this->skelAnime, &object_gm_Anim_0002B8, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_gm_Anim_0002B8), ANIMMODE_LOOP, 0.0f);
        this->actor.draw = En_Gm_actor_draw;
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set3(play, &this->collider, &this->actor, &GmPipeData);
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 35.0f);
        Actor_set_scale(&this->actor, 0.05f);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->eyeTexIndex = 0;
        this->blinkTimer = 20;
        this->actor.textId = 0x3049;
        this->updateFunc = Gm_move;
        this->actionFunc = Gm_wall_wait;
        this->actor.speed = 0.0f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 0.0f;
    }
}

void Gm_wink(EnGm* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else {
        this->eyeTexIndex++;

        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
            this->blinkTimer = rnd_f(60.0f) + 20.0f;
        }
    }
}

void Gm_message_set(EnGm* this) {
    switch (Gm_Player_Check()) {
        case 0:
            if (GET_INFTABLE(INFTABLE_B0)) {
                this->actor.textId = 0x304B;
            } else {
                this->actor.textId = 0x304A;
            }
            break;
        case 1:
            if (GET_INFTABLE(INFTABLE_B1)) {
                this->actor.textId = 0x304F;
            } else {
                this->actor.textId = 0x304C;
            }
            break;
        case 2:
            this->actor.textId = 0x304E;
            break;
        case 3:
            this->actor.textId = 0x304D;
            break;
    }
}

void Gm_wall_wait(EnGm* this, PlayState* play) {
    f32 dx;
    f32 dz;
    Player* player = GET_PLAYER(play);

    dx = this->talkPos.x - player->actor.world.pos.x;
    dz = this->talkPos.z - player->actor.world.pos.z;

    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        Gm_message_set(this);
        this->actionFunc = Gm_talk_wait;
    } else if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = Gm_wall_talk_start;
    } else if ((this->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_talk_request2(&this->actor, play, 415.0f);
    }
}

void Gm_wall_talk_start(EnGm* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        this->actionFunc = Gm_wall_wait;
    }
}

void Gm_talk_wait(EnGm* this, PlayState* play) {
    f32 dx;
    f32 dz;
    s32 pad;
    Player* player = GET_PLAYER(play);

    Gm_message_set(this);

    dx = this->talkPos.x - player->actor.world.pos.x;
    dz = this->talkPos.z - player->actor.world.pos.z;

    if (Actor_talk_check(&this->actor, play)) {
        switch (Gm_Player_Check()) {
            case 0:
                SET_INFTABLE(INFTABLE_B0);
                FALLTHROUGH;
            case 3:
                this->actionFunc = Gm_talk_start;
                return;
            case 1:
                SET_INFTABLE(INFTABLE_B1);
                FALLTHROUGH;
            case 2:
                this->actionFunc = Gm_talk2_start;
                FALLTHROUGH;
            default:
                return;
        }

        this->actionFunc = Gm_talk2_start;
    }
    if ((this->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_talk_request2(&this->actor, play, 415.0f);
    }
}

void Gm_talk_start(EnGm* this, PlayState* play) {
    u8 dialogState = message_check(&play->msgCtx);

    if ((dialogState == TEXT_STATE_DONE || dialogState == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actionFunc = Gm_talk_wait;
        if (dialogState == TEXT_STATE_EVENT) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
        }
    }
}

void Gm_talk2_start(EnGm* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (z_common_data.save.info.playerData.rupees < 200) {
                    message_set2(play, 0xC8);
                    this->actionFunc = Gm_talk_start;
                } else {
                    Actor_carry_request_set2(&this->actor, play, GI_SWORD_KNIFE, 415.0f, 10.0f);
                    this->actionFunc = Gm_carry_wait;
                }
                break;
            case 1: // no
                message_set2(play, 0x3050);
                this->actionFunc = Gm_talk_start;
                break;
        }
    }
}

void Gm_carry_wait(EnGm* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = Gm_carry_start;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_SWORD_KNIFE, 415.0f, 10.0f);
    }
}

void Gm_carry_start(EnGm* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        lupy_increase(-200);
        this->actionFunc = Gm_talk_wait;
    }
}

void Gm_move(EnGm* this, PlayState* play) {
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->gmObjectSlot].segment);
    this->timer++;
    this->actionFunc(this, play);
    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
    Gm_wink(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Gm_actor_move(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    this->updateFunc(this, play);
}

void Gm_cross(EnGm* this) {
    Vec3f vec1;
    Vec3f vec2;

    Matrix_push();
    Matrix_translate(0.0f, 0.0f, 2600.0f, MTXMODE_APPLY);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_Position(&vec1, &vec2);
    this->collider.dim.pos.x = vec2.x;
    this->collider.dim.pos.y = vec2.y;
    this->collider.dim.pos.z = vec2.z;
    Matrix_pull();
    Matrix_push();
    Matrix_translate(0.0f, 0.0f, 4300.0f, MTXMODE_APPLY);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_Position(&vec1, &this->talkPos);
    Matrix_pull();
    Matrix_translate(0.0f, 0.0f, 3800.0f, MTXMODE_APPLY);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_Position(&vec1, &this->actor.focus.pos);
    this->actor.focus.pos.y += 100.0f;
}

void En_Gm_actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    EnGm* this = (EnGm*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_gm.c", 613);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_gm.c", 629);

    Gm_cross(this);
}
