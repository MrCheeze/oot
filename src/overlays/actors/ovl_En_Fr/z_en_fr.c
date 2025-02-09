#include "z_en_fr.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"
#include "terminal.h"
#include "assets/objects/object_fr/object_fr.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Fr_Actor_ct(Actor* thisx, PlayState* play);
void En_Fr_Actor_dt(Actor* thisx, PlayState* play);
void En_Fr_Actor_init(Actor* thisx, PlayState* play);
void En_Fr_Ocarina_Actor_move(Actor* thisx, PlayState* play);
void En_Fr_Actor_move(Actor* thisx, PlayState* play);
void En_Fr_Actor_draw(Actor* thisx, PlayState* play);

// Animation Functions
void Fr_Wait(EnFr* this, PlayState* play);
void Fr_StageIN(EnFr* this, PlayState* play);
void Fr_Turn(EnFr* this, PlayState* play);
void Fr_DemoWait(EnFr* this, PlayState* play);
void Fr_Jump(EnFr* this, PlayState* play);
void Fr_Escape(EnFr* this, PlayState* play);
void Fr_WinkProc(EnFr* this);

// Activation
void Fr_Ocarina_Wait(EnFr* this, PlayState* play);
void Fr_Ocarina_In(EnFr* this, PlayState* play);
void Fr_Ocarina_Demo(EnFr* this, PlayState* play);

// Listening for Child Songs
void Fr_Ocarina_Frog_Song_StartWait(EnFr* this, PlayState* play);
void Fr_Ocarina_Frog_Song(EnFr* this, PlayState* play);
void Fr_Ocarina_Frog_Song_Success(EnFr* this, PlayState* play);
void Fr_Ocarina_Frog_Song_ZoomWait(EnFr* this, PlayState* play);

// Frog Song for HP Functions
void Fr_Ocarina_JumpGameMessage(EnFr* this, PlayState* play);
void Fr_Ocarina_JumpGameInterval(EnFr* this, PlayState* play);
void Fr_Ocarina_Start(EnFr* this, PlayState* play);
void Fr_Ocarina_Cancel(EnFr* this, PlayState* play);

// Reward Functions
void Fr_Change_Ocarina_TalkRequest(EnFr* this, PlayState* play, u8 unkCondition);
void Fr_Ocarina_TalkRequest(EnFr* this, PlayState* play);
void Fr_Ocarina_Talk(EnFr* this, PlayState* play);
void Fr_Ocarina_Item_Set_Interval(EnFr* this, PlayState* play);

// Deactivate
void Fr_Ocarina_End(EnFr* this, PlayState* play);
void Fr_Ocarina_CarryWait(EnFr* this, PlayState* play);
void Fr_Ocarina_CarryFinish(EnFr* this, PlayState* play);

/*
Frogs params WIP docs

Represents 6 Actor Instances for frogs:
    - 1 Prop actor instance set to where Link plays Ocarina, manages 5 frogs
    - 5 NPC actor instances for the frogs themselves

0: Prop Actor Instance (located where link detects ocarina, interacts with Link)
1: Frog 0 (Yellow)
2: Frog 1 (Blue)
3: Frog 2 (Red)
4: Frog 3 (Purple)
5: Frog 4 (White)

Note that because of the Prop Actor, actor.params is 1 shifted from frogIndex
Therefore, frogIndex = actor.params - 1


frInfo.flags = 1
     - Activate frogs, frogs will jump out of the water

frInfo.flags = 1 to 11:
     - Counter: Frogs will sequentially jump out based on counter:
         - 1: Frog 1 (Blue)
         - 3: frog 3 (Purple)
         - 5: frog 0 (Yellow)
         - 7: frog 2 (Red)
         - 9: frog 4 (White)
     - Will proceed when counter reaches 11

frInfo.flags = 12
     - Deactivate frogs, frogs will jump back into the water
*/

typedef struct EnFrPointers {
    u8 flags;
    EnFr* frogs[5];
} EnFrPointers;

typedef struct LogSpotToFromWater {
    f32 xzDist;
    f32 yaw;
    f32 yDist;
} LogSpotToFromWater;

static EnFrPointers frInfo = {
    0x00,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

#define FROG_HAS_SONG_BEEN_PLAYED(frogSongIndex)                             \
    (z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_SONGS_FOR_FROGS] & \
     Frog_Song_Mask[frogSongIndex])

#define FROG_SET_SONG_PLAYED(frogSongIndex)                                  \
    z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_SONGS_FOR_FROGS] |= \
        Frog_Song_Mask[frogSongIndex];

static u16 Frog_Song_Mask[] = {
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_ZL),     // FROG_ZL
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_EPONA),  // FROG_EPONA
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_SARIA),  // FROG_SARIA
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_SUNS),   // FROG_SUNS
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_SOT),    // FROG_SOT
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_STORMS), // FROG_STORMS
    EVENTCHKINF_MASK(EVENTCHKINF_SONGS_FOR_FROGS_CHOIR),  // FROG_CHOIR_SONG
    0,                                                    // FROG_NO_SONG
};

static u8 Frog_Song_ArgIndex[] = {
    FROG_SARIA, FROG_SUNS, FROG_SOT, FROG_ZL, FROG_EPONA,
};

// Song to Frog Index Mapping
static s32 Frog_index_Data[] = {
    FROG_PURPLE, FROG_WHITE, FROG_YELLOW, FROG_BLUE, FROG_RED,
};

ActorProfile En_Fr_Profile = {
    /**/ ACTOR_EN_FR,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FR,
    /**/ sizeof(EnFr),
    /**/ En_Fr_Actor_ct,
    /**/ En_Fr_Actor_dt,
    /**/ En_Fr_Actor_init,
    /**/ NULL,
};

static Color_RGBA8 frog_col[] = {
    { 200, 170, 0, 255 }, { 0, 170, 200, 255 }, { 210, 120, 100, 255 }, { 120, 130, 230, 255 }, { 190, 190, 190, 255 },
};

// Jumping back into water frog animation
//      frog_data[frog].xzDist is magnitude of xz distance frog travels
//      frog_data[frog].yaw is rot around y-axis of jumping back into water
//      frog_data[frog].yDist is change in y distance frog travels
static LogSpotToFromWater frog_data[] = {
    { 0.0f, 0.0f, 0.0f },              // Prop   (Where link pulls ocarina)
    { 80.0f, -0.375f * M_PI, -80.0f }, // FROG_YELLOW
    { 80.0f, -0.5f * M_PI, -80.0f },   // FROG_BLUE
    { 80.0f, -0.25f * M_PI, -80.0f },  // FROG_RED
    { 80.0f, 0.875f * M_PI, -80.0f },  // FROG_PURPLE
    { 80.0f, 0.5f * M_PI, -80.0f },    // FROG_WHITE
};

// Timer values for the frog choir song
static s16 Fr_Ocarina_Time_Table[] = {
    40, 20, 15, 12, 12,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

// Counter to Coordinate Frog jumping out of water one at a time
// Used as part of frInfo.flags
static u8 start_flag[] = {
    5, 1, 7, 3, 9,
};

// targetScale (default = 150.0) Actor scale target for add_calc2
// Used as a frog grows from hearing a new child song
static f32 zoomDT[] = {
    180.0f,
    210.0f,
    240.0f,
    270.0f,
};

static u8 Frog_sound_idx[] = {
    5,  // C-Down Ocarina
    2,  // A Button Ocarina
    9,  // C-Right Ocarina
    11, // C-Left Ocarina
    14, // C Up Ocarina
};

static s8 Frog_sound_big_idx[] = {
    -7,  // C-Down Ocarina
    -10, // A Button Ocarina
    -3,  // C-Right Ocarina
    -1,  // C-Left Ocarina
    2,   // C Up Ocarina
};

static u8 frog_id[] = {
    FROG_BLUE, FROG_YELLOW, FROG_RED, FROG_PURPLE, FROG_WHITE, FROG_BLUE, FROG_YELLOW, FROG_RED,
};

static u8 ocarina_no_data[] = {
    OCARINA_BTN_A, OCARINA_BTN_C_DOWN, OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_LEFT, OCARINA_BTN_C_UP,
};

void Fr_InitialSet(EnFr* this) {
    Vec3f vec1;
    Vec3f vec2;

    vec1.x = vec1.y = 0.0f;
    vec1.z = this->xzDistToLogSpot = frog_data[this->actor.params].xzDist;
    Matrix_rotateY(frog_data[this->actor.params].yaw, MTXMODE_NEW);
    Matrix_Position(&vec1, &vec2);
    this->actor.world.pos.x = this->posLogSpot.x + vec2.x;
    this->actor.world.pos.z = this->posLogSpot.z + vec2.z;
    this->actor.world.pos.y = frog_data[this->actor.params].yDist + this->posLogSpot.y;
    this->actor.world.rot.y = this->actor.shape.rot.y =
        RAD_TO_BINANG(frog_data[this->actor.params].yaw) + 0x8000;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
}

void En_Fr_Actor_ct(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (this->actor.params == 0) {
        this->actor.destroy = NULL;
        this->actor.draw = NULL;
        this->actor.update = En_Fr_Ocarina_Actor_move;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED);
        this->actor.flags &= ~0;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        this->actor.textId = 0x40AC;
        this->actionFunc = Fr_Ocarina_Wait;
    } else {
        if ((this->actor.params >= 6) || (this->actor.params < 0)) {
            PRINTF_COLOR_ERROR();
            // "The argument is wrong!!"
            PRINTF("%s[%d] : 引数が間違っている！！(%d)\n", "../z_en_fr.c", 370, this->actor.params);
            PRINTF_RST();
            ASSERT(0, "0", "../z_en_fr.c", 372);
        }

        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP);
        if (this->requiredObjectSlot < 0) {
            Actor_delete(&this->actor);
            PRINTF_COLOR_ERROR();
            // "There is no bank!!"
            PRINTF("%s[%d] : バンクが無いよ！！\n", "../z_en_fr.c", 380);
            PRINTF_RST();
            ASSERT(0, "0", "../z_en_fr.c", 382);
        }
    }
}

// Draw only the purple frog when ocarina is not pulled out on the log spot
void Frog_DeleteDisplay(EnFr* this) {
    this->actor.draw = (this->actor.params - 1) != FROG_PURPLE ? NULL : En_Fr_Actor_draw;
}

void Frog_EnterDisplay(EnFr* this) {
    this->actor.draw = En_Fr_Actor_draw;
}

void En_Fr_Actor_init(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;
    s32 pad;
    s32 frogIndex;
    s32 pad2;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        frogIndex = this->actor.params - 1;
        frInfo.frogs[frogIndex] = this;
        ValueSet_process(&this->actor, value_init);
        // frog
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_fr_Skel_00B498, &object_fr_Anim_001534, this->jointTable,
                           this->morphTable, 24);
        // butterfly
        Skeleton_Info2_M_ct(play, &this->skelAnimeButterfly, &gButterflySkel, &gButterflyAnim, this->jointTableButterfly,
                       this->morphTableButterfly, 8);
        // When playing the song for the HP, the frog with the next note and the butterfly turns on its lightsource
        this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
        Light_point_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y,
                                  this->actor.home.pos.z, 255, 255, 255, -1);
        // Check to see if the song for a particular frog has been played.
        // If it has, the frog is larger. If not, the frog is smaller
        this->scale = FROG_HAS_SONG_BEEN_PLAYED(Frog_Song_ArgIndex[frogIndex]) ? 270.0f : 150.0f;
        // When the frogs are not active (link doesn't have his ocarina out),
        // Then shrink the frogs down by a factor of 10,000
        Actor_set_scale(&this->actor, this->scale * 0.0001f);
        this->actor.minVelocityY = -9999.0f;
        Actor_world_to_eye(&this->actor, 10.0f);
        this->eyeTexIndex = 1;
        this->blinkTimer = (s16)(rnd_f(60.0f) + 20.0f);
        this->blinkFunc = Fr_WinkProc;
        this->isBelowWaterSurfacePrevious = this->isBelowWaterSurfaceCurrent = false;
        this->isJumpingUp = false;
        this->posLogSpot = this->actor.world.pos;
        this->actionFunc = Fr_Wait;
        this->isDeactivating = false;
        this->growingScaleIndex = 0;
        this->isActive = false;
        this->isJumpingToFrogSong = false;
        this->songIndex = FROG_NO_SONG;
        this->unusedButterflyActor = NULL;
        Fr_InitialSet(this);
        Frog_DeleteDisplay(this);
        this->actor.update = En_Fr_Actor_move;
        this->isButterflyDrawn = false;
        this->xyAngleButterfly = 0x1000 * (s16)rnd_f(255.0f);
        this->posButterflyLight.x = this->posButterfly.x = this->posLogSpot.x;
        this->posButterflyLight.y = this->posButterfly.y = this->posLogSpot.y + 50.0f;
        this->posButterflyLight.z = this->posButterfly.z = this->posLogSpot.z;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void En_Fr_Actor_dt(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
}

void Fr_WaterSurfaceCheck(EnFr* this, PlayState* play) {
    WaterBox* waterBox;
    f32 waterSurface;

    if (T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                                &waterBox)) {
        this->isBelowWaterSurfacePrevious = this->isBelowWaterSurfaceCurrent;
        this->isBelowWaterSurfaceCurrent = this->actor.world.pos.y <= waterSurface ? true : false;
    }
}

void Fr_SibukiSet(EnFr* this, PlayState* play) {
    Vec3f vec;

    // Jumping into or out of water
    if (this->isBelowWaterSurfaceCurrent != this->isBelowWaterSurfacePrevious) {
        vec.x = this->actor.world.pos.x;
        vec.y = this->actor.world.pos.y - 10.0f;
        vec.z = this->actor.world.pos.z;
        Effect_SS_G_Splash_sc_cl_ct(play, &vec, NULL, NULL, 1, 1);

        if (!this->isBelowWaterSurfaceCurrent) {
            Actor_SE_set(&this->actor, NA_SE_EV_DIVE_INTO_WATER_L);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

s32 Fr_FloorCheck(EnFr* this, f32* yDistToLogSpot) {
    yDistToLogSpot[0] = this->actor.world.pos.y - this->posLogSpot.y;
    if ((this->actor.velocity.y < 0.0f) && (yDistToLogSpot[0] <= 0.0f)) {
        this->actor.velocity.y = 0.0f;
        this->actor.world.pos.y = this->posLogSpot.y;
        return true;
    } else {
        return false;
    }
}

int CHK_Fr_Ocarina_Player_Distance(Player* player, EnFr* this) {
    f32 xDistToPlayer = player->actor.world.pos.x - this->actor.world.pos.x;
    f32 zDistToPlayer = player->actor.world.pos.z - this->actor.world.pos.z;
    f32 yDistToPlayer = player->actor.world.pos.y - this->actor.world.pos.y;

    return ((SQ(xDistToPlayer) + SQ(zDistToPlayer)) <= SQ(30.0f)) && (yDistToPlayer >= 0.0f);
}

void Fr_GlareProc(EnFr* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else {
        this->blinkFunc = Fr_WinkProc;
    }
}

void Fr_WinkProc(EnFr* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else if (this->eyeTexIndex) {
        this->eyeTexIndex = 0;
        this->blinkTimer = (s16)(rnd_f(60.0f) + 20.0f);
        this->blinkFunc = Fr_GlareProc;
    } else {
        this->eyeTexIndex = 1;
        this->blinkTimer = 1;
    }
}

void Fr_Wait(EnFr* this, PlayState* play) {
    if (frInfo.flags == start_flag[this->actor.params - 1]) {
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        Frog_EnterDisplay(this);
        this->actionFunc = Fr_StageIN;
    }
}

void Fr_StageIN(EnFr* this, PlayState* play) {
    Vec3f vec1;
    Vec3f vec2;

    if (this->skelAnime.curFrame == 6.0f) {
        frInfo.flags++;
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.gravity = -10.0f;
        this->actor.speed = 0.0f;
        this->actor.velocity.y = 47.0f;
    }

    vec1.x = vec1.y = 0.0f;
    vec1.z = this->xzDistToLogSpot;
    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.world.rot.y + 0x8000), MTXMODE_NEW);
    Matrix_Position(&vec1, &vec2);
    this->actor.world.pos.x = this->posLogSpot.x + vec2.x;
    this->actor.world.pos.z = this->posLogSpot.z + vec2.z;
    if (this->skelAnime.curFrame >= 3.0f) {
        add_calc2(&this->xzDistToLogSpot, 0.0f, 1.0f, 10.0f);
    }

    if (Fr_FloorCheck(this, &vec2.y)) {
        this->actor.gravity = 0.0f;
        this->actionFunc = Fr_Turn;
        this->unusedFloat = 0.0f;
    }

    if ((this->actor.velocity.y <= 0.0f) && (vec2.y < 40.0f)) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void Fr_Turn(EnFr* this, PlayState* play) {
    s16 rotYRemaining = add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 10000, 100);

    this->actor.world.rot.y = this->actor.shape.rot.y;

    if ((rotYRemaining == 0) && (this->skelAnime.curFrame == this->skelAnime.endFrame)) {
        frInfo.flags++;
        this->actionFunc = Fr_DemoWait;
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
    }
}

void Fr_DemoWait(EnFr* this, PlayState* play) {
    if (frInfo.flags == 12) {
        this->actor.world.rot.y = RAD_TO_BINANG(frog_data[this->actor.params].yaw);
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        this->actionFunc = Fr_Escape;
    } else if (this->isJumpingUp) {
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        this->actionFunc = Fr_Jump;
    }
}

void Fr_Jump(EnFr* this, PlayState* play) {
    f32 yDistToLogSpot;

    if (this->skelAnime.curFrame == 6.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.gravity = -10.0f;
        this->actor.velocity.y = 25.0f;
        if (this->isJumpingToFrogSong) {
            this->isJumpingToFrogSong = false;
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_EAT);
        }
    }

    if (Fr_FloorCheck(this, &yDistToLogSpot)) {
        this->isJumpingUp = false;
        this->actor.gravity = 0.0f;
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_0011C0, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_0011C0), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = Fr_DemoWait;
    } else if ((this->actor.velocity.y <= 0.0f) && (yDistToLogSpot < 40.0f)) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void Fr_Escape(EnFr* this, PlayState* play) {
    f32 yUnderwater = frog_data[this->actor.params].yDist + this->posLogSpot.y;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 10000, 100);
    if (this->skelAnime.curFrame == 6.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.speed = 6.0f;
        this->actor.gravity = -10.0f;
        this->actor.velocity.y = 25.0f;
    }

    // Final Spot Reached
    if ((this->actor.velocity.y < 0.0f) && (this->actor.world.pos.y < yUnderwater)) {
        Skeleton_Info2_init(&this->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = Fr_Wait;
        Frog_DeleteDisplay(this);
        this->isDeactivating = true;
        Fr_InitialSet(this);
    }
}

void Fr_Zoom(EnFr* this, PlayState* play) {
    switch (this->isGrowing) {
        case false:
            add_calc2(&this->scale, zoomDT[this->growingScaleIndex], 2.0f, 25.0f);
            if (this->scale >= zoomDT[this->growingScaleIndex]) {
                this->scale = zoomDT[this->growingScaleIndex];
                if (this->growingScaleIndex < 3) {
                    this->isGrowing = true;
                } else {
                    this->isJumpingUp = false;
                    this->isActive = false;
                }
            }
            break;
        case true:
            add_calc2(&this->scale, 150.0f, 2.0f, 25.0f);
            if (this->scale <= 150.0f) {
                this->scale = 150.0f;
                this->growingScaleIndex++;
                if (this->growingScaleIndex >= 4) {
                    this->growingScaleIndex = 3;
                }
                this->isGrowing = false;
            }
            break;
    }
}

void Fr_choo_move(EnFr* this, PlayState* play) {
    s16 rotY = this->actor.shape.rot.y;
    f32 sin;
    Vec3f vec1;
    Vec3f vec2;

    this->xyAngleButterfly += 0x1000;
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = 0.0f;
    vec1.z = 25.0f;
    Matrix_Position(&vec1, &vec2);
    sin = sin_s(this->xyAngleButterfly * 2) * 5.0f;
    this->posButterfly.x = (sin_s(rotY) * sin) + vec2.x;
    this->posButterfly.y = (2.0f * cos_s(this->xyAngleButterfly)) + (this->posLogSpot.y + 50.0f);
    this->posButterfly.z = (cos_s(rotY) * sin) + vec2.z;
    Matrix_translate(this->posButterfly.x, this->posButterfly.y, this->posButterfly.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = 0.0f;
    vec1.y = -15.0f;
    vec1.z = 20.0f;
    Matrix_Position(&vec1, &this->posButterflyLight);
}

void En_Fr_Actor_move(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    this->jumpCounter++;
    Actor_set_scale(&this->actor, this->scale * 0.0001f);

    if (this->isActive) {
        Fr_Zoom(this, play);
    } else {
        Actor_world_to_eye(&this->actor, 10.0f);
        this->blinkFunc(this);
        this->actionFunc(this, play);
        Fr_WaterSurfaceCheck(this, play);
        Fr_SibukiSet(this, play);
        Skeleton_Info2_anime_play(&this->skelAnime);
        Skeleton_Info2_anime_play(&this->skelAnimeButterfly);
        Fr_choo_move(this, play);
        Actor_position_moveF(&this->actor);
    }
}

s32 Frog_JumpSet(EnFr* this, s32 frogIndex) {
    EnFr* frog = frInfo.frogs[frogIndex];
    u8 semitone;

    if ((frog != NULL) && !frog->isJumpingUp) {
        semitone = frog->growingScaleIndex == 3 ? Frog_sound_big_idx[frogIndex] : Frog_sound_idx[frogIndex];
        if (this->songIndex == FROG_CHOIR_SONG) {
            frog->isJumpingToFrogSong = true;
        }
        frog->isJumpingUp = true;
        Na_StartTransposeSe(&frog->actor.projectedPos, NA_SE_EV_FROG_JUMP, semitone);
        return true;
    } else {
        return false;
    }
}

void Fr_Ocarina_Wait(EnFr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_25) {
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }

        makeOnepointDemo(play, 4110, ~0x62, &this->actor, CAM_ID_MAIN);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        player->actor.world.pos.x = this->actor.world.pos.x; // x = 990.0f
        player->actor.world.pos.y = this->actor.world.pos.y; // y = 205.0f
        player->actor.world.pos.z = this->actor.world.pos.z; // z = -1220.0f
        player->yaw = player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y;
        this->reward = GI_NONE;
        this->actionFunc = Fr_Ocarina_In;
    } else if (CHK_Fr_Ocarina_Player_Distance(player, this)) {
        player->unk_6A8 = &this->actor;
    }
}

void Fr_Ocarina_In(EnFr* this, PlayState* play) {
    if (play->msgCtx.msgMode == MSGMODE_OCARINA_PLAYING) {
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        frInfo.flags = 1;
        this->actionFunc = Fr_Ocarina_Demo;
    } else if (play->msgCtx.msgMode == MSGMODE_PAUSED) { // Goes to Frogs 2 Song
        frInfo.flags = 1;
        this->actionFunc = Fr_Ocarina_Demo;
    }
}

void Fr_Ocarina_Demo(EnFr* this, PlayState* play) {
    if (frInfo.flags == 11) {
        // Check if all 6 child songs have been played for the frogs
        if (GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_ZL) && GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_EPONA) &&
            GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_SARIA) && GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_SUNS) &&
            GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_SOT) && GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_STORMS)) {
            this->actionFunc = Fr_Ocarina_JumpGameMessage;
            this->songIndex = FROG_CHOIR_SONG;
            message_set(play, 0x40AB, &this->actor);
        } else {
            this->songIndex = FROG_ZL;
            this->actionFunc = Fr_Ocarina_Frog_Song_StartWait;
        }
    }
}

void Fr_Ocarina_Frog_Song_StartWait(EnFr* this, PlayState* play) {
    EnFr* frog;
    s32 frogIndex;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(frInfo.frogs); frogIndex++) {
        frog = frInfo.frogs[frogIndex];
        if (frog != NULL && frog->actionFunc == Fr_DemoWait) {
            continue;
        } else {
            return;
        }
    }

    ocarina_set(play, OCARINA_ACTION_CHECK_NOWARP);
    this->actionFunc = Fr_Ocarina_Frog_Song;
}

void Fr_Ocarina_Frog_Song(EnFr* this, PlayState* play) {
    this->songIndex = FROG_NO_SONG;
    switch (play->msgCtx.ocarinaMode) { // Ocarina Song played
        case OCARINA_MODE_07:
            this->songIndex = FROG_ZL;
            break;
        case OCARINA_MODE_06:
            this->songIndex = FROG_EPONA;
            break;
        case OCARINA_MODE_05:
            this->songIndex = FROG_SARIA;
            break;
        case OCARINA_MODE_08:
            this->songIndex = FROG_SUNS;
            break;
        case OCARINA_MODE_09:
            this->songIndex = FROG_SOT;
            break;
        case OCARINA_MODE_0A:
            this->songIndex = FROG_STORMS;
            break;
        case OCARINA_MODE_04:
            Fr_Ocarina_Cancel(this, play);
            break;
        case OCARINA_MODE_01: // Ocarina note played, but no song played
            switch (play->msgCtx.lastOcarinaButtonIndex) {
                // Jumping frogs in open ocarina based on ocarina note played
                case OCARINA_BTN_A:
                    Frog_JumpSet(this, FROG_BLUE);
                    break;
                case OCARINA_BTN_C_DOWN:
                    Frog_JumpSet(this, FROG_YELLOW);
                    break;
                case OCARINA_BTN_C_RIGHT:
                    Frog_JumpSet(this, FROG_RED);
                    break;
                case OCARINA_BTN_C_LEFT:
                    Frog_JumpSet(this, FROG_PURPLE);
                    break;
                case OCARINA_BTN_C_UP:
                    Frog_JumpSet(this, FROG_WHITE);
                    break;
            }
    }
    if (this->songIndex != FROG_NO_SONG) {
        this->jumpCounter = 0;
        this->actionFunc = Fr_Ocarina_Frog_Song_Success;
    }
}

void Fr_Ocarina_Frog_Song_Success(EnFr* this, PlayState* play) {
    if (this->jumpCounter < 48) {
        if (this->jumpCounter % 4 == 0) {
            Frog_JumpSet(this, frog_id[(this->jumpCounter >> 2) & 7]);
        }
    } else {
        if (this->songIndex == FROG_STORMS) {
            this->actor.textId = 0x40AA;
            Fr_Change_Ocarina_TalkRequest(this, play, false);
        } else if (!FROG_HAS_SONG_BEEN_PLAYED(this->songIndex)) {
            EnFr* frog = frInfo.frogs[Frog_index_Data[this->songIndex]];

            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
            if (frog->actionFunc == Fr_DemoWait) {
                frog->isJumpingUp = true;
                frog->isActive = true;
                Actor_SE_set(&frog->actor, NA_SE_EV_FROG_GROW_UP);
                this->actionFunc = Fr_Ocarina_Frog_Song_ZoomWait;
            } else {
                this->jumpCounter = 48;
            }
        } else {
            this->actor.textId = 0x40A9;
            Fr_Change_Ocarina_TalkRequest(this, play, true);
        }
    }
}

void Fr_Ocarina_Frog_Song_ZoomWait(EnFr* this, PlayState* play) {
    EnFr* frog = frInfo.frogs[Frog_index_Data[this->songIndex]];

    if (!frog->isActive) {
        this->actor.textId = 0x40A9;
        Fr_Change_Ocarina_TalkRequest(this, play, true);
    }
}

void Fr_Ocarina_JumpGameMessage(EnFr* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->frogSongTimer = 2;
        this->actionFunc = Fr_Ocarina_JumpGameInterval;
    }
}

void Frog_Choo_Set(u8 ocarinaNote) {
    EnFr* frog;
    s32 frogIndexButterfly;
    s32 frogIndex;

    switch (ocarinaNote) {
        case OCARINA_BTN_A:
            frogIndexButterfly = FROG_BLUE;
            break;
        case OCARINA_BTN_C_DOWN:
            frogIndexButterfly = FROG_YELLOW;
            break;
        case OCARINA_BTN_C_RIGHT:
            frogIndexButterfly = FROG_RED;
            break;
        case OCARINA_BTN_C_LEFT:
            frogIndexButterfly = FROG_PURPLE;
            break;
        case OCARINA_BTN_C_UP:
            frogIndexButterfly = FROG_WHITE;
    }
    // Turn on or off butterfly above frog
    for (frogIndex = 0; frogIndex < ARRAY_COUNT(frInfo.frogs); frogIndex++) {
        frog = frInfo.frogs[frogIndex];
        frog->isButterflyDrawn = frogIndex == frogIndexButterfly ? true : false;
    }
}

void Frog_Choo_Clear(void) {
    s32 frogIndex;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(frInfo.frogs); frogIndex++) {
        EnFr* frog = frInfo.frogs[frogIndex];

        frog->isButterflyDrawn = false;
    }
}

u8 Get_Fr_JumpGame_chk_ocarina_no(u8 ocarinaNoteIndex) {
    if (!GET_EVENTCHKINF(EVENTCHKINF_SONGS_FOR_FROGS_CHOIR)) {
        return na_frog_tbl_ptr[ocarinaNoteIndex];
    } else {
        return ocarina_no_data[(s32)rnd_f(60.0f) % 5];
    }
}

void Fr_Ocarina_JumpGameInterval(EnFr* this, PlayState* play) {
    if (this->frogSongTimer != 0) {
        this->frogSongTimer--;
    } else {
        this->frogSongTimer = 40;
        this->ocarinaNoteIndex = 0;
        ocarina_set(play, OCARINA_ACTION_FROGS);
        this->ocarinaNote = Get_Fr_JumpGame_chk_ocarina_no(this->ocarinaNoteIndex);
        Frog_Choo_Set(this->ocarinaNote);
        this->actionFunc = Fr_Ocarina_Start;
    }
}

s32 Fr_Ocarina_RightCheck(EnFr* this, PlayState* play) {
    u8 index;
    u8 ocarinaNote;
    MessageContext* msgCtx = &play->msgCtx;
    u8 ocarinaNoteIndex;

    if (this->ocarinaNote == (*msgCtx).lastOcarinaButtonIndex) { // required to match, possibly an array?
        this->ocarinaNoteIndex++;
        ocarinaNoteIndex = this->ocarinaNoteIndex;
        if (ocarinaNoteIndex >= 14) { // Frog Song is completed
            this->ocarinaNoteIndex = 13;
            return true;
        }
        // The first four notes have more frames to receive an input
        index = ocarinaNoteIndex < 4 ? (s32)ocarinaNoteIndex : 4;
        ocarinaNote = Get_Fr_JumpGame_chk_ocarina_no(ocarinaNoteIndex);
        this->ocarinaNote = ocarinaNote;
        Frog_Choo_Set(ocarinaNote);
        this->frogSongTimer = Fr_Ocarina_Time_Table[index];
    }
    return false;
}

void Fr_Ocarina_Cancel(EnFr* this, PlayState* play) {
    message_close(play);
    this->reward = GI_NONE;
    Na_StartSystemSe_F(NA_SE_SY_OCARINA_ERROR);
    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
    frInfo.flags = 12;
    Frog_Choo_Clear();
    this->actionFunc = Fr_Ocarina_End;
}

void Fr_Ocarina_Start(EnFr* this, PlayState* play) {
    if (this->frogSongTimer == 0) {
        Fr_Ocarina_Cancel(this, play);
    } else {
        this->frogSongTimer--;
        if (play->msgCtx.msgMode == MSGMODE_FROGS_PLAYING) {
            s32 counter = 0;
            s32 i;

            for (i = 0; i < ARRAY_COUNT(frInfo.frogs); i++) {
                EnFr* frog = frInfo.frogs[i];

                if (frog == NULL || frog->actionFunc != Fr_DemoWait) {
                    counter++;
                }
            }
            if (counter == 0 && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
                Fr_Ocarina_Cancel(this, play);
                return;
            }
        }

        if (play->msgCtx.msgMode == MSGMODE_FROGS_WAITING) {
            play->msgCtx.msgMode = MSGMODE_FROGS_START;
            switch (play->msgCtx.lastOcarinaButtonIndex) {
                case OCARINA_BTN_A:
                    Frog_JumpSet(this, FROG_BLUE);
                    break;
                case OCARINA_BTN_C_DOWN:
                    Frog_JumpSet(this, FROG_YELLOW);
                    break;
                case OCARINA_BTN_C_RIGHT:
                    Frog_JumpSet(this, FROG_RED);
                    break;
                case OCARINA_BTN_C_LEFT:
                    Frog_JumpSet(this, FROG_PURPLE);
                    break;
                case OCARINA_BTN_C_UP:
                    Frog_JumpSet(this, FROG_WHITE);
            }
            if (Fr_Ocarina_RightCheck(this, play)) {
                this->actor.textId = 0x40AC;
                Fr_Change_Ocarina_TalkRequest(this, play, false);
            }
        }
    }
}

void Fr_Change_Ocarina_TalkRequest(EnFr* this, PlayState* play, u8 unkCondition) {
    Frog_Choo_Clear();
    if (unkCondition) {
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
    } else {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
    }

    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
    play->msgCtx.msgMode = MSGMODE_PAUSED;
    this->actionFunc = Fr_Ocarina_TalkRequest;
}

void Fr_Ocarina_TalkRequest(EnFr* this, PlayState* play) {
    message_set(play, this->actor.textId, &this->actor);
    this->actionFunc = Fr_Ocarina_Talk;
}

void Fr_Ocarina_Talk(EnFr* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->frogSongTimer = 100;
        message_close(play);
        this->actionFunc = Fr_Ocarina_Item_Set_Interval;
    }
}

void Fr_Ocarina_Item_Set_Interval(EnFr* this, PlayState* play) {
    u16 songIndex;

    frInfo.flags = 12;
    songIndex = this->songIndex;
    this->actionFunc = Fr_Ocarina_End;
    this->reward = GI_NONE;
    if ((songIndex >= FROG_ZL) && (songIndex <= FROG_SOT)) {
        if (!FROG_HAS_SONG_BEEN_PLAYED(songIndex)) {
            FROG_SET_SONG_PLAYED(songIndex);
            this->reward = GI_RUPEE_PURPLE;
        } else {
            this->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_STORMS) {
        if (!FROG_HAS_SONG_BEEN_PLAYED(songIndex)) {
            FROG_SET_SONG_PLAYED(songIndex);
            this->reward = GI_HEART_PIECE;
        } else {
            this->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_CHOIR_SONG) {
        if (!FROG_HAS_SONG_BEEN_PLAYED(songIndex)) {
            FROG_SET_SONG_PLAYED(songIndex);
            this->reward = GI_HEART_PIECE;
        } else {
            this->reward = GI_RUPEE_PURPLE;
        }
    }
}

void Fr_Ocarina_End(EnFr* this, PlayState* play) {
    s32 frogIndex;

    // Originally was going to have separate butterfly actor
    // Changed to include butterfly as part of frog actor
    // This unused code would have frozen the butterfly actor above frog
    if (this->unusedButterflyActor != NULL) {
        this->unusedButterflyActor->freezeTimer = 10;
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(frInfo.frogs); frogIndex++) {
        EnFr* frog = frInfo.frogs[frogIndex];

        if (frog == NULL) {
            PRINTF_COLOR_ERROR();
            // "There are no frogs!?"
            PRINTF("%s[%d]カエルがいない！？\n", "../z_en_fr.c", 1604);
            PRINTF_RST();
            return;
        } else if (frog->isDeactivating != true) {
            return;
        }
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(frInfo.frogs); frogIndex++) {
        EnFr* frog = frInfo.frogs[frogIndex];

        if (frog == NULL) {
            PRINTF_COLOR_ERROR();
            // "There are no frogs!?"
            PRINTF("%s[%d]カエルがいない！？\n", "../z_en_fr.c", 1618);
            PRINTF_RST();
            return;
        }
        frog->isDeactivating = false;
    }

    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    Actor_SE_set(&this->actor, NA_SE_EV_FROG_CRY_0);
    if (this->reward == GI_NONE) {
        this->actionFunc = Fr_Ocarina_Wait;
    } else {
        this->actionFunc = Fr_Ocarina_CarryWait;
        Actor_carry_request_set2(&this->actor, play, this->reward, 30.0f, 100.0f);
    }
}

void Fr_Ocarina_CarryWait(EnFr* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = Fr_Ocarina_CarryFinish;
    } else {
        Actor_carry_request_set2(&this->actor, play, this->reward, 30.0f, 100.0f);
    }
}

void Fr_Ocarina_CarryFinish(EnFr* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        this->actionFunc = Fr_Ocarina_Wait;
    }
}

void En_Fr_Ocarina_Actor_move(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }

    this->jumpCounter++;
    this->actionFunc(this, play);
}

s32 En_Fr_before(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if ((limbIndex == 7) || (limbIndex == 8)) {
        *dList = NULL;
    }
    return 0;
}

void En_Fr_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFr* this = (EnFr*)thisx;

    if ((limbIndex == 7) || (limbIndex == 8)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_fr.c", 1735);
        Matrix_push();
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_fr.c", 1738);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_fr.c", 1741);
    }
}

void En_Fr_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        object_fr_Tex_0059A0,
        object_fr_Tex_005BA0,
    };
    s16 lightRadius;
    EnFr* this = (EnFr*)thisx;
    s16 frogIndex = this->actor.params - 1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fr.c", 1754);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    // For the frogs 2 HP, the frog with the next note and the butterfly lights up
    lightRadius = this->isButterflyDrawn ? 95 : -1;
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    Light_point_ct(&this->lightInfo, this->posButterflyLight.x, this->posButterflyLight.y,
                              this->posButterflyLight.z, 255, 255, 255, lightRadius);
    gDPSetEnvColor(POLY_OPA_DISP++, frog_col[frogIndex].r, frog_col[frogIndex].g, frog_col[frogIndex].b, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          En_Fr_before, En_Fr_after, this);
    if (this->isButterflyDrawn) {
        Matrix_translate(this->posButterfly.x, this->posButterfly.y, this->posButterfly.z, MTXMODE_NEW);
        Matrix_scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);
        Matrix_rotateXYZ(this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, MTXMODE_APPLY);
        Si2_draw(play, this->skelAnimeButterfly.skeleton, this->skelAnimeButterfly.jointTable, NULL, NULL,
                          NULL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fr.c", 1816);
}
