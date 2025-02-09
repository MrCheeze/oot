void EnZo_SpawnRipple(EnZo* this, Vec3f* pos, f32 scale, f32 targetScale, u8 alpha) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    s16 i;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_NONE) {
            effect->type = ENZO_EFFECT_RIPPLE;
            effect->pos = *pos;
            effect->scale = scale;
            effect->targetScale = targetScale;
            effect->color.a = alpha;
            break;
        }
        effect++;
    }
}

void EnZo_SpawnBubble(EnZo* this, Vec3f* pos) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    f32 waterSurface;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENZO_EFFECT_NONE) {
            continue;
        }

        waterSurface = this->actor.world.pos.y + this->actor.depthInWater;

        if (!(waterSurface <= pos->y)) {
            effect->type = ENZO_EFFECT_BUBBLE;
            effect->pos = *pos;
            effect->vec = *pos;
            effect->vel = vel;
            effect->scale = ((Rand_ZeroOne() - 0.5f) * 0.02f) + 0.12f;
            break;
        }
    }
}

void EnZo_SpawnSplash(EnZo* this, Vec3f* pos, Vec3f* vel, f32 scale) {
    EnZoEffect* effect;
    Vec3f accel = { 0.0f, -1.0f, 0.0f };
    s16 i;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++, effect++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            continue;
        }
        effect->type = ENZO_EFFECT_SPLASH;
        effect->pos = *pos;
        effect->vec = accel;
        effect->vel = *vel;
        effect->color.a = (Rand_ZeroOne() * 100.0f) + 100.0f;
        effect->scale = scale;
        break;
    }
}

void EnZo_UpdateEffectsRipples(EnZo* this) {
    EnZoEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            Math_ApproachF(&effect->scale, effect->targetScale, 0.2f, 0.8f);
            if (effect->color.a > 20) {
                effect->color.a -= 20;
            } else {
                effect->color.a = 0;
            }

            if (effect->color.a == 0) {
                effect->type = ENZO_EFFECT_NONE;
            }
        }
        effect++;
    }
}

void EnZo_UpdateEffectsBubbles(EnZo* this) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            effect->pos.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.x;
            effect->pos.z = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.z;
            effect->pos.y += effect->vel.y;

            // Bubbles turn into ripples when they reach the surface
            waterSurface = this->actor.world.pos.y + this->actor.depthInWater;
            if (waterSurface <= effect->pos.y) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_SpawnRipple(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_UpdateEffectsSplashes(EnZo* this) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;

            if (effect->vel.y >= -20.0f) {
                effect->vel.y += effect->vec.y;
            } else {
                effect->vel.y = -20.0f;
                effect->vec.y = 0.0f;
            }

            // Splash particles turn into ripples when they hit the surface
            waterSurface = this->actor.world.pos.y + this->actor.depthInWater;
            if (effect->pos.y < waterSurface) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_SpawnRipple(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_DrawEffectsRipples(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    s16 materialFlag;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 217);
    materialFlag = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENZO_EFFECT_RIPPLE) {
            continue;
        }

        if (!materialFlag) {
            gDPPipeSync(POLY_XLU_DISP++);
            gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesMaterialDL);
            gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
            materialFlag = true;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->color.a);
        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_Scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 242);
        gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 248);
}

void EnZo_DrawEffectsBubbles(EnZo* this, PlayState* play) {
    EnZoEffect* effect = this->effects;
    s16 i;
    u8 materialFlag;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 260);
    materialFlag = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENZO_EFFECT_BUBBLE) {
            continue;
        }

        if (!materialFlag) {
            gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesMaterialDL);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

            materialFlag = true;
        }

        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 281);
        gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesModelDL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 286);
}

void EnZo_DrawEffectsSplashes(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    u8 materialFlag;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 298);
    materialFlag = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENZO_EFFECT_SPLASH) {
            continue;
        }
        if (!materialFlag) {
            gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesMaterialDL);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
            materialFlag = true;
        }
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->color.a);

        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 325);

        gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesModelDL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 331);
}

void EnZo_TreadWaterRipples(EnZo* this, f32 scale, f32 targetScale, u8 alpha) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z;
    EnZo_SpawnRipple(this, &pos, scale, targetScale, alpha);
}

void EnZo_SpawnSplashes(EnZo* this) {
    Vec3f pos;
    Vec3f vel;
    s32 i;

    // Convert 20 particles into splashes (all of them since there are only 15)
    for (i = 0; i < 20; i++) {
        f32 speed = Rand_ZeroOne() * 1.5f + 0.5f;
        f32 angle = Rand_ZeroOne() * 6.28f; // ~pi * 2

        vel.y = Rand_ZeroOne() * 3.0f + 3.0f;

        vel.x = sinf(angle) * speed;
        vel.z = cosf(angle) * speed;

        pos = this->actor.world.pos;
        pos.x += vel.x * 6.0f;
        pos.z += vel.z * 6.0f;
        pos.y += this->actor.depthInWater;
        EnZo_SpawnSplash(this, &pos, &vel, 0.08f);
    }
}
