/*
 * File: z_en_wall_tubo.c
 * Overlay: ovl_En_Wall_Tubo
 * Description: Bombchu Bowling Alley Bullseyes/Pits
 */

#include "z_en_wall_tubo.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
#include "overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#include "rand.h"
#include "regs.h"
#include "sfx.h"
#include "quake.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "global.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Wall_Tubo_actor_ct(Actor* thisx, PlayState* play);
void En_Wall_Tubo_actor_dt(Actor* thisx, PlayState* play);
void En_Wall_Tubo_actor_move(Actor* thisx, PlayState* play);

static void mode_wait_init(EnWallTubo* this, PlayState* play);
static void mode_wait(EnWallTubo* this, PlayState* play);
static void mode_break(EnWallTubo* this, PlayState* play);

ActorProfile En_Wall_Tubo_Profile = {
    /**/ ACTOR_EN_WALL_TUBO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnWallTubo),
    /**/ En_Wall_Tubo_actor_ct,
    /**/ En_Wall_Tubo_actor_dt,
    /**/ En_Wall_Tubo_actor_move,
    /**/ NULL,
};

void En_Wall_Tubo_actor_ct(Actor* thisx, PlayState* play) {
    EnWallTubo* this = (EnWallTubo*)thisx;

    PRINTF("\n\n");
    // "Wall Target"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 壁のツボ ☆☆☆☆☆ \n" VT_RST);
    this->unk_164 = this->actor.world.pos;
    this->actionFunc = mode_wait_init;
}

void En_Wall_Tubo_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_wait_init(EnWallTubo* this, PlayState* play) {
    Actor* lookForGirl;

    lookForGirl = play->actorCtx.actorLists[ACTORCAT_NPC].head;

    while (lookForGirl != NULL) {
        if (lookForGirl->id != ACTOR_EN_BOM_BOWL_MAN) {
            lookForGirl = lookForGirl->next;
        } else {
            this->chuGirl = (EnBomBowlMan*)lookForGirl;
            break;
        }
    }

    this->actionFunc = mode_wait;
}

static void mode_wait(EnWallTubo* this, PlayState* play) {
    EnBomChu* chu;
    s32 pad;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f chuPosDiff;
    s16 quakeIndex;

    if (this->chuGirl->minigamePlayStatus != 0) {
        if (play->cameraPtrs[CAM_ID_MAIN]->setting == CAM_SET_CHU_BOWLING) {
            chu = (EnBomChu*)play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

            while (chu != NULL) {
                if ((&chu->actor == &this->actor) || (chu->actor.id != ACTOR_EN_BOM_CHU)) {
                    chu = (EnBomChu*)chu->actor.next;
                    continue;
                }

                chuPosDiff.x = chu->actor.world.pos.x - this->actor.world.pos.x;
                chuPosDiff.y = chu->actor.world.pos.y - this->actor.world.pos.y;
                chuPosDiff.z = chu->actor.world.pos.z - this->actor.world.pos.z;

                if (((fabsf(chuPosDiff.x) < 40.0f) || (BREG(2))) && ((fabsf(chuPosDiff.y) < 40.0f) || (BREG(2))) &&
                    (fabsf(chuPosDiff.z) < 40.0f || (BREG(2)))) {
                    this->chuGirl->wallStatus[this->actor.params] = 1;
                    chu->timer = 2;
                    Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                    this->timer = 60;
                    Effect_SS_Bomb2_2_ct(play, &this->explosionCenter, &effVelocity, &effAccel, 200, 40);
                    quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_1);
                    setSpeedQuake(quakeIndex, 0x7FFF);
                    setScaleQuake(quakeIndex, 100, 0, 0, 0);
                    setTimerQuake(quakeIndex, 100);
                    this->actionFunc = mode_break;
                    break;
                }

                chu = (EnBomChu*)chu->actor.next;
            }
        }
    }
}

static void mode_break(EnWallTubo* this, PlayState* play) {
    BgBowlWall* wall;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;

    if ((play->gameplayFrames & 1) == 0) {
        effPos.x = this->explosionCenter.x + rnd_fx(300.0f);
        effPos.y = this->explosionCenter.y + rnd_fx(300.0f);
        effPos.z = this->explosionCenter.z;
        Effect_SS_Bomb2_2_ct(play, &effPos, &effVelocity, &effAccel, 100, 30);
        Effect_Hahen_Kakusan_ct3(play, &effPos, 10.0f, 0, 50, 15, 3, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (this->timer == 0) {
        wall = (BgBowlWall*)this->actor.parent;

        if ((wall != NULL) && (wall->dyna.actor.update != NULL)) {
            wall->isHit = true;
            // "You did it field!" (repeated 5 times)
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            PRINTF(VT_FGCOL(BLUE) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            PRINTF(VT_FGCOL(CYAN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
        }

        Actor_delete(&this->actor);
    }
}

void En_Wall_Tubo_actor_move(Actor* thisx, PlayState* play) {
    EnWallTubo* this = (EnWallTubo*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0, 0, 255, 255, 4, play->state.gfxCtx);
    }
}
