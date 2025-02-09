/*
 * File: z_shot_sun.c
 * Overlay: ovl_Shot_Sun
 * Description: Lake Hylia Sun hitbox and Song of Storms Fairy spawner
 */

#include "z_shot_sun.h"
#include "overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/scenes/overworld/spot06/spot06_scene.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void Shot_Sun_Actor_ct(Actor* thisx, PlayState* play);
void Shot_Sun_Actor_dt(Actor* thisx, PlayState* play);
void Shot_Sun_Actor_move(Actor* thisx, PlayState* play);

static void move_appeare(ShotSun* this, PlayState* play);
static void move_waitcamera(ShotSun* this, PlayState* play);
void move_ocarina(ShotSun* this, PlayState* play);
static void move_wait(ShotSun* this, PlayState* play);

ActorProfile Shot_Sun_Profile = {
    /**/ ACTOR_SHOT_SUN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ShotSun),
    /**/ Shot_Sun_Actor_ct,
    /**/ Shot_Sun_Actor_dt,
    /**/ Shot_Sun_Actor_move,
    /**/ NULL,
};

typedef enum FairySpawnerState {
    /* 0 */ SPAWNER_OUT_OF_RANGE,
    /* 1 */ SPAWNER_OCARINA_START,
    /* 2 */ SPAWNER_OCARINA_PLAYING
} FairySpawnerState;

static ColliderCylinderInit ShotSun_OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000020, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 60, 0, { 0, 0, 0 } },
};

void Shot_Sun_Actor_ct(Actor* thisx, PlayState* play) {
    ShotSun* this = (ShotSun*)thisx;
    s32 params;

    // "Ocarina secret occurrence"
    PRINTF("%d ---- オカリナの秘密発生!!!!!!!!!!!!!\n", this->actor.params);
    params = PARAMS_GET_U(this->actor.params, 0, 8);
    if (params == 0x40 || params == 0x41) {
        this->fairySpawnerState = SPAWNER_OUT_OF_RANGE;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        this->actionFunc = move_ocarina;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    } else {
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &ShotSun_OcInfoData);
        this->actionFunc = move_wait;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void Shot_Sun_Actor_dt(Actor* thisx, PlayState* play) {
    ShotSun* this = (ShotSun*)thisx;
    s32 params = PARAMS_GET_U(this->actor.params, 0, 8);

    if (params != 0x40 && params != 0x41) {
        ClObjPipe_dt(play, &this->collider);
    }
}

static void move_appeare(ShotSun* this, PlayState* play) {
    s32 params = PARAMS_GET_U(this->actor.params, 0, 8);
    s32 fairyType;

    if (this->timer > 0) {
        this->timer--;
    } else {
        switch (params) {
            case 0x40:
                fairyType = FAIRY_HEAL_BIG;
                break;
            case 0x41:
                fairyType = FAIRY_HEAL_BIG;
                break;
        }

        //! @bug fairyType may be uninitialized
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.home.pos.x, this->actor.home.pos.y,
                    this->actor.home.pos.z, 0, 0, 0, fairyType);

        Actor_delete(&this->actor);
    }
}

static void move_waitcamera(ShotSun* this, PlayState* play) {
    if ((getAttentionDemoPart() == this->actor.category) || (this->timer != 0)) {
        this->actionFunc = move_appeare;
        this->timer = 50;

        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_KANKYO, this->actor.home.pos.x, this->actor.home.pos.y,
                    this->actor.home.pos.z, 0, 0, 0, DEMOKANKYO_SPARKLES);

        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_TRE_BOX_APPEAR);
    }
}

void move_ocarina(ShotSun* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 params = PARAMS_GET_U(this->actor.params, 0, 8);

    if (Math3DLengthSquare(&this->actor.world.pos, &player->actor.world.pos) > SQ(150.0f)) {
        this->fairySpawnerState = SPAWNER_OUT_OF_RANGE;
    } else {
        if (this->fairySpawnerState == SPAWNER_OUT_OF_RANGE) {
            if (!(player->stateFlags2 & PLAYER_STATE2_24)) {
                player->stateFlags2 |= PLAYER_STATE2_23;
                return;
            } else {
                this->fairySpawnerState = SPAWNER_OCARINA_START;
            }
        }
        if (this->fairySpawnerState == SPAWNER_OCARINA_START) {
            ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
            this->fairySpawnerState = SPAWNER_OCARINA_PLAYING;
        } else if (this->fairySpawnerState == SPAWNER_OCARINA_PLAYING && play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            if ((params == 0x40 && play->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS) ||
                (params == 0x41 && play->msgCtx.lastPlayedSong == OCARINA_SONG_STORMS)) {
                this->actionFunc = move_waitcamera;
                makeActorAttentionDemo(play, &this->actor);
                this->timer = 0;
            } else {
                this->fairySpawnerState = SPAWNER_OUT_OF_RANGE;
            }
            this->fairySpawnerState = SPAWNER_OUT_OF_RANGE;
        }
    }
}

static void move_wait(ShotSun* this, PlayState* play) {
    Vec3s cylinderPos;
    Player* player = GET_PLAYER(play);
    EnItem00* collectible;
    s32 pad;

    if (this->collider.base.acFlags & AC_HIT) {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        PRINTF(VT_FGCOL(CYAN) "SHOT_SUN HIT!!!!!!!\n" VT_RST);
        if (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_ETCETERA, 700.0f, -800.0f, 7261.0f, 0, 0, 0, 7);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gLakeHyliaFireArrowsCS);
            if (1) {}
            z_common_data.cutsceneTrigger = 1;
        } else {
            Vec3f spawnPos;

            spawnPos.x = 700.0f;
            spawnPos.y = -800.0f;
            spawnPos.z = 7261.0f;

            collectible = Item_set0(play, &spawnPos, ITEM00_MAGIC_LARGE);
            if (collectible != NULL) {
                collectible->despawnTimer = 6000;
                collectible->actor.speed = 0.0f;
            }
        }
        Actor_delete(&this->actor);
    } else {
        if (!(this->actor.xzDistToPlayer > 120.0f) && z_common_data.save.dayTime >= CLOCK_TIME(6, 30) &&
            z_common_data.save.dayTime < CLOCK_TIME(7, 30)) {
            cylinderPos.x = player->bodyPartsPos[PLAYER_BODYPART_HEAD].x + play->envCtx.sunPos.x * (1.0f / 6.0f);
            cylinderPos.y =
                player->bodyPartsPos[PLAYER_BODYPART_HEAD].y - 30.0f + play->envCtx.sunPos.y * (1.0f / 6.0f);
            cylinderPos.z = player->bodyPartsPos[PLAYER_BODYPART_HEAD].z + play->envCtx.sunPos.z * (1.0f / 6.0f);

            this->hitboxPos = cylinderPos;

            CollisionCheck_Uty_SetPipeC_s(&this->collider, &cylinderPos);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void Shot_Sun_Actor_move(Actor* thisx, PlayState* play) {
    ShotSun* this = (ShotSun*)thisx;

    this->actionFunc(this, play);
}
