#pragma increment_block_number "gc-eu:208 gc-eu-mq:208 gc-jp:192 gc-jp-ce:192 gc-jp-mq:192 gc-us:192 gc-us-mq:192" \
                               "ique-cn:192 ntsc-1.0:208 ntsc-1.1:208 ntsc-1.2:208 pal-1.0:220 pal-1.1:220 hiratsu3:192"

#include "global.h"
#include "ultra64.h"
#include "terminal.h"
#include "versions.h"

#include "z64frame_advance.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

typedef enum LightningBoltState {
    /* 0x00 */ LIGHTNING_BOLT_START,
    /* 0x01 */ LIGHTNING_BOLT_WAIT,
    /* 0x02 */ LIGHTNING_BOLT_DRAW,
    /* 0xFF */ LIGHTNING_BOLT_INACTIVE = 0xFF
} LightningBoltState;

typedef struct ZBufValConversionEntry {
    /* 0x00 */ s32 mantissaShift; // shift applied to the mantissa of the z buffer value
    /* 0x04 */ s32 base;          // 15.3 fixed-point base value for the exponent
} ZBufValConversionEntry;         // size = 0x8

// This table needs as many values as there are values for the 3-bit exponent
ZBufValConversionEntry z_format[1 << 3] = {
    { 6, 0x0000 << 3 }, { 5, 0x4000 << 3 }, { 4, 0x6000 << 3 }, { 3, 0x7000 << 3 },
    { 2, 0x7800 << 3 }, { 1, 0x7C00 << 3 }, { 0, 0x7E00 << 3 }, { 0, 0x7F00 << 3 },
};

u8 E_wether_flg = WEATHER_MODE_CLEAR; // "E_wether_flg"

u8 E_water_colbak = 0;

u8 E_wether_apl = false;

// Indicates whether the skybox is changing to a different index of the same config (based on time)
u8 E_vr_box_ovl = false;

// how many units of time that pass every update
u16 E_day_time_plus = 0;

u16 lens_answer = GPACK_ZDZ(G_MAXFBZ, 0);

#include "z_kankyo_data.inc.c"

u8 E_colindex1 = 0;
u8 E_colindex2 = 0;
f32 E_colparcent = 0.0f;

u8 E_lf_flag;
Vec3f E_lf_position;
s16 E_lf_kiten;
s16 E_lf_wscale;
f32 E_lf_alpha;
s16 E_lf_rectalpha;

typedef struct LightningBolt {
    /* 0x00 */ u8 state;
    /* 0x04 */ Vec3f offset;
    /* 0x10 */ Vec3f pos;
    /* 0x1C */ s8 pitch;
    /* 0x1D */ s8 roll;
    /* 0x1E */ u8 textureIndex;
    /* 0x1F */ u8 delayTimer;
} LightningBolt; // size = 0x20

LightningBolt Thunder[3];

LightningStrike Eleki;

s16 E_eleki_alpha;

s16 E_lens_x;
s16 E_lens_y;

#pragma increment_block_number "gc-eu:240 gc-eu-mq:240 gc-jp:224 gc-jp-ce:224 gc-jp-mq:224 gc-us:224 gc-us-mq:224" \
                               "ique-cn:224 ntsc-1.0:224 ntsc-1.1:224 ntsc-1.2:224 pal-1.0:240 pal-1.1:240 hiratsu3:224"

LightNode* K_light_list;
LightInfo K_light_data;
LightNode* K2_light_list;
LightInfo K2_light_data;
u8 P_col;
u16 sound_scroll;

#define ZBUFVAL_EXPONENT(v) (((v) >> 15) & 7)
#define ZBUFVAL_MANTISSA(v) (((v) >> 4) & 0x7FF)

/**
 * Convert an 18-bits Z buffer value to a fixed point 15.3 value
 *
 * zBufferVal is 18 bits:
 *   3: Exponent of z value
 *  11: Mantissa of z value
 *   4: dz value (unused)
 */
s32 zmem2z(s32 zBufferVal) {
    // base[exp] + (mantissa << shift[exp])
    s32 ret = (ZBUFVAL_MANTISSA(zBufferVal) << z_format[ZBUFVAL_EXPONENT(zBufferVal)].mantissaShift) +
              z_format[ZBUFVAL_EXPONENT(zBufferVal)].base;

    return ret;
}

u16 get_zmem(s32 x, s32 y) {
    s32 pixelDepth = sys_zb[y][x];

    return pixelDepth;
}

void lens_callback(GraphicsContext* gfxCtx, void* param) {
    PlayState* play = (PlayState*)param;

    lens_answer = get_zmem(E_lens_x, E_lens_y);
    Light_list_point_draw_check(play);
}

void Global_kankyo_ct(PlayState* play2, EnvironmentContext* envCtx, s32 unused) {
    u8 i;
    PlayState* play = play2;

    z_common_data.sunsSongState = SUNSSONG_INACTIVE;

    //! FAKE: (void)0 on CLOCK_TIME(18, 0)
    if (((void)0, z_common_data.save.dayTime) > ((void)0, CLOCK_TIME(18, 0)) ||
        ((void)0, z_common_data.save.dayTime) < CLOCK_TIME(6, 30)) {
        ((void)0, z_common_data.save.nightFlag = 1);
    } else {
        ((void)0, z_common_data.save.nightFlag = 0);
    }

    play->state.gfxCtx->callback = lens_callback;
    play->state.gfxCtx->callbackParam = play;

    Light_diffuse_ct(&envCtx->dirLight1, 80, 80, 80, 80, 80, 80);
    Global_light_list_new(play, &play->lightCtx, &envCtx->dirLight1);

    Light_diffuse_ct(&envCtx->dirLight2, 80, 80, 80, 80, 80, 80);
    Global_light_list_new(play, &play->lightCtx, &envCtx->dirLight2);

    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;

    envCtx->changeSkyboxState = CHANGE_SKYBOX_INACTIVE;
    envCtx->changeSkyboxTimer = 0;
    envCtx->changeLightEnabled = false;
    envCtx->changeLightTimer = 0;

    envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
    envCtx->lightConfig = 0;
    envCtx->changeLightNextConfig = 0;

    envCtx->glareAlpha = 0.0f;
    envCtx->lensFlareAlphaScale = 0.0f;

    envCtx->lightSetting = 0;
    envCtx->prevLightSetting = 0;
    envCtx->lightBlend = 1.0f;
    envCtx->lightBlendOverride = LIGHT_BLEND_OVERRIDE_NONE;

    envCtx->stormRequest = STORM_REQUEST_NONE;
    envCtx->stormState = STORM_STATE_OFF;
    envCtx->lightningState = LIGHTNING_OFF;
    envCtx->timeSeqState = TIMESEQ_DAY_BGM;
    envCtx->fillScreen = false;

    envCtx->screenFillColor[0] = 0;
    envCtx->screenFillColor[1] = 0;
    envCtx->screenFillColor[2] = 0;
    envCtx->screenFillColor[3] = 0;

    envCtx->customSkyboxFilter = false;

    envCtx->skyboxFilterColor[0] = 0;
    envCtx->skyboxFilterColor[1] = 0;
    envCtx->skyboxFilterColor[2] = 0;
    envCtx->skyboxFilterColor[3] = 0;

    envCtx->sandstormState = SANDSTORM_OFF;
    envCtx->sandstormPrimA = 0;
    envCtx->sandstormEnvA = 0;

    Eleki.state = LIGHTNING_STRIKE_WAIT;
    Eleki.flashRed = 0;
    Eleki.flashGreen = 0;
    Eleki.flashBlue = 0;

    E_eleki_alpha = 0;

    z_common_data.cutsceneTransitionControl = 0;

    envCtx->adjAmbientColor[0] = envCtx->adjAmbientColor[1] = envCtx->adjAmbientColor[2] = envCtx->adjLight1Color[0] =
        envCtx->adjLight1Color[1] = envCtx->adjLight1Color[2] = envCtx->adjFogColor[0] = envCtx->adjFogColor[1] =
            envCtx->adjFogColor[2] = envCtx->adjFogNear = envCtx->adjZFar = 0;

    envCtx->sunPos.x = -(sin_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    envCtx->sunPos.y = +(cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    envCtx->sunPos.z = +(cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;

    envCtx->windDirection.x = 80;
    envCtx->windDirection.y = 80;
    envCtx->windDirection.z = 80;

    envCtx->lightBlendEnabled = false;
    envCtx->lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    envCtx->lightBlendRateOverride = LIGHT_BLENDRATE_OVERRIDE_NONE;

    envCtx->sceneTimeSpeed = 0;
    E_day_time_plus = envCtx->sceneTimeSpeed;

#if DEBUG_FEATURES
    R_ENV_TIME_SPEED_OLD = E_day_time_plus;
    R_ENV_DISABLE_DBG = true;

    if (CREG(3) != 0) {
        z_common_data.chamberCutsceneNum = CREG(3) - 1;
    }
#endif

    play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
    play->envCtx.precipitation[PRECIP_RAIN_CUR] = 0;
    play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
    play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
    play->envCtx.precipitation[PRECIP_SOS_MAX] = 0;

    if (z_common_data.retainWeatherMode) {
        if (!IS_CUTSCENE_LAYER) {
            switch (E_wether_flg) {
                case WEATHER_MODE_CLOUDY_CONFIG3:
                    envCtx->skyboxConfig = 1;
                    envCtx->changeSkyboxNextConfig = 1;
                    envCtx->lightConfig = 3;
                    envCtx->changeLightNextConfig = 3;
                    play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
                    play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
                    break;

                case WEATHER_MODE_CLOUDY_CONFIG2:
                case WEATHER_MODE_SNOW:
                case WEATHER_MODE_RAIN:
                    envCtx->skyboxConfig = 1;
                    envCtx->changeSkyboxNextConfig = 1;
                    envCtx->lightConfig = 2;
                    envCtx->changeLightNextConfig = 2;
                    play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
                    play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
                    break;

                case WEATHER_MODE_HEAVY_RAIN:
                    envCtx->skyboxConfig = 1;
                    envCtx->changeSkyboxNextConfig = 1;
                    envCtx->lightConfig = 4;
                    envCtx->changeLightNextConfig = 4;
                    play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
                    play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
                    break;

                default:
                    break;
            }

            if (play->skyboxId == SKYBOX_NORMAL_SKY) {
                if (E_wether_flg == WEATHER_MODE_SNOW) {
                    play->envCtx.precipitation[PRECIP_SNOW_CUR] = play->envCtx.precipitation[PRECIP_SNOW_MAX] = 64;
                } else if (E_wether_flg == WEATHER_MODE_RAIN) {
                    play->envCtx.precipitation[PRECIP_RAIN_MAX] = 20;
                    play->envCtx.precipitation[PRECIP_RAIN_CUR] = 20;
                } else if (E_wether_flg == WEATHER_MODE_HEAVY_RAIN) {
                    play->envCtx.precipitation[PRECIP_RAIN_MAX] = 30;
                    play->envCtx.precipitation[PRECIP_RAIN_CUR] = 30;
                }
            }
        }
    } else {
        E_wether_flg = WEATHER_MODE_CLEAR;
    }

    E_wether_apl = false;
    E_water_colbak = 0;
    E_vr_box_ovl = false;
    z_common_data.retainWeatherMode = false;

#if DEBUG_FEATURES
    R_ENV_LIGHT1_DIR(0) = 80;
    R_ENV_LIGHT1_DIR(1) = 80;
    R_ENV_LIGHT1_DIR(2) = 80;

    R_ENV_LIGHT2_DIR(0) = -80;
    R_ENV_LIGHT2_DIR(1) = -80;
    R_ENV_LIGHT2_DIR(2) = -80;

    cREG(9) = 10;
    cREG(10) = 0;
    cREG(11) = 0;
    cREG(12) = 0;
    cREG(13) = 0;
    cREG(14) = 0;
#endif

    DEMOCAM_SW = true;

    for (i = 0; i < ARRAY_COUNT(Thunder); i++) {
        Thunder[i].state = LIGHTNING_BOLT_INACTIVE;
    }

    play->roomCtx.drawParams[0] = 0;
    play->roomCtx.drawParams[1] = 0;

    for (i = 0; i < ARRAY_COUNT(play->csCtx.actorCues); i++) {
        play->csCtx.actorCues[i] = NULL;
    }

    if (Object_Exchange_bank_check(&play->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP) < 0 && !play->envCtx.sunMoonDisabled) {
        play->envCtx.sunMoonDisabled = true;
        PRINTF(VT_COL(YELLOW, BLACK) T("\n\nフィールド常駐以外、太陽設定！よって強制解除！\n",
                                       "\n\nSun setting other than field keep! So forced release!\n") VT_RST);
    }

    E_lf_flag = false;
    z_vibctl2_StageInit();
}

u8 add_calc_char(u8* pvalue, u8 target, u8 scale, u8 step, u8 minStep) {
    s16 stepSize = 0;
    s16 diff = target - *pvalue;

    if (target != *pvalue) {
        stepSize = diff / scale;
        if ((stepSize >= (s16)minStep) || ((s16)-minStep >= stepSize)) {
            if ((s16)step < stepSize) {
                stepSize = step;
            }
            if ((s16)-step > stepSize) {
                stepSize = -step;
            }
            *pvalue += (u8)stepSize;
        } else {
            if (stepSize < (s16)minStep) {
                stepSize = minStep;
                *pvalue += (u8)stepSize;
                if (target < *pvalue) {
                    *pvalue = target;
                }
            }
            if ((s16)-minStep < stepSize) {
                stepSize = -minStep;
                *pvalue += (u8)stepSize;
                if (*pvalue < target) {
                    *pvalue = target;
                }
            }
        }
    }
    return diff;
}

u8 add_calc_schar(s8* pvalue, s8 target, u8 scale, u8 step, u8 minStep) {
    s16 stepSize = 0;
    s16 diff = target - *pvalue;

    if (target != *pvalue) {
        stepSize = diff / scale;
        if ((stepSize >= (s16)minStep) || ((s16)-minStep >= stepSize)) {
            if ((s16)step < stepSize) {
                stepSize = step;
            }
            if ((s16)-step > stepSize) {
                stepSize = -step;
            }
            *pvalue += (s8)stepSize;
        } else {
            if (stepSize < (s16)minStep) {
                stepSize = minStep;
                *pvalue += (s8)stepSize;
                if (target < *pvalue) {
                    *pvalue = target;
                }
            }
            if ((s16)-minStep < stepSize) {
                stepSize = -minStep;
                *pvalue += (s8)stepSize;
                if (*pvalue < target) {
                    *pvalue = target;
                }
            }
        }
    }
    return diff;
}

f32 get_parcent(u16 max, u16 min, u16 val) {
    f32 diff = max - min;

    if (diff != 0.0f) {
        f32 ret = 1.0f - (max - val) / diff;

        if (!(ret >= 1.0f)) {
            return ret;
        }
    }

    return 1.0f;
}

f32 get_parcent_forAccelBrake(u16 endFrame, u16 startFrame, u16 curFrame, u16 accelDuration, u16 decelDuration) {
    f32 endFrameF;
    f32 startFrameF;
    f32 curFrameF;
    f32 accelDurationF;
    f32 decelDurationF;
    f32 totalFrames;
    f32 temp;
    f32 framesElapsed;
    f32 ret;

    if (curFrame <= startFrame) {
        return 0.0f;
    }

    if (curFrame >= endFrame) {
        return 1.0f;
    }

    endFrameF = (s32)endFrame;
    startFrameF = (s32)startFrame;
    curFrameF = (s32)curFrame;
    totalFrames = endFrameF - startFrameF;
    framesElapsed = curFrameF - startFrameF;
    accelDurationF = (s32)accelDuration;
    decelDurationF = (s32)decelDuration;

    if ((startFrameF >= endFrameF) || (accelDurationF + decelDurationF > totalFrames)) {
        PRINTF(VT_COL(RED, WHITE) T("\nend_frameとstart_frameのフレーム関係がおかしい!!!",
                                    "\nThe frame relation between end_frame and start_frame is wrong!!!") VT_RST);
        PRINTF(VT_COL(RED, WHITE) "\nby get_parcent_forAccelBrake!!!!!!!!!" VT_RST);

        return 0.0f;
    }

    temp = 1.0f / ((totalFrames * 2.0f) - accelDurationF - decelDurationF);

    if (accelDurationF != 0.0f) {
        if (framesElapsed <= accelDurationF) {
            return temp * framesElapsed * framesElapsed / accelDurationF;
        }
        ret = temp * accelDurationF;
    } else {
        ret = 0.0f;
    }

    if (framesElapsed <= totalFrames - decelDurationF) {
        ret += 2.0f * temp * (framesElapsed - accelDurationF);
        return ret;
    }

    ret += 2.0f * temp * (totalFrames - accelDurationF - decelDurationF);

    if (decelDurationF != 0.0f) {
        ret += temp * decelDurationF;
        if (framesElapsed < totalFrames) {
            ret -= temp * (totalFrames - framesElapsed) * (totalFrames - framesElapsed) / decelDurationF;
        }
    }

    return ret;
}

void raindrop_proc(EnvironmentContext* envCtx, u8 unused) {
    if (envCtx->stormRequest != STORM_REQUEST_NONE) {
        switch (envCtx->stormState) {
            case STORM_STATE_OFF:
                if ((envCtx->stormRequest == STORM_REQUEST_START) && !E_vr_box_ovl) {
                    envCtx->changeSkyboxState = CHANGE_SKYBOX_REQUESTED;
                    envCtx->skyboxConfig = 0;
                    envCtx->changeSkyboxNextConfig = 1;
                    envCtx->changeSkyboxTimer = 100;
                    envCtx->changeLightEnabled = true;
                    envCtx->lightConfig = 0;
                    envCtx->changeLightNextConfig = 2;
                    E_water_colbak = 2;
                    envCtx->changeLightTimer = envCtx->changeDuration = 100;
                    envCtx->stormState++;
                }
                break;

            case STORM_STATE_ON:
                if (!E_vr_box_ovl && (envCtx->stormRequest == STORM_REQUEST_STOP)) {
                    E_wether_flg = WEATHER_MODE_CLEAR;
                    envCtx->changeSkyboxState = CHANGE_SKYBOX_REQUESTED;
                    envCtx->skyboxConfig = 1;
                    envCtx->changeSkyboxNextConfig = 0;
                    envCtx->changeSkyboxTimer = 100;
                    envCtx->changeLightEnabled = true;
                    envCtx->lightConfig = 2;
                    envCtx->changeLightNextConfig = 0;
                    E_water_colbak = 0;
                    envCtx->changeLightTimer = envCtx->changeDuration = 100;
                    envCtx->precipitation[PRECIP_RAIN_MAX] = 0;
                    envCtx->stormRequest = STORM_REQUEST_NONE;
                    envCtx->stormState = STORM_STATE_OFF;
                }
                break;
        }
    }
}

void Kankyo_vrbox_change(u8 skyboxId, EnvironmentContext* envCtx, SkyboxContext* skyboxCtx) {
    u32 size;
    u8 i;
    u8 newSkybox1Index = 0xFF;
    u8 newSkybox2Index = 0xFF;
    u8 skyboxBlend = 0;

    if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        envCtx->skyboxConfig = 3;

        for (i = 0; i < ARRAY_COUNT(vrbox_chg[envCtx->skyboxConfig]); i++) {
            if (z_common_data.skyboxTime >= vrbox_chg[envCtx->skyboxConfig][i].startTime &&
                (z_common_data.skyboxTime < vrbox_chg[envCtx->skyboxConfig][i].endTime ||
                 vrbox_chg[envCtx->skyboxConfig][i].endTime == 0xFFFF)) {
                if (vrbox_chg[envCtx->skyboxConfig][i].changeSkybox) {
                    envCtx->skyboxBlend =
                        get_parcent(vrbox_chg[envCtx->skyboxConfig][i].endTime,
                                               vrbox_chg[envCtx->skyboxConfig][i].startTime,
                                               ((void)0, z_common_data.skyboxTime)) *
                        255;
                } else {
                    envCtx->skyboxBlend = 0;
                }
                break;
            }
        }
    } else if (skyboxId == SKYBOX_NORMAL_SKY && !envCtx->skyboxDisabled) {
        for (i = 0; i < ARRAY_COUNT(vrbox_chg[envCtx->skyboxConfig]); i++) {
            if (z_common_data.skyboxTime >= vrbox_chg[envCtx->skyboxConfig][i].startTime &&
                (z_common_data.skyboxTime < vrbox_chg[envCtx->skyboxConfig][i].endTime ||
                 vrbox_chg[envCtx->skyboxConfig][i].endTime == 0xFFFF)) {
                newSkybox1Index = vrbox_chg[envCtx->skyboxConfig][i].skybox1Index;
                newSkybox2Index = vrbox_chg[envCtx->skyboxConfig][i].skybox2Index;
                E_vr_box_ovl = vrbox_chg[envCtx->skyboxConfig][i].changeSkybox;

                if (E_vr_box_ovl) {
                    skyboxBlend = get_parcent(vrbox_chg[envCtx->skyboxConfig][i].endTime,
                                                         vrbox_chg[envCtx->skyboxConfig][i].startTime,
                                                         ((void)0, z_common_data.skyboxTime)) *
                                  255;
                } else {
                    skyboxBlend = get_parcent(vrbox_chg[envCtx->skyboxConfig][i].endTime,
                                                         vrbox_chg[envCtx->skyboxConfig][i].startTime,
                                                         ((void)0, z_common_data.skyboxTime)) *
                                  255;

                    skyboxBlend = (skyboxBlend < 128) ? 255 : 0;

                    if ((envCtx->changeSkyboxState != CHANGE_SKYBOX_INACTIVE) &&
                        (envCtx->changeSkyboxState < CHANGE_SKYBOX_ACTIVE)) {
                        envCtx->changeSkyboxState++;
                        skyboxBlend = 0;
                    }
                }
                break;
            }
        }

        raindrop_proc(envCtx, skyboxBlend);

        if (envCtx->changeSkyboxState >= CHANGE_SKYBOX_ACTIVE) {
            newSkybox1Index = vrbox_chg[envCtx->skyboxConfig][i].skybox1Index;
            newSkybox2Index = vrbox_chg[envCtx->changeSkyboxNextConfig][i].skybox2Index;

            skyboxBlend = ((f32)envCtx->changeDuration - envCtx->changeSkyboxTimer) / (f32)envCtx->changeDuration * 255;
            envCtx->changeSkyboxTimer--;

            if (envCtx->changeSkyboxTimer <= 0) {
                envCtx->changeSkyboxState = CHANGE_SKYBOX_INACTIVE;
                envCtx->skyboxConfig = envCtx->changeSkyboxNextConfig;
            }
        }

#if DEBUG_FEATURES
        if (newSkybox1Index == 0xFF) {
            PRINTF(VT_COL(RED, WHITE) T("\n環境ＶＲデータ取得失敗！ ささきまでご報告を！",
                                        "\nEnvironment VR data acquisition failed! Report to Sasaki!") VT_RST);
        }
#endif

        if ((envCtx->skybox1Index != newSkybox1Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TEXTURE1_START;
            size = vrbox_tenso[newSkybox1Index].file.vromEnd - vrbox_tenso[newSkybox1Index].file.vromStart;

            osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
            DMA_REQUEST_ASYNC(&envCtx->dmaRequest, skyboxCtx->staticSegments[0],
                              vrbox_tenso[newSkybox1Index].file.vromStart, size, 0, &envCtx->loadQueue, NULL,
                              "../z_kankyo.c", 1264);
            envCtx->skybox1Index = newSkybox1Index;
        }

        if ((envCtx->skybox2Index != newSkybox2Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TEXTURE2_START;
            size = vrbox_tenso[newSkybox2Index].file.vromEnd - vrbox_tenso[newSkybox2Index].file.vromStart;

            osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
            DMA_REQUEST_ASYNC(&envCtx->dmaRequest, skyboxCtx->staticSegments[1],
                              vrbox_tenso[newSkybox2Index].file.vromStart, size, 0, &envCtx->loadQueue, NULL,
                              "../z_kankyo.c", 1281);
            envCtx->skybox2Index = newSkybox2Index;
        }

        if (envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE1_DONE) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TLUT1_START;

            if ((newSkybox1Index & 1) ^ ((newSkybox1Index & 4) >> 2)) {
                size = vrbox_tenso[newSkybox1Index].palette.vromEnd -
                       vrbox_tenso[newSkybox1Index].palette.vromStart;

                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DMA_REQUEST_ASYNC(&envCtx->dmaRequest, skyboxCtx->palettes,
                                  vrbox_tenso[newSkybox1Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                  "../z_kankyo.c", 1307);
            } else {
                size = vrbox_tenso[newSkybox1Index].palette.vromEnd -
                       vrbox_tenso[newSkybox1Index].palette.vromStart;
                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DMA_REQUEST_ASYNC(&envCtx->dmaRequest, (u8*)skyboxCtx->palettes + size,
                                  vrbox_tenso[newSkybox1Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                  "../z_kankyo.c", 1320);
            }
        }

        if (envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE2_DONE) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TLUT2_START;

            if ((newSkybox2Index & 1) ^ ((newSkybox2Index & 4) >> 2)) {
                size = vrbox_tenso[newSkybox2Index].palette.vromEnd -
                       vrbox_tenso[newSkybox2Index].palette.vromStart;

                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DMA_REQUEST_ASYNC(&envCtx->dmaRequest, skyboxCtx->palettes,
                                  vrbox_tenso[newSkybox2Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                  "../z_kankyo.c", 1342);
            } else {
                size = vrbox_tenso[newSkybox2Index].palette.vromEnd -
                       vrbox_tenso[newSkybox2Index].palette.vromStart;
                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DMA_REQUEST_ASYNC(&envCtx->dmaRequest, (u8*)skyboxCtx->palettes + size,
                                  vrbox_tenso[newSkybox2Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                  "../z_kankyo.c", 1355);
            }
        }

        if ((envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE1_START) ||
            (envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE2_START)) {
            if (osRecvMesg(&envCtx->loadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
                envCtx->skyboxDmaState++;
            }
        } else if (envCtx->skyboxDmaState >= SKYBOX_DMA_TEXTURE1_DONE) {
            if (osRecvMesg(&envCtx->loadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
                envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
            }
        }

        envCtx->skyboxBlend = skyboxBlend;
    }
}

void water_in_kankyo(PlayState* play, s32 waterLightsIndex) {
    if (waterLightsIndex == WATERBOX_LIGHT_INDEX_NONE) {
        waterLightsIndex = 0;
        PRINTF(VT_COL(YELLOW, BLACK) T("\n水ポリゴンデータに水中カラーが設定されておりません!",
                                       "\nUnderwater color is not set in the water poly data!") VT_RST);
    }

    if (play->envCtx.lightMode == LIGHT_MODE_TIME) {
        E_water_colbak = play->envCtx.changeLightNextConfig;

        if (play->envCtx.lightConfig != waterLightsIndex) {
            play->envCtx.lightConfig = waterLightsIndex;
            play->envCtx.changeLightNextConfig = waterLightsIndex;
        }
    } else {
        play->envCtx.lightBlendEnabled = false; // instantly switch to water lights
        play->envCtx.lightSettingOverride = waterLightsIndex;
    }
}

void water_out_kankyo(PlayState* play) {
    if (play->envCtx.lightMode == LIGHT_MODE_TIME) {
        play->envCtx.lightConfig = E_water_colbak;
        play->envCtx.changeLightNextConfig = E_water_colbak;
    } else {
        play->envCtx.lightBlendEnabled = false; // instantly switch to previous lights
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        play->envCtx.lightBlend = 1.0f;
    }
}

#if DEBUG_FEATURES
void Environment_PrintDebugInfo(PlayState* play, Gfx** gfx) {
    GfxPrint printer;
    s32 pad[2];

    gfxprint_init(&printer);
    gfxprint_open(&printer, *gfx);

    gfxprint_locate8x8(&printer, 22, 7);
    gfxprint_color(&printer, 155, 155, 255, 64);
    gfxprint_printf(&printer, "T%03d ", ((void)0, z_common_data.save.totalDays));
    gfxprint_printf(&printer, "E%03d", ((void)0, z_common_data.save.bgsDayCount));

    gfxprint_color(&printer, 255, 255, 55, 64);
    gfxprint_locate8x8(&printer, 22, 8);
    gfxprint_printf(&printer, "%s", "ZELDATIME ");

    gfxprint_color(&printer, 255, 255, 255, 64);
    gfxprint_printf(&printer, "%02d", (u8)(24 * 60 / (f32)0x10000 * ((void)0, z_common_data.save.dayTime) / 60.0f));

    if ((z_common_data.save.dayTime & 0x1F) >= 0x10 || E_day_time_plus >= 6) {
        gfxprint_printf(&printer, "%s", ":");
    } else {
        gfxprint_printf(&printer, "%s", " ");
    }

    gfxprint_printf(&printer, "%02d", (s16)(24 * 60 / (f32)0x10000 * ((void)0, z_common_data.save.dayTime)) % 60);

    gfxprint_color(&printer, 255, 255, 55, 64);
    gfxprint_locate8x8(&printer, 22, 9);
    gfxprint_printf(&printer, "%s", "VRBOXTIME ");

    gfxprint_color(&printer, 255, 255, 255, 64);
    gfxprint_printf(&printer, "%02d", (u8)(24 * 60 / (f32)0x10000 * ((void)0, z_common_data.skyboxTime) / 60.0f));

    if ((((void)0, z_common_data.skyboxTime) & 0x1F) >= 0x10 || E_day_time_plus >= 6) {
        gfxprint_printf(&printer, "%s", ":");
    } else {
        gfxprint_printf(&printer, "%s", " ");
    }

    gfxprint_printf(&printer, "%02d", (s16)(24 * 60 / (f32)0x10000 * ((void)0, z_common_data.skyboxTime)) % 60);

    gfxprint_color(&printer, 55, 255, 255, 64);
    gfxprint_locate8x8(&printer, 22, 6);

    if (!IS_DAY) {
        gfxprint_printf(&printer, "%s", "YORU"); // "night"
    } else {
        gfxprint_printf(&printer, "%s", "HIRU"); // "day"
    }

    *gfx = gfxprint_close(&printer);
    gfxprint_cleanup(&printer);
}
#endif

void henka_music(PlayState* play);
void kankyo_event_proc(PlayState* play);

void Global_kankyo_set(PlayState* play, EnvironmentContext* envCtx, LightContext* lightCtx, PauseContext* pauseCtx,
                        MessageContext* msgCtx, GameOverContext* gameOverCtx, GraphicsContext* gfxCtx) {
    f32 timeChangeBlend;
    f32 configChangeBlend = 0.0f;
    u16 i;
    u16 j;
    u16 time;
    EnvLightSettings* lightSettingsList = play->envCtx.lightSettingsList;
    u8 blendRate;

    if ((((void)0, z_common_data.gameMode) != GAMEMODE_NORMAL) &&
        (((void)0, z_common_data.gameMode) != GAMEMODE_END_CREDITS)) {
        z_vibctl2_StageCancel();
    }

    if (pauseCtx->state == PAUSE_STATE_OFF) {
        if (!IS_PAUSED(&play->pauseCtx)) {
            if (play->skyboxId == SKYBOX_NORMAL_SKY) {
                play->skyboxCtx.rot.y -= 0.001f;
            } else if (play->skyboxId == SKYBOX_CUTSCENE_MAP) {
                play->skyboxCtx.rot.y -= 0.005f;
            }
        }

        kankyo_event_proc(play);
        henka_music(play);

        if (((void)0, z_common_data.nextDayTime) >= 0xFF00 && ((void)0, z_common_data.nextDayTime) != NEXT_TIME_NONE) {
            z_common_data.nextDayTime -= 0x10;
            PRINTF("\nnext_zelda_time=[%x]", ((void)0, z_common_data.nextDayTime));

            // nextDayTime is used as both a time of day value and a timer to delay sfx when changing days.
            // When Sun's Song is played, nextDayTime is set to 0x8001 or 0 for day and night respectively.
            // These values will actually get used as a time of day value.
            // After this, nextDayTime is assigned magic values of 0xFFFE or 0xFFFD for day and night respectively.
            // From here, 0x10 is decremented from nextDayTime until it reaches either 0xFF0E or 0xFF0D, effectively
            // delaying the chicken crow or dog howl sfx by 15 frames when loading the new area.

            if (((void)0, z_common_data.nextDayTime) == (NEXT_TIME_DAY_SET - (15 * 0x10))) {
                Na_StartSystemSe_F(NA_SE_EV_CHICKEN_CRY_M);
                z_common_data.nextDayTime = NEXT_TIME_NONE;
            } else if (((void)0, z_common_data.nextDayTime) == (NEXT_TIME_NIGHT_SET - (15 * 0x10))) {
                Na_StartFixSe_F(NA_SE_EV_DOG_CRY_EVENING);
                z_common_data.nextDayTime = NEXT_TIME_NONE;
            }
        }

        if ((pauseCtx->state == PAUSE_STATE_OFF) && (gameOverCtx->state == GAMEOVER_INACTIVE)) {
            if (((msgCtx->msgLength == 0) && (msgCtx->msgMode == MSGMODE_NONE)) ||
                (((void)0, z_common_data.gameMode) == GAMEMODE_END_CREDITS)) {
                if ((envCtx->changeSkyboxTimer == 0) && !_Game_play_isPause(play) &&
                    (play->transitionMode == TRANS_MODE_OFF || ((void)0, z_common_data.gameMode) != GAMEMODE_NORMAL)) {

                    if (IS_DAY || E_day_time_plus >= 400) {
                        z_common_data.save.dayTime += E_day_time_plus;
                    } else {
                        z_common_data.save.dayTime += E_day_time_plus * 2; // time moves twice as fast at night
                    }
                }
            }
        }

        //! @bug `E_day_time_plus` is unsigned, it can't be negative
#if OOT_VERSION < PAL_1_0
        if ((((void)0, z_common_data.save.dayTime) > ((void)0, z_common_data.skyboxTime)) ||
            (((void)0, z_common_data.save.dayTime) < CLOCK_TIME(1, 0) || E_day_time_plus < 0))
#else
        if (((((void)0, z_common_data.sceneLayer) >= 5 || E_day_time_plus != 0) &&
             ((void)0, z_common_data.save.dayTime) > ((void)0, z_common_data.skyboxTime)) ||
            (((void)0, z_common_data.save.dayTime) < CLOCK_TIME(1, 0) || E_day_time_plus < 0))
#endif
        {

            z_common_data.skyboxTime = ((void)0, z_common_data.save.dayTime);
        }

        time = z_common_data.save.dayTime;

        if (time > CLOCK_TIME(18, 0) || time < CLOCK_TIME(6, 30)) {
            z_common_data.save.nightFlag = 1;
        } else {
            z_common_data.save.nightFlag = 0;
        }

#if DEBUG_FEATURES
        if (R_ENABLE_ARENA_DBG != 0 || CREG(2) != 0) {
            Gfx* displayList;
            Gfx* prevDisplayList;

            OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 1682);

            prevDisplayList = POLY_OPA_DISP;
            displayList = gfxopen(POLY_OPA_DISP);
            gSPDisplayList(OVERLAY_DISP++, displayList);
            Environment_PrintDebugInfo(play, &displayList);
            gSPEndDisplayList(displayList++);
            gfxclose(prevDisplayList, displayList);
            POLY_OPA_DISP = displayList;
            CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 1690);
        }
#endif

        if ((envCtx->lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE) &&
            (envCtx->lightBlendOverride != LIGHT_BLEND_OVERRIDE_FULL_CONTROL) &&
            (envCtx->lightSetting != envCtx->lightSettingOverride) && (envCtx->lightBlend >= 1.0f) &&
            (envCtx->lightSettingOverride <= LIGHT_SETTING_MAX)) {

            envCtx->lightBlend = 0.0f;
            envCtx->prevLightSetting = envCtx->lightSetting;
            envCtx->lightSetting = envCtx->lightSettingOverride;
        }

        if (envCtx->lightSettingOverride == LIGHT_SETTING_OVERRIDE_FULL_CONTROL) {
            // Do nothing; Skip updating lights based on time or light settings
        } else if ((envCtx->lightMode == LIGHT_MODE_TIME) &&
                   (envCtx->lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE)) {
            for (i = 0; i < ARRAY_COUNT(klight_chg[envCtx->lightConfig]); i++) {
                if ((z_common_data.skyboxTime >= klight_chg[envCtx->lightConfig][i].startTime) &&
                    ((z_common_data.skyboxTime < klight_chg[envCtx->lightConfig][i].endTime) ||
                     klight_chg[envCtx->lightConfig][i].endTime == 0xFFFF)) {
                    u8 blend8[2];
                    s16 blend16[2];

                    timeChangeBlend = get_parcent(klight_chg[envCtx->lightConfig][i].endTime,
                                                             klight_chg[envCtx->lightConfig][i].startTime,
                                                             ((void)0, z_common_data.skyboxTime));

                    E_colindex1 = klight_chg[envCtx->lightConfig][i].lightSetting & 3;
                    E_colindex2 = klight_chg[envCtx->lightConfig][i].nextLightSetting & 3;
                    E_colparcent = timeChangeBlend;

                    if (envCtx->changeLightEnabled) {
                        configChangeBlend =
                            ((f32)envCtx->changeDuration - envCtx->changeLightTimer) / envCtx->changeDuration;
                        envCtx->changeLightTimer--;

                        if (envCtx->changeLightTimer <= 0) {
                            envCtx->changeLightEnabled = false;
                            envCtx->lightConfig = envCtx->changeLightNextConfig;
                        }
                    }

                    for (j = 0; j < 3; j++) {
                        // blend ambient color
                        blend8[0] =
                            LERP(lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting]
                                     .ambientColor[j],
                                 lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting]
                                     .ambientColor[j],
                                 timeChangeBlend);
                        blend8[1] = LERP(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting]
                                .ambientColor[j],
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                                .ambientColor[j],
                            timeChangeBlend);
                        envCtx->lightSettings.ambientColor[j] = LERP(blend8[0], blend8[1], configChangeBlend);
                    }

                    // set light1 direction for the sun
                    envCtx->lightSettings.light1Dir[0] =
                        -(sin_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f);
                    envCtx->lightSettings.light1Dir[1] =
                        cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f;
                    envCtx->lightSettings.light1Dir[2] =
                        cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 20.0f;

                    // set light2 direction for the moon
                    envCtx->lightSettings.light2Dir[0] = -envCtx->lightSettings.light1Dir[0];
                    envCtx->lightSettings.light2Dir[1] = -envCtx->lightSettings.light1Dir[1];
                    envCtx->lightSettings.light2Dir[2] = -envCtx->lightSettings.light1Dir[2];

                    for (j = 0; j < 3; j++) {
                        // blend light1Color
                        blend8[0] =
                            LERP(lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting]
                                     .light1Color[j],
                                 lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting]
                                     .light1Color[j],
                                 timeChangeBlend);
                        blend8[1] = LERP(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting]
                                .light1Color[j],
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                                .light1Color[j],
                            timeChangeBlend);
                        envCtx->lightSettings.light1Color[j] = LERP(blend8[0], blend8[1], configChangeBlend);

                        // blend light2Color
                        blend8[0] =
                            LERP(lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting]
                                     .light2Color[j],
                                 lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting]
                                     .light2Color[j],
                                 timeChangeBlend);
                        blend8[1] = LERP(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting]
                                .light2Color[j],
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                                .light2Color[j],
                            timeChangeBlend);
                        envCtx->lightSettings.light2Color[j] = LERP(blend8[0], blend8[1], configChangeBlend);
                    }

                    // blend fogColor
                    for (j = 0; j < 3; j++) {
                        blend8[0] = LERP(
                            lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting].fogColor[j],
                            lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting]
                                .fogColor[j],
                            timeChangeBlend);
                        blend8[1] = LERP(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting]
                                .fogColor[j],
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                                .fogColor[j],
                            timeChangeBlend);
                        envCtx->lightSettings.fogColor[j] = LERP(blend8[0], blend8[1], configChangeBlend);
                    }

                    blend16[0] =
                        LERP16(ENV_LIGHT_SETTINGS_FOG_NEAR(
                                   lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting]
                                       .blendRateAndFogNear),
                               ENV_LIGHT_SETTINGS_FOG_NEAR(
                                   lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting]
                                       .blendRateAndFogNear),
                               timeChangeBlend);
                    blend16[1] = LERP16(
                        ENV_LIGHT_SETTINGS_FOG_NEAR(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting]
                                .blendRateAndFogNear),
                        ENV_LIGHT_SETTINGS_FOG_NEAR(
                            lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                                .blendRateAndFogNear),
                        timeChangeBlend);

                    envCtx->lightSettings.fogNear = LERP16(blend16[0], blend16[1], configChangeBlend);

                    blend16[0] =
                        LERP16(lightSettingsList[klight_chg[envCtx->lightConfig][i].lightSetting].zFar,
                               lightSettingsList[klight_chg[envCtx->lightConfig][i].nextLightSetting].zFar,
                               timeChangeBlend);
                    blend16[1] = LERP16(
                        lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].lightSetting].zFar,
                        lightSettingsList[klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting]
                            .zFar,
                        timeChangeBlend);

                    envCtx->lightSettings.zFar = LERP16(blend16[0], blend16[1], configChangeBlend);

#if DEBUG_FEATURES
                    if (klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting >=
                        envCtx->numLightSettings) {
                        PRINTF(VT_COL(RED, WHITE) T("\nカラーパレットの設定がおかしいようです！",
                                                    "\nThe color palette setting seems to be wrong!") VT_RST);

                        PRINTF(VT_COL(RED, WHITE) T("\n設定パレット＝[%d] 最後パレット番号＝[%d]\n",
                                                    "\nPalette setting = [%d] Last palette number = [%d]\n") VT_RST,
                               klight_chg[envCtx->changeLightNextConfig][i].nextLightSetting,
                               envCtx->numLightSettings - 1);
                    }
#endif

                    break;
                }
            }
        } else {
            if (!envCtx->lightBlendEnabled) {
                for (i = 0; i < 3; i++) {
                    envCtx->lightSettings.ambientColor[i] = lightSettingsList[envCtx->lightSetting].ambientColor[i];
                    envCtx->lightSettings.light1Dir[i] = lightSettingsList[envCtx->lightSetting].light1Dir[i];
                    envCtx->lightSettings.light1Color[i] = lightSettingsList[envCtx->lightSetting].light1Color[i];
                    envCtx->lightSettings.light2Dir[i] = lightSettingsList[envCtx->lightSetting].light2Dir[i];
                    envCtx->lightSettings.light2Color[i] = lightSettingsList[envCtx->lightSetting].light2Color[i];
                    envCtx->lightSettings.fogColor[i] = lightSettingsList[envCtx->lightSetting].fogColor[i];
                }

                envCtx->lightSettings.fogNear =
                    ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[envCtx->lightSetting].blendRateAndFogNear);
                envCtx->lightSettings.zFar = lightSettingsList[envCtx->lightSetting].zFar;
                envCtx->lightBlend = 1.0f;
            } else {
                blendRate =
                    ENV_LIGHT_SETTINGS_BLEND_RATE_U8(lightSettingsList[envCtx->lightSetting].blendRateAndFogNear);

                if (blendRate == 0) {
                    blendRate++;
                }

                if (envCtx->lightBlendRateOverride != LIGHT_BLENDRATE_OVERRIDE_NONE) {
                    blendRate = envCtx->lightBlendRateOverride;
                }

                if (envCtx->lightBlendOverride == LIGHT_BLEND_OVERRIDE_NONE) {
                    envCtx->lightBlend += blendRate / 255.0f;
                }

                if (envCtx->lightBlend > 1.0f) {
                    envCtx->lightBlend = 1.0f;
                }

                for (i = 0; i < 3; i++) {
                    envCtx->lightSettings.ambientColor[i] =
                        LERP(lightSettingsList[envCtx->prevLightSetting].ambientColor[i],
                             lightSettingsList[envCtx->lightSetting].ambientColor[i], envCtx->lightBlend);
                    envCtx->lightSettings.light1Dir[i] =
                        LERP16(lightSettingsList[envCtx->prevLightSetting].light1Dir[i],
                               lightSettingsList[envCtx->lightSetting].light1Dir[i], envCtx->lightBlend);
                    envCtx->lightSettings.light1Color[i] =
                        LERP(lightSettingsList[envCtx->prevLightSetting].light1Color[i],
                             lightSettingsList[envCtx->lightSetting].light1Color[i], envCtx->lightBlend);
                    envCtx->lightSettings.light2Dir[i] =
                        LERP16(lightSettingsList[envCtx->prevLightSetting].light2Dir[i],
                               lightSettingsList[envCtx->lightSetting].light2Dir[i], envCtx->lightBlend);
                    envCtx->lightSettings.light2Color[i] =
                        LERP(lightSettingsList[envCtx->prevLightSetting].light2Color[i],
                             lightSettingsList[envCtx->lightSetting].light2Color[i], envCtx->lightBlend);
                    envCtx->lightSettings.fogColor[i] =
                        LERP(lightSettingsList[envCtx->prevLightSetting].fogColor[i],
                             lightSettingsList[envCtx->lightSetting].fogColor[i], envCtx->lightBlend);
                }

                envCtx->lightSettings.fogNear =
                    LERP16(ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[envCtx->prevLightSetting].blendRateAndFogNear),
                           ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[envCtx->lightSetting].blendRateAndFogNear),
                           envCtx->lightBlend);
                envCtx->lightSettings.zFar = LERP16(lightSettingsList[envCtx->prevLightSetting].zFar,
                                                    lightSettingsList[envCtx->lightSetting].zFar, envCtx->lightBlend);
            }

#if DEBUG_FEATURES
            if (envCtx->lightSetting >= envCtx->numLightSettings) {
                PRINTF("\n" VT_FGCOL(RED)
                           T("カラーパレットがおかしいようです！", "The color palette seems to be wrong!"));

                PRINTF("\n" VT_FGCOL(YELLOW) T("設定パレット＝[%d] パレット数＝[%d]\n",
                                               "Palette setting = [%d] Last palette number = [%d]\n") VT_RST,
                       envCtx->lightSetting, envCtx->numLightSettings);
            }
#endif
        }

        envCtx->lightBlendEnabled = true;

        // Apply lighting adjustments
        for (i = 0; i < 3; i++) {
            if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]) > 255) {
                lightCtx->ambientColor[i] = 255;
            } else if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]) < 0) {
                lightCtx->ambientColor[i] = 0;
            } else {
                lightCtx->ambientColor[i] = (s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]);
            }

            if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]) > 255) {
                envCtx->dirLight1.params.dir.color[i] = 255;
            } else if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]) < 0) {
                envCtx->dirLight1.params.dir.color[i] = 0;
            } else {
                envCtx->dirLight1.params.dir.color[i] =
                    (s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]);
            }

            if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]) > 255) {
                envCtx->dirLight2.params.dir.color[i] = 255;
            } else if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]) < 0) {
                envCtx->dirLight2.params.dir.color[i] = 0;
            } else {
                envCtx->dirLight2.params.dir.color[i] =
                    (s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]);
            }

            if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]) > 255) {
                lightCtx->fogColor[i] = 255;
            } else if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]) < 0) {
                lightCtx->fogColor[i] = 0;
            } else {
                lightCtx->fogColor[i] = (s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]);
            }
        }

        // Set both directional light directions
        envCtx->dirLight1.params.dir.x = envCtx->lightSettings.light1Dir[0];
        envCtx->dirLight1.params.dir.y = envCtx->lightSettings.light1Dir[1];
        envCtx->dirLight1.params.dir.z = envCtx->lightSettings.light1Dir[2];

        envCtx->dirLight2.params.dir.x = envCtx->lightSettings.light2Dir[0];
        envCtx->dirLight2.params.dir.y = envCtx->lightSettings.light2Dir[1];
        envCtx->dirLight2.params.dir.z = envCtx->lightSettings.light2Dir[2];

        // Adjust fog near and far if necessary

        if ((envCtx->lightSettings.fogNear + envCtx->adjFogNear) <= ENV_FOGNEAR_MAX) {
            lightCtx->fogNear = envCtx->lightSettings.fogNear + envCtx->adjFogNear;
        } else {
            lightCtx->fogNear = ENV_FOGNEAR_MAX;
        }

        if ((envCtx->lightSettings.zFar + envCtx->adjZFar) <= ENV_ZFAR_MAX) {
            lightCtx->zFar = envCtx->lightSettings.zFar + envCtx->adjZFar;
        } else {
            lightCtx->zFar = ENV_ZFAR_MAX;
        }

#if DEBUG_FEATURES
        // When environment debug is enabled, various environment related variables can be configured via the reg editor
        if (R_ENV_DISABLE_DBG) {
            R_ENV_AMBIENT_COLOR(0) = lightCtx->ambientColor[0];
            R_ENV_AMBIENT_COLOR(1) = lightCtx->ambientColor[1];
            R_ENV_AMBIENT_COLOR(2) = lightCtx->ambientColor[2];

            R_ENV_LIGHT1_COLOR(0) = envCtx->dirLight1.params.dir.color[0];
            R_ENV_LIGHT1_COLOR(1) = envCtx->dirLight1.params.dir.color[1];
            R_ENV_LIGHT1_COLOR(2) = envCtx->dirLight1.params.dir.color[2];

            R_ENV_LIGHT2_COLOR(0) = envCtx->dirLight2.params.dir.color[0];
            R_ENV_LIGHT2_COLOR(1) = envCtx->dirLight2.params.dir.color[1];
            R_ENV_LIGHT2_COLOR(2) = envCtx->dirLight2.params.dir.color[2];

            R_ENV_FOG_COLOR(0) = lightCtx->fogColor[0];
            R_ENV_FOG_COLOR(1) = lightCtx->fogColor[1];
            R_ENV_FOG_COLOR(2) = lightCtx->fogColor[2];

            R_ENV_Z_FAR = lightCtx->zFar;
            R_ENV_FOG_NEAR = lightCtx->fogNear;

            R_ENV_LIGHT1_DIR(0) = envCtx->dirLight1.params.dir.x;
            R_ENV_LIGHT1_DIR(1) = envCtx->dirLight1.params.dir.y;
            R_ENV_LIGHT1_DIR(2) = envCtx->dirLight1.params.dir.z;

            R_ENV_LIGHT2_DIR(0) = envCtx->dirLight2.params.dir.x;
            R_ENV_LIGHT2_DIR(1) = envCtx->dirLight2.params.dir.y;
            R_ENV_LIGHT2_DIR(2) = envCtx->dirLight2.params.dir.z;

            R_ENV_WIND_DIR(0) = envCtx->windDirection.x;
            R_ENV_WIND_DIR(1) = envCtx->windDirection.y;
            R_ENV_WIND_DIR(2) = envCtx->windDirection.z;
            R_ENV_WIND_SPEED = envCtx->windSpeed;
        } else {
            lightCtx->ambientColor[0] = R_ENV_AMBIENT_COLOR(0);
            lightCtx->ambientColor[1] = R_ENV_AMBIENT_COLOR(1);
            lightCtx->ambientColor[2] = R_ENV_AMBIENT_COLOR(2);

            envCtx->dirLight1.params.dir.color[0] = R_ENV_LIGHT1_COLOR(0);
            envCtx->dirLight1.params.dir.color[1] = R_ENV_LIGHT1_COLOR(1);
            envCtx->dirLight1.params.dir.color[2] = R_ENV_LIGHT1_COLOR(2);

            envCtx->dirLight2.params.dir.color[0] = R_ENV_LIGHT2_COLOR(0);
            envCtx->dirLight2.params.dir.color[1] = R_ENV_LIGHT2_COLOR(1);
            envCtx->dirLight2.params.dir.color[2] = R_ENV_LIGHT2_COLOR(2);
            lightCtx->fogColor[0] = R_ENV_FOG_COLOR(0);
            lightCtx->fogColor[1] = R_ENV_FOG_COLOR(1);
            lightCtx->fogColor[2] = R_ENV_FOG_COLOR(2);
            lightCtx->fogNear = R_ENV_FOG_NEAR;
            lightCtx->zFar = R_ENV_Z_FAR;

            if (cREG(14)) {
                R_ENV_LIGHT1_DIR(0) = cos_s(cREG(10)) * cos_s(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.x = R_ENV_LIGHT1_DIR(0);
                R_ENV_LIGHT1_DIR(1) = sin_s(cREG(10)) * cos_s(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.y = R_ENV_LIGHT1_DIR(1);
                R_ENV_LIGHT1_DIR(2) = sin_s(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.z = R_ENV_LIGHT1_DIR(2);

                R_ENV_LIGHT2_DIR(0) = cos_s(cREG(12)) * cos_s(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.x = R_ENV_LIGHT2_DIR(0);
                R_ENV_LIGHT2_DIR(1) = sin_s(cREG(12)) * cos_s(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.y = R_ENV_LIGHT2_DIR(1);
                R_ENV_LIGHT2_DIR(2) = sin_s(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.z = R_ENV_LIGHT2_DIR(2);
            } else {
                envCtx->dirLight1.params.dir.x = R_ENV_LIGHT1_DIR(0);
                envCtx->dirLight1.params.dir.y = R_ENV_LIGHT1_DIR(1);
                envCtx->dirLight1.params.dir.z = R_ENV_LIGHT1_DIR(2);

                envCtx->dirLight2.params.dir.x = R_ENV_LIGHT2_DIR(0);
                envCtx->dirLight2.params.dir.y = R_ENV_LIGHT2_DIR(1);
                envCtx->dirLight2.params.dir.z = R_ENV_LIGHT2_DIR(2);
            }

            envCtx->windDirection.x = R_ENV_WIND_DIR(0);
            envCtx->windDirection.y = R_ENV_WIND_DIR(1);
            envCtx->windDirection.z = R_ENV_WIND_DIR(2);
            envCtx->windSpeed = R_ENV_WIND_SPEED;
        }
#endif

        if ((envCtx->dirLight1.params.dir.x == 0) && (envCtx->dirLight1.params.dir.y == 0) &&
            (envCtx->dirLight1.params.dir.z == 0)) {
            envCtx->dirLight1.params.dir.x = 1;
        }

        if ((envCtx->dirLight2.params.dir.x == 0) && (envCtx->dirLight2.params.dir.y == 0) &&
            (envCtx->dirLight2.params.dir.z == 0)) {
            envCtx->dirLight2.params.dir.x = 1;
        }
    }
}

void sun_moon_disp(PlayState* play) {
    f32 alpha;
    f32 color;
    f32 y;
    f32 scale;
    f32 temp;

    OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 2266);

    if (play->csCtx.state != CS_STATE_IDLE) {
        add_calc(&play->envCtx.sunPos.x,
                           -(sin_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f,
                           1.0f, 0.8f, 0.8f);
        add_calc(&play->envCtx.sunPos.y,
                           (cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f, 1.0f,
                           0.8f, 0.8f);
        //! @bug This should be z.
        add_calc(&play->envCtx.sunPos.y,
                           (cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f, 1.0f,
                           0.8f, 0.8f);
    } else {
        play->envCtx.sunPos.x = -(sin_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
        play->envCtx.sunPos.y = +(cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
        play->envCtx.sunPos.z = +(cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;
    }

    if (z_common_data.save.entranceIndex != ENTR_HYRULE_FIELD_0 || ((void)0, z_common_data.sceneLayer) != 5) {
        Matrix_translate(play->view.eye.x + play->envCtx.sunPos.x, play->view.eye.y + play->envCtx.sunPos.y,
                         play->view.eye.z + play->envCtx.sunPos.z, MTXMODE_NEW);

        y = play->envCtx.sunPos.y / 25.0f;
        temp = y / 80.0f;

        alpha = temp * 255.0f;
        if (alpha < 0.0f) {
            alpha = 0.0f;
        }
        if (alpha > 255.0f) {
            alpha = 255.0f;
        }

        alpha = 255.0f - alpha;

        color = temp;
        if (color < 0.0f) {
            color = 0.0f;
        }

        if (color > 1.0f) {
            color = 1.0f;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, (u8)(color * 75.0f) + 180, (u8)(color * 155.0f) + 100, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, (u8)(color * 255.0f), (u8)(color * 255.0f), alpha);

        scale = (color * 2.0f) + 10.0f;
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_kankyo.c", 2364);
        xlu_morf_texture_cullback(play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gSunDL);

        Matrix_translate(play->view.eye.x - play->envCtx.sunPos.x, play->view.eye.y - play->envCtx.sunPos.y,
                         play->view.eye.z - play->envCtx.sunPos.z, MTXMODE_NEW);

        color = -y / 120.0f;
        color = CLAMP_MIN(color, 0.0f);

        scale = -15.0f * color + 25.0f;
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);

        temp = -y / 80.0f;
        temp = CLAMP_MAX(temp, 1.0f);

        alpha = temp * 255.0f;

        if (alpha > 0.0f) {
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_kankyo.c", 2406);
            xlu_texture_cullback(play->state.gfxCtx);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 240, 255, 180, alpha);
            gDPSetEnvColor(POLY_OPA_DISP++, 80, 70, 20, alpha);
            gSPDisplayList(POLY_OPA_DISP++, gMoonDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 2429);
}

void sun_lens_disp(PlayState* play, EnvironmentContext* envCtx, View* view, GraphicsContext* gfxCtx,
                                  Vec3f pos, s32 unused) {
    if ((play->envCtx.precipitation[PRECIP_RAIN_CUR] == 0) && (play->envCtx.skyboxConfig == 0)) {
        dammy_sun_lens_disp(play, &play->envCtx, &play->view, play->state.gfxCtx, pos, 2000, 370,
                                  cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f, 400,
                                  true);
    }
}

f32 lens_size_tbl[] = { 23.0f, 12.0f, 7.0f, 5.0f, 3.0f, 10.0f, 6.0f, 2.0f, 3.0f, 1.0f };

typedef enum LensFlareType {
    /* 0 */ LENS_FLARE_CIRCLE0,
    /* 1 */ LENS_FLARE_CIRCLE1,
    /* 2 */ LENS_FLARE_RING
} LensFlareType;

void dammy_sun_lens_disp(PlayState* play, EnvironmentContext* envCtx, View* view, GraphicsContext* gfxCtx,
                               Vec3f pos, s32 unused, s16 scale, f32 colorIntensity, s16 glareStrength, u8 isSun) {
    s16 i;
    f32 tempX;
    f32 tempY;
    f32 tempZ;
    f32 lookDirX;
    f32 lookDirY;
    f32 lookDirZ;
    f32 tempX2;
    f32 tempY2;
    f32 tempZ2;
    f32 posDirX;
    f32 posDirY;
    f32 posDirZ;
    f32 length;
    f32 dist;
    f32 halfPosX;
    f32 halfPosY;
    f32 halfPosZ;
    f32 cosAngle;
    s32 pad;
    f32 lensFlareAlphaScaleTarget;
    u32 isOffScreen = false;
    f32 alpha;
    f32 adjScale;
    Vec3f screenPos;
    f32 fogInfluence;
    f32 temp;
    f32 glareAlphaScale;
    Color_RGB8 lensFlareColors[] = {
        { 155, 205, 255 }, // blue
        { 255, 255, 205 }, // yellow
        { 255, 255, 205 }, // yellow
        { 255, 255, 205 }, // yellow
        { 155, 255, 205 }, // green
        { 205, 255, 255 }, // light blue
        { 155, 155, 255 }, // dark blue
        { 205, 175, 255 }, // purple
        { 175, 255, 205 }, // light green
        { 255, 155, 235 }, // pink
    };
    u32 lensFlareAlphas[] = {
        50, 10, 25, 40, 70, 30, 50, 70, 50, 40,
    };
    u32 lensFlareTypes[] = {
        LENS_FLARE_RING,    LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
        LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
    };

    OPEN_DISPS(gfxCtx, "../z_kankyo.c", 2516);

    dist = Math3DLength(&pos, &view->eye) / 12.0f;

    // compute a unit vector in the look direction
    tempX = view->at.x - view->eye.x;
    tempY = view->at.y - view->eye.y;
    tempZ = view->at.z - view->eye.z;

    length = sqrtf(SQ(tempX) + SQ(tempY) + SQ(tempZ));

    lookDirX = tempX / length;
    lookDirY = tempY / length;
    lookDirZ = tempZ / length;

    // compute a position along the look vector half as far as pos
    halfPosX = view->eye.x + lookDirX * (dist * 6.0f);
    halfPosY = view->eye.y + lookDirY * (dist * 6.0f);
    halfPosZ = view->eye.z + lookDirZ * (dist * 6.0f);

    // compute a unit vector in the direction from halfPos to pos
    tempX2 = pos.x - halfPosX;
    tempY2 = pos.y - halfPosY;
    tempZ2 = pos.z - halfPosZ;

    length = sqrtf(SQ(tempX2) + SQ(tempY2) + SQ(tempZ2));

    posDirX = tempX2 / length;
    posDirY = tempY2 / length;
    posDirZ = tempZ2 / length;

    // compute the cosine of the angle between lookDir and posDir
    cosAngle = (lookDirX * posDirX + lookDirY * posDirY + lookDirZ * posDirZ) /
               sqrtf((SQ(lookDirX) + SQ(lookDirY) + SQ(lookDirZ)) * (SQ(posDirX) + SQ(posDirY) + SQ(posDirZ)));

    lensFlareAlphaScaleTarget = cosAngle * 3.5f;
    if (lensFlareAlphaScaleTarget > 1.0f) {
        lensFlareAlphaScaleTarget = 1.0f;
    }

    if (!isSun) {
        lensFlareAlphaScaleTarget = cosAngle;
    }

    if (cosAngle < 0.0f) {
        // don't draw lens flare
    } else {
        if (isSun) {
            Gama_play_Projection_Trans(play, &pos, &screenPos);
            E_lens_x = (s16)screenPos.x;
            E_lens_y = (s16)screenPos.y - 5.0f;
            if (lens_answer != GPACK_ZDZ(G_MAXFBZ, 0) || screenPos.x < 0.0f || screenPos.y < 0.0f ||
                screenPos.x > SCREEN_WIDTH || screenPos.y > SCREEN_HEIGHT) {
                isOffScreen = true;
            }
        }

        for (i = 0; i < ARRAY_COUNT(lensFlareTypes); i++) {
            Matrix_translate(pos.x, pos.y, pos.z, MTXMODE_NEW);

            if (isSun) {
                temp = get_parcent(60, 15, play->view.fovy);
            }

            Matrix_translate(-posDirX * i * dist, -posDirY * i * dist, -posDirZ * i * dist, MTXMODE_APPLY);
            adjScale = lens_size_tbl[i] * cosAngle;

            if (isSun) {
                adjScale *= 0.001 * (scale + 630.0f * temp);
            } else {
                adjScale *= 0.0001f * scale * (2.0f * dist);
            }

            Matrix_scale(adjScale, adjScale, adjScale, MTXMODE_APPLY);

            alpha = colorIntensity / 10.0f;
            alpha = CLAMP_MAX(alpha, 1.0f);
            alpha = alpha * lensFlareAlphas[i];
            alpha = CLAMP_MIN(alpha, 0.0f);

            fogInfluence = (ENV_FOGNEAR_MAX - play->lightCtx.fogNear) / 50.0f;

            fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

            alpha *= 1.0f - fogInfluence;

#if !PLATFORM_N64
            if (1) {}
#endif

            if (!(isOffScreen ^ 0)) {
                add_calc(&envCtx->lensFlareAlphaScale, lensFlareAlphaScaleTarget, 0.5f, 0.05f, 0.001f);
            } else {
                add_calc(&envCtx->lensFlareAlphaScale, 0.0f, 0.5f, 0.05f, 0.001f);
            }

            POLY_XLU_DISP = gfx_softsprite_prim_xlu(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, lensFlareColors[i].r, lensFlareColors[i].g, lensFlareColors[i].b,
                            alpha * envCtx->lensFlareAlphaScale);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_kankyo.c", 2662);
            gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0,
                              0, PRIMITIVE, 0);
            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gSPMatrix(POLY_XLU_DISP++, &D_01000000, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);

            switch (lensFlareTypes[i]) {
                case LENS_FLARE_CIRCLE0:
                case LENS_FLARE_CIRCLE1:
                    gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);
                    break;
                case LENS_FLARE_RING:
                    gSPDisplayList(POLY_XLU_DISP++, gLensFlareRingDL);
                    break;
            }
        }

        glareAlphaScale = cosAngle - (1.5f - cosAngle);

        if (glareStrength != 0) {
            if (glareAlphaScale > 0.0f) {
                POLY_XLU_DISP = fill_rectangle_prim_free(POLY_XLU_DISP);

                alpha = colorIntensity / 10.0f;
                alpha = CLAMP_MAX(alpha, 1.0f);
                alpha = alpha * glareStrength;
                alpha = CLAMP_MIN(alpha, 0.0f);

                fogInfluence = (ENV_FOGNEAR_MAX - play->lightCtx.fogNear) / 50.0f;

                fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

                alpha *= 1.0f - fogInfluence;

                gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
                gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

                if (!(isOffScreen ^ 0)) {
                    add_calc(&envCtx->glareAlpha, alpha * glareAlphaScale, 0.5f, 50.0f, 0.1f);
                } else {
                    add_calc(&envCtx->glareAlpha, 0.0f, 0.5f, 50.0f, 0.1f);
                }

                temp = colorIntensity / 120.0f;
                temp = CLAMP_MIN(temp, 0.0f);

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, (u8)(temp * 75.0f) + 180, (u8)(temp * 155.0f) + 100,
                                (u8)envCtx->glareAlpha);
                gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
            } else {
                envCtx->glareAlpha = 0.0f;
            }
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_kankyo.c", 2750);
}

f32 hani_get(void) {
    return fqrand() - 0.5f;
}

void rain_disp(PlayState* play, View* view, GraphicsContext* gfxCtx) {
    s16 i;
    s32 pad;
    Vec3f vec;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 length;
    f32 rotX;
    f32 rotY;
    f32 x50;
    f32 y50;
    f32 z50;
    f32 x280;
    f32 z280;
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f windDirection = { 0.0f, 0.0f, 0.0f };
    Player* player = GET_PLAYER(play);

#if OOT_VERSION < PAL_1_0
    if (!(play->cameraPtrs[CAM_ID_MAIN]->stateFlags & CAM_STATE_CAMERA_IN_WATER))
#else
    if (!(play->cameraPtrs[CAM_ID_MAIN]->stateFlags & CAM_STATE_CAMERA_IN_WATER) &&
        (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0))
#endif
    {
        OPEN_DISPS(gfxCtx, "../z_kankyo.c", 2799);

        vec.x = view->at.x - view->eye.x;
        vec.y = view->at.y - view->eye.y;
        vec.z = view->at.z - view->eye.z;

        length = sqrtf(SQXYZ(vec));

        temp1 = vec.x / length;
        temp2 = vec.y / length;
        temp3 = vec.z / length;

        x50 = view->eye.x + temp1 * 50.0f;
        y50 = view->eye.y + temp2 * 50.0f;
        z50 = view->eye.z + temp3 * 50.0f;

        x280 = view->eye.x + temp1 * 280.0f;
        z280 = view->eye.z + temp3 * 280.0f;

        if (play->envCtx.precipitation[PRECIP_RAIN_CUR]) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 30);
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);
        }

        // draw rain drops
        for (i = 0; i < play->envCtx.precipitation[PRECIP_RAIN_CUR]; i++) {
            temp2 = fqrand();
            temp1 = fqrand();
            temp3 = fqrand();

            Matrix_translate((temp2 - 0.7f) * 100.0f + x50, (temp1 - 0.7f) * 100.0f + y50,
                             (temp3 - 0.7f) * 100.0f + z50, MTXMODE_NEW);

            windDirection.x = play->envCtx.windDirection.x;
            windDirection.y = play->envCtx.windDirection.y;
            windDirection.z = play->envCtx.windDirection.z;

            vec.x = windDirection.x;
            vec.y = windDirection.y + 500.0f + fqrand() * 200.0f;
            vec.z = windDirection.z;
            length = sqrtf(SQXZ(vec));

            gSPMatrix(POLY_XLU_DISP++, &D_01000000, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            rotX = atanf_table(length, -vec.y);
            rotY = atanf_table(vec.z, vec.x);
            Matrix_rotateY(-rotY, MTXMODE_APPLY);
            Matrix_rotateX(M_PI / 2 - rotX, MTXMODE_APPLY);
            Matrix_scale(0.4f, 1.2f, 0.4f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_kankyo.c", 2887);
            gSPDisplayList(POLY_XLU_DISP++, gRaindropDL);
        }

        // draw droplet rings on the ground
        if (player->actor.world.pos.y < view->eye.y) {
            u8 materialFlag = false;

            for (i = 0; i < play->envCtx.precipitation[PRECIP_RAIN_CUR]; i++) {
                if (!materialFlag) {
                    _texture_z_light_fog_prim_xlu(gfxCtx);
                    gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 120);
                    materialFlag++;
                }

                Matrix_translate(hani_get() * 280.0f + x280, player->actor.world.pos.y + 2.0f,
                                 hani_get() * 280.0f + z280, MTXMODE_NEW);

                if ((LINK_IS_ADULT && ((player->actor.world.pos.y + 2.0f - view->eye.y) > -48.0f)) ||
                    (!LINK_IS_ADULT && ((player->actor.world.pos.y + 2.0f - view->eye.y) > -30.0f))) {
                    Matrix_scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);
                } else {
                    Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
                }

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_kankyo.c", 2940);
                gSPDisplayList(POLY_XLU_DISP++, gEffShockwaveDL);
            }
        }

        CLOSE_DISPS(gfxCtx, "../z_kankyo.c", 2946);
    }
}

void get_pol_color(PlayState* play, u32 lightSetting) {
    if ((play->envCtx.lightSetting != lightSetting) && (play->envCtx.lightBlend >= 1.0f) &&
        (play->envCtx.lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE)) {
        if (lightSetting >= LIGHT_SETTING_MAX) {
            lightSetting = 0;
        }

        play->envCtx.lightBlend = 0.0f;
        play->envCtx.prevLightSetting = play->envCtx.lightSetting;
        play->envCtx.lightSetting = lightSetting;
    }
}

/**
 * Draw color filters over the skybox. There are two filters.
 * The first uses the global fog color, and an alpha calculated with `fogNear`.
 * This filter draws unconditionally for skybox 29 at full alpha.
 * (note: skybox 29 is unused in the original game)
 * For the rest of the skyboxes it will draw if fogNear is less than 980.
 *
 * The second filter uses a custom color specified in `skyboxFilterColor`
 * and can be enabled with `customSkyboxFilter`.
 *
 * An example usage of a filter is to dim the skybox in cloudy conditions.
 */
void vr_kakusi(PlayState* play) {
    if (((play->skyboxId != SKYBOX_NONE) && (play->lightCtx.fogNear < 980)) || (play->skyboxId == SKYBOX_UNSET_1D)) {
        f32 alpha;

        OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3032);

        fill_rectangle_prim(play->state.gfxCtx);

        alpha = (1000 - play->lightCtx.fogNear) * 0.02f;

        if (play->skyboxId == SKYBOX_UNSET_1D) {
            alpha = 1.0f;
        }

        if (alpha > 1.0f) {
            alpha = 1.0f;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->lightCtx.fogColor[0], play->lightCtx.fogColor[1],
                        play->lightCtx.fogColor[2], 255.0f * alpha);
        gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

        CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3043);
    }

    if (play->envCtx.customSkyboxFilter) {
        OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3048);

        fill_rectangle_prim(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->envCtx.skyboxFilterColor[0], play->envCtx.skyboxFilterColor[1],
                        play->envCtx.skyboxFilterColor[2], play->envCtx.skyboxFilterColor[3]);
        gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

        CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3056);
    }
}

void rectangle_dsp(PlayState* play, u8 red, u8 green, u8 blue, u8 alpha) {
    OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3069);

    fill_rectangle_prim(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
    gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3079);
}

void electricthander(PlayState* play) {
    if (play->envCtx.lightningState != LIGHTNING_OFF) {
        switch (Eleki.state) {
            case LIGHTNING_STRIKE_WAIT:
                // every frame theres a 10% chance of the timer advancing 50 units
                if (fqrand() < 0.1f) {
                    Eleki.delayTimer += 50.0f;
                }

                Eleki.delayTimer += fqrand();

                if (Eleki.delayTimer > 500.0f) {
                    Eleki.flashRed = 200;
                    Eleki.flashGreen = 200;
                    Eleki.flashBlue = 255;
                    Eleki.flashAlphaTarget = 200;

                    Eleki.delayTimer = 0.0f;
                    thunder_start(play,
                                                  (u8)(fqrand() * (ARRAY_COUNT(Thunder) - 0.1f)) + 1);
                    E_eleki_alpha = 0;
                    Eleki.state++;
                }
                break;
            case LIGHTNING_STRIKE_START:
                Eleki.flashRed = 200;
                Eleki.flashGreen = 200;
                Eleki.flashBlue = 255;

                play->envCtx.adjAmbientColor[0] += 80;
                play->envCtx.adjAmbientColor[1] += 80;
                play->envCtx.adjAmbientColor[2] += 100;

                E_eleki_alpha += 100;

                if (E_eleki_alpha >= Eleki.flashAlphaTarget) {
                    Na_ControlNaturalSound(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_0, 0);
                    Eleki.state++;
                    Eleki.flashAlphaTarget = 0;
                }
                break;
            case LIGHTNING_STRIKE_END:
                if (play->envCtx.adjAmbientColor[0] > 0) {
                    play->envCtx.adjAmbientColor[0] -= 10;
                    play->envCtx.adjAmbientColor[1] -= 10;
                }

                if (play->envCtx.adjAmbientColor[2] > 0) {
                    play->envCtx.adjAmbientColor[2] -= 10;
                }

                E_eleki_alpha -= 10;

                if (E_eleki_alpha <= Eleki.flashAlphaTarget) {
                    play->envCtx.adjAmbientColor[0] = 0;
                    play->envCtx.adjAmbientColor[1] = 0;
                    play->envCtx.adjAmbientColor[2] = 0;

                    Eleki.state = LIGHTNING_STRIKE_WAIT;

                    if (play->envCtx.lightningState == LIGHTNING_LAST) {
                        play->envCtx.lightningState = LIGHTNING_OFF;
                    }
                }
                break;
        }
    }

    if (Eleki.state != LIGHTNING_STRIKE_WAIT) {
        rectangle_dsp(play, Eleki.flashRed, Eleki.flashGreen,
                                       Eleki.flashBlue, E_eleki_alpha);
    }
}

/**
 * Request the number of lightning bolts specified by `num`
 * Note: only 3 lightning bolts can be active at the same time.
 */
void thunder_start(PlayState* play, u8 num) {
    s16 boltsAdded = 0;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(Thunder); i++) {
        if (Thunder[i].state == LIGHTNING_BOLT_INACTIVE) {
            Thunder[i].state = LIGHTNING_BOLT_START;
            boltsAdded++;

            if (boltsAdded >= num) {
                break;
            }
        }
    }
}

/**
 * Draw any active lightning bolt entries contained in `Thunder`
 */
void scene_proc_thunder(PlayState* play, s32 unused) {
    static void* thunder_txt[] = {
        gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex,
        gEffLightning4Tex, gEffLightning5Tex, gEffLightning6Tex,
        gEffLightning7Tex, gEffLightning8Tex, NULL,
    };
    s16 i;
    f32 dx;
    f32 dz;
    f32 x;
    f32 z;
    s32 pad[2];
    Vec3f unused1 = { 0.0f, 0.0f, 0.0f };
    Vec3f env = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3253);

    for (i = 0; i < ARRAY_COUNT(Thunder); i++) {
        switch (Thunder[i].state) {
            case LIGHTNING_BOLT_START:
                dx = play->view.at.x - play->view.eye.x;
                dz = play->view.at.z - play->view.eye.z;

                x = dx / sqrtf(SQ(dx) + SQ(dz));
                z = dz / sqrtf(SQ(dx) + SQ(dz));

                Thunder[i].pos.x = play->view.eye.x + x * 9500.0f;
                Thunder[i].pos.y = fqrand() * 1000.0f + 4000.0f;
                Thunder[i].pos.z = play->view.eye.z + z * 9500.0f;

                Thunder[i].offset.x = (fqrand() - 0.5f) * 5000.0f;
                Thunder[i].offset.y = 0.0f;
                Thunder[i].offset.z = (fqrand() - 0.5f) * 5000.0f;

                Thunder[i].textureIndex = 0;
                Thunder[i].pitch = (fqrand() - 0.5f) * 40.0f;
                Thunder[i].roll = (fqrand() - 0.5f) * 40.0f;
                Thunder[i].delayTimer = 3 * (i + 1);
                Thunder[i].state++;
                break;
            case LIGHTNING_BOLT_WAIT:
                Thunder[i].delayTimer--;

                if (Thunder[i].delayTimer <= 0) {
                    Thunder[i].state++;
                }
                break;
            case LIGHTNING_BOLT_DRAW:
                if (Thunder[i].textureIndex < 7) {
                    Thunder[i].textureIndex++;
                } else {
                    Thunder[i].state = LIGHTNING_BOLT_INACTIVE;
                }
                break;
        }

        if (Thunder[i].state == LIGHTNING_BOLT_DRAW) {
            Matrix_translate(Thunder[i].pos.x + Thunder[i].offset.x,
                             Thunder[i].pos.y + Thunder[i].offset.y,
                             Thunder[i].pos.z + Thunder[i].offset.z, MTXMODE_NEW);
            Matrix_rotateX(DEG_TO_RAD(Thunder[i].pitch), MTXMODE_APPLY);
            Matrix_rotateZ(DEG_TO_RAD(Thunder[i].roll), MTXMODE_APPLY);
            Matrix_scale(22.0f, 100.0f, 22.0f, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 128);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 128);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_kankyo.c", 3333);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(thunder_txt[Thunder[i].textureIndex]));
            texture_z_cld_poly_xlu_nd(play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, &D_01000000, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffLightningDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 3353);
}

void stagemusic_play(PlayState* play) {
    play->envCtx.timeSeqState = TIMESEQ_DISABLED;

    // both lost woods exits on the bridge from kokiri to hyrule field
    if (((void)0, z_common_data.save.entranceIndex) == ENTR_LOST_WOODS_8 ||
        ((void)0, z_common_data.save.entranceIndex) == ENTR_LOST_WOODS_9) {
        Na_NscInitStart(NATURE_ID_KOKIRI_REGION);
    } else if (((void)0, z_common_data.forcedSeqId) != NA_BGM_GENERAL_SFX) {
        if (!Nottori_Bgmoff_check()) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, ((void)0, z_common_data.forcedSeqId));
        }
        z_common_data.forcedSeqId = NA_BGM_GENERAL_SFX;
    } else if (play->sceneSequences.seqId == NA_BGM_NO_MUSIC) {
        if (play->sceneSequences.natureAmbienceId == NATURE_ID_NONE) {
            return;
        }
        if (((void)0, z_common_data.natureAmbienceId) != play->sceneSequences.natureAmbienceId) {
            Na_NscInitStart(play->sceneSequences.natureAmbienceId);
        }
    } else if (play->sceneSequences.natureAmbienceId == NATURE_ID_NONE) {
        PRINTF(T("\n\n\nBGM設定game_play->sound_info.BGM=[%d] old_bgm=[%d]\n\n",
                 "\n\n\nBGM Configuration game_play->sound_info.BGM=[%d] old_bgm=[%d]\n\n"),
               play->sceneSequences.seqId, ((void)0, z_common_data.seqId));
        if (((void)0, z_common_data.seqId) != play->sceneSequences.seqId) {
            Na_StartContinueBgm(play->sceneSequences.seqId);
        }
    } else if (((void)0, z_common_data.save.dayTime) >= CLOCK_TIME(7, 0) &&
               ((void)0, z_common_data.save.dayTime) <= CLOCK_TIME(17, 10)) {
        if (((void)0, z_common_data.seqId) != play->sceneSequences.seqId) {
            Na_StartContinueBgm(play->sceneSequences.seqId);
        }

        play->envCtx.timeSeqState = TIMESEQ_FADE_DAY_BGM;
    } else {
        if (((void)0, z_common_data.natureAmbienceId) != play->sceneSequences.natureAmbienceId) {
            Na_NscInitStart(play->sceneSequences.natureAmbienceId);
        }

        if (((void)0, z_common_data.save.dayTime) > CLOCK_TIME(17, 10) &&
            ((void)0, z_common_data.save.dayTime) <= CLOCK_TIME(19, 0)) {
            play->envCtx.timeSeqState = TIMESEQ_EARLY_NIGHT_CRITTERS;
        } else if (((void)0, z_common_data.save.dayTime) > CLOCK_TIME(19, 0) + 1 ||
                   ((void)0, z_common_data.save.dayTime) < CLOCK_TIME(6, 30)) {
            play->envCtx.timeSeqState = TIMESEQ_NIGHT_CRITTERS;
        } else {
            play->envCtx.timeSeqState = TIMESEQ_MORNING_CRITTERS;
        }
    }

    PRINTF("\n-----------------\n", ((void)0, z_common_data.forcedSeqId));
    PRINTF(T("\n 強制ＢＧＭ=[%d]", "\n Forced BGM=[%d]"), ((void)0, z_common_data.forcedSeqId));
    PRINTF("\n     ＢＧＭ=[%d]", play->sceneSequences.seqId);
    PRINTF(T("\n     エンブ=[%d]", "\n      Embed=[%d]"), play->sceneSequences.natureAmbienceId);
    PRINTF("\n     status=[%d]", play->envCtx.timeSeqState);

    Na_SetEnvEcho(play->roomCtx.curRoom.echo);
}

void henka_music(PlayState* play) {
    switch (play->envCtx.timeSeqState) {
        case TIMESEQ_DAY_BGM:
            Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_4 << 4 | NATURE_CHANNEL_CRITTER_5,
                                             CHANNEL_IO_PORT_1, 0);

            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] == 0 && play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                PRINTF("\n\n\nNa_StartMorinigBgm\n\n");
                Na_StartMorinigBgm(play->sceneSequences.seqId);
            }

            play->envCtx.timeSeqState++;
            break;

        case TIMESEQ_FADE_DAY_BGM:
            if (z_common_data.save.dayTime > CLOCK_TIME(17, 10)) {
                if (play->envCtx.precipitation[PRECIP_RAIN_MAX] == 0 &&
                    play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 240);
                }

                play->envCtx.timeSeqState++;
            }
            break;

        case TIMESEQ_NIGHT_BEGIN_SFX:
            if (z_common_data.save.dayTime > CLOCK_TIME(18, 0)) {
                Na_StartFixSe_F(NA_SE_EV_DOG_CRY_EVENING);
                play->envCtx.timeSeqState++;
            }
            break;

        case TIMESEQ_EARLY_NIGHT_CRITTERS:
            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] == 0 && play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                Na_NscInitStart(play->sceneSequences.natureAmbienceId);
                Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_1, 1);
            }

            play->envCtx.timeSeqState++;
            break;

        case TIMESEQ_NIGHT_DELAY:
            if (z_common_data.save.dayTime > CLOCK_TIME(19, 0)) {
                play->envCtx.timeSeqState++;
            }
            break;

        case TIMESEQ_NIGHT_CRITTERS:
            Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_1, 0);

            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] == 0 && play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_1 << 4 | NATURE_CHANNEL_CRITTER_3,
                                                 CHANNEL_IO_PORT_1, 1);
            }

            play->envCtx.timeSeqState++;
            break;

        case TIMESEQ_DAY_BEGIN_SFX:
            if ((z_common_data.save.dayTime <= CLOCK_TIME(19, 0)) && (z_common_data.save.dayTime > CLOCK_TIME(6, 30))) {
                z_common_data.save.totalDays++;
                z_common_data.save.bgsDayCount++;
                z_common_data.dogIsLost = true;
                Na_StartSystemSe_F(NA_SE_EV_CHICKEN_CRY_M);

                if ((item_change_setting(play, ITEM_WEIRD_EGG, ITEM_CHICKEN) ||
                     item_change_setting(play, ITEM_POCKET_EGG, ITEM_POCKET_CUCCO)) &&
                    play->csCtx.state == 0 && !player_demo_check(play)) {
                    message_set(play, 0x3066, NULL);
                }

                play->envCtx.timeSeqState++;
            }
            break;

        case TIMESEQ_MORNING_CRITTERS:
            Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_1 << 4 | NATURE_CHANNEL_CRITTER_3,
                                             CHANNEL_IO_PORT_1, 0);

            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] == 0 && play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                Na_ControlNaturalSound(NATURE_CHANNEL_CRITTER_4 << 4 | NATURE_CHANNEL_CRITTER_5,
                                                 CHANNEL_IO_PORT_1, 1);
            }

            play->envCtx.timeSeqState++;
            break;

        case TIMESEQ_DAY_DELAY:
            if (z_common_data.save.dayTime > CLOCK_TIME(7, 0)) {
                play->envCtx.timeSeqState = 0;
            }
            break;
    }
}

void Tokusyu_lenseflare(PlayState* play) {
    Vec3f pos;

    if (E_lf_flag) {
        pos.x = E_lf_position.x;
        pos.y = E_lf_position.y;
        pos.z = E_lf_position.z;

        dammy_sun_lens_disp(play, &play->envCtx, &play->view, play->state.gfxCtx, pos, E_lf_kiten,
                                  E_lf_wscale, E_lf_alpha, E_lf_rectalpha, false);
    }
}

void Gameover_anten_init(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    P_col = 0;

    Light_point_ct(&K_light_data, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f, 0, 0, 0,
                              255);
    K_light_list = Global_light_list_new(play, &play->lightCtx, &K_light_data);

    Light_point_ct(&K2_light_data, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f, 0, 0, 0,
                              255);
    K2_light_list = Global_light_list_new(play, &play->lightCtx, &K2_light_data);
}

void Gameover_anten_proc(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;

    Light_point_ct(&K_light_data, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f,
                              P_col, P_col, P_col, 255);
    Light_point_ct(&K2_light_data, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f,
                              P_col, P_col, P_col, 255);

    if (P_col < 254) {
        P_col += 2;
    }

    if (Game_play_change_camera_check(play)) {
        for (i = 0; i < 3; i++) {
            if (play->envCtx.adjAmbientColor[i] > -255) {
                play->envCtx.adjAmbientColor[i] -= 12;
                play->envCtx.adjLight1Color[i] -= 12;
            }
            play->envCtx.adjFogColor[i] = -255;
        }

        if (play->envCtx.lightSettings.zFar + play->envCtx.adjZFar > 900) {
            play->envCtx.adjZFar -= 100;
        }

        if (play->envCtx.lightSettings.fogNear + play->envCtx.adjFogNear > 950) {
            play->envCtx.adjFogNear -= 10;
        }
    } else {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = 0;
        play->envCtx.screenFillColor[1] = 0;
        play->envCtx.screenFillColor[2] = 0;
        play->envCtx.screenFillColor[3] = P_col;
    }
}

void Gameover_anten_return(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;

    if (P_col >= 3) {
        P_col -= 3;
    } else {
        P_col = 0;
    }

    if (P_col == 1) {
        Global_light_list_delete(play, &play->lightCtx, K_light_list);
        Global_light_list_delete(play, &play->lightCtx, K2_light_list);
    } else if (P_col >= 2) {
        Light_point_ct(&K_light_data, (s16)player->actor.world.pos.x - 10.0f,
                                  (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f,
                                  P_col, P_col, P_col, 255);
        Light_point_ct(&K2_light_data, (s16)player->actor.world.pos.x + 10.0f,
                                  (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f,
                                  P_col, P_col, P_col, 255);
    }

    if (Game_play_change_camera_check(play)) {
        for (i = 0; i < 3; i++) {
            add_calc_short_angle2(&play->envCtx.adjAmbientColor[i], 0, 5, 12, 1);
            add_calc_short_angle2(&play->envCtx.adjLight1Color[i], 0, 5, 12, 1);
            play->envCtx.adjFogColor[i] = 0;
        }
        play->envCtx.adjZFar = 0;
        play->envCtx.adjFogNear = 0;
    } else {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = 0;
        play->envCtx.screenFillColor[1] = 0;
        play->envCtx.screenFillColor[2] = 0;
        play->envCtx.screenFillColor[3] = P_col;
        if (P_col == 0) {
            play->envCtx.fillScreen = false;
        }
    }
}

void kankyo_event_proc(PlayState* play) {
    u8 max = MAX(play->envCtx.precipitation[PRECIP_RAIN_MAX], play->envCtx.precipitation[PRECIP_SOS_MAX]);

    if (play->envCtx.precipitation[PRECIP_RAIN_CUR] != max && ((play->state.frames % 8) == 0)) {
        if (play->envCtx.precipitation[PRECIP_RAIN_CUR] < max) {
            play->envCtx.precipitation[PRECIP_RAIN_CUR] += 2;
        } else {
            play->envCtx.precipitation[PRECIP_RAIN_CUR] -= 2;
        }
    }
}

void Display_Fillter(GraphicsContext* gfxCtx, u8 red, u8 green, u8 blue, u8 alpha, u8 drawFlags) {
    if (alpha != 0) {
        OPEN_DISPS(gfxCtx, "../z_kankyo.c", 3835);

        if (drawFlags & FILL_SCREEN_OPA) {
            POLY_OPA_DISP = fill_rectangle_prim_free(POLY_OPA_DISP);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
            gDPSetAlphaDither(POLY_OPA_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_OPA_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        if (drawFlags & FILL_SCREEN_XLU) {
            POLY_XLU_DISP = fill_rectangle_prim_free(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, red, green, blue, alpha);

            if ((u32)alpha == 255) {
                gDPSetRenderMode(POLY_XLU_DISP++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
            }

            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        CLOSE_DISPS(gfxCtx, "../z_kankyo.c", 3863);
    }
}

Color_RGB8 tabel_prim[] = {
    { 210, 156, 85 },
    { 255, 200, 100 },
    { 225, 160, 50 },
    { 105, 90, 40 },
};

Color_RGB8 tabel_env[] = {
    { 155, 106, 35 },
    { 200, 150, 50 },
    { 170, 110, 0 },
    { 50, 40, 0 },
};

void Display_Sstorm(PlayState* play, u8 sandstormState) {
    s32 primA1;
    s32 envA1;
    s32 primA = play->envCtx.sandstormPrimA;
    s32 envA = play->envCtx.sandstormEnvA;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 pad;
    f32 sp98;

    switch (sandstormState) {
        case SANDSTORM_ACTIVE:
            if ((play->sceneId == SCENE_HAUNTED_WASTELAND) && (play->roomCtx.curRoom.num == 0)) {
                envA1 = 0;
                primA1 = (play->envCtx.sandstormEnvA > 128) ? 255 : play->envCtx.sandstormEnvA >> 1;
            } else {
                primA1 = play->state.frames % 128;
                if (primA1 > 64) {
                    primA1 = 128 - primA1;
                }
                primA1 += 73;
                envA1 = 128;
            }
            break;

        case SANDSTORM_FILL:
            primA1 = 255;
            envA1 = (play->envCtx.sandstormPrimA >= 255) ? 255 : 128;
            break;

        case SANDSTORM_UNFILL:
            envA1 = 128;
            if (play->envCtx.sandstormEnvA > 128) {
                primA1 = 255;
            } else {
                primA1 = play->state.frames % 128;
                if (primA1 > 64) {
                    primA1 = 128 - primA1;
                }
                primA1 += 73;
            }
            if ((primA1 >= primA) && (primA1 != 255)) {
                play->envCtx.sandstormState = SANDSTORM_ACTIVE;
            }
            break;

        case SANDSTORM_DISSIPATE:
            envA1 = 0;
            primA1 = (play->envCtx.sandstormEnvA > 128) ? 255 : play->envCtx.sandstormEnvA >> 1;

            if (primA == 0) {
                play->envCtx.sandstormState = SANDSTORM_OFF;
            }
            break;
    }

    if (ABS(primA - primA1) < 9) {
        primA = primA1;
    } else if (primA1 < primA) {
        primA -= 9;
    } else {
        primA += 9;
    }

    if (ABS(envA - envA1) < 9) {
        envA = envA1;
    } else if (envA1 < envA) {
        envA -= 9;
    } else {
        envA += 9;
    }

    play->envCtx.sandstormPrimA = primA;
    play->envCtx.sandstormEnvA = envA;

    sp98 = (512.0f - (primA + envA)) * (3.0f / 128.0f);

    if (sp98 > 6.0f) {
        sp98 = 6.0f;
    }

    if ((play->envCtx.lightMode != LIGHT_MODE_TIME) ||
        (play->envCtx.lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE)) {
        primColor.r = tabel_prim[1].r;
        primColor.g = tabel_prim[1].g;
        primColor.b = tabel_prim[1].b;
        envColor.r = tabel_env[1].r;
        envColor.g = tabel_env[1].g;
        envColor.b = tabel_env[1].b;
    } else if (E_colindex1 == E_colindex2) {
        primColor.r = tabel_prim[E_colindex1].r;
        primColor.g = tabel_prim[E_colindex1].g;
        primColor.b = tabel_prim[E_colindex1].b;
        envColor.r = tabel_env[E_colindex1].r;
        envColor.g = tabel_env[E_colindex1].g;
        envColor.b = tabel_env[E_colindex1].b;
    } else {
        primColor.r = (s32)F32_LERP(tabel_prim[E_colindex1].r,
                                    tabel_prim[E_colindex2].r, E_colparcent);
        primColor.g = (s32)F32_LERP(tabel_prim[E_colindex1].g,
                                    tabel_prim[E_colindex2].g, E_colparcent);
        primColor.b = (s32)F32_LERP(tabel_prim[E_colindex1].b,
                                    tabel_prim[E_colindex2].b, E_colparcent);
        envColor.r = (s32)F32_LERP(tabel_env[E_colindex1].r,
                                   tabel_env[E_colindex2].r, E_colparcent);
        envColor.g = (s32)F32_LERP(tabel_env[E_colindex1].g,
                                   tabel_env[E_colindex2].g, E_colparcent);
        envColor.b = (s32)F32_LERP(tabel_env[E_colindex1].b,
                                   tabel_env[E_colindex2].b, E_colparcent);
    }

    envColor.r = ((envColor.r * sp98) + ((6.0f - sp98) * primColor.r)) * (1.0f / 6.0f);
    envColor.g = ((envColor.g * sp98) + ((6.0f - sp98) * primColor.g)) * (1.0f / 6.0f);
    envColor.b = ((envColor.b * sp98) + ((6.0f - sp98) * primColor.b)) * (1.0f / 6.0f);

    {
        u16 sp96 = (s32)(sound_scroll * (11.0f / 6.0f));
        u16 sp94 = (s32)(sound_scroll * (9.0f / 6.0f));
        u16 sp92 = (s32)(sound_scroll * (6.0f / 6.0f));

        OPEN_DISPS(play->state.gfxCtx, "../z_kankyo.c", 4044);

        POLY_XLU_DISP = gfx_xlu_rectangle_2c(POLY_XLU_DISP);

        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColor.r, primColor.g, primColor.b, play->envCtx.sandstormPrimA);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, play->envCtx.sandstormEnvA);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (u32)sp96 % 4096, 0, 512, 32, 1,
                                    (u32)sp94 % 4096, 4095 - ((u32)sp92 % 4096), 256, 64));
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        gSPDisplayList(POLY_XLU_DISP++, gFieldSandstormDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_kankyo.c", 4068);
    }

    sound_scroll += (s32)sp98;
}

void set_add_light_global(PlayState* play, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp;
    s32 i;

    if (play->roomCtx.curRoom.type != ROOM_TYPE_BOSS && Game_play_change_camera_check(play)) {
        arg1 = CLAMP_MIN(arg1, 0.0f);
        arg1 = CLAMP_MAX(arg1, 1.0f);

        temp = arg1 - arg3;

        if (arg1 < arg3) {
            temp = 0.0f;
        }

        play->envCtx.adjFogNear = (arg2 - play->envCtx.lightSettings.fogNear) * temp;

        if (arg1 == 0.0f) {
            for (i = 0; i < 3; i++) {
                play->envCtx.adjFogColor[i] = 0;
            }
        } else {
            temp = arg1 * 5.0f;
            temp = CLAMP_MAX(temp, 1.0f);

            for (i = 0; i < 3; i++) {
                play->envCtx.adjFogColor[i] = -(s16)(play->envCtx.lightSettings.fogColor[i] * temp);
            }
        }

        if (arg4 <= 0.0f) {
            return;
        }

        arg1 *= arg4;

        for (i = 0; i < 3; i++) {
            play->envCtx.adjAmbientColor[i] = -(s16)(play->envCtx.lightSettings.ambientColor[i] * arg1);
            play->envCtx.adjLight1Color[i] = -(s16)(play->envCtx.lightSettings.light1Color[i] * arg1);
        }
    }
}

s32 Get_Eventday(void) {
    return z_common_data.save.bgsDayCount;
}

void Clr_Eventday(void) {
    z_common_data.save.bgsDayCount = 0;
}

s32 Get_Totalday(void) {
    return z_common_data.save.totalDays;
}

void Nottori_Bgm(u16 seqId) {
    z_common_data.forcedSeqId = seqId;
}

s32 Nottori_Bgmoff_check(void) {
    s32 isDisabled = false;

    if (z_common_data.forcedSeqId == NA_BGM_DISABLED) {
        isDisabled = true;
    }

    return isDisabled;
}

void Rain_sound_set_on(PlayState* play) {
    if (play->sceneSequences.natureAmbienceId == NATURE_ID_NONE) {
        Na_NscInitStart(NATURE_ID_MARKET_NIGHT);
    } else {
        Na_NscInitStart(play->sceneSequences.natureAmbienceId);
    }

    Na_ControlNaturalSound(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 1);
    Na_ControlNaturalSound(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 1);
}

void Rain_sound_set_off(PlayState* play) {
    Na_ControlNaturalSound(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
    Na_ControlNaturalSound(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);

    if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
        z_common_data.seqId = NA_BGM_NATURE_SFX_RAIN;
        stagemusic_play(play);
    }
}

void OcaWarpJumpProc(PlayState* play) {
    E_wether_flg = WEATHER_MODE_CLEAR;
    z_common_data.save.cutsceneIndex = 0;
    z_common_data.respawnFlag = -3;
    play->nextEntranceIndex = z_common_data.respawn[RESPAWN_MODE_RETURN].entranceIndex;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE;

    switch (play->nextEntranceIndex) {
        case ENTR_DEATH_MOUNTAIN_CRATER_0:
            event_set(EVENTCHKINF_B9);
            break;

        case ENTR_LAKE_HYLIA_0:
            event_set(EVENTCHKINF_B1);
            break;

        case ENTR_DESERT_COLOSSUS_0:
            event_set(EVENTCHKINF_B8);
            break;

        case ENTR_GRAVEYARD_0:
            event_set(EVENTCHKINF_B6);
            break;

        case ENTR_TEMPLE_OF_TIME_0:
            event_set(EVENTCHKINF_A7);
            break;

        case ENTR_SACRED_FOREST_MEADOW_0:
            break;
    }
}
