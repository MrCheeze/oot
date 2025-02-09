/*
 * File: z_en_river_sound.c
 * Overlay: ovl_En_River_Sound
 * Description: Controls various sounds. Includes sound effects and bgms
 */

#include "z_en_river_sound.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_River_Sound_actor_ct(Actor* thisx, PlayState* play);
void En_River_Sound_actor_dt(Actor* thisx, PlayState* play);
void En_River_Sound_actor_move(Actor* thisx, PlayState* play);
void En_River_Sound_actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_River_Sound_Profile = {
    /**/ ACTOR_EN_RIVER_SOUND,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnRiverSound),
    /**/ En_River_Sound_actor_ct,
    /**/ En_River_Sound_actor_dt,
    /**/ En_River_Sound_actor_move,
    /**/ En_River_Sound_actor_draw,
};

void En_River_Sound_actor_ct(Actor* thisx, PlayState* play) {
    EnRiverSound* this = (EnRiverSound*)thisx;

    this->playSfx = false;
    this->pathIndex = PARAMS_GET_U(this->actor.params, 8, 8);
    this->actor.params = PARAMS_GET_U(this->actor.params, 0, 8);

    if (this->actor.params >= RS_GANON_TOWER_0) {
        // Incrementally increase volume of NA_BGM_GANON_TOWER for each new room during the climb of Ganon's Tower
        Na_SetVolumeDistanceBgm(this->actor.params - RS_GANON_TOWER_0);
        Actor_delete(&this->actor);
    } else if (this->actor.params == RS_NATURE_AMBIENCE) {
        Na_NscInitStart(NATURE_ID_KOKIRI_REGION);
        Actor_delete(&this->actor);
    } else if (this->actor.params == RS_LOST_WOODS_SARIAS_SONG) {
        if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) || CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
            Actor_delete(&this->actor);
        }
    }
}

void En_River_Sound_actor_dt(Actor* thisx, PlayState* play) {
    EnRiverSound* this = (EnRiverSound*)thisx;

    if (this->actor.params == RS_LOST_WOODS_SARIAS_SONG) {
        Na_ClearSariaMelodyTag2(&this->actor.projectedPos);
    } else if (this->actor.params == RS_GORON_CITY_SARIAS_SONG) {
        Na_ClearCrossBgmTag();
    }
}

/**
 * Determines the closest point to hearPos that is contained on the line connecting points A & B
 * If the closest point on the line will not be on the line segment connecting points A or B, return false.
 * Otherwise, calculate the point between A & B, assign it to `newSoundPos`, and return true
 */
s32 get_norm_line_pos(Vec3f* pointA, Vec3f* pointB, Vec3f* hearPos, Vec3f* newSoundPos) {
    Vec3f lineSeg[3];
    f32 temp;

    // Line Segment from the hearPos to the first point in the path
    lineSeg[0].x = pointA->x - hearPos->x;
    lineSeg[0].y = pointA->y - hearPos->y;
    lineSeg[0].z = pointA->z - hearPos->z;

    // Line Segment from the hearPos to the second point in the path
    lineSeg[1].x = pointB->x - hearPos->x;
    lineSeg[1].y = pointB->y - hearPos->y;
    lineSeg[1].z = pointB->z - hearPos->z;

    // Line Segment from the first point to the second point in the path
    lineSeg[2].x = lineSeg[1].x - lineSeg[0].x;
    lineSeg[2].y = lineSeg[1].y - lineSeg[0].y;
    lineSeg[2].z = lineSeg[1].z - lineSeg[0].z;

    temp = DOTXYZ(lineSeg[2], lineSeg[0]);

    /**
     *  |                   |
     *  |                   |
     *  |                   |
     *  A ----------------- B
     *  |                   |
     *  |                   |
     *  |                   |
     * This condition uses dot products to check to see that `hearPos` is contained within the above region
     * i.e. The closest point on line AB must be between A & B
     */
    if ((DOTXYZ(lineSeg[2], lineSeg[1]) * temp) < 0.0f) {
        temp = -temp / SQXYZ(lineSeg[2]);

        // Closest point to hearPos contained on line segment A-B
        newSoundPos->x = (lineSeg[2].x * temp) + pointA->x;
        newSoundPos->y = (lineSeg[2].y * temp) + pointA->y;
        newSoundPos->z = (lineSeg[2].z * temp) + pointA->z;

        return true;
    }

    return false;
}

/**
 * Writes the position along the river path to `sfxPos` based on the `hearPos`, which is usually the position of the
 * player.
 * Returns true if the distance between the `hearPos` and `sfxPos` is less than 10000, false if not.
 */
s32 get_river_sound_pos(Vec3s* points, s32 numPoints, Vec3f* hearPos, Vec3f* sfxPos) {
    s32 i;
    s32 closestPointIdx;
    s32 useAdjacentPoints[2] = {
        false, // use previous point
        false, // use next point
    };
    Vec3f closestPointPos;
    Vec3f nextLineSegClosestPos;
    Vec3f prevLineSegClosestPos;
    Vec3f point;
    f32 closestPointDist = 10000.0f;
    Vec3s* closestPoint;

    for (i = 0; i < numPoints; i++) {
        f32 dist;

        point.x = points[i].x;
        point.y = points[i].y;
        point.z = points[i].z;
        dist = search_position_distance(hearPos, &point);

        if (dist < closestPointDist) {
            closestPointDist = dist;
            closestPointIdx = i;
        }
    }

    if (closestPointDist >= 10000.0f) {
        return false;
    }

    closestPoint = &points[closestPointIdx];
    closestPointPos.x = closestPoint->x;
    closestPointPos.y = closestPoint->y;
    closestPointPos.z = closestPoint->z;

    // point on path before closest point
    if (closestPointIdx != 0) {
        point.x = closestPoint[-1].x;
        point.y = closestPoint[-1].y;
        point.z = closestPoint[-1].z;
        useAdjacentPoints[0] =
            get_norm_line_pos(&point, &closestPointPos, hearPos, &prevLineSegClosestPos);
    }

    // point on path after closest point
    if (closestPointIdx + 1 != numPoints) {
        point.x = closestPoint[1].x;
        point.y = closestPoint[1].y;
        point.z = closestPoint[1].z;
        useAdjacentPoints[1] =
            get_norm_line_pos(&closestPointPos, &point, hearPos, &nextLineSegClosestPos);
    }

    if (useAdjacentPoints[0] && useAdjacentPoints[1]) {
        if (!get_norm_line_pos(&prevLineSegClosestPos, &nextLineSegClosestPos, hearPos,
                                                        sfxPos)) {
            sfxPos->x = (prevLineSegClosestPos.x + nextLineSegClosestPos.x) * 0.5f;
            sfxPos->y = (prevLineSegClosestPos.y + nextLineSegClosestPos.y) * 0.5f;
            sfxPos->z = (prevLineSegClosestPos.z + nextLineSegClosestPos.z) * 0.5f;
        }
    } else if (useAdjacentPoints[0]) {
        sfxPos->x = prevLineSegClosestPos.x;
        sfxPos->y = prevLineSegClosestPos.y;
        sfxPos->z = prevLineSegClosestPos.z;
    } else if (useAdjacentPoints[1]) {
        sfxPos->x = nextLineSegClosestPos.x;
        sfxPos->y = nextLineSegClosestPos.y;
        sfxPos->z = nextLineSegClosestPos.z;
    } else {
        sfxPos->x = closestPointPos.x;
        sfxPos->y = closestPointPos.y;
        sfxPos->z = closestPointPos.z;
    }

    return true;
}

void En_River_Sound_actor_move(Actor* thisx, PlayState* play) {
    Path* path;
    Vec3f* pos;
    Player* player = GET_PLAYER(play);
    EnRiverSound* this = (EnRiverSound*)thisx;
    s32 bgId;

    if ((thisx->params == RS_RIVER_DEFAULT_LOW_FREQ) || (thisx->params == RS_RIVER_DEFAULT_MEDIUM_FREQ) ||
        (thisx->params == RS_RIVER_DEFAULT_HIGH_FREQ)) {
        path = &play->pathList[this->pathIndex];
        pos = &thisx->world.pos;

        if (get_river_sound_pos(SEGMENTED_TO_VIRTUAL(path->points), path->count, &player->actor.world.pos, pos)) {
            if (T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &thisx->floorPoly, &bgId, thisx, pos) != BGCHECK_Y_MIN) {
                // Get the river sfx frequency based on the speed of the river current under the actor
                this->sfxFreqIndex = T_BGCheck_getSlidePowerIndex(&play->colCtx, thisx->floorPoly, bgId);
            } else {
                this->sfxFreqIndex = CONVEYOR_SPEED_DISABLED;
            }

            if (this->sfxFreqIndex == CONVEYOR_SPEED_DISABLED) {
                if (thisx->params == RS_RIVER_DEFAULT_MEDIUM_FREQ) {
                    this->sfxFreqIndex = 0;
                } else if (thisx->params == RS_RIVER_DEFAULT_LOW_FREQ) {
                    this->sfxFreqIndex = 1;
                } else {
                    // RS_RIVER_DEFAULT_HIGH_FREQ
                    this->sfxFreqIndex = 2;
                }
            } else {
                this->sfxFreqIndex--;
                this->sfxFreqIndex = CLAMP_MAX(this->sfxFreqIndex, CONVEYOR_SPEED_MAX - 2);
            }
        }
    } else if ((thisx->params == RS_GORON_CITY_SARIAS_SONG) || (thisx->params == RS_GREAT_FAIRY)) {
        Actor_search_position_project_distanceXZ(&player->actor, &thisx->home.pos, &thisx->world.pos);
    } else if (play->sceneId == SCENE_DODONGOS_CAVERN_BOSS && Actor_Environment_room_clear_Check(play, thisx->room)) {
        Actor_delete(thisx);
    }
}

void En_River_Sound_actor_draw(Actor* thisx, PlayState* play) {
    static s16 sound_type[] = {
        0,
        NA_SE_EV_WATER_WALL - SFX_FLAG,
        NA_SE_EV_MAGMA_LEVEL - SFX_FLAG,
        NA_SE_EV_WATER_WALL_BIG - SFX_FLAG,
        0,
        0,
        NA_SE_EV_MAGMA_LEVEL_M - SFX_FLAG,
        NA_SE_EV_MAGMA_LEVEL_L - SFX_FLAG,
        NA_SE_EV_WATERDROP - SFX_FLAG,
        NA_SE_EV_FOUNTAIN - SFX_FLAG,
        NA_SE_EV_CROWD - SFX_FLAG,
        0,
        NA_SE_EV_SARIA_MELODY - SFX_FLAG,
        0,
        NA_SE_EV_SAND_STORM - SFX_FLAG,
        NA_SE_EV_WATER_BUBBLE - SFX_FLAG,
        NA_SE_EV_KENJA_ENVIROMENT_0 - SFX_FLAG,
        NA_SE_EV_KENJA_ENVIROMENT_1 - SFX_FLAG,
        NA_SE_EV_EARTHQUAKE - SFX_FLAG,
        0,
        NA_SE_EV_TORCH - SFX_FLAG,
        NA_SE_EV_COW_CRY_LV - SFX_FLAG,
    };
    static f32 river_sound_type[CONVEYOR_SPEED_MAX - 1] = {
        0.7f, // CONVEYOR_SPEED_SLOW
        1.0f, // CONVEYOR_SPEED_MEDIUM
        1.4f, // CONVEYOR_SPEED_FAST
    };
    EnRiverSound* this = (EnRiverSound*)thisx;

    if (!this->playSfx) {
        this->playSfx = true;
    } else if ((this->actor.params == RS_RIVER_DEFAULT_LOW_FREQ) ||
               (this->actor.params == RS_RIVER_DEFAULT_MEDIUM_FREQ) ||
               (this->actor.params == RS_RIVER_DEFAULT_HIGH_FREQ)) {
        Na_SetRiverSe(&this->actor.projectedPos, river_sound_type[this->sfxFreqIndex]);
    } else if (this->actor.params == RS_LOWER_MAIN_BGM_VOLUME) {
        // Responsible for lowering market bgm in Child Market Entrance and Child Market Back Alley
        // Lower volume from default 127 to a volume of 90
        Na_SetLevelMuteFlag(90);
    } else if (this->actor.params == RS_LOST_WOODS_SARIAS_SONG) {
        // Play Sarias Song at the next correct Lost Woods path to Sacred Forest Meadow
        // Volume depends on distance to source
        Na_SetSariaMelodyTag2(&this->actor.projectedPos, this->actor.xzDistToPlayer);
    } else if (this->actor.params == RS_GORON_CITY_SARIAS_SONG) {
        // Play Sarias Song in Goron City at the entrance to lost woods
        // Volume depends on distance to source
        Na_SetCrossBgmTag(&this->actor.home.pos, NA_BGM_SARIA_THEME, 1000);
    } else if (this->actor.params == RS_GREAT_FAIRY) {
        // Play the Great Fairy Song inside the fairy fountain
        // Volume depends on distance to source
        Na_SetCrossBgmTag(&this->actor.home.pos, NA_BGM_GREAT_FAIRY, 800);
    } else if ((this->actor.params == RS_SANDSTORM) || (this->actor.params == RS_CHAMBER_OF_SAGES_1) ||
               (this->actor.params == RS_CHAMBER_OF_SAGES_2) || (this->actor.params == RS_RUMBLING)) {
        // Play sfx in the fixed center of the screen
        Na_StartFixSe_F(sound_type[this->actor.params]);
    } else {
        // Play sfx at the location of riverSounds projected position
        Actor_SE_set(&this->actor, sound_type[this->actor.params]);
    }
}
