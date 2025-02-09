/*
 * File: z_en_pu_box.c
 * Overlay: ovl_En_Pu_Box
 * Description: An unused stone cube
 */

#include "z_en_pu_box.h"
#include "assets/objects/object_pu_box/object_pu_box.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_pubox_Actor_ct(Actor* thisx, PlayState* play);
void En_pubox_Actor_dt(Actor* thisx, PlayState* play);
void En_pubox_move(Actor* thisx, PlayState* play);
void En_pubox_display(Actor* thisx, PlayState* play);

ActorProfile En_Pu_box_Profile = {
    /**/ ACTOR_EN_PU_BOX,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_PU_BOX,
    /**/ sizeof(EnPubox),
    /**/ En_pubox_Actor_ct,
    /**/ En_pubox_Actor_dt,
    /**/ En_pubox_move,
    /**/ En_pubox_display,
};

void En_pubox_Actor_ct(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    EnPubox* this = (EnPubox*)thisx;

    switch (thisx->params) {
        case 0:
            Actor_set_scale(thisx, 0.0025f);
            break;
        case 1:
            Actor_set_scale(thisx, 0.005f);
            break;
        case 2:
            Actor_set_scale(thisx, 0.0075f);
            break;
        case 3:
            Actor_set_scale(thisx, 0.01f);
        default:
            break;
    }
    this->unk_164 = 1;
    thisx->colChkInfo.cylRadius = 20;
    thisx->colChkInfo.cylHeight = 50;
    thisx->cullingVolumeDownward = 1200.0f;
    thisx->cullingVolumeScale = 720.0f;
    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 6.0f);
    this->dyna.interactFlags = 0;
    this->dyna.transformFlags = 0;
    thisx->attentionRangeType = ATTENTION_RANGE_1;
    thisx->gravity = -2.0f;
    DynaPolyUty_bgdi_SG2KSG(&gBlockMediumCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void En_pubox_Actor_dt(Actor* thisx, PlayState* play) {
    EnPubox* this = (EnPubox*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void En_pubox_move(Actor* thisx, PlayState* play) {
    EnPubox* this = (EnPubox*)thisx;

    thisx->speed += this->dyna.unk_150;
    thisx->world.rot.y = this->dyna.unk_158;
    thisx->speed = CLAMP(thisx->speed, -2.5f, 2.5f);
    add_calc(&thisx->speed, 0.0f, 1.0f, 1.0f, 0.0f);
    if (thisx->speed != 0.0f) {
        Nai_FxFlagEntry(NA_SE_EV_ROCK_SLIDE - SFX_FLAG, &thisx->projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    this->dyna.unk_154 = 0.0f;
    this->dyna.unk_150 = 0.0f;
    Actor_position_moveF(thisx);
    Actor_BGcheck2(
        play, thisx, thisx->colChkInfo.cylHeight, thisx->colChkInfo.cylRadius, thisx->colChkInfo.cylRadius,
        UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    thisx->focus.pos = thisx->world.pos;
}

void En_pubox_display(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gBlockMediumDL);
}
