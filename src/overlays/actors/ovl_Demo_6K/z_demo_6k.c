/*
 * File: z_demo_6k.c
 * Overlay: ovl_Demo_6K
 * Description: Sages, balls of light (cutscene)
 */

#include "z_demo_6k.h"
#include "terminal.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_demo_6k/object_demo_6k.h"
#include "assets/objects/object_gnd_magic/object_gnd_magic.h"
#include "overlays/actors/ovl_Eff_Dust/z_eff_dust.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Demo_6k_Actor_ct(Actor* thisx, PlayState* play);
void Demo_6k_Actor_dt(Actor* thisx, PlayState* play);
void Demo_6k_Actor_move(Actor* thisx, PlayState* play);

static void move_wait(Demo6K* this, PlayState* play);
void move_6k_light_wait(Demo6K* this, PlayState* play);
void move_6k_light(Demo6K* this, PlayState* play);
void move_6k_hole(Demo6K* this, PlayState* play);
void move_6k_flash(Demo6K* this, PlayState* play);
void move_6k_flash_end(Demo6K* this, PlayState* play);
void move_tr_flash(Demo6K* this, PlayState* play);
void move_na_flash(Demo6K* this, PlayState* play);
void set_6k_wall(Demo6K* this, s32 i);
void move_6k_wall(Demo6K* this, PlayState* play);
void move_gnd_magic2(Demo6K* this, PlayState* play);
void move_gnd_magic(Demo6K* this, PlayState* play);
void Demo_6k_light_draw(Actor* thisx, PlayState* play);
void Demo_6k_hole_draw(Actor* thisx, PlayState* play);
void Demo_6k_flash_draw(Actor* thisx, PlayState* play);
void Demo_6k_wall_draw(Actor* thisx, PlayState* play2);
void Demo_gnd_magic_draw(Actor* thisx, PlayState* play);
void Demo_gnd_magic2_draw(Actor* thisx, PlayState* play);
void Demo6kMoveProc_local(Demo6K* this, PlayState* play, s32 cueChannel);

ActorProfile Demo_6K_Profile = {
    /**/ ACTOR_DEMO_6K,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(Demo6K),
    /**/ Demo_6k_Actor_ct,
    /**/ Demo_6k_Actor_dt,
    /**/ Demo_6k_Actor_move,
    /**/ NULL,
};

static s16 demo_effect_shape_data[] = {
    OBJECT_GAMEPLAY_KEEP, OBJECT_DEMO_6K,       OBJECT_DEMO_6K,       OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GND_MAGIC,     OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
};
static Color_RGB8 fct[] = {
    { 255, 50, 0 }, { 0, 200, 0 }, { 200, 255, 0 }, { 200, 50, 255 }, { 255, 150, 0 }, { 0, 150, 255 },
};
static f32 table[] = { 1.0f, 1.04f, 1.0f, 0.96f };
static f32 yuragi[] = { 1.1f, 1.0f, 0.9f, 0.8f };

void Demo_6k_actor_set_process(Demo6K* this, Demo6KActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Demo_6k_Actor_ct(Actor* thisx, PlayState* play) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    s32 params = this->actor.params;
    s32 objectSlot;
    s32 i;

    PRINTF("no = %d\n", params);

    if (demo_effect_shape_data[params] != OBJECT_GAMEPLAY_KEEP) {
        objectSlot = Object_Exchange_bank_check(&play->objectCtx, demo_effect_shape_data[params]);
    } else {
        objectSlot = 0;
    }

    PRINTF("bank_ID = %d\n", objectSlot);

    if (objectSlot < 0) {
        ASSERT(0, "0", "../z_demo_6k.c", 334);
    } else {
        this->requiredObjectSlot = objectSlot;
    }

    Demo_6k_actor_set_process(this, move_wait);
    this->timer1 = 0;
    this->flags = 0;
    this->timer2 = 0;

    switch (params) {
        case 0:
            this->drawFunc = Demo_6k_wall_draw;
            this->initActionFunc = move_6k_wall;
            Actor_set_scale(&this->actor, 1.0f);

            for (i = 0; i < 16; i++) {
                set_6k_wall(this, i);
                this->unk_1B4[i] = 0.0f;
            }

            this->unk_170 = 0.0f;
            break;
        case 1:
            this->drawFunc = Demo_6k_light_draw;
            this->initActionFunc = move_6k_light_wait;
            Actor_set_scale(&this->actor, 0.228f);
            break;
        case 2:
            this->drawFunc = Demo_6k_hole_draw;
            this->initActionFunc = move_6k_hole;
            Actor_set_scale(&this->actor, 0.1f);
            this->unk_164 = 1.0f;
            this->unk_168 = 1.0f;
            this->unk_16C = 0.0f;
            this->unk_170 = 0.0f;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            this->drawFunc = Demo_6k_flash_draw;
            this->initActionFunc = move_6k_flash;
            Actor_set_scale(&this->actor, 0.0f);
            this->unk_293 = params - 3;
            break;
        case 9:
        case 10:
            this->drawFunc = Demo_6k_flash_draw;
            Actor_set_scale(&this->actor, 0.0f);
            this->initActionFunc = move_tr_flash;
            break;
        case 11:
            this->drawFunc = Demo_6k_flash_draw;
            Actor_set_scale(&this->actor, 0.0f);
            this->initActionFunc = move_na_flash;
            this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
            Actor_SE_set(&this->actor, NA_SE_EV_NABALL_VANISH);
            break;
        case 12:
            Actor_set_scale(&this->actor, 0.0f);
            this->initActionFunc = move_gnd_magic;
            this->drawFunc = Demo_gnd_magic_draw;
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ITEMACTION);
            break;
        case 13:
            Actor_set_scale(&this->actor, 0.14f);
            Demo_6k_actor_set_process(this, move_gnd_magic2);
            this->actor.draw = Demo_gnd_magic2_draw;
            this->unk_293 = 0;
            break;
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
            this->drawFunc = Demo_6k_flash_draw;
            this->initActionFunc = move_6k_flash_end;
            this->flags |= 1;
            Actor_set_scale(&this->actor, 0.2f);
            this->unk_293 = params - 14;
            break;
        default:
            ASSERT(0, "0", "../z_demo_6k.c", 435);
            break;
    }

    switch (params) {
        case 9:
            this->unk_293 = 0;
            break;
        case 10:
            this->unk_293 = 5;
            break;
        case 11:
            this->unk_293 = 4;
            break;
    }

    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 255, 255, 100);
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
}

void Demo_6k_Actor_dt(Actor* thisx, PlayState* play) {
    Demo6K* this = (Demo6K*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
}

static void move_wait(Demo6K* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.draw = this->drawFunc;
        this->actionFunc = this->initActionFunc;
    }
}

void move_6k_light_wait(Demo6K* this, PlayState* play) {
    if (play->csCtx.curFrame > 214) {
        Actor_fix_level_SE_set(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
    }

    if (play->csCtx.curFrame > 264) {
        Actor_fix_level_SE_set(&this->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2)) {
        Demo_6k_actor_set_process(this, move_6k_light);
    }
}

void move_6k_light(Demo6K* this, PlayState* play) {
    if (play->csCtx.curFrame < 353) {
        Actor_fix_level_SE_set(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
        Actor_fix_level_SE_set(&this->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
    }

    if (play->csCtx.curFrame == 342) {
        Na_StartDemoSe(SEQ_CS_EFFECTS_SAGE_SEAL);
    }

    if (this->timer1 == 39) {
        Na_StartFixSe_F(NA_SE_EV_CONSENTRATION);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_6K, this->actor.world.pos.x, this->actor.world.pos.y + 10.0f,
                    this->actor.world.pos.z, 0, 0, 0, 2);
    }

    if (this->timer1 == 64) {
        Actor_delete(&this->actor);
    }

    this->timer1++;
}

void move_6k_hole(Demo6K* this, PlayState* play) {
    if (this->timer1 < 5) {
        this->unk_168 = table[this->timer1 & 3];
    } else if (this->timer1 < 15) {
        this->actor.scale.x += 0.012f;
        Actor_set_scale(&this->actor, this->actor.scale.x);
        chase_f(&this->unk_170, 0.6f, 0.05f);
        this->unk_168 = 1.0f;
    } else {
        if (this->timer1 == 15) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EFF_DUST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, EFF_DUST_TYPE_1);
        }
        chase_f(&this->unk_16C, 1.0f, 0.02f);
        this->unk_168 = table[this->timer1 & 1];
    }

    this->timer1++;
}

void move_6k_flash2(Demo6K* this, PlayState* play) {
    this->timer2++;

    if (this->timer1 < 10) {
        this->timer1++;
    } else if (this->actor.scale.x > 0.0f) {
        this->actor.scale.x -= 1.0f / 120.0f;
        Actor_set_scale(&this->actor, this->actor.scale.x);
    } else {
        Actor_delete(&this->actor);
    }
}

void move_6k_flash(Demo6K* this, PlayState* play) {
    static u16 se_timing[] = { 275, 275, 275, 275, 275, 275 };
    u32 frames = play->state.frames;

    if (this->actor.scale.x < 0.1f) {
        this->actor.scale.x += 0.0017f;
    } else if (frames & 1) {
        this->actor.scale.x = 0.1f * 1.04f;
    } else {
        this->actor.scale.x = 0.1f;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2)) {
        Demo_6k_actor_set_process(this, move_6k_flash2);
        this->timer1 = 0;
        this->actor.scale.x = 0.1f;
    }

    Actor_set_scale(&this->actor, this->actor.scale.x);

    this->timer2++;

    if ((play->sceneId == SCENE_INSIDE_GANONS_CASTLE) && (play->csCtx.curFrame < se_timing[this->actor.params - 3])) {
        Actor_level_SE_set(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
    }
}

void ending_kirakira(Demo6K* this, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, 0.0f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 255, 150, 0, 0 };
    Vec3f pos;
    s16 rand1;
    s16 rand2;
    s32 scale;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;

    rand1 = rnd_f(0xFFFF);
    rand2 = rnd_f(0xFFFF);

    kirakira_vec.x = sin_s(rand2) * cos_s(rand1) * 20.0f;
    kirakira_vec.z = cos_s(rand2) * cos_s(rand1) * 20.0f;
    kirakira_vec.y = sin_s(rand1) * 20.0f;

    kirakira_acc.y = 0.0f;

    kirakira_env.r = fct[this->unk_293].r;
    kirakira_env.g = fct[this->unk_293].g;
    kirakira_env.b = fct[this->unk_293].b;

    if (play->sceneId == SCENE_TEMPLE_OF_TIME) {
        scale = 6000;
    } else if (play->csCtx.curFrame < 419) {
        scale = 6000;
    } else {
        scale = 18000;
    }

    Effect_SS_KiraKira_soul_sc_ct_ct(play, &pos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env, scale, 20);
}

void move_6k_flash_end(Demo6K* this, PlayState* play) {
    s32 cueChannel = this->actor.params - 14;

    this->timer2++;

    Actor_set_scale(&this->actor, 0.2f);

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[cueChannel] != NULL)) {
        Demo6kMoveProc_local(this, play, cueChannel);

        if (play->csCtx.actorCues[cueChannel]->id == 3) {
            this->flags &= ~1;
            ending_kirakira(this, play);
        } else {
            this->flags |= 1;
        }
    } else {
        this->flags |= 1;
    }
}

void move_tr_flash(Demo6K* this, PlayState* play) {
    u32 frames = play->state.frames;

    if (this->actor.scale.x < 0.05f) {
        this->actor.scale.x += 0.005f;
    } else if (frames & 1) {
        this->actor.scale.x = 0.05f * 1.04f;
    } else {
        this->actor.scale.x = 0.05f;
    }

    Actor_set_scale(&this->actor, this->actor.scale.x);

    this->timer2++;

    if (this->timer2 > 47) {
        Actor_delete(&this->actor);
    } else if (this->timer2 > 39) {
        f32 dTimer = this->timer2 - 39;
        f32 temp = 1.0f / (9.0f - dTimer);

        this->actor.world.pos.x += (-1611.0f - this->actor.world.pos.x) * temp;
        this->actor.world.pos.y += (19.0f - this->actor.world.pos.y) * temp;
        this->actor.world.pos.z += (1613.0f - this->actor.world.pos.z) * temp;

        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_FIRE - SFX_FLAG);
    }

    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, fct[this->unk_293].r, fct[this->unk_293].g,
                              fct[this->unk_293].b, this->actor.scale.x * 4000.0f);
}

void demo_6k_kirakira(Demo6K* this, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, 0.0f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 255, 150, 0, 0 };
    Vec3f pos;
    f32 temp = this->actor.scale.x * 500.0f;
    s32 i;

    for (i = 0; i < 8; i++) {
        pos.x = this->actor.world.pos.x + rnd_fx(temp);
        pos.y = this->actor.world.pos.y + rnd_fx(temp);
        pos.z = this->actor.world.pos.z + rnd_fx(temp);

        kirakira_vec.x = rnd_fx(2.0f);
        kirakira_vec.y = (rnd_f(-10.0f) - 5.0f) * 0.1f;
        kirakira_vec.z = rnd_fx(2.0f);

        kirakira_acc.y = 0.0f;

        Effect_SS_KiraKira_soul_sc_ct_ct(play, &pos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env, 500, 20);
    }
}

void move_na_flash(Demo6K* this, PlayState* play) {
    u32 frames = play->state.frames;

    this->timer2++;

    if (this->timer2 > 24) {
        Actor_delete(&this->actor);
    } else if (this->timer2 > 4) {
        this->actor.velocity.x += rnd_fx(0.2f);
        this->actor.velocity.y += 0.12f;
        this->actor.velocity.z += rnd_fx(0.2f);

        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;

        this->actor.scale.x -= 0.0015f;

        demo_6k_kirakira(this, play);
    } else if (frames & 1) {
        this->actor.scale.x = 0.033f;
    } else {
        this->actor.scale.x = 0.03f;
    }

    Actor_set_scale(&this->actor, this->actor.scale.x);
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, fct[this->unk_293].r, fct[this->unk_293].g,
                              fct[this->unk_293].b, this->actor.scale.x * 4000.0f);
}

void set_6k_wall(Demo6K* this, s32 i) {
    this->unk_174[i] = (i * 10.0f) - 80.0f;
    this->unk_1B4[i] = rnd_f(100.0f) + 100.0f;
    this->unk_1F4[i] = -3.0f - rnd_f(6.0f);
    this->unk_274[i] = (s32)rnd_f(6.0f);
    this->unk_234[i] = rnd_f(0.02f) + 0.01f;
}

void move_6k_wall(Demo6K* this, PlayState* play) {
    s32 i;

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[1] != NULL)) {
        if (play->csCtx.actorCues[1]->id == 2) {
            this->unk_170++;
            Actor_fix_level_SE_set(&this->actor, NA_SE_EV_RAINBOW_SHOWER - SFX_FLAG);
        }

        Demo6kMoveProc_local(this, play, 1);
    }

    for (i = 0; (i < (s32)this->unk_170) && (i < 16); i++) {
        this->unk_1B4[i] += this->unk_1F4[i];
        if (this->unk_1B4[i] < 0.0f) {
            set_6k_wall(this, i);
        }
    }

    this->timer1++;
}

void gnd_magic_kirakira(Player* player, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, 0.0f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 255, 200, 0, 0 };
    Vec3f pos;
    s32 i;

    for (i = 0; i < 150; i++) {
        pos.x = rnd_fx(15.0f) + player->actor.world.pos.x;
        pos.y = rnd_fx(15.0f) + player->actor.world.pos.y + 30.0f;
        pos.z = rnd_fx(15.0f) + player->actor.world.pos.z;

        kirakira_vec.x = rnd_fx(8.0f) + 1.0f;
        kirakira_vec.y = rnd_fx(4.0f);
        kirakira_vec.z = rnd_fx(8.0f) + 2.0f;

        kirakira_acc.y = 0.0f;

        Effect_SS_KiraKira_soul_sc_ct_ct(play, &pos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env, 1000,
                                      (s32)rnd_f(60.0f) + 60);
    }
}

void move_gnd_magic2(Demo6K* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EN_GANON_ATTACK_DEMO - SFX_FLAG);

    this->timer2++;

    if (this->timer2 > 44) {
        if (this->unk_293 < 236) {
            this->unk_293 += 20;
        } else {
            this->unk_293 = 255;
        }

        if (this->timer2 > 104) {
            gnd_magic_kirakira(GET_PLAYER(play), play);
            Actor_delete(&this->actor);
            Actor_SE_set(&GET_PLAYER(play)->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
        } else if (this->timer2 > 94) {
            Actor_set_scale(&this->actor, this->actor.scale.x + 0.03f);

            if (this->timer2 == 95) {
                PRINTF(VT_FGCOL(CYAN) "  NA_SE_EN_GANON_FIRE_DEMO\n" VT_RST);
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_FIRE_DEMO);
            }
        }

        Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 200, 0, this->unk_293);
    }
}

void move_gnd_magic(Demo6K* this, PlayState* play) {
    if (this->timer2 == 0) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_6K, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 13);
    }

    this->timer2++;

    if (this->timer2 > 60) {
        Actor_delete(&this->actor);
    }

    Actor_set_scale(&this->actor, 0.05f - (this->timer2 * 0.00075f));
}

void Demo_6k_Actor_move(Actor* thisx, PlayState* play) {
    Demo6K* this = (Demo6K*)thisx;

    this->actionFunc(this, play);
}

void Demo_6k_light_draw(Actor* thisx, PlayState* play) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    u16 timer1 = this->timer1;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1070);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_rotateX(-M_PI / 2, MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0x7FFF - ((timer1 * 8) & 0x7FFF), 16, 512, 1, 0,
                                0x7FFF - ((timer1 * 8) & 0x7FFF), 16, 32));

    {
        s32 i;
        s32 j;
        Color_RGB8 colors[12] = {
            { 255, 170, 255 }, { 255, 0, 100 }, { 255, 255, 170 }, { 0, 255, 0 },   { 255, 255, 170 }, { 255, 255, 0 },
            { 255, 170, 255 }, { 50, 0, 255 },  { 255, 255, 170 }, { 255, 100, 0 }, { 170, 255, 255 }, { 0, 100, 255 },
        };

        Matrix_rotateZ(-M_PI / 2, MTXMODE_APPLY);

        for (i = 0, j = 0; i < 6; i++, j += 2) {
            Matrix_rotateZ(M_PI / 3, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1115);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, colors[j + 0].r, colors[j + 0].g, colors[j + 0].b, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, colors[j + 1].r, colors[j + 1].g, colors[j + 1].b, 255);
            gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_0022B0);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1127);
}

void Demo_6k_hole_draw(Actor* thisx, PlayState* play) {
    static u8 ani_vtx_table[] = { 6, 7, 11, 16, 20, 24, 28, 33, 35, 41, 45, 50, 57, 58, 62, 255 };
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    u32 timer1 = this->timer1;
    f32 scale = this->unk_164 * this->unk_168;
    Vtx* vertices = SEGMENTED_TO_VIRTUAL(object_demo_6kVtx_0035E0);
    s32 i;
    s32 i2;
    u8 alpha;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1145);

    alpha = (s32)(this->unk_170 * 255.0f);
    POLY_XLU_DISP = fill_rectangle_prim_free(POLY_XLU_DISP);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, alpha);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
    gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    alpha = (s32)(this->unk_16C * 255.0f);
    for (i2 = 0, i = 0; i < 63; i++) {
        if (i == ani_vtx_table[i2]) {
            i2++;
        } else {
            vertices[i].v.cn[3] = alpha;
        }
    }

    Matrix_rotateX(-M_PI / 2, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1170);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 210, 210, 210, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 100, 100, 100, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (0xFFF - (timer1 * 6)) & 0xFFF,
                                (timer1 * 12) & 0xFFF, 128, 64, 1, (0xFFF - (timer1 * 6)) & 0xFFF,
                                (timer1 * 12) & 0xFFF, 64, 32));
    gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_0039D0);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1189);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 50, 50, 50, 255);
    gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_001040);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1198);
}

void Demo_6k_flash_draw(Actor* thisx, PlayState* play) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    Gfx* displayList;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1208);

    if (!(this->flags & 1)) {
        if (this->actor.params > 8) {
            displayList = gEffFlash1DL;
        } else {
            displayList = gEffFlash2DL;
        }

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, fct[this->unk_293].r, fct[this->unk_293].g,
                       fct[this->unk_293].b, 255);
        Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_push();
        Matrix_rotateZ(DEG_TO_RAD(this->timer2 * 6), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1230);
        gSPDisplayList(POLY_XLU_DISP++, displayList);
        Matrix_pull();
        Matrix_rotateZ(DEG_TO_RAD(-(f32)(this->timer2 * 6)), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1236);
        gSPDisplayList(POLY_XLU_DISP++, displayList);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1242);
}

void Demo_6k_wall_draw(Actor* thisx, PlayState* play2) {
    Demo6K* this = (Demo6K*)thisx;
    PlayState* play = play2;
    u32 frames = play->state.frames;
    s32 i;

    if ((i = (play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[1] != NULL)) &&
        (play->csCtx.actorCues[1]->id != 1)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1277);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000)), MTXMODE_APPLY);

        for (i = 0; i < 16; i++) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, fct[this->unk_274[i]].r, fct[this->unk_274[i]].g,
                           fct[this->unk_274[i]].b, 255);
            Matrix_push();
            Matrix_translate(this->unk_174[i], this->unk_1B4[i], 0.0f, MTXMODE_APPLY);
            Matrix_scale(this->unk_234[i] * yuragi[(frames + i) & 3],
                         this->unk_234[i] * yuragi[(frames + i) & 3],
                         this->unk_234[i] * yuragi[(frames + i) & 3], MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1297);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
            Matrix_pull();
        }

        gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1305);
    }
}

void Demo_gnd_magic_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Demo6K* this = (Demo6K*)thisx;
    u32 timer2 = this->timer2;
    u8 kirakira_prim[4];
    u8 kirakira_env[3];

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1316);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1322);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0xFF - ((timer2 * 2) & 0xFF), 0, 32, 32, 1,
                                0xFF - ((timer2 * 2) & 0xFF), (timer2 * 15) & 0x3FF, 16, 64));

    if (this->timer2 < 40) {
        kirakira_prim[0] = kirakira_prim[2] = 100 - (this->timer2 * 2.5f);
        kirakira_prim[1] = kirakira_env[1] = 0;
        kirakira_env[0] = 100 - kirakira_prim[2];
        kirakira_prim[3] = this->timer2 * 6.375f;
        kirakira_env[2] = kirakira_env[0] * 2;
    } else if (this->timer2 < 50) {
        kirakira_prim[0] = (this->timer2 * 5) - 200;
        kirakira_prim[1] = kirakira_prim[2] = 0;
        kirakira_prim[3] = 255;
        kirakira_env[0] = 100 - kirakira_prim[0];
        kirakira_env[1] = kirakira_prim[0] * 2;
        kirakira_env[2] = 200 - (kirakira_prim[0] * 4);
    } else {
        kirakira_prim[2] = (this->timer2 * 5) - 250;
        kirakira_env[2] = 0;
        kirakira_prim[1] = kirakira_prim[2] * 3;
        kirakira_prim[3] = 255;
        kirakira_prim[0] = kirakira_env[0] = (kirakira_prim[2] * 2) + 50;
        kirakira_env[1] = 100 - kirakira_prim[2];
    }

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, kirakira_prim[0], kirakira_prim[1], kirakira_prim[2], kirakira_prim[3]);
    gDPSetEnvColor(POLY_XLU_DISP++, kirakira_env[0], kirakira_env[1], kirakira_env[2], 128);
    gSPDisplayList(POLY_XLU_DISP++, object_gnd_magic_DL_001190);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1368);
}

void Demo_gnd_magic2_draw(Actor* thisx, PlayState* play) {
    static u8 yuragi_table[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1 };
    Demo6K* this = (Demo6K*)thisx;
    Gfx* displayList = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Gfx));
    u16 frames = play->gameplayFrames;
    f32 scaleFactor;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1386);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    scaleFactor = ((s16)yuragi_table[(frames * 4) & 0xF] * 0.01f) + 1.0f;
    Matrix_scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                 this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_6k.c", 1394);
    gSPSegment(POLY_XLU_DISP++, 0x08, displayList);
    gDPPipeSync(displayList++);
    gDPSetPrimColor(displayList++, 0, 0x80, 255, 255, 255, this->unk_293);
    gDPSetRenderMode(displayList++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
    gSPEndDisplayList(displayList);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 255);
    gSPDisplayList(POLY_XLU_DISP++, gGlowCircleSmallDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_6k.c", 1411);
}

void Demo6kMoveProc_local(Demo6K* this, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    f32 temp;
    CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

    startPos.x = cue->startPos.x;
    startPos.y = cue->startPos.y;
    startPos.z = cue->startPos.z;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    temp = get_parcent(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    this->actor.world.pos.x = LERP(startPos.x, endPos.x, temp);
    this->actor.world.pos.y = LERP(startPos.y, endPos.y, temp);
    this->actor.world.pos.z = LERP(startPos.z, endPos.z, temp);
}
