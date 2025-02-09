#include "z_efc_erupc.h"
#include "assets/objects/object_efc_erupc/object_efc_erupc.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Efc_Erupc_Actor_ct(Actor* thisx, PlayState* play);
void Efc_Erupc_Actor_dt(Actor* thisx, PlayState* play);
void Efc_Erupc_Actor_move(Actor* thisx, PlayState* play);
void Efc_Erupc_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EfcErupc* this, PlayState* play);
static void Boss_Eff_disp(EfcErupcEffect* effect, PlayState* play);
static void Boss_Eff_move(EfcErupc* this, PlayState* play);
void Effect_hinoko_ct_IN(EfcErupcEffect* effect, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scaleFactor);
void Effect_hinoko_init(EfcErupcEffect* effect);

ActorProfile Efc_Erupc_Profile = {
    /**/ ACTOR_EFC_ERUPC,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_EFC_ERUPC,
    /**/ sizeof(EfcErupc),
    /**/ Efc_Erupc_Actor_ct,
    /**/ Efc_Erupc_Actor_dt,
    /**/ Efc_Erupc_Actor_move,
    /**/ Efc_Erupc_Actor_draw,
};

void Efc_Erupc_actor_set_process(EfcErupc* this, EfcErupcActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Efc_Erupc_Actor_ct(Actor* thisx, PlayState* play) {
    EfcErupc* this = (EfcErupc*)thisx;

    Efc_Erupc_actor_set_process(this, move_wait);
    Actor_set_scale(&this->actor, 1.0f);
    Effect_hinoko_init(this->effects);
    this->unk_14C = this->unk_14E = this->unk_150 = 0;
    this->unk_152 = 5;
    this->unk_154 = -100;
}

void Efc_Erupc_Actor_dt(Actor* thisx, PlayState* play) {
}

static void move_wait(EfcErupc* this, PlayState* play) {
    Vec3f pos;
    Vec3f vel;
    Vec3f accel;
    s32 i;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[1] != NULL) {
            if (play->csCtx.actorCues[1]->id == 2) {
                if (this->unk_150 == 30) {
                    Na_StartFixSe_F(NA_SE_IT_EARTHQUAKE);
                }
                if (this->unk_150 <= 64) {
                    if (this->unk_154 < 200) {
                        this->unk_154 += 10;
                    }
                } else {
                    if (this->unk_154 > -100) {
                        this->unk_154 -= 10;
                    }
                }
                this->unk_150++;
            } else {
                if (this->unk_154 > -100) {
                    this->unk_154 -= 10;
                }
            }
        }
    }
    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[2] != NULL) {
            switch (play->csCtx.actorCues[2]->id) {
                case 2:
                    if (this->unk_14E == 0) {
                        Na_StartDemoSe(SEQ_CS_EFFECTS_LAVA_ERUPT);
                        SET_EVENTCHKINF(EVENTCHKINF_2F);
                    }
                    this->unk_14E++;
                    break;
                case 3:
                    this->unk_14E = 30;
            }
            this->unk_14C++;
        }
    }
    accel.z = 0.0f;
    accel.x = 0.0f;
    pos.y = this->actor.world.pos.y + 300.0f;
    for (i = 0; i < this->unk_152; i++) {
        pos.x = rnd_fx(100.0f) + this->actor.world.pos.x;
        pos.z = rnd_fx(100.0f) + this->actor.world.pos.z;
        vel.x = rnd_fx(100.0f);
        vel.y = rnd_f(100.0f);
        vel.z = rnd_fx(100.0f);
        accel.y = this->unk_154 * 0.1f;
        Effect_hinoko_ct_IN(this->effects, &pos, &vel, &accel, 80.0f);
    }
}

void Efc_Erupc_Actor_move(Actor* thisx, PlayState* play) {
    EfcErupc* this = (EfcErupc*)thisx;

    this->actionFunc(this, play);
    Boss_Eff_move(this, play);
}

void Efc_Erupc_Actor_draw(Actor* thisx, PlayState* play) {
    EfcErupc* this = (EfcErupc*)thisx;
    u16 cueId;

    OPEN_DISPS(play->state.gfxCtx, "../z_efc_erupc.c", 282);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->unk_14C * 1, this->unk_14E * -4, 32, 64, 1,
                                this->unk_14C * 4, this->unk_14E * -20, 64, 64));

    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, this->unk_150 * -4, 16, 128, 1, 0,
                                this->unk_150 * 12, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, this->unk_150 * -4, 16, 128, 1, 0,
                                this->unk_150 * 12, 32, 32));

    Matrix_push();
    Matrix_scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_efc_erupc.c", 321);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.actorCues[1] != NULL) && (play->csCtx.actorCues[1]->id == 2)) {
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_002570);
        }
    }
    Matrix_pull();
    Matrix_scale(3.4f, 3.4f, 3.4f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_efc_erupc.c", 333);
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if (cue != NULL) {
            cueId = cue->id;

            if ((cueId == 2) || (cueId == 3)) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 200, 255);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 0, 0, 255);
                gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_001720);
            }
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_efc_erupc.c", 356);
    Boss_Eff_disp(this->effects, play);
}

static void Boss_Eff_disp(EfcErupcEffect* effect, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_efc_erupc.c", 368);
    for (i = 0; i < EFC_ERUPC_EFFECT_COUNT; i++, effect++) {
        if (effect->isActive) {
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_002760);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->color.r, effect->color.g, effect->color.b, effect->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 0, 0, 0);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_efc_erupc.c", 393);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_0027D8);
        }
    }
    CLOSE_DISPS(gfxCtx, "../z_efc_erupc.c", 399);
}

static Color_RGB8 col_d[] = {
    { 255, 128, 0 },
    { 255, 0, 0 },
    { 255, 255, 0 },
    { 255, 0, 0 },
};

static void Boss_Eff_move(EfcErupc* this, PlayState* play) {
    s16 i;
    s16 index;
    Color_RGB8 effectColors[] = {
        { 255, 128, 0 },
        { 255, 0, 0 },
        { 0, 0, 0 },
        { 100, 0, 0 },
    };
    Color_RGB8* color;
    EfcErupcEffect* cur = this->effects;

    for (i = 0; i < EFC_ERUPC_EFFECT_COUNT; i++, cur++) {
        if (cur->isActive) {
            cur->pos.x += cur->vel.x;
            cur->pos.y += cur->vel.y;
            cur->pos.z += cur->vel.z;
            cur->vel.x += cur->accel.x;
            cur->vel.y += cur->accel.y;
            cur->vel.z += cur->accel.z;
            cur->animTimer++;
            index = cur->animTimer % 4;
            color = &effectColors[index];
            cur->color.r = color->r;
            cur->color.g = color->g;
            cur->color.b = color->b;
            cur->alpha -= 20;
            if (cur->alpha <= 0) {
                cur->alpha = 0;
                cur->isActive = false;
            }
        }
    }
}

void Effect_hinoko_ct_IN(EfcErupcEffect* effect, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scaleFactor) {
    s16 i;

    for (i = 0; i < EFC_ERUPC_EFFECT_COUNT; i++, effect++) {
        if (!effect->isActive) {
            effect->isActive = true;
            effect->pos = *pos;
            effect->vel = *vel;
            effect->accel = *accel;
            effect->scale = scaleFactor / 1000.0f;
            effect->alpha = 255;
            effect->animTimer = (s16)rnd_f(10.0f);
            return;
        }
    }
}

void Effect_hinoko_init(EfcErupcEffect* effect) {
    s16 i;

    for (i = 0; i < EFC_ERUPC_EFFECT_COUNT; i++, effect++) {
        effect->isActive = false;
    }
}
