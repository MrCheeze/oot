/*
 * File: z_demo_shd.c
 * Overlay: Demo_Shd
 * Description: Bongo Bongo's Shadow
 */

#include "z_demo_shd.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Shd_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Shd_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Shd_Actor_move(Actor* thisx, PlayState* play);
void Demo_Shd_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(DemoShd* this, PlayState* play);

ActorProfile Demo_Shd_Profile = {
    /**/ ACTOR_DEMO_SHD,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(DemoShd),
    /**/ Demo_Shd_Actor_ct,
    /**/ Demo_Shd_Actor_dt,
    /**/ Demo_Shd_Actor_move,
    /**/ Demo_Shd_Actor_draw,
};

#include "assets/overlays/ovl_Demo_Shd/z_demo_shd.c"

void Demo_Shd_actor_set_process(DemoShd* this, DemoShdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Demo_Shd_Actor_ct(Actor* thisx, PlayState* play) {
    DemoShd* this = (DemoShd*)thisx;

    this->unk_14C = 0;
    Demo_Shd_actor_set_process(this, move_wait);
    Actor_set_scale(&this->actor, 0.4f);
    this->actor.world.pos.y = 0.0f;
    this->actor.world.pos.x = 0.0f;
}

void Demo_Shd_Actor_dt(Actor* thisx, PlayState* play) {
}

static void move_wait(DemoShd* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[0] != NULL) ||
        (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[1] != NULL)) {
        if (play->csCtx.curFrame == 800) {
            Na_StartDemoSe(SEQ_CS_EFFECTS_BONGO_EMERGES);
        }
        if (play->csCtx.curFrame == 1069) {
            Na_StartDemoSe(SEQ_CS_EFFECTS_BONGO_HOVER);
        }
    }

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[0] != NULL) {
            if (play->csCtx.actorCues[0]->id == 2) {
                if (!(this->unk_14C & 1)) {
                    this->unk_14E = play->csCtx.actorCues[0]->startPos.x;
                }
                this->unk_14C |= 1;
            } else {
                this->unk_14C &= ~1;
            }
        }
    }

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[1] != NULL) {
            if (play->csCtx.actorCues[1]->id == 2) {
                if (!(this->unk_14C & 2)) {
                    this->unk_14E = play->csCtx.actorCues[1]->startPos.x;
                }
                this->unk_14C |= 2;
            } else {
                this->unk_14C &= ~2;
            }
        }
    }

    this->unk_14E++;
}

void Demo_Shd_Actor_move(Actor* thisx, PlayState* play) {
    DemoShd* this = (DemoShd*)thisx;

    this->actionFunc(this, play);
}

void Demo_Shd_Actor_draw(Actor* thisx, PlayState* play) {
    DemoShd* this = (DemoShd*)thisx;
    s32 pad;
    u32 unk_14E = this->unk_14E;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_shd.c", 726);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_shd.c", 729);
    gSPDisplayList(POLY_XLU_DISP++, demo_shd_stoker_modelT0);

    if (this->unk_14C & 1) {
        gSPDisplayList(POLY_XLU_DISP++,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0x3FF - ((unk_14E * 5) & 0x3FF), 16,
                                        256, 1, 0, 255 - ((unk_14E * 5) & 255), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, demo_shd_stoker_modelT1);
    } else if (this->unk_14C & 2) {
        gSPDisplayList(POLY_XLU_DISP++,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0x3FF - ((unk_14E * 5) & 0x3FF), 16,
                                        256, 1, 0, 255 - ((unk_14E * 5) & 255), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, demo_shd_stoker_modelT2);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_shd.c", 762);
}
