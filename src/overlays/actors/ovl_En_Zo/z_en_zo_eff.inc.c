void zo_eff_hamon_ct(EnZo* this, Vec3f* pos, f32 scale, f32 targetScale, u8 alpha) {
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

void zo_eff_bubble_ct(EnZo* this, Vec3f* pos) {
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
            effect->scale = ((fqrand() - 0.5f) * 0.02f) + 0.12f;
            break;
        }
    }
}

void zo_eff_mizu_ct(EnZo* this, Vec3f* pos, Vec3f* vel, f32 scale) {
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
        effect->color.a = (fqrand() * 100.0f) + 100.0f;
        effect->scale = scale;
        break;
    }
}

void zo_eff_hamon_mv(EnZo* this) {
    EnZoEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            add_calc2(&effect->scale, effect->targetScale, 0.2f, 0.8f);
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

void zo_eff_bubble_mv(EnZo* this) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = this->effects;
    for (i = 0; i < EN_ZO_EFFECT_COUNT; i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            effect->pos.x = ((fqrand() * 0.5f) - 0.25f) + effect->vec.x;
            effect->pos.z = ((fqrand() * 0.5f) - 0.25f) + effect->vec.z;
            effect->pos.y += effect->vel.y;

            // Bubbles turn into ripples when they reach the surface
            waterSurface = this->actor.world.pos.y + this->actor.depthInWater;
            if (waterSurface <= effect->pos.y) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                zo_eff_hamon_ct(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void zo_eff_mizu_mv(EnZo* this) {
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
                zo_eff_hamon_ct(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void zo_eff_hamon_dr(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    s16 materialFlag;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 217);
    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

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
        Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 242);
        gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 248);
}

void zo_eff_bubble_dr(EnZo* this, PlayState* play) {
    EnZoEffect* effect = this->effects;
    s16 i;
    u8 materialFlag;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 260);
    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

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

        Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 281);
        gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesModelDL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 286);
}

void zo_eff_mizu_dr(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    u8 materialFlag;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 298);
    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
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

        Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_zo_eff.c", 325);

        gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesModelDL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo_eff.c", 331);
}

void set_hamon_effect(EnZo* this, f32 scale, f32 targetScale, u8 alpha) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z;
    zo_eff_hamon_ct(this, &pos, scale, targetScale, alpha);
}

void set_mizu_effect(EnZo* this) {
    Vec3f pos;
    Vec3f vel;
    s32 i;

    // Convert 20 particles into splashes (all of them since there are only 15)
    for (i = 0; i < 20; i++) {
        f32 speed = fqrand() * 1.5f + 0.5f;
        f32 angle = fqrand() * 6.28f; // ~pi * 2

        vel.y = fqrand() * 3.0f + 3.0f;

        vel.x = sinf(angle) * speed;
        vel.z = cosf(angle) * speed;

        pos = this->actor.world.pos;
        pos.x += vel.x * 6.0f;
        pos.z += vel.z * 6.0f;
        pos.y += this->actor.depthInWater;
        zo_eff_mizu_ct(this, &pos, &vel, 0.08f);
    }
}
