/*
 * File: z_en_lightbox.c
 * Overlay: ovl_En_Lightbox
 * Description:
 */

#include "z_en_lightbox.h"
#include "assets/objects/object_lightbox/object_lightbox.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_lightbox_Actor_ct(Actor* thisx, PlayState* play);
void En_lightbox_Actor_dt(Actor* thisx, PlayState* play);
void En_lightbox_move(Actor* thisx, PlayState* play);
void En_lightbox_display(Actor* thisx, PlayState* play);

ActorProfile En_Lightbox_Profile = {
    /**/ ACTOR_EN_LIGHTBOX,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_LIGHTBOX,
    /**/ sizeof(EnLightbox),
    /**/ En_lightbox_Actor_ct,
    /**/ En_lightbox_Actor_dt,
    /**/ En_lightbox_move,
    /**/ En_lightbox_display,
};

void En_lightbox_Actor_ct(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    EnLightbox* this = (EnLightbox*)thisx;
    s32 pad[4];

    switch (thisx->params) {
        case 0:
            Actor_set_scale(thisx, 0.025f);
            break;
        case 1:
            Actor_set_scale(thisx, 0.05f);
            break;
        case 2:
            Actor_set_scale(thisx, 0.075f);
            break;
        case 3:
            Actor_set_scale(thisx, 0.1f);
        default:
            break;
    }

    thisx->focus.pos = thisx->world.pos;
    thisx->colChkInfo.cylRadius = 30;
    thisx->colChkInfo.cylHeight = 50;
    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 6.0f);
    this->dyna.interactFlags = 0;
    this->dyna.transformFlags = 0;
    thisx->attentionRangeType = ATTENTION_RANGE_0;
    thisx->gravity = -2.0f;
    DynaPolyUty_bgdi_SG2KSG(&object_lightbox_Col_001F10, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void En_lightbox_Actor_dt(Actor* thisx, PlayState* play) {
    EnLightbox* this = (EnLightbox*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void En_lightbox_move(Actor* thisx, PlayState* play) {
    EnLightbox* this = (EnLightbox*)thisx;

    if (this->dyna.unk_162 != 0) {
        if (Actor_carry_end_check(thisx, play)) {
            this->dyna.unk_162 = 0;
        }
    } else {
        if (Actor_carry_check(thisx, play)) {
            this->dyna.unk_162++;
        } else {
            if (thisx->speed) {
                if (thisx->bgCheckFlags & BGCHECKFLAG_WALL) {
                    thisx->world.rot.y = (thisx->world.rot.y + thisx->wallYaw) - thisx->world.rot.y;
                    Nai_FxFlagEntry(NA_SE_EV_BOMB_BOUND, &thisx->projectedPos, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    thisx->speed *= 0.7f;
                    thisx->bgCheckFlags &= ~BGCHECKFLAG_WALL;
                }
            }

            if (!(thisx->bgCheckFlags & BGCHECKFLAG_GROUND)) {
                chase_f(&thisx->speed, 0, IREG(57) / 100.0f);
            } else {
                chase_f(&thisx->speed, 0, IREG(58) / 100.0f);
                if ((thisx->bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) && (thisx->velocity.y < IREG(59) / 100.0f)) {
                    Nai_FxFlagEntry(NA_SE_EV_BOMB_BOUND, &thisx->projectedPos, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    thisx->velocity.y *= IREG(60) / 100.0f;
                    thisx->bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                } else {
                    Actor_carry_request(thisx, play);
                }
            }
        }
    }
    Actor_position_moveF(thisx);
    Actor_BGcheck2(
        play, thisx, thisx->colChkInfo.cylHeight, thisx->colChkInfo.cylRadius, thisx->colChkInfo.cylRadius,
        UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    thisx->focus.pos = thisx->world.pos;
}

void En_lightbox_display(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, object_lightbox_DL_000B70);
}
