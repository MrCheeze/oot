static void* smoke_txt[] = { gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex };

static Vec3f sPos = { 0.0f, 0.0f, 0.0f };
static Vec3f sVelocity = { 0.0f, 0.0f, 0.0f };
static Vec3f sAccel = { 0.0f, 0.3f, 0.0f };

static void go_eff_dust_ct(EnGo2* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale,
                           f32 scaleStep) {
    EnGoEffect* dustEffect = this->effects;
    s16 i;

    for (i = 0; i < EN_GO2_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            dustEffect->initialTimer = dustEffect->timer = initialTimer;
            dustEffect->type = 1;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            break;
        }
    }
}

static void go_eff_dust_mv(EnGo2* this) {
    EnGoEffect* dustEffect = this->effects;
    f32 randomNumber;
    s16 i;

    for (i = 0; i < EN_GO2_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;
            if (dustEffect->timer == 0) {
                dustEffect->type = 0;
            }
            dustEffect->accel.x = (fqrand() * 0.4f) - 0.2f;
            randomNumber = fqrand() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

static void go_eff_dust_dr(EnGo2* this, PlayState* play) {
    EnGoEffect* dustEffect = this->effects;
    s16 alpha;
    s16 materialFlag;
    s16 index;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go2_eff.c", 111);

    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_GO2_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type == 0) {
            continue;
        }

        if (!materialFlag) {
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD40);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
            materialFlag = true;
        }

        alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_go2_eff.c", 137);
        index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(smoke_txt[index]));
        gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD50);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go2_eff.c", 151);
}

static s32 set_dust_effect(EnGo2* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius,
                    f32 yAccel) {
    Vec3f pos = sPos;
    Vec3f velocity = sVelocity;
    Vec3f accel = sAccel;
    s32 i;
    s16 angle;

    pos = this->actor.world.pos; // overwrites sPos data
    pos.y = this->actor.floorHeight;
    angle = (fqrand() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.y += fqrand() * yAccel;
        pos.x = (sin_s(angle) * radius) + this->actor.world.pos.x;
        pos.z = (cos_s(angle) * radius) + this->actor.world.pos.z;
        go_eff_dust_ct(this, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}
