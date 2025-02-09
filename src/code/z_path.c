#include "global.h"

Path* get_path_data(PlayState* play, s16 index, s16 max) {
    Path* path;

    if (index != max) {
        path = &play->pathList[index];
    } else {
        path = NULL;
    }

    return path;
}

f32 path_move(Actor* actor, Path* path, s16 waypoint, s16* yaw) {
    f32 dx;
    f32 dz;
    Vec3s* pointPos;

    if (path == NULL) {
        return -1.0;
    }

    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos = &pointPos[waypoint];

    dx = pointPos->x - actor->world.pos.x;
    dz = pointPos->z - actor->world.pos.z;

    *yaw = RAD_TO_BINANG(fatan2(dx, dz));

    return SQ(dx) + SQ(dz);
}

void get_path_goal_position(Path* path, Vec3f* dest) {
    Vec3s* pointPos;

    if (path != NULL) {
        pointPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[path->count - 1];

        dest->x = pointPos->x;
        dest->y = pointPos->y;
        dest->z = pointPos->z;
    }
}
