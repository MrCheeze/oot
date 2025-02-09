/*
 * File: z_demo_ec.c
 * Overlay: ovl_Demo_Ec
 * Description: Credits revelers in Lon Lon
 */

#include "z_demo_ec.h"
#include "terminal.h"
#include "assets/objects/object_zo/object_zo.h"
#include "assets/objects/object_ec/object_ec.h"
#include "assets/objects/object_ma2/object_ma2.h"
#include "assets/objects/object_in/object_in.h"
#include "assets/objects/object_ge1/object_ge1.h"
#include "assets/objects/object_fu/object_fu.h"
#include "assets/objects/object_fish/object_fish.h"
#include "assets/objects/object_ta/object_ta.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"
#include "assets/objects/object_ma2/object_ma2.h"
#include "assets/objects/object_in/object_in.h"
#include "assets/objects/object_ta/object_ta.h"
#include "assets/objects/object_fu/object_fu.h"
#include "assets/objects/object_toryo/object_toryo.h"
#include "assets/objects/object_daiku/object_daiku.h"
#include "assets/objects/object_ge1/object_ge1.h"
#include "assets/objects/object_kz/object_kz.h"
#include "assets/objects/object_md/object_md.h"
#include "assets/objects/object_niw/object_niw.h"
#include "assets/objects/object_ds2/object_ds2.h"
#include "assets/objects/object_os/object_os.h"
#include "assets/objects/object_rs/object_rs.h"
#include "assets/objects/object_gm/object_gm.h"
#include "assets/objects/object_km1/object_km1.h"
#include "assets/objects/object_kw1/object_kw1.h"
#include "assets/objects/object_bji/object_bji.h"
#include "assets/objects/object_ahg/object_ahg.h"
#include "assets/objects/object_bob/object_bob.h"
#include "assets/objects/object_bba/object_bba.h"
#include "assets/objects/object_ane/object_ane.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Demo_Ec_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Ec_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Ec_main(Actor* thisx, PlayState* play);
void Demo_Ec_draw(Actor* thisx, PlayState* play);

typedef enum DemoEcUpdateMode {
    /* 00 */ EC_UPDATE_COMMON,
    /* 01 */ EC_UPDATE_INGO,
    /* 02 */ EC_UPDATE_TALON,
    /* 03 */ EC_UPDATE_WINDMILL_MAN,
    /* 04 */ EC_UPDATE_KOKIRI_BOY,
    /* 05 */ EC_UPDATE_KOKIRI_GIRL,
    /* 06 */ EC_UPDATE_OLD_MAN,
    /* 07 */ EC_UPDATE_BEARDED_MAN,
    /* 08 */ EC_UPDATE_WOMAN,
    /* 09 */ EC_UPDATE_OLD_WOMAN,
    /* 10 */ EC_UPDATE_BOSS_CARPENTER,
    /* 11 */ EC_UPDATE_CARPENTER,
    /* 12 */ EC_UPDATE_DANCING_KOKIRI_BOY,
    /* 13 */ EC_UPDATE_DANCING_KOKIRI_GIRL,
    /* 14 */ EC_UPDATE_GERUDO,
    /* 15 */ EC_UPDATE_DANCING_ZORA,
    /* 16 */ EC_UPDATE_KING_ZORA,
    /* 17 */ EC_UPDATE_17,
    /* 18 */ EC_UPDATE_18,
    /* 19 */ EC_UPDATE_MIDO,
    /* 20 */ EC_UPDATE_20,
    /* 21 */ EC_UPDATE_CUCCO,
    /* 22 */ EC_UPDATE_CUCCO_LADY,
    /* 23 */ EC_UPDATE_POTION_SHOP_OWNER,
    /* 24 */ EC_UPDATE_MASK_SHOP_OWNER,
    /* 25 */ EC_UPDATE_FISHING_MAN,
    /* 26 */ EC_UPDATE_BOMBCHU_SHOP_OWNER,
    /* 27 */ EC_UPDATE_GORON,
    /* 28 */ EC_UPDATE_MALON
} DemoEcUpdateMode;

typedef enum DemoEcDrawconfig {
    /* 00 */ EC_DRAW_COMMON,
    /* 01 */ EC_DRAW_INGO,
    /* 02 */ EC_DRAW_TALON,
    /* 03 */ EC_DRAW_WINDMILL_MAN,
    /* 04 */ EC_DRAW_KOKIRI_BOY,
    /* 05 */ EC_DRAW_KOKIRI_GIRL,
    /* 06 */ EC_DRAW_OLD_MAN,
    /* 07 */ EC_DRAW_BEARDED_MAN,
    /* 08 */ EC_DRAW_WOMAN,
    /* 09 */ EC_DRAW_OLD_WOMAN,
    /* 10 */ EC_DRAW_BOSS_CARPENTER,
    /* 11 */ EC_DRAW_CARPENTER,
    /* 12 */ EC_DRAW_GERUDO,
    /* 13 */ EC_DRAW_DANCING_ZORA,
    /* 14 */ EC_DRAW_KING_ZORA,
    /* 15 */ EC_DRAW_MIDO,
    /* 16 */ EC_DRAW_CUCCO,
    /* 17 */ EC_DRAW_CUCCO_LADY,
    /* 18 */ EC_DRAW_POTION_SHOP_OWNER,
    /* 19 */ EC_DRAW_MASK_SHOP_OWNER,
    /* 20 */ EC_DRAW_FISHING_MAN,
    /* 21 */ EC_DRAW_BOMBCHU_SHOP_OWNER,
    /* 22 */ EC_DRAW_GORON,
    /* 23 */ EC_DRAW_MALON
} DemoEcDrawconfig;

static s16 Demo_Ec_Shape_Bank[] = {
    /*  0 */ OBJECT_IN,
    /*  1 */ OBJECT_TA,
    /*  2 */ OBJECT_FU,
    /*  3 */ OBJECT_KM1,
    /*  4 */ OBJECT_KW1,
    /*  5 */ OBJECT_BJI,
    /*  6 */ OBJECT_AHG,
    /*  7 */ OBJECT_BOB,
    /*  8 */ OBJECT_BBA,
    /*  9 */ OBJECT_TORYO,
    /* 10 */ OBJECT_DAIKU,
    /* 11 */ OBJECT_DAIKU,
    /* 12 */ OBJECT_DAIKU,
    /* 13 */ OBJECT_DAIKU,
    /* 14 */ OBJECT_KM1,
    /* 15 */ OBJECT_KW1,
    /* 16 */ OBJECT_GE1,
    /* 17 */ OBJECT_GE1,
    /* 18 */ OBJECT_GE1,
    /* 19 */ OBJECT_ZO,
    /* 20 */ OBJECT_KZ,
    /* 21 */ OBJECT_MD,
    /* 22 */ OBJECT_NIW,
    /* 23 */ OBJECT_NIW,
    /* 24 */ OBJECT_NIW,
    /* 25 */ OBJECT_ANE,
    /* 26 */ OBJECT_DS2,
    /* 27 */ OBJECT_OS,
    /* 28 */ OBJECT_FISH,
    /* 29 */ OBJECT_RS,
    /* 30 */ OBJECT_OF1D_MAP,
    /* 31 */ OBJECT_OF1D_MAP,
    /* 32 */ OBJECT_OF1D_MAP,
    /* 33 */ OBJECT_OF1D_MAP,
    /* 34 */ OBJECT_MA2,
};

static s16 Demo_Ec_Anime_Bank[] = {
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_GM, OBJECT_MA2,
};

void Demo_Ec_Actor_dt(Actor* thisx, PlayState* play) {
    DemoEc* this = (DemoEc*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void Demo_Ec_Actor_ct(Actor* thisx, PlayState* play) {
    DemoEc* this = (DemoEc*)thisx;

    if ((this->actor.params < 0) || (this->actor.params > 34)) {
        PRINTF(VT_FGCOL(RED) "Demo_Ec_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_delete(&this->actor);
    } else {
        this->updateMode = EC_UPDATE_COMMON;
        this->drawConfig = EC_DRAW_COMMON;
    }
}

s32 Demo_Ec_Animation_Base(DemoEc* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

void Demo_Ec_BGcheck(DemoEc* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 30.0f, 25.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
}

void Demo_Ec_Movement_byAnimation_CorrectNone(DemoEc* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void Demo_Ec_Start_Movement_byAnimation(DemoEc* this, PlayState* play) {
    this->skelAnime.baseTransl = this->skelAnime.jointTable[0];
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void Demo_Ec_Start2_Movement_byAnimation(DemoEc* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void Demo_Ec_set_eye_pattern(DemoEc* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyeTexIndex = *blinkTimer;

    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void Demo_Ec_set_eye_Num(DemoEc* this, s16 texIndex) {
    this->eyeTexIndex = texIndex;
}

void Demo_Ec_Setup_Mdl(DemoEc* this, PlayState* play, FlexSkeletonHeader* skeletonHeader) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, SEGMENTED_TO_VIRTUAL(skeletonHeader), NULL, NULL, NULL, 0);
}

void Demo_Ec_Change_Anime(DemoEc* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 reverse) {
    f32 frameCount;
    f32 startFrame;
    AnimationHeader* anim;
    f32 playbackSpeed;
    s16 frameCountS;

    anim = SEGMENTED_TO_VIRTUAL(animation);
    frameCountS = Si2_anime_end_frame(anim);

    if (!reverse) {
        startFrame = 0.0f;
        frameCount = frameCountS;
        playbackSpeed = 1.0f;
    } else {
        frameCount = 0.0f;
        startFrame = frameCountS;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, anim, playbackSpeed, startFrame, frameCount, mode, morphFrames);
}

Gfx* Demo_Ec_SetDrawColor(GraphicsContext* gfxCtx, u8* color) {
    Gfx* dList;

    dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 2);
    gDPSetEnvColor(dList, color[0], color[1], color[2], color[3]);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void Demo_Ec_draw_normal_1(DemoEc* this, PlayState* play, void* eyeTexture, void* arg3, OverrideLimbDraw overrideLimbDraw,
                         PostLimbDraw postLimbDraw) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 565);

    _texture_z_light_fog_prim(gfxCtx);

    if (eyeTexture != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    }

    if (arg3 != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(arg3));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, &this->actor, POLY_OPA_DISP);
    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 595);
}

void Demo_Ec_draw_normal_2(DemoEc* this, PlayState* play, Gfx* arg2, Gfx* arg3, u8* color1, u8* color2,
                                    OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 609);

    _texture_z_light_fog_prim(gfxCtx);

    if (arg2 != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(arg2));
    }

    if (arg3 != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x0B, SEGMENTED_TO_VIRTUAL(arg3));
    }

    if (color1 != NULL) {
        //! @bug Demo_Ec_SetDrawColor is called twice in SEGMENTED_TO_VIRTUAL, allocating two display lists
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(Demo_Ec_SetDrawColor(gfxCtx, color1)));
    }

    if (color2 != NULL) {
        //! @bug Demo_Ec_SetDrawColor is called twice in SEGMENTED_TO_VIRTUAL, allocating two display lists
        //! @bug meant to pass color2 instead of color1?
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(Demo_Ec_SetDrawColor(gfxCtx, color1)));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, &this->actor, POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 646);
}

void Demo_Ec_Change_ShapeBank(DemoEc* this, PlayState* play) {
    s32 pad[2];
    s32 drawObjectSlot = this->drawObjectSlot;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 662);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[drawObjectSlot].segment);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[drawObjectSlot].segment);
    if (!play) {}

    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 670);
}

void Demo_Ec_Change_AnimeBank(DemoEc* this, PlayState* play) {
    s32 animObjectSlot = this->animObjectSlot;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[animObjectSlot].segment);
}

CsCmdActorCue* Demo_Ec_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void Demo_Ec_Set_StartPos_npcdemopnt(DemoEc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Ec_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

#include "z_demo_ec_inIn.inc.c"

#include "z_demo_ec_inTa.inc.c"

#include "z_demo_ec_inFu.inc.c"

#include "z_demo_ec_inKm1.inc.c"

#include "z_demo_ec_inKw1.inc.c"

#include "z_demo_ec_inBji.inc.c"

#include "z_demo_ec_inAhg.inc.c"

#include "z_demo_ec_inBob.inc.c"

#include "z_demo_ec_inBba.inc.c"

#include "z_demo_ec_inDk1.inc.c"

#include "z_demo_ec_inDk2.inc.c"

#include "z_demo_ec_inGe1.inc.c"

#include "z_demo_ec_inZo.inc.c"

#include "z_demo_ec_inKz.inc.c"

#include "z_demo_ec_inMd.inc.c"

#include "z_demo_ec_inNiw.inc.c"

#include "z_demo_ec_inAne.inc.c"

#include "z_demo_ec_inDs2.inc.c"

#include "z_demo_ec_inOs.inc.c"

#include "z_demo_ec_inFs.inc.c"

#include "z_demo_ec_inRs.inc.c"

#include "z_demo_ec_inGo.inc.c"

#include "z_demo_ec_inMa2.inc.c"

void Demo_Ec_main_init(DemoEc* this, PlayState* play) {
    static DemoEcInitFunc proc[] = {
        /*  0 */ Demo_Ec_main_init_In,
        /*  1 */ Demo_Ec_main_init_Ta,
        /*  2 */ Demo_Ec_main_init_Fu,
        /*  3 */ Demo_Ec_main_init_Km1,
        /*  4 */ Demo_Ec_main_init_Kw1,
        /*  5 */ Demo_Ec_main_init_Bji,
        /*  6 */ Demo_Ec_main_init_Ahg,
        /*  7 */ Demo_Ec_main_init_Bob,
        /*  8 */ Demo_Ec_main_init_Bba,
        /*  9 */ Demo_Ec_main_init_Dk1,
        /* 10 */ Demo_Ec_main_init_Dk2,
        /* 11 */ Demo_Ec_main_init_Dk2,
        /* 12 */ Demo_Ec_main_init_Dk2,
        /* 13 */ Demo_Ec_main_init_Dk2,
        /* 14 */ Demo_Ec_main_init_Km1_dance,
        /* 15 */ Demo_Ec_main_init_Kw1_dance,
        /* 16 */ Demo_Ec_main_init_Ge1,
        /* 17 */ Demo_Ec_main_init_Ge1,
        /* 18 */ Demo_Ec_main_init_Ge1,
        /* 19 */ Demo_Ec_main_init_Zo,
        /* 20 */ Demo_Ec_main_init_Kz,
        /* 21 */ Demo_Ec_main_init_Md,
        /* 22 */ Demo_Ec_main_init_Niw,
        /* 23 */ Demo_Ec_main_init_Niw,
        /* 24 */ Demo_Ec_main_init_Niw,
        /* 25 */ Demo_Ec_main_init_Ane,
        /* 26 */ Demo_Ec_main_init_Ds2,
        /* 27 */ Demo_Ec_main_init_Os,
        /* 28 */ Demo_Ec_main_init_Fs,
        /* 29 */ Demo_Ec_main_init_Rs,
        /* 30 */ Demo_Ec_main_init_Go,
        /* 31 */ Demo_Ec_main_init_Go,
        /* 32 */ Demo_Ec_main_init_Go,
        /* 33 */ Demo_Ec_main_init_Go,
        /* 34 */ Demo_Ec_main_init_Ma2,
    };

    s16 type = this->actor.params;

    if (proc[type] == NULL) {
        // "Demo_Ec_main_init: Initialization process is wrong arg_data"
        PRINTF(VT_FGCOL(RED) " Demo_Ec_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, type);
        Actor_delete(&this->actor);
        return;
    }

    proc[type](this, play);
}

void Demo_Ec_main_bank(DemoEc* this, PlayState* play) {
    s32 pad;
    s16 primary;
    s32 type;
    s16 pad2;
    s16 sp28;
    s32 primaryObjectSlot;
    s32 secondaryObjectSlot;

    type = this->actor.params;
    primary = Demo_Ec_Shape_Bank[type];
    sp28 = Demo_Ec_Anime_Bank[type];
    primaryObjectSlot = Object_Exchange_bank_check(&play->objectCtx, primary);
    secondaryObjectSlot = Object_Exchange_bank_check(&play->objectCtx, sp28);

    if ((secondaryObjectSlot < 0) || (primaryObjectSlot < 0)) {
        // "Demo_Ec_main_bank: Bank unreadable arg_data = %d!"
        PRINTF(VT_FGCOL(RED) "Demo_Ec_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, type);
        Actor_delete(&this->actor);
        return;
    }

    if (Object_Exchange_bank_dma_check(&play->objectCtx, primaryObjectSlot) &&
        Object_Exchange_bank_dma_check(&play->objectCtx, secondaryObjectSlot)) {

        this->drawObjectSlot = primaryObjectSlot;
        this->animObjectSlot = secondaryObjectSlot;

        Demo_Ec_main_init(this, play);
    }
}

void Demo_Ec_main(Actor* thisx, PlayState* play) {
    static DemoEcUpdateFunc proc[] = {
        Demo_Ec_main_bank,
        Demo_Ec_main_In_Wait,
        Demo_Ec_main_Ta_Wait,
        Demo_Ec_main_Fu_Wait,
        Demo_Ec_main_Km1_Wait,
        Demo_Ec_main_Kw1_Wait,
        Demo_Ec_main_Bji_Wait,
        Demo_Ec_main_Ahg_Wait,
        Demo_Ec_main_Bob_Wait,
        Demo_Ec_main_Bba_Wait,
        Demo_Ec_main_Dk1_Wait,
        Demo_Ec_main_Dk2_Wait,
        Demo_Ec_main_Km1_Dance,
        Demo_Ec_main_Kw1_Dance,
        Demo_Ec_main_Ge1_Wait,
        Demo_Ec_main_Zo_Wait,
        Demo_Ec_main_Kz_Wait,
        Demo_Ec_main_Kz_Standup,
        Demo_Ec_main_Kz_Stand,
        Demo_Ec_main_Md_Wait,
        Demo_Ec_main_Md_Stand,
        Demo_Ec_main_Niw_Wait,
        Demo_Ec_main_Ane_Wait,
        Demo_Ec_main_Ds2_Wait,
        Demo_Ec_main_Os_Wait,
        Demo_Ec_main_Fs_Wait,
        Demo_Ec_main_Rs_Wait,
        Demo_Ec_main_Go_Wait,
        Demo_Ec_main_Ma2_Wait,
    };

    DemoEc* this = (DemoEc*)thisx;
    s32 updateMode = this->updateMode;

    if ((updateMode < 0) || (updateMode >= ARRAY_COUNT(proc)) || proc[updateMode] == NULL) {
        // "The main mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        if (updateMode != EC_UPDATE_COMMON) {
            Demo_Ec_Change_AnimeBank(this, play);
        }
        proc[updateMode](this, play);
    }
}

void Demo_Ec_draw_none(DemoEc* this, PlayState* play) {
}

static DemoEcDrawFunc proc[] = {
    Demo_Ec_draw_none,          Demo_Ec_draw_normal_In,
    Demo_Ec_draw_normal_Ta,           Demo_Ec_draw_normal_Fu,
    Demo_Ec_draw_normal_Km1,       Demo_Ec_draw_normal_Kw1,
    Demo_Ec_draw_normal_Bji,          Demo_Ec_draw_normal_Ahg,
    Demo_Ec_draw_normal_Bob,           Demo_Ec_draw_normal_Bba,
    Demo_Ec_draw_normal_Dk1,   Demo_Ec_draw_normal_Dk2,
    Demo_Ec_draw_normal_Ge1,          Demo_Ec_draw_normal_Zo,
    Demo_Ec_draw_normal_Kz,        Demo_Ec_draw_normal_Md,
    Demo_Ec_draw_normal_Niw,           Demo_Ec_draw_normal_Ane,
    Demo_Ec_draw_normal_Ds2, Demo_Ec_draw_normal_Os,
    Demo_Ec_draw_normal_Fs,    Demo_Ec_draw_normal_Rs,
    Demo_Ec_draw_normal_Go,          Demo_Ec_draw_normal_Ma2,
};

void Demo_Ec_draw(Actor* thisx, PlayState* play) {
    DemoEc* this = (DemoEc*)thisx;
    s32 drawConfig = this->drawConfig;

    if ((drawConfig < 0) || (drawConfig >= ARRAY_COUNT(proc)) || proc[drawConfig] == NULL) {
        // "The main mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        if (drawConfig != EC_DRAW_COMMON) {
            Demo_Ec_Change_ShapeBank(this, play);
        }
        proc[drawConfig](this, play);
    }
}

ActorProfile Demo_Ec_Profile = {
    /**/ ACTOR_DEMO_EC,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_EC,
    /**/ sizeof(DemoEc),
    /**/ Demo_Ec_Actor_ct,
    /**/ Demo_Ec_Actor_dt,
    /**/ Demo_Ec_main,
    /**/ Demo_Ec_draw,
};
