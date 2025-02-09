#include "z_en_ex_ruppy.h"
#include "terminal.h"
#include "../ovl_En_Diving_Game/z_en_diving_game.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Ex_Ruppy_actor_ct(Actor* thisx, PlayState* play);
void En_Ex_Ruppy_actor_dt(Actor* thisx, PlayState* play);
void En_Ex_Ruppy_actor_move(Actor* thisx, PlayState* play);
void En_Ex_Ruppy_actor_disp(Actor* thisx, PlayState* play);

void mode_zora_game_first_down(EnExRuppy* this, PlayState* play);
void mode_heishi_trap(EnExRuppy* this, PlayState* play);
void mode_nomal_ruppy(EnExRuppy* this, PlayState* play);
void mode_syateki_hit_ruppy(EnExRuppy* this, PlayState* play);
void mode_zora_game_stop_wait(EnExRuppy* this, PlayState* play);
void mode_drop_land_check(EnExRuppy* this, PlayState* play);
void mode_player_hit_check(EnExRuppy* this, PlayState* play);
static void mode_clear(EnExRuppy* this, PlayState* play);

static s16 Ex_Ruppy_Data[] = {
    ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE, ITEM00_RUPEE_RED, ITEM00_RUPEE_ORANGE, ITEM00_RUPEE_PURPLE,
};

// Unused, as the function sets these directly
static s16 Ex_Ruppy_Get_Data[] = {
    1, 5, 20, 500, 50,
};

ActorProfile En_Ex_Ruppy_Profile = {
    /**/ ACTOR_EN_EX_RUPPY,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnExRuppy),
    /**/ En_Ex_Ruppy_actor_ct,
    /**/ En_Ex_Ruppy_actor_dt,
    /**/ En_Ex_Ruppy_actor_move,
    /**/ En_Ex_Ruppy_actor_disp,
};

void En_Ex_Ruppy_actor_ct(Actor* thisx, PlayState* play) {
    EnExRuppy* this = (EnExRuppy*)thisx;
    EnDivingGame* divingGame;
    f32 temp1;
    f32 temp2;
    s16 temp3;

    this->type = this->actor.params;
    // "Index"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ インデックス ☆☆☆☆☆ %x\n" VT_RST, this->type);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 25.0f);

    switch (this->type) {
        case 0:
            this->unk_160 = 0.01f;
            Actor_set_scale(&this->actor, this->unk_160);
            this->actor.room = -1;
            this->actor.gravity = 0.0f;

            // If you haven't won the diving game before, you will always get 5 rupees
            if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
                this->rupeeValue = 5;
                this->colorIdx = 1;
            } else {
                temp1 = 200.99f;
                if (this->actor.parent != NULL) {
                    divingGame = (EnDivingGame*)this->actor.parent;
                    if (divingGame->actor.update != NULL) {
                        temp2 = divingGame->extraWinCount * 10.0f;
                        temp1 += temp2;
                    }
                }

                temp3 = rnd_f(temp1);
                if ((temp3 >= 0) && (temp3 < 40)) {
                    this->rupeeValue = 1;
                    this->colorIdx = 0;
                } else if ((temp3 >= 40) && (temp3 < 170)) {
                    this->rupeeValue = 5;
                    this->colorIdx = 1;
                } else if ((temp3 >= 170) && (temp3 < 190)) {
                    this->rupeeValue = 20;
                    this->colorIdx = 2;
                } else if ((temp3 >= 190) && (temp3 < 200)) {
                    this->rupeeValue = 50;
                    this->colorIdx = 4;
                } else {
                    this->unk_160 = 0.02f;
                    Actor_set_scale(&this->actor, this->unk_160);
                    this->rupeeValue = 500;
                    this->colorIdx = 3;
                    if (this->actor.parent != NULL) {
                        divingGame = (EnDivingGame*)this->actor.parent;
                        if (divingGame->actor.update != NULL) {
                            divingGame->extraWinCount = 0;
                        }
                    }
                }
            }

            this->actor.shape.shadowScale = 7.0f;
            this->actor.shape.yOffset = 700.0f;
            this->unk_15A = this->actor.world.rot.z;
            this->actor.world.rot.z = 0;
            this->timer = 30;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = mode_zora_game_first_down;
            break;

        case 1:
        case 2: // Giant pink ruppe that explodes when you touch it
            if (this->type == 1) {
                Actor_set_scale(&this->actor, 0.1f);
                this->colorIdx = 4;
            } else {
                Actor_set_scale(thisx, 0.02f);
                this->colorIdx = (s16)rnd_f(3.99f) + 1;
            }
            this->actor.gravity = -3.0f;
            // "Wow Coin"
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ わーなーコイン ☆☆☆☆☆ \n" VT_RST);
            this->actor.shape.shadowScale = 6.0f;
            this->actor.shape.yOffset = 700.0f;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = mode_heishi_trap;
            break;

        case 3: // Spawned by the guard in Hyrule courtyard
            Actor_set_scale(&this->actor, 0.02f);
            this->colorIdx = 0;
            switch ((s16)rnd_f(30.99f)) {
                case 0:
                    this->colorIdx = 2;
                    break;
                case 10:
                case 20:
                case 30:
                    this->colorIdx = 1;
                    break;
            }
            this->actor.gravity = -3.0f;
            // "Normal rupee"
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ノーマルルピー ☆☆☆☆☆ \n" VT_RST);
            this->actor.shape.shadowScale = 6.0f;
            this->actor.shape.yOffset = 700.0f;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = mode_nomal_ruppy;
            break;

        case 4: // Progress markers in the shooting gallery
            this->actor.gravity = -3.0f;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_set_scale(&this->actor, 0.01f);
            this->actor.shape.shadowScale = 6.0f;
            this->actor.shape.yOffset = -700.0f;
            this->actionFunc = mode_syateki_hit_ruppy;
            break;
    }
}

void En_Ex_Ruppy_actor_dt(Actor* thisx, PlayState* play) {
}

void mode_kirakira(EnExRuppy* this, PlayState* play, s16 numSparkles, s32 movementType) {
    static Vec3f kirakira_vec[] = { { 0.0f, 0.1f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    static Vec3f kirakira_acc[] = { { 0.0f, 0.01f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 i;
    s16 scale;
    s16 life;

    if (numSparkles < 1) {
        numSparkles = 1;
    }

    primColor.r = 255;
    primColor.g = 255;
    primColor.b = 0;
    envColor.r = 255;
    envColor.g = 255;
    envColor.b = 255;
    velocity = kirakira_vec[movementType];
    accel = kirakira_acc[movementType];
    scale = 3000;
    life = 16;

    for (i = 0; i < numSparkles; i++) {
        if (movementType == 1) {
            accel.x = rnd_fx(20.0f);
            accel.z = rnd_fx(20.0f);
            scale = 5000;
            life = 20;
        }
        pos.x = (fqrand() - 0.5f) * 10.0f + this->actor.world.pos.x;
        pos.y = (fqrand() - 0.5f) * 10.0f + (this->actor.world.pos.y + this->unk_160 * 600.0f);
        pos.z = (fqrand() - 0.5f) * 10.0f + this->actor.world.pos.z;
        Effect_SS_KiraKira_sc_ct_ct(play, &pos, &velocity, &accel, &primColor, &envColor, scale, life);
    }
}

void mode_zora_game_first_down(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;

    this->actor.shape.rot.y += 0x7A8;
    add_calc2(&this->actor.gravity, -2.0f, 0.3f, 1.0f);
    mode_kirakira(this, play, 2, 0);
    Na_StartSystemSe_F(NA_SE_EV_RAINBOW_SHOWER - SFX_FLAG);
    divingGame = (EnDivingGame*)this->actor.parent;
    if ((divingGame != NULL) && (divingGame->actor.update != NULL) &&
        ((divingGame->unk_296 == 0) || (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) || (this->timer == 0))) {
        this->invisible = true;
        this->actor.speed = 0.0f;
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
        this->actor.gravity = 0.0f;
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_BOMB_DROP_WATER);
        this->actionFunc = mode_zora_game_stop_wait;
    }
}

void mode_zora_game_stop_wait(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame = (EnDivingGame*)this->actor.parent;
    f32 temp_f2;

    if ((divingGame != NULL) && (divingGame->actor.update != NULL) && (divingGame->unk_2A2 == 2)) {
        this->invisible = false;
        this->actor.world.pos.x = ((fqrand() - 0.5f) * 300.0f) + -260.0f;
        this->actor.world.pos.y = ((fqrand() - 0.5f) * 200.0f) + 370.0f;
        temp_f2 = this->unk_15A * -50.0f;
        if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
            temp_f2 += -500.0f;
            this->actor.world.pos.z = ((fqrand() - 0.5f) * 80.0f) + temp_f2;
        } else {
            temp_f2 += -300.0f;
            this->actor.world.pos.z = ((fqrand() - 0.5f) * 60.0f) + temp_f2;
        }
        this->actionFunc = mode_drop_land_check;
        this->actor.gravity = -1.0f;
    }
}

void mode_drop_land_check(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;
    Vec3f pos;
    s32 pad;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 15.0f)) {
        pos = this->actor.world.pos;
        pos.y += this->actor.depthInWater;
        this->actor.velocity.y = -1.0f;
        this->actor.gravity = -0.2f;
        Effect_SS_G_Splash_sc_cl_ct(play, &pos, NULL, NULL, 0, 800);
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_BOMB_DROP_WATER);
        this->actionFunc = mode_player_hit_check;
    }
    divingGame = (EnDivingGame*)this->actor.parent;
    if ((divingGame != NULL) && (divingGame->actor.update != NULL) && (divingGame->phase == ENDIVINGGAME_PHASE_ENDED)) {
        this->timer = 20;
        this->actionFunc = mode_clear;
    }
}

void mode_player_hit_check(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;
    Vec3f D_80A0B388 = { 0.0f, 0.1f, 0.0f };
    Vec3f D_80A0B394 = { 0.0f, 0.0f, 0.0f };
    f32 localConst = 30.0f;

    if (this->timer == 0) {
        this->timer = 10;
        Effect_SS_Bubble_ct(play, &this->actor.world.pos, 0.0f, 5.0f, 5.0f, rnd_f(0.03f) + 0.07f);
    }
    if (this->actor.parent != NULL) {
        divingGame = (EnDivingGame*)this->actor.parent;
        if (divingGame->actor.update != NULL) {
            if (divingGame->phase == ENDIVINGGAME_PHASE_ENDED) {
                this->timer = 20;
                this->actionFunc = mode_clear;
                if (1) {}
            } else if (this->actor.xyzDistToPlayerSq < SQ(localConst)) {
                lupy_increase(this->rupeeValue);
                Na_StartSystemSe_F(NA_SE_SY_GET_RUPY);
                divingGame->grabbedRupeesCounter++;
                Actor_delete(&this->actor);
            }
        } else {
            Actor_delete(&this->actor);
        }
    }
}

static void mode_clear(EnExRuppy* this, PlayState* play) {
    this->invisible++;
    this->invisible &= 1; // Net effect is this->invisible = !this->invisible;
    if (this->timer == 0) {
        Actor_delete(&this->actor);
    }
}

typedef struct EnExRuppyParentActor {
    /* 0x000 */ Actor actor;
    /* 0x14C */ char unk_14C[0x11A];
    /* 0x226 */ s16 unk_226;
} EnExRuppyParentActor; // Unclear what actor was intended to spawn this.

void mode_heishi_trap(EnExRuppy* this, PlayState* play) {
    EnExRuppyParentActor* parent;
    Vec3f accel = { 0.0f, 0.1f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 distToBlowUp = 50.0f;
    s16 explosionScale;
    s16 explosionScaleStep;
    s32 pad;

    if (this->type == 2) {
        distToBlowUp = 30.0f;
    }
    if (this->actor.xyzDistToPlayerSq < SQ(distToBlowUp)) {
        parent = (EnExRuppyParentActor*)this->actor.parent;
        if (parent != NULL) {
            if (parent->actor.update != NULL) {
                parent->unk_226 = 1;
            }
        } else {
            // "That idiot! error"
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ そ、そんなばかな！エラー！！！！！ ☆☆☆☆☆ \n" VT_RST);
        }
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ バカめ！ ☆☆☆☆☆ \n" VT_RST); // "Stupid!"
        explosionScale = 100;
        explosionScaleStep = 30;
        if (this->type == 2) {
            explosionScale = 20;
            explosionScaleStep = 6;
        }
        Effect_SS_Bomb2_2_ct(play, &this->actor.world.pos, &velocity, &accel, explosionScale, explosionScaleStep);
        Actor_player_power_damage_set(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f);
        Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
        Actor_delete(&this->actor);
    }
}

void mode_nomal_ruppy(EnExRuppy* this, PlayState* play) {
    f32 localConst = 30.0f;

    if (this->actor.xyzDistToPlayerSq < SQ(localConst)) {
        Na_StartSystemSe_F(NA_SE_SY_GET_RUPY);
        Item_set0(play, &this->actor.world.pos, (Ex_Ruppy_Data[this->colorIdx] | 0x8000));
        Actor_delete(&this->actor);
    }
}

void mode_syateki_hit_ruppy(EnExRuppy* this, PlayState* play) {
    if (this->galleryFlag) {
        add_calc2(&this->actor.shape.yOffset, 700.0f, 0.5f, 200.0f);
    } else {
        add_calc2(&this->actor.shape.yOffset, -700.0f, 0.5f, 200.0f);
    }
}

void En_Ex_Ruppy_actor_move(Actor* thisx, PlayState* play) {
    EnExRuppy* this = (EnExRuppy*)thisx;

    this->actor.shape.rot.y += 1960;
    this->actionFunc(this, play);
    if (this->timer != 0) {
        this->timer--;
    }
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
}

void En_Ex_Ruppy_actor_disp(Actor* thisx, PlayState* play) {
    static void* r_model[] = {
        gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeePinkTex, gRupeeOrangeTex,
    };
    s32 pad;
    EnExRuppy* this = (EnExRuppy*)thisx;

    if (!this->invisible) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_ex_ruppy.c", 774);

        _texture_z_light_fog_prim(play->state.gfxCtx);
        Actor_HiliteReflect_set_init(thisx, play, 0);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ex_ruppy.c", 780);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(r_model[this->colorIdx]));
        gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_ex_ruppy.c", 784);
    }
}
