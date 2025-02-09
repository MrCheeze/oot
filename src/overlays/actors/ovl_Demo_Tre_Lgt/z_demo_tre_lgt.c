#include "z_demo_tre_lgt.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "assets/objects/object_box/object_box.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Demo_Tre_Lgt_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Tre_Lgt_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Tre_Lgt_Actor_move(Actor* thisx, PlayState* play);
void Demo_Tre_Lgt_Actor_display(Actor* thisx, PlayState* play);

void Demo_Tre_Lgt_anim(DemoTreLgt* this, PlayState* play);
void Demo_Tre_Lgt_wait_init(DemoTreLgt* this);
void Demo_Tre_Lgt_wait(DemoTreLgt* this, PlayState* play);
void Demo_Tre_Lgt_anim_init(DemoTreLgt* this, PlayState* play, f32 currentFrame);

typedef struct DemoTreLgtInfo {
    /* 0x00 */ f32 startFrame;
    /* 0x04 */ f32 endFrame;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
} DemoTreLgtInfo; // size = 0x10

static DemoTreLgtInfo TreLgtInfo[] = {
    { 1.0f, 136.0f, 190.0f, 40.0f },
    { 1.0f, 136.0f, 220.0f, 50.0f },
};

ActorProfile Demo_Tre_Lgt_Profile = {
    /**/ ACTOR_DEMO_TRE_LGT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_BOX,
    /**/ sizeof(DemoTreLgt),
    /**/ Demo_Tre_Lgt_Actor_ct,
    /**/ Demo_Tre_Lgt_Actor_dt,
    /**/ Demo_Tre_Lgt_Actor_move,
    /**/ Demo_Tre_Lgt_Actor_display,
};

static CurveAnimationHeader* anim_tbl[] = { &gTreasureChestCurveAnim_4B60, &gTreasureChestCurveAnim_4F70 };

static DemoTreLgtActionFunc mode_func[] = {
    Demo_Tre_Lgt_wait,
    Demo_Tre_Lgt_anim,
};

void Demo_Tre_Lgt_Actor_ct(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    if (!FcSkeletonInfo_ct(play, &this->skelCurve, &gTreasureChestCurveSkel, anim_tbl[0])) {
        // "Demo_Tre_Lgt_Actor_ct (); Construct failed"
        PRINTF("Demo_Tre_Lgt_Actor_ct();コンストラクト失敗\n");
    }

    ASSERT(true, "1", "../z_demo_tre_lgt.c", UNK_LINE);

    this->unk_170 = 255;
    this->unk_174 = 255;
    this->status = 0;
    Demo_Tre_Lgt_wait_init(this);
}

void Demo_Tre_Lgt_Actor_dt(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    FcSkeletonInfo_dt(play, &this->skelCurve);
}

void Demo_Tre_Lgt_wait_init(DemoTreLgt* this) {
    this->action = DEMO_TRE_LGT_ACTION_WAIT;
}

void Demo_Tre_Lgt_wait(DemoTreLgt* this, PlayState* play) {
    EnBox* treasureChest = (EnBox*)this->actor.parent;

    if ((treasureChest != NULL) && Skeleton_Info_frame_check(&treasureChest->skelanime, 10.0f)) {
        Demo_Tre_Lgt_anim_init(this, play, treasureChest->skelanime.curFrame);
    }
}

void Demo_Tre_Lgt_anim_init(DemoTreLgt* this, PlayState* play, f32 currentFrame) {
    SkelCurve* skelCurve = &this->skelCurve;
    s32 pad[2];

    this->action = DEMO_TRE_LGT_ACTION_ANIMATE;

    FcSkeletonInfo_init(skelCurve, anim_tbl[z_common_data.save.linkAge], 1.0f,
                      TreLgtInfo[z_common_data.save.linkAge].endFrame +
                          TreLgtInfo[z_common_data.save.linkAge].unk_08,
                      currentFrame, 1.0f);
    FcSkeletonInfo_play(play, skelCurve);
}

void Demo_Tre_Lgt_anim(DemoTreLgt* this, PlayState* play) {
    f32 currentFrame = this->skelCurve.curFrame;

    if (currentFrame < TreLgtInfo[((void)0, z_common_data.save.linkAge)].endFrame) {
        this->unk_170 = 255;
    } else {
        if (currentFrame <= (TreLgtInfo[((void)0, z_common_data.save.linkAge)].endFrame +
                             TreLgtInfo[((void)0, z_common_data.save.linkAge)].unk_08)) {
            this->unk_170 = ((((TreLgtInfo[((void)0, z_common_data.save.linkAge)].endFrame - currentFrame) /
                               TreLgtInfo[((void)0, z_common_data.save.linkAge)].unk_08) *
                              255.0f) +
                             255.0f);
        } else {
            this->unk_170 = 0;
        }
    }
    if (currentFrame < TreLgtInfo[((void)0, z_common_data.save.linkAge)].unk_0C) {
        this->unk_174 = 255;
    } else if (currentFrame < (TreLgtInfo[((void)0, z_common_data.save.linkAge)].unk_0C + 10.0f)) {
        this->unk_174 =
            ((((TreLgtInfo[((void)0, z_common_data.save.linkAge)].unk_0C - currentFrame) / 10.0f) * 255.0f) +
             255.0f);
    } else {
        this->unk_174 = 0;
    }
    if ((currentFrame > 30.0f) && !(this->status & 1)) {
        this->status |= 1;
        Nai_FxFlagEntry(NA_SE_EV_TRE_BOX_FLASH, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    if (FcSkeletonInfo_play(play, &this->skelCurve)) {
        Actor_delete(&this->actor);
    }
}

void Demo_Tre_Lgt_Actor_move(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    mode_func[this->action](this, play);
}

s32 Demo_Tre_Lgt_Actor_disp_proc(PlayState* play, SkelCurve* skelCurve, s32 limbIndex, void* thisx) {
    s32 pad;
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_tre_lgt.c", 423);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->state.frames * 2) % 256, 0, 64, 32, 1,
                                (play->state.frames * -2) % 256, 0, 64, 32));

    if (limbIndex == 1) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 180, this->unk_170);
    } else if ((limbIndex == 13) || (limbIndex == 7) || (limbIndex == 4) || (limbIndex == 10)) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 180, this->unk_174);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_tre_lgt.c", 448);

    //! @bug missing return
    //! If the returned value (i.e. the contents of v0) ends up being false (0), the limb won't draw. Therefore what
    //! matters is what was last written to v0 before the end of the function.
    //! - In debug versions, the last instruction that does this is in `Graph_CloseDisps`.
    //! - In retail versions, `gDPSetPrimColor` writes to it last.
    //! In both cases, that instruction sets v0 to a non-NULL pointer, which is "true", so the limb happens to be drawn.
#ifdef AVOID_UB
    return true;
#endif
}

void Demo_Tre_Lgt_Actor_display(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    OPEN_DISPS(gfxCtx, "../z_demo_tre_lgt.c", 461);

    if (this->action != DEMO_TRE_LGT_ACTION_ANIMATE) {
        return;
    }

    _texture_z_light_fog_prim_xlu(gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 0);
    FcSkeletonInfo_draw(&this->actor, play, &this->skelCurve, Demo_Tre_Lgt_Actor_disp_proc, NULL, 1, &this->actor);

    CLOSE_DISPS(gfxCtx, "../z_demo_tre_lgt.c", 476);
}
