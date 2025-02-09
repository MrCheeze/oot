/*
 * File: z_en_guest.c
 * Overlay: ovl_En_Guest
 * Description: Happy Mask Shop Customer
 */

#include "z_en_guest.h"
#include "assets/objects/object_os_anime/object_os_anime.h"
#include "assets/objects/object_boj/object_boj.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Guest_actor_ct(Actor* thisx, PlayState* play);
void En_Guest_actor_dt(Actor* thisx, PlayState* play);
void En_Guest_actor_init(Actor* thisx, PlayState* play);
void En_Guest_actor_draw(Actor* thisx, PlayState* play);

void Guest_TalkWait(EnGuest* this, PlayState* play);
void Guest_Talk(EnGuest* this, PlayState* play);
void En_Guest_actor_move(Actor* thisx, PlayState* play);

ActorProfile En_Guest_Profile = {
    /**/ ACTOR_EN_GUEST,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BOJ,
    /**/ sizeof(EnGuest),
    /**/ En_Guest_actor_ct,
    /**/ En_Guest_actor_dt,
    /**/ En_Guest_actor_init,
    /**/ NULL,
};

static ColliderCylinderInitType1 GuestPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 10, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void En_Guest_actor_ct(Actor* thisx, PlayState* play) {
    EnGuest* this = (EnGuest*)thisx;

    if (GET_INFTABLE(INFTABLE_76)) {
        Actor_delete(&this->actor);
    } else {
        this->osAnimeObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_OS_ANIME);
        if (this->osAnimeObjectSlot < 0) {
            PRINTF_COLOR_ERROR();
            // "No such bank!!"
            PRINTF("%s[%d] : バンクが無いよ！！\n", "../z_en_guest.c", 129);
            PRINTF_RST();
            ASSERT(0, "0", "../z_en_guest.c", 132);
        }
    }
}

void En_Guest_actor_dt(Actor* thisx, PlayState* play) {
    EnGuest* this = (EnGuest*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_Guest_actor_init(Actor* thisx, PlayState* play) {
    EnGuest* this = (EnGuest*)thisx;
    s32 pad;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->osAnimeObjectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ValueSet_process(&this->actor, value_init);

        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gHylianMan2Skel, NULL, this->jointTable, this->morphTable, 16);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->osAnimeObjectSlot].segment);
        Skeleton_Info2_init(&this->skelAnime, &gObjOsAnim_42AC, 1.0f, 0.0f, Si2_anime_end_frame(&gObjOsAnim_42AC),
                         ANIMMODE_LOOP, 0.0f);

        this->actor.draw = En_Guest_actor_draw;
        this->actor.update = En_Guest_actor_move;

        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set3(play, &this->collider, &this->actor, &GuestPipeData);

        Actor_world_to_eye(&this->actor, 60.0f);

        this->unk_30E = 0;
        this->unk_30D = 0;
        this->unk_2CA = 0;
        this->actor.textId = 0x700D;
        this->actionFunc = Guest_TalkWait;
    }
}

void Guest_EyesProc(EnGuest* this) {
    if (this->unk_30D == 0) {
        if (this->unk_2CA != 0) {
            this->unk_2CA--;
        } else {
            this->unk_30D = 1;
        }
    } else {
        if (this->unk_2CA != 0) {
            this->unk_2CA--;
        } else {
            this->unk_30E++;
            if (this->unk_30E >= 3) {
                this->unk_30E = 0;
                this->unk_30D = 0;
                this->unk_2CA = (s32)rnd_f(60.0f) + 20;
            } else {
                this->unk_2CA = 1;
            }
        }
    }
}

void Guest_TalkWait(EnGuest* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = Guest_Talk;
    } else if (this->actor.xzDistToPlayer < 100.0f) {
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
}

void Guest_Talk(EnGuest* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        this->actionFunc = Guest_TalkWait;
    }
}

void En_Guest_actor_move(Actor* thisx, PlayState* play) {
    EnGuest* this = (EnGuest*)thisx;
    s32 pad;
    Player* player;

    player = GET_PLAYER(play);
    this->unk_2C8++;

    Guest_EyesProc(this);
    this->actionFunc(this, play);

    this->interactInfo.trackPos = player->actor.world.pos;
    if (LINK_IS_ADULT) {
        this->interactInfo.yOffset = 10.0f;
    } else {
        this->interactInfo.yOffset = 20.0f;
    }
    eye_moveM(&this->actor, &this->interactInfo, 6, NPC_TRACKING_HEAD_AND_TORSO);

    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->osAnimeObjectSlot].segment);

    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_world_to_eye(&this->actor, 60.0f);

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

static Gfx* pa(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dlist;

    dlist = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    gDPSetEnvColor(dlist, r, g, b, a);
    gSPEndDisplayList(dlist + 1);

    return dlist;
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGuest* this = (EnGuest*)thisx;
    Vec3s limbRot;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_guest.c", 352);

    if (limbIndex == 15) {
        *dList = gHylianMan2BeardedHeadDL;
        Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }

    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 12) {
        rot->y += sin_s(this->fidgetTableY[limbIndex]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limbIndex]) * FIDGET_AMPLITUDE;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_guest.c", 388);

    return false;
}

void En_Guest_actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gHylianMan2MustachedEyeOpenTex,
        gHylianMan2MustachedEyeHalfTex,
        gHylianMan2MustachedEyeClosedTex,
    };
    EnGuest* this = (EnGuest*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_guest.c", 404);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, pa(play->state.gfxCtx, 0xFF, 0xFF, 0xFF, 0xFF));
    gSPSegment(POLY_OPA_DISP++, 0x09, pa(play->state.gfxCtx, 0xA0, 0x3C, 0xDC, 0xFF));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(eye_txt[this->unk_30E]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_guest.c", 421);
}
