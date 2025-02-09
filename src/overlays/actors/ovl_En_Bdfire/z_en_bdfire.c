/*
 * File: z_en_bdfire.c
 * Overlay: ovl_En_Bdfire
 * Description: King Dodongo's Fire Breath
 */

#include "z_en_bdfire.h"
#include "assets/objects/object_kingdodongo/object_kingdodongo.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Bdfire_actor_ct(Actor* thisx, PlayState* play);
void En_Bdfire_actor_dt(Actor* thisx, PlayState* play);
void En_Bdfire_actor_move(Actor* thisx, PlayState* play);
void En_Bdfire_actor_draw(Actor* thisx, PlayState* play);

void Bdfire_draw(EnBdfire* this, PlayState* play);
void mode_mouse(EnBdfire* this, PlayState* play);
void mode_head(EnBdfire* this, PlayState* play);

ActorProfile En_Bdfire_Profile = {
    /**/ 0,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_KINGDODONGO,
    /**/ sizeof(EnBdfire),
    /**/ En_Bdfire_actor_ct,
    /**/ En_Bdfire_actor_dt,
    /**/ En_Bdfire_actor_move,
    /**/ En_Bdfire_actor_draw,
};

void En_Bdfire_actor_set_process(EnBdfire* this, EnBdfireActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Bdfire_actor_set_D_process(EnBdfire* this, EnBdfireDrawFunc drawFunc) {
    this->drawFunc = drawFunc;
}

void En_Bdfire_actor_ct(Actor* thisx, PlayState* play) {
    EnBdfire* this = (EnBdfire*)thisx;
    s32 pad;

    Actor_set_scale(&this->actor, 0.6f);
    En_Bdfire_actor_set_D_process(this, Bdfire_draw);
    if (this->actor.params < 0) {
        En_Bdfire_actor_set_process(this, mode_mouse);
        this->actor.scale.x = 2.8f;
        this->unk_154 = 90;
        Light_point_ct(&this->lightInfoNoGlow, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 255, 300);
        this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfoNoGlow);
    } else {
        En_Bdfire_actor_set_process(this, mode_head);
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
        this->actor.speed = 30.0f;
        this->unk_154 = (25 - (s32)(this->actor.params * 0.8f));
        if (this->unk_154 < 0) {
            this->unk_154 = 0;
        }
        this->unk_188 = 4.2000003f - (this->actor.params * 0.25f * 0.6f);

        if (this->unk_188 < 0.90000004f) {
            this->unk_188 = 0.90000004f;
        }
        this->unk_18C = 255.0f - (this->actor.params * 10.0f);
        if (this->unk_18C < 20.0f) {
            this->unk_18C = 20.0f;
        }
        this->unk_156 = (fqrand() * 8.0f);
    }
}

void En_Bdfire_actor_dt(Actor* thisx, PlayState* play) {
    EnBdfire* this = (EnBdfire*)thisx;

    if (this->actor.params < 0) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    }
}

void mode_mouse(EnBdfire* this, PlayState* play) {
    BossDodongo* kingDodongo;
    s32 temp;

    kingDodongo = (BossDodongo*)this->actor.parent;
    this->actor.world.pos.x = kingDodongo->firePos.x;
    this->actor.world.pos.y = kingDodongo->firePos.y;
    this->actor.world.pos.z = kingDodongo->firePos.z;
    if (kingDodongo->unk_1E2 == 0) {
        add_calc(&this->actor.scale.x, 0.0f, 1.0f, 0.6f, 0.0f);
        if (add_calc(&this->unk_18C, 0.0f, 1.0f, 20.0f, 0.0f) == 0.0f) {
            Actor_delete(&this->actor);
        }
    } else {
        if (this->unk_154 < 70) {
            add_calc(&this->unk_18C, 128.0f, 0.1f, 1.5f, 0.0f);
            add_calc(&this->unk_190, 255.0f, 1.0f, 3.8249998f, 0.0f);
            add_calc(&this->unk_194, 100.0f, 1.0f, 1.5f, 0.0f);
        }
        if (this->unk_154 == 0) {
            temp = 0;
        } else {
            this->unk_154--;
            temp = this->unk_154;
        }
        if (temp == 0) {
            add_calc(&this->actor.scale.x, 0.0f, 1.0f, 0.3f, 0.0f);
            add_calc(&this->unk_190, 0.0f, 1.0f, 25.5f, 0.0f);
            add_calc(&this->unk_194, 0.0f, 1.0f, 10.0f, 0.0f);
            if (add_calc(&this->unk_18C, 0.0f, 1.0f, 10.0f, 0.0f) == 0.0f) {
                Actor_delete(&this->actor);
            }
        }
        Actor_set_scale(&this->actor, this->actor.scale.x);
        Light_point_color_set(&this->lightInfoNoGlow, this->unk_190, this->unk_194, 0, 300);
    }
}

void mode_head(EnBdfire* this, PlayState* play) {
    s16 quarterTurn;
    Player* player = GET_PLAYER(play);
    BossDodongo* bossDodongo;

    bossDodongo = ((BossDodongo*)this->actor.parent);
    this->unk_158 = bossDodongo->unk_1A2;
    quarterTurn = false;
    if (this->actor.params == 0) {
        Nai_FxFlagEntry(NA_SE_EN_DODO_K_FIRE - SFX_FLAG, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    add_calc(&this->actor.scale.x, this->unk_188, 0.3f, 0.5f, 0.0f);
    Actor_set_scale(&this->actor, this->actor.scale.x);
    if ((this->actor.world.pos.x < -1390.0f) && (this->actor.velocity.x < -10.0f)) {
        this->actor.world.pos.x = -1390.0f;
        quarterTurn = true;
    }
    if ((this->actor.world.pos.x > -390.0f) && (this->actor.velocity.x > 10.0f)) {
        this->actor.world.pos.x = -390.0f;
        quarterTurn = true;
    }
    if ((this->actor.world.pos.z > -2804.0f) && (this->actor.velocity.z > 10.0f)) {
        this->actor.world.pos.z = -2804.0f;
        quarterTurn = true;
    }
    if ((this->actor.world.pos.z < -3804.0f) && (this->actor.velocity.z < -10.0f)) {
        this->actor.world.pos.z = -3804.0f;
        quarterTurn = true;
    }
    if (quarterTurn) {
        if (this->unk_158 == 0) {
            this->actor.world.rot.y += 0x4000;
        } else {
            this->actor.world.rot.y -= 0x4000;
        }
    }
    if (DECR(this->unk_154) == 0) {
        add_calc(&this->unk_18C, 0.0f, 1.0f, 10.0f, 0.0f);
        if (this->unk_18C < 10.0f) {
            Actor_delete(&this->actor);
            return;
        }
    } else if (!player->bodyIsBurning) {
        f32 distToBurn = (this->actor.scale.x * 130.0f) / 4.2000003f;

        if (this->actor.xyzDistToPlayerSq < SQ(distToBurn)) {
            s16 i;

            for (i = 0; i < 18; i++) {
                player->bodyFlameTimers[i] = get_random_timer(0, 200);
            }
            player->bodyIsBurning = true;
            Actor_player_power_damage_AT_set(play, &this->actor, 20.0f, this->actor.world.rot.y, 0.0f, 8);
            PRINTF("POWER\n");
        }
    }
}

void En_Bdfire_actor_move(Actor* thisx, PlayState* play) {
    EnBdfire* this = (EnBdfire*)thisx;

    this->unk_156++;
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
}

void Bdfire_draw(EnBdfire* this, PlayState* play) {
    static void* Bdfire_txt[] = {
        object_kingdodongo_Tex_0264E0, object_kingdodongo_Tex_0274E0, object_kingdodongo_Tex_0284E0,
        object_kingdodongo_Tex_0294E0, object_kingdodongo_Tex_02A4E0, object_kingdodongo_Tex_02B4E0,
        object_kingdodongo_Tex_02C4E0, object_kingdodongo_Tex_02D4E0,
    };
    s16 texIndex;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bdfire.c", 612);
    texIndex = this->unk_156 & 7;
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    texture_z_cld_poly_xlu(play->state.gfxCtx);
    POLY_XLU_DISP = gfx_softsprite_z_prim_cld(POLY_XLU_DISP);
    gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                      ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                      ENVIRONMENT);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 100, (s8)this->unk_18C);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 0);
    gSPSegment(POLY_XLU_DISP++, 8, SEGMENTED_TO_VIRTUAL(Bdfire_txt[texIndex]));
    Matrix_translate(0.0f, 11.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_bdfire.c", 647);
    gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_01D950);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bdfire.c", 651);
}

void En_Bdfire_actor_draw(Actor* thisx, PlayState* play) {
    EnBdfire* this = (EnBdfire*)thisx;

    this->drawFunc(this, play);
}
