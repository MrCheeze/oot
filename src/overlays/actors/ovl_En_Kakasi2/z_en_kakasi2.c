/*
 * File: z_en_kakasi2.c
 * Overlay: ovl_En_Kakasi2
 * Description: Pierre the Scarecrow Spawn
 */

#include "z_en_kakasi2.h"
#include "terminal.h"
#include "assets/objects/object_ka/object_ka.h"

#define FLAGS                                                                                               \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_LOCK_ON_DISABLED)

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

void En_Kakasi2_actor_ct(Actor* thisx, PlayState* play);
void En_Kakasi2_actor_dt(Actor* thisx, PlayState* play);
void En_Kakasi2_actor_move(Actor* thisx, PlayState* play2);
void En_Kakasi2_actor_disp(Actor* thisx, PlayState* play);

static void mode_stop_init(EnKakasi2* this, PlayState* play);
static void mode_wait(EnKakasi2* this, PlayState* play);
static void mode_move_init(EnKakasi2* this, PlayState* play);
static void mode_move(EnKakasi2* this, PlayState* play);
static void mode_stop(EnKakasi2* this, PlayState* play);

ActorProfile En_Kakasi2_Profile = {
    /**/ ACTOR_EN_KAKASI2,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_KA,
    /**/ sizeof(EnKakasi2),
    /**/ En_Kakasi2_actor_ct,
    /**/ En_Kakasi2_actor_dt,
    /**/ En_Kakasi2_actor_move,
    /**/ NULL,
};

void En_Kakasi2_actor_ct(Actor* thisx, PlayState* play) {
    EnKakasi2* this = (EnKakasi2*)thisx;
    s32 pad;
    f32 spawnRangeY;
    f32 spawnRangeXZ;

    PRINTF("\n\n");
    // "Visit Umeda"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 梅田参号見参！ ☆☆☆☆☆ \n" VT_RST);

    this->switchFlag = PARAMS_GET_U(this->actor.params, 0, 6);
    spawnRangeY = PARAMS_GET_U(this->actor.params, 6, 8);
    spawnRangeXZ = this->actor.world.rot.z;
    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_4;
    this->maxSpawnDistance.x = (spawnRangeY * 40.0f) + 40.0f;
    this->maxSpawnDistance.y = (spawnRangeXZ * 40.0f) + 40.0f;

    // "Former? (Argument 0)"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(引数０) ☆☆☆☆ %f\n" VT_RST, spawnRangeY);
    // "Former? (Z angle)"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(Ｚアングル) ☆☆ %f\n" VT_RST, spawnRangeXZ);
    // "Correction coordinates X"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｘ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.x);
    // "Correction coordinates Y"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｙ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.y);
    // "Correction coordinates Z"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｚ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.z);
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ SAVE       ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
    PRINTF("\n\n");

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->height = 60.0f;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->unk_198 = this->actor.shape.rot.y;

    if (this->switchFlag >= 0 && Actor_Environment_sw_Check(play, this->switchFlag)) {
        this->actor.draw = En_Kakasi2_actor_disp;
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        this->actionFunc = mode_stop_init;
    } else {
        this->actionFunc = mode_wait;
        this->actor.shape.yOffset = -8000.0f;
    }
}

void En_Kakasi2_actor_dt(Actor* thisx, PlayState* play) {
    EnKakasi2* this = (EnKakasi2*)thisx;

    ClObjPipe_dt(play, &this->collider);
    //! @bug Skeleton_Info_dt is not called
}

static void mode_wait(EnKakasi2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_194++;

    if (DEBUG_FEATURES && (BREG(1) != 0) && (this->actor.xzDistToPlayer < this->maxSpawnDistance.x) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < this->maxSpawnDistance.y)) {

        this->actor.draw = En_Kakasi2_actor_disp;
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        makeActorAttentionDemo(play, &this->actor);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_LOCK_ON_DISABLED;

        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        if (this->switchFlag >= 0) {
            Actor_Environment_sw_On(play, this->switchFlag);
        }

        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
        this->actionFunc = mode_move_init;
    } else if ((this->actor.xzDistToPlayer < this->maxSpawnDistance.x) &&
               (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < this->maxSpawnDistance.y) &&
               GET_EVENTCHKINF(EVENTCHKINF_9C)) {

        this->unk_194 = 0;
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_0B) {
            if (this->switchFlag >= 0) {
                Actor_Environment_sw_On(play, this->switchFlag);
            }
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
            this->actor.draw = En_Kakasi2_actor_disp;
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
            makeActorAttentionDemo(play, &this->actor);
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);

            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_LOCK_ON_DISABLED;
            this->actionFunc = mode_move_init;
        }
    }
}

static void mode_move_init(EnKakasi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&object_ka_Anim_000214);

    Skeleton_Info2_init(&this->skelAnime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    Actor_SE_set(&this->actor, NA_SE_EV_COME_UP_DEKU_JR);
    this->actionFunc = mode_move;
}

static void mode_move(EnKakasi2* this, PlayState* play) {
    s16 currentFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);

    currentFrame = this->skelAnime.curFrame;
    if (currentFrame == 11 || currentFrame == 17) {
        Actor_SE_set(&this->actor, NA_SE_EV_KAKASHI_SWING);
    }

    this->actor.shape.rot.y += 0x800;
    add_calc0(&this->actor.shape.yOffset, 0.5f, 500.0f);

    if (this->actor.shape.yOffset > -100.0f) {
        this->actionFunc = mode_stop_init;
        this->actor.shape.yOffset = 0.0f;
    }
}

static void mode_stop_init(EnKakasi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&object_ka_Anim_000214);

    Skeleton_Info2_init(&this->skelAnime, &object_ka_Anim_000214, 0.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_stop;
}

static void mode_stop(EnKakasi2* this, PlayState* play) {
    if (this->skelAnime.curFrame != 0) {
        add_calc0(&this->skelAnime.curFrame, 0.5f, 1.0f);
    }
    add_calc_short_angle2(&this->actor.shape.rot.y, this->unk_198, 5, 0xBB8, 0);
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void En_Kakasi2_actor_move(Actor* thisx, PlayState* play2) {
    EnKakasi2* this = (EnKakasi2*)thisx;
    PlayState* play = play2;

    this->actor.world.rot = this->actor.shape.rot;
    Actor_world_to_eye(&this->actor, this->height);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);

    if (this->actor.shape.yOffset == 0.0f) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (DEBUG_FEATURES && BREG(0) != 0) {
        if (BREG(5) != 0) {
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ this->actor.player_distance ☆☆☆☆☆ %f\n" VT_RST, this->actor.xzDistToPlayer);
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ this->hosei.x ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.x);
            PRINTF("\n\n");
        }
        if (this->actor.draw == NULL) {
            if (this->unk_194 != 0) {
                if ((this->unk_194 % 2) == 0) {
                    Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                           this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z,
                                           1.0f, 1.0f, 1.0f, 70, 70, 70, 255, 4, play->state.gfxCtx);
                }
            } else {
                Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 255, 255, 255, 4, play->state.gfxCtx);
            }
        }
    }
}

void En_Kakasi2_actor_disp(Actor* thisx, PlayState* play) {
    EnKakasi2* this = (EnKakasi2*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
}
