/*
 * File: z_en_weather_tag.c
 * Overlay: ovl_En_Weather_Tag
 * Description: Proximity Activated Weather Effects
 */

#include "z_en_weather_tag.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Weather_Tag_actor_ct(Actor* thisx, PlayState* play);
void En_Weather_Tag_actor_dt(Actor* thisx, PlayState* play);
void En_Weather_Tag_actor_move(Actor* thisx, PlayState* play);

void mode_weather_dark(EnWeatherTag* this, PlayState* play);
void mode_weather_dark_off(EnWeatherTag* this, PlayState* play);
void mode_weather_cloud(EnWeatherTag* this, PlayState* play);
void mode_weather_cloud_off(EnWeatherTag* this, PlayState* play);
void mode_weather_snow(EnWeatherTag* this, PlayState* play);
void mode_weather_snow_off(EnWeatherTag* this, PlayState* play);
void mode_weather_rain(EnWeatherTag* this, PlayState* play);
void mode_weather_rain_off(EnWeatherTag* this, PlayState* play);
void mode_weather_cloud2(EnWeatherTag* this, PlayState* play);
void mode_weather_cloud_off2(EnWeatherTag* this, PlayState* play);
void mode_weather_thunderrain(EnWeatherTag* this, PlayState* play);
void mode_weather_thunderrain_off(EnWeatherTag* this, PlayState* play);
void mode_weather_storm(EnWeatherTag* this, PlayState* play);
void mode_haka_rain(EnWeatherTag* this, PlayState* play);
void mode_haka_rain_off(EnWeatherTag* this, PlayState* play);

#define WEATHER_TAG_RANGE100(x) ((x >> 8) * 100.0f)

ActorProfile En_Weather_Tag_Profile = {
    /**/ ACTOR_EN_WEATHER_TAG,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnWeatherTag),
    /**/ En_Weather_Tag_actor_ct,
    /**/ En_Weather_Tag_actor_dt,
    /**/ En_Weather_Tag_actor_move,
    /**/ NULL,
};

void En_Weather_Tag_actor_set_process(EnWeatherTag* this, EnWeatherTagActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Weather_Tag_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Weather_Tag_actor_ct(Actor* thisx, PlayState* play) {
    EnWeatherTag* this = (EnWeatherTag*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    switch (PARAMS_GET_U(this->actor.params, 0, 4)) {
        case EN_WEATHER_TAG_TYPE_CLOUDY_MARKET:
            PRINTF("\n\n");
            // "☆☆☆☆☆ (;o;) About ☆☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ （;o;) くらいよー ☆☆☆☆☆ \n" VT_RST);
            En_Weather_Tag_actor_set_process(this, mode_weather_dark);
            break;
        case EN_WEATHER_TAG_TYPE_CLOUDY_LON_LON_RANCH:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Cloudy (._.) Ah Melancholy ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり (._.) あーあ 憂鬱 ☆☆☆☆☆ \n" VT_RST);
            if (event_check(EVENTCHKINF_EPONA_OBTAINED)) {
                Actor_delete(&this->actor);
            }
            En_Weather_Tag_actor_set_process(this, mode_weather_cloud);
            break;
        case EN_WEATHER_TAG_TYPE_SNOW_ZORAS_DOMAIN:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Yukigafuru You won't come (._.) ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ ゆきがふるー あなたはこないー (._.) ☆☆☆☆☆ \n" VT_RST);

            if (GET_EVENTCHKINF(EVENTCHKINF_4A)) {
                Actor_delete(&this->actor);
            }
            En_Weather_Tag_actor_set_process(this, mode_weather_snow);
            break;
        case EN_WEATHER_TAG_TYPE_RAIN_LAKE_HYLIA:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Wow wa wa na wa saa ki ha (^o^) ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ わわわわー なーがーさーきーはー (^o^) ☆☆☆☆☆ \n" VT_RST);

            if (GET_EVENTCHKINF(EVENTCHKINF_4A)) {
                Actor_delete(&this->actor);
            }
            En_Weather_Tag_actor_set_process(this, mode_weather_rain);
            break;
        case EN_WEATHER_TAG_TYPE_CLOUDY_DEATH_MOUNTAIN:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Cloudy (._.) Ah Melancholy ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり (._.) あーあ 憂鬱 ☆☆☆☆☆ \n" VT_RST);
            if (GET_EVENTCHKINF(EVENTCHKINF_49)) {
                Actor_delete(&this->actor);
            }
            En_Weather_Tag_actor_set_process(this, mode_weather_cloud2);
            break;
        case EN_WEATHER_TAG_TYPE_THUNDERSTORM_KAKARIKO:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Cloudy Rain Thunder (;O;) Uo Melancholy ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり雨雷 (;O;) うおお 憂鬱 ☆☆☆☆☆ \n" VT_RST);

            if (!GET_EVENTCHKINF(EVENTCHKINF_48) || !GET_EVENTCHKINF(EVENTCHKINF_49) ||
                !GET_EVENTCHKINF(EVENTCHKINF_4A) || CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                Actor_delete(&this->actor);
            }
            En_Weather_Tag_actor_set_process(this, mode_weather_thunderrain);
            break;
        case EN_WEATHER_TAG_TYPE_SANDSTORM_INTENSITY:
            PRINTF("\n\n");
            // "☆☆☆☆☆ The desert becomes thicker ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 砂漠が濃くなります ☆☆☆☆☆ \n" VT_RST);
            En_Weather_Tag_actor_set_process(this, mode_weather_storm);
            break;
        case EN_WEATHER_TAG_TYPE_THUNDERSTORM_GRAVEYARD:
            PRINTF("\n\n");
            // "☆☆☆☆☆ Wow wa wa na wa saa ki ha (^o^) ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ わわわわー なーがーさーきーはー (^o^) ☆☆☆☆☆ \n" VT_RST);

            En_Weather_Tag_actor_set_process(this, mode_haka_rain);
            break;
    }
}

u8 mode_weather_control(EnWeatherTag* this, PlayState* play, u8 skyboxConfig, u8 changeSkyboxNextConfig,
                                       u8 lightConfig, u8 changeLightNextConfig, u16 changeDuration, u8 weatherMode) {
    s32 pad;
    u8 ret = false;
    Player* player = GET_PLAYER(play);

    if (Actor_search_actor_distanceXZ(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        if ((play->envCtx.lightMode != LIGHT_MODE_TIME) || !E_vr_box_ovl ||
            (play->skyboxId != SKYBOX_NORMAL_SKY && play->envCtx.lightConfig == play->envCtx.changeLightNextConfig)) {
            E_wether_apl = true;
            if (play->envCtx.stormRequest == STORM_REQUEST_NONE &&
                ((play->envCtx.lightMode != LIGHT_MODE_TIME) ||
                 (play->envCtx.lightConfig != 1 && !play->envCtx.changeLightEnabled))) {
#if OOT_VERSION >= PAL_1_0
                E_wether_apl = false;
#endif
                if (E_wether_flg != weatherMode) {
#if OOT_VERSION < PAL_1_0
                    E_wether_apl = false;
#endif
                    E_wether_flg = weatherMode;
                    if (play->envCtx.stormRequest == STORM_REQUEST_NONE) {
                        play->envCtx.changeSkyboxState = CHANGE_SKYBOX_REQUESTED;
                        play->envCtx.skyboxConfig = skyboxConfig;
                        play->envCtx.changeSkyboxNextConfig = changeSkyboxNextConfig;
                        play->envCtx.changeSkyboxTimer = changeDuration;
                        play->envCtx.changeLightEnabled = true;
                        play->envCtx.lightConfig = lightConfig;
                        play->envCtx.changeLightNextConfig = changeLightNextConfig;
                        E_water_colbak = changeLightNextConfig;
                        play->envCtx.changeDuration = changeDuration;
                        play->envCtx.changeLightTimer = play->envCtx.changeDuration;
                    }
                }
                ret = true;
            }
        } else {
            if (E_day_time_plus != 0) {
                z_common_data.save.dayTime += 20;
            }
        }
    }

    return ret;
}

u8 mode_weather_control2(EnWeatherTag* this, PlayState* play, u8 skyboxConfig, u8 changeSkyboxNextConfig,
                                  u8 lightConfig, u8 changeLightNextConfig, u16 changeDuration) {
    s32 pad;
    u8 ret = false;
    Player* player = GET_PLAYER(play);

    if ((WEATHER_TAG_RANGE100(this->actor.params) + 100.0f) < Actor_search_actor_distanceXZ(&player->actor, &this->actor)) {
        if ((play->envCtx.lightMode != LIGHT_MODE_TIME) || !E_vr_box_ovl ||
            (play->skyboxId != SKYBOX_NORMAL_SKY && play->envCtx.lightConfig == play->envCtx.changeLightNextConfig)) {
            E_wether_apl = true;
            if ((play->envCtx.stormRequest == STORM_REQUEST_NONE) &&
                ((play->envCtx.lightMode != LIGHT_MODE_TIME) ||
                 (play->envCtx.lightConfig != 1 && !play->envCtx.changeLightEnabled))) {
                E_wether_apl = false;
                E_wether_flg = WEATHER_MODE_CLEAR;
                play->envCtx.changeSkyboxState = CHANGE_SKYBOX_REQUESTED;
                play->envCtx.skyboxConfig = skyboxConfig;
                play->envCtx.changeSkyboxNextConfig = changeSkyboxNextConfig;
                play->envCtx.changeSkyboxTimer = changeDuration;
                play->envCtx.changeLightEnabled = true;
                play->envCtx.lightConfig = lightConfig;
                play->envCtx.changeLightNextConfig = changeLightNextConfig;
                E_water_colbak = changeLightNextConfig;
                play->envCtx.changeDuration = changeDuration;
                play->envCtx.changeLightTimer = play->envCtx.changeDuration;

                ret = true;
            }
        } else if (E_day_time_plus != 0) {
            z_common_data.save.dayTime += 20;
        }
    }
    return ret;
}

void mode_weather_dark(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 3, 60, WEATHER_MODE_CLOUDY_CONFIG3)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_dark_off);
    }
}

void mode_weather_dark_off(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 3, 0, 60)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_dark);
    }
}

void mode_weather_cloud(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 2, 100, WEATHER_MODE_CLOUDY_CONFIG2)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_cloud_off);
    }
}

void mode_weather_cloud_off(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 2, 0, 100)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_cloud);
    }
}

void mode_weather_cloud2(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 2, 60, WEATHER_MODE_CLOUDY_CONFIG2)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_cloud_off2);
    }
}

void mode_weather_cloud_off2(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 2, 0, 60)) {
        En_Weather_Tag_actor_set_process(this, mode_weather_cloud);
    }
}

void mode_weather_snow(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 2, 60, WEATHER_MODE_SNOW)) {
        play->envCtx.precipitation[PRECIP_SNOW_MAX] = 64;
        En_Weather_Tag_actor_set_process(this, mode_weather_snow_off);
    }
}

void mode_weather_snow_off(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 2, 0, 60)) {
        play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
        En_Weather_Tag_actor_set_process(this, mode_weather_snow);
    }
}

void mode_weather_rain(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 2, 100, WEATHER_MODE_RAIN)) {
        Rain_sound_set_on(play);
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 25;
        En_Weather_Tag_actor_set_process(this, mode_weather_rain_off);
    }
}

void mode_weather_rain_off(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 2, 0, 100)) {
        Rain_sound_set_off(play);
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
        En_Weather_Tag_actor_set_process(this, mode_weather_rain);
    }
}

void mode_weather_thunderrain(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control(this, play, 0, 1, 0, 4, 100, WEATHER_MODE_HEAVY_RAIN)) {
        Rain_sound_set_on(play);
        play->envCtx.lightningState = LIGHTNING_ON;
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 30;
        En_Weather_Tag_actor_set_process(this, mode_weather_thunderrain_off);
    }
}

void mode_weather_thunderrain_off(EnWeatherTag* this, PlayState* play) {
    if (mode_weather_control2(this, play, 1, 0, 4, 0, 100)) {
        Rain_sound_set_off(play);
        play->envCtx.lightningState = LIGHTNING_LAST;
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
        En_Weather_Tag_actor_set_process(this, mode_weather_thunderrain);
    }
}

void mode_weather_storm(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_search_actor_distanceXZ(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        add_calc_short_angle2(&play->envCtx.adjFogNear, -80, 1, 2, 1);
        add_calc_short_angle2(&play->envCtx.adjZFar, -2000, 1, 50, 1);
    } else {
        add_calc_short_angle2(&play->envCtx.adjFogNear, 0, 1, 1, 1);
        add_calc_short_angle2(&play->envCtx.adjZFar, 0, 1, 25, 1);
    }
}

void mode_haka_rain(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_search_actor_distanceXZ(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        Rain_sound_set_on(play);
        play->envCtx.lightningState = LIGHTNING_ON;
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 25;
        En_Weather_Tag_actor_set_process(this, mode_haka_rain_off);
    }
}

void mode_haka_rain_off(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((WEATHER_TAG_RANGE100(this->actor.params) + 10.0f) < Actor_search_actor_distanceXZ(&player->actor, &this->actor)) {
        Rain_sound_set_off(play);
        play->envCtx.lightningState = LIGHTNING_LAST;
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
        play->envCtx.precipitation[PRECIP_RAIN_CUR] = 10;
        En_Weather_Tag_actor_set_process(this, mode_haka_rain);
    }
}

void En_Weather_Tag_actor_move(Actor* thisx, PlayState* play) {
    EnWeatherTag* this = (EnWeatherTag*)thisx;

    this->actionFunc(this, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 255, 255, 4, play->state.gfxCtx);
    }
}
