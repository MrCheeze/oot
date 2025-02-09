#include "global.h"
#include "quake.h"
#include "terminal.h"
#include "versions.h"
#include "overlays/actors/ovl_En_Sw/z_en_sw.h"

static s16 stop_attention = false;
static s16 attention_part = -1;
static s32 magic_frame = -4096;

static CutsceneCameraPoint op00Lookat[14] = {
    { CS_CAM_CONTINUE, 25, 40, 70.79991f, { -1814, 533, -1297 } },
    { CS_CAM_CONTINUE, 20, 40, 70.99991f, { -1805, 434, -1293 } },
    { CS_CAM_CONTINUE, 10, 30, 60.0f, { -1794, 323, -1280 } },
    { CS_CAM_CONTINUE, 5, 25, 60.0f, { -1817, 218, -1270 } },
    { CS_CAM_CONTINUE, 3, 20, 60.0f, { -1836, 168, -1243 } },
    { CS_CAM_CONTINUE, 0, 20, 60.0f, { -1905, 115, -1193 } },
    { CS_CAM_CONTINUE, 0, 30, 55.0f, { -1969, 58, -1212 } },
    { CS_CAM_CONTINUE, 0, 30, 55.0f, { -1969, 31, -1164 } },
    { CS_CAM_CONTINUE, 0, 30, 60.0f, { -1969, 54, -1209 } },
    { CS_CAM_CONTINUE, 0, 30, 60.0f, { -1973, 35, -1206 } },
    { CS_CAM_CONTINUE, 0, 50, 60.0f, { -1974, 12, -1179 } },
    { CS_CAM_CONTINUE, 0, 50, 60.0f, { -1974, 12, -1179 } },
    { CS_CAM_STOP, 0, 50, 60.0f, { -1974, 12, -1179 } },
    { CS_CAM_STOP, 0, 30, 60.0f, { -1974, 12, -1179 } },
};
static CutsceneCameraPoint op00Position[14] = {
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1751, 604, -1233 } }, { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1752, 516, -1233 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1751, 417, -1233 } }, { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1767, 306, -1219 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1776, 257, -1205 } }, { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1881, 147, -1149 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1969, 72, -1077 } },  { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1969, 7, -1048 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1969, 1, -1030 } },   { CS_CAM_CONTINUE, 0, 0, 60.0f, { -1987, 17, -1076 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -2007, 10, -1004 } },  { CS_CAM_CONTINUE, 0, 0, 60.0f, { -2007, 10, -1004 } },
    { CS_CAM_STOP, 0, 0, 60.0f, { -2007, 10, -1004 } },      { CS_CAM_STOP, 0, 0, 60.0f, { -2007, 10, -1004 } },
};
static s16 op00NCtlPoints = 13;
static s16 op00PlayFrames = 210;
static s16 op00Mode = 0;

static CutsceneCameraPoint op02Lookat[9] = {
    { CS_CAM_CONTINUE, 0, 10, 40.0f, { 0, 4, 0 } },  { CS_CAM_CONTINUE, 0, 10, 40.000004f, { 0, 4, 0 } },
    { CS_CAM_CONTINUE, 0, 10, 50.0f, { 0, 9, 0 } },  { CS_CAM_CONTINUE, 0, 12, 55.0f, { 0, 12, 0 } },
    { CS_CAM_CONTINUE, 0, 15, 61.0f, { 0, 18, 0 } }, { CS_CAM_CONTINUE, 0, 20, 65.0f, { 0, 29, 0 } },
    { CS_CAM_CONTINUE, 0, 40, 60.0f, { 0, 34, 0 } }, { CS_CAM_STOP, 0, 40, 60.0f, { 0, 34, 0 } },
    { CS_CAM_STOP, 0, 10, 60.0f, { 0, 34, 0 } },
};
static CutsceneCameraPoint op02Position[9] = {
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 9, 45 } },   { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 8, 50 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 17, 58 } },  { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 21, 78 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 46, 109 } }, { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 58, 118 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 63, 119 } }, { CS_CAM_STOP, 0, 0, 60.0f, { 0, 62, 119 } },
    { CS_CAM_STOP, 0, 0, 60.0f, { 0, 62, 119 } },
};
static s16 op02nPoints = 9;
static s16 op02nFrames = 90;
static s16 op02Mode = 1;
static CutsceneCameraPoint op02bPosition[10] = {
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 9, -45 } },   { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 9, -45 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 8, -50 } },   { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 17, -58 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 21, -78 } },  { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 46, -109 } },
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 58, -118 } }, { CS_CAM_CONTINUE, 0, 0, 60.0f, { 0, 63, -119 } },
    { CS_CAM_STOP, 0, 0, 60.0f, { 0, 62, -119 } },     { CS_CAM_STOP, 0, 0, 60.0f, { 0, 62, -119 } },
};

static CutsceneCameraPoint op04Lookat[14] = {
    { CS_CAM_CONTINUE, -15, 40, 80.600006f, { -60, 332, 183 } },
    { CS_CAM_CONTINUE, -22, 30, 80.600006f, { -60, 332, 183 } },
    { CS_CAM_CONTINUE, -20, 38, 80.600006f, { -118, 344, 41 } },
    { CS_CAM_CONTINUE, -18, 32, 80.600006f, { -80, 251, -8 } },
    { CS_CAM_CONTINUE, -12, 28, 80.600006f, { -64, 259, -28 } },
    { CS_CAM_CONTINUE, -8, 22, 80.600006f, { -79, 200, -342 } },
    { CS_CAM_CONTINUE, -5, 10, 65.80005f, { -110, 140, -549 } },
    { CS_CAM_CONTINUE, -2, 8, 65.2f, { -74, 109, -507 } },
    { CS_CAM_CONTINUE, 0, 10, 65.80002f, { -32, 78, -680 } },
    { CS_CAM_CONTINUE, 0, 20, 85.199936f, { 25, 127, -950 } },
    { CS_CAM_CONTINUE, 0, 30, 85.199936f, { 25, 127, -950 } },
    { CS_CAM_CONTINUE, 0, 40, 85.199936f, { 25, 127, -950 } },
    { CS_CAM_STOP, 6, 30, 85.199936f, { 25, 127, -950 } },
    { CS_CAM_STOP, 0, 30, 85.199936f, { 25, 127, -950 } },
};
static CutsceneCameraPoint op04Position[14] = {
    { CS_CAM_CONTINUE, 0, 0, 60.0f, { -225, 785, -242 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -245, 784, -242 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -288, 485, -379 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -250, 244, -442 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -163, 21, -415 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -98, 86, -520 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -86, 31, -816 } },
    { CS_CAM_CONTINUE, -21, 0, 80.600006f, { -74, 18, -931 } },
    { CS_CAM_CONTINUE, 1, 0, 80.600006f, { -91, 80, -1220 } },
    { CS_CAM_CONTINUE, 0, 0, 85.199936f, { 14, 153, -1340 } },
    { CS_CAM_CONTINUE, 0, 0, 85.199936f, { 28, 125, -1340 } },
    { CS_CAM_CONTINUE, 0, 0, 85.199936f, { 48, 124, -1340 } },
    { CS_CAM_STOP, 0, 0, 85.199936f, { 48, 124, -1502 } },
    { CS_CAM_STOP, 0, 0, 85.199936f, { 48, 124, -1262 } },
};
static s16 op04nPoints = 14;
static s16 op04nFrames = 190;
static s16 op04Mode = 8;

static CutsceneCameraPoint op05Lookat[12] = {
    { CS_CAM_CONTINUE, 6, 20, 80.0f, { -96, 40, 170 } }, { CS_CAM_CONTINUE, 6, 20, 80.0f, { -96, 40, 170 } },
    { CS_CAM_CONTINUE, 6, 20, 70.0f, { -70, 35, 150 } }, { CS_CAM_CONTINUE, 5, 10, 60.0f, { -57, 34, 133 } },
    { CS_CAM_CONTINUE, 4, 25, 65.0f, { -22, 32, 110 } }, { CS_CAM_CONTINUE, 3, 12, 60.0f, { -9, 33, 98 } },
    { CS_CAM_CONTINUE, 3, 5, 65.0f, { -3, 29, 87 } },    { CS_CAM_CONTINUE, 2, 10, 65.0f, { -1, 15, 84 } },
    { CS_CAM_CONTINUE, 1, 200, 65.0f, { 0, 17, 82 } },   { CS_CAM_CONTINUE, 1, 500, 65.0f, { 0, 18, 82 } },
    { CS_CAM_STOP, 8, 50, 65.0f, { 0, 18, 82 } },        { CS_CAM_STOP, 11, 60, 65.0f, { 0, 18, 82 } },
};
static CutsceneCameraPoint op05Position[12] = {
    { CS_CAM_CONTINUE, 6, 0, 80.0f, { -50, 10, 180 } }, { CS_CAM_CONTINUE, 6, 0, 80.0f, { -50, 20, 180 } },
    { CS_CAM_CONTINUE, 6, 0, 70.0f, { -40, 30, 177 } }, { CS_CAM_CONTINUE, 5, 0, 65.0f, { 0, 35, 172 } },
    { CS_CAM_CONTINUE, 4, 0, 65.0f, { 34, 35, 162 } },  { CS_CAM_CONTINUE, 3, 0, 65.0f, { 61, 32, 147 } },
    { CS_CAM_CONTINUE, 3, 0, 65.0f, { 72, 30, 128 } },  { CS_CAM_CONTINUE, 2, 0, 65.0f, { 74, 20, 125 } },
    { CS_CAM_CONTINUE, 1, 0, 65.0f, { 75, 18, 123 } },  { CS_CAM_CONTINUE, 1, 0, 65.0f, { 75, 10, 123 } },
    { CS_CAM_STOP, 0, 0, 65.0f, { 75, 10, 122 } },      { CS_CAM_STOP, 0, 0, 65.0f, { 75, 10, 122 } },
};
static CutsceneCameraPoint op05bPosition[12] = {
    { CS_CAM_CONTINUE, 6, 0, 80.0f, { 85, 5, 170 } },  { CS_CAM_CONTINUE, 6, 0, 80.0f, { 85, 10, 170 } },
    { CS_CAM_CONTINUE, 6, 0, 70.0f, { 80, 20, 167 } }, { CS_CAM_CONTINUE, 5, 0, 65.0f, { 74, 25, 165 } },
    { CS_CAM_CONTINUE, 4, 0, 65.0f, { 63, 30, 162 } }, { CS_CAM_CONTINUE, 3, 0, 65.0f, { 66, 34, 147 } },
    { CS_CAM_CONTINUE, 3, 0, 65.0f, { 72, 34, 128 } }, { CS_CAM_CONTINUE, 2, 0, 65.0f, { 74, 20, 125 } },
    { CS_CAM_CONTINUE, 1, 0, 65.0f, { 75, 18, 123 } }, { CS_CAM_CONTINUE, 1, 0, 65.0f, { 75, 10, 123 } },
    { CS_CAM_STOP, 0, 0, 65.0f, { 75, 10, 122 } },     { CS_CAM_STOP, 0, 0, 65.0f, { 75, 10, 122 } },
};
static s16 op05nPoints = 12;
static s16 op05nFrames = 90;
static s16 op05Mode = 8;

static Vec3f translate_by_sglobe(Vec3f* a, VecGeo* geo) {
    Vec3f sum;
    Vec3f b = sglobe2world(geo);

    sum.x = a->x + b.x;
    sum.y = a->y + b.y;
    sum.z = a->z + b.z;

    return sum;
}

/**
 * @see search_position_angleY
 */
static s16 get_y_angle_by_2pos(Vec3f* origin, Vec3f* point) {
    return CAM_DEG_TO_BINANG(RAD_TO_DEG(fatan2(point->x - origin->x, point->z - origin->z)));
}

void xyz2sv(Vec3f* src, Vec3s* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

static s32 check_wall(CollisionContext* colCtx, Vec3f* vec1, Vec3f* vec2) {
    Vec3f posResult;
    s32 bgId;
    CollisionPoly* outPoly = NULL;

    return T_BGCheck_CameraLineCheck_poly_chgrp_ai(colCtx, vec1, vec2, &posResult, &outPoly, true, true, true, false, &bgId);
}

static f32 floor_at(CollisionContext* colCtx, Vec3f* pos) {
    CollisionPoly* outPoly;
    s32 bgId;

    return T_BGCheck_ObjGroundCheck_ai(colCtx, &outPoly, &bgId, pos);
}

void set_onepoint_spline(Camera* camera, s16 actionParameters, s16 initTimer, CutsceneCameraPoint* atPoints,
                                     CutsceneCameraPoint* eyePoints) {
    OnePointCamData* onePointCamData = &camera->paramData.demo9.onePointCamData;

    onePointCamData->atPoints = atPoints;
    onePointCamData->eyePoints = eyePoints;
    onePointCamData->actionParameters = actionParameters;
    onePointCamData->initTimer = initTimer;
}

s32 set_onepointdemo(PlayState* play, s16 subCamId, s16 csId, Actor* actor, s16 timer) {
    Camera* subCam = play->cameraPtrs[subCamId];
    Camera* childCam = play->cameraPtrs[subCam->childCamId];
    Camera* mainCam = play->cameraPtrs[CAM_ID_MAIN];
    Player* player = mainCam->player;
    VecGeo spD0;
    s32 i;
    Vec3f spC0;
    Vec3f spB4;
    PosRot spA0;
    PosRot sp8C;
    f32 tempRand;
    OnePointCsInfo* csInfo = &subCam->paramData.uniq9.csInfo;

    switch (csId) {
        case 1020: { // smoothly return to main camera from current view
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            if (timer < 20) {
                timer = 20;
            }
            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;
            cuts_of_this[1].atTargetInit = mainCam->at;
            cuts_of_this[1].eyeTargetInit = mainCam->eye;
            cuts_of_this[1].fovTargetInit = mainCam->fov;
            cuts_of_this[1].timerInit = timer - 1;
            subCam->timer = timer + 1;
            cuts_of_this[1].lerpStepScale = 1.0f / (0.5f * timer);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 1030: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0xA121,
                    1,
                    0,
                    75.0f,
                    0.6f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;
            spD0 = sglobe_by_2pos(&mainCam->at, &mainCam->eye);
            cuts_of_this[1].eyeTargetInit.y = CAM_BINANG_TO_DEG(spD0.yaw);
            cuts_of_this[1].timerInit = timer - 1;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 5000: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    1,
                    0,
                    60.0f,
                    0.9f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0100,
                    29,
                    0,
                    45.0f,
                    0.1f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    10,
                    0,
                    60.0f,
                    0.2f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit = cuts_of_this[1].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = cuts_of_this[2].fovTargetInit = play->view.fovy;
            spD0 = sglobe_by_2pos(&actor->focus.pos, &mainCam->at);
            spD0.r = mainCam->dist;
            cuts_of_this[1].eyeTargetInit = translate_by_sglobe(&cuts_of_this[1].atTargetInit, &spD0);
            cuts_of_this[1].atTargetInit.y += 20.0f;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 5010: // attention cutscene
            // Setup keyFrames in `demo_camerawork_05`
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_ATTENTION);
            Gama_play_camera_setting(play, subCamId, &mainCam->at, &mainCam->eye);
            subCam->roll = 0;
            break;

        case 9500: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2525,
                    1,
                    0,
                    75.0f,
                    0.1f,
                    { 0.0f, 20.0f, -10.0f },
                    { 0.0f, 10.0f, -40.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    9,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0022,
                    5000,
                    0,
                    75.0f,
                    0.005f,
                    { 0.0f, 0.0f, -10.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 2260: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0442,
                    10,
                    0,
                    40.0f,
                    1.0f,
                    { -10.0f, 45.0f, 20.0f },
                    { 20.0f, 30.0f, 160.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    40.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0442,
                    10,
                    0,
                    40.0f,
                    1.0f,
                    { -10.0f, 45.0f, 20.0f },
                    { 20.0f, 30.0f, 160.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    40.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit.x = cuts_of_this[2].atTargetInit.x =
                ((mainCam->play->state.frames & 1) ? -10.0f : 10.0f) + (fqrand() * 8.0f);

            cuts_of_this[0].eyeTargetInit.x = cuts_of_this[2].eyeTargetInit.x =
                ((mainCam->play->state.frames & 1) ? 20.0f : -20.0f) + (fqrand() * 5.0f);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 2270: {
            static OnePointCsFull cuts_of_this[11] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2142,
                    1,
                    0,
                    40.0f,
                    1.0f,
                    { 20.0f, 40.0f, 20.0f },
                    { -20.0f, 0.0f, -30.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    19,
                    5,
                    70.0f,
                    0.01f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    20,
                    0,
                    60.0f,
                    0.01f,
                    { 0.0f, 20.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    40,
                    -10,
                    50.0f,
                    0.02f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    1,
                    0,
                    40.0f,
                    1.0f,
                    { 0.0f, -10.0f, 20.0f },
                    { 0.0f, 20.0f, 50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    19,
                    0,
                    70.0f,
                    0.01f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    40,
                    10,
                    50.0f,
                    0.01f,
                    { 0.0f, 20.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    70,
                    0,
                    60.0f,
                    0.01f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, -10.0f, 0.0f },
                    { 80.0f, 20.0f, 60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_13, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    150,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 5.0f, 0.0f },
                    { 0.0f, 4.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_24, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            for (i = 0; i < csInfo->keyFrameCount - 3; i++) {
                if (cuts_of_this[i].actionFlags != ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true)) {
                    cuts_of_this[i].atTargetInit.x = fqrand() * 5.0f;
                    cuts_of_this[i].atTargetInit.z = (fqrand() * 30.0f) + 10.0f;
                    cuts_of_this[i].eyeTargetInit.x = (fqrand() * 100.0f) + 20.0f;
                    cuts_of_this[i].eyeTargetInit.z = (fqrand() * 80.0f) + 50.0f;
                }
            }

            cuts_of_this[subCamId - 1].eyeTargetInit.y =
                ((mainCam->play->state.frames & 1) ? 3.0f : -3.0f) + fqrand();
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_5);
            setSpeedQuake(i, 400);
            setScaleQuake(i, 4, 5, 40, 0x3C);
            setTimerQuake(i, 1600);
            break;
        }

        case 2280: {
            static OnePointCsFull cuts_of_this[7] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2142,
                    1,
                    0,
                    40.0f,
                    1.0f,
                    { 20.0f, 40.0f, 20.0f },
                    { -20.0f, 0.0f, -30.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    19,
                    5,
                    70.0f,
                    0.01f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    20,
                    0,
                    60.0f,
                    0.01f,
                    { 0.0f, 20.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    40,
                    -10,
                    50.0f,
                    0.02f,
                    { 0.0f, 30.0f, 20.0f },
                    { 120.0f, 60.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, -10.0f, 0.0f },
                    { 80.0f, 20.0f, 60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_13, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    150,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 5.0f, 0.0f },
                    { 0.0f, 4.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_24, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            for (i = 0; i < csInfo->keyFrameCount - 3; i++) {
                if (cuts_of_this[i].actionFlags != ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true)) {
                    cuts_of_this[i].atTargetInit.x = fqrand() * 20.0f;
                    cuts_of_this[i].atTargetInit.z = (fqrand() * 40.0f) + 10.0f;
                    cuts_of_this[i].eyeTargetInit.x = (fqrand() * 40.0f) + 60.0f;
                    cuts_of_this[i].eyeTargetInit.z = (fqrand() * 40.0f) + 80.0f;
                }
            }
            cuts_of_this[subCamId - 1].eyeTargetInit.y =
                ((mainCam->play->state.frames & 1) ? 3.0f : -3.0f) + fqrand();
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_5);
            setSpeedQuake(i, 400);
            setScaleQuake(i, 2, 3, 200, 0x32);
            setTimerQuake(i, 9999);
            break;
        }

        case 2220: {
            static OnePointCsFull cuts_of_this[8] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2142,
                    20,
                    0,
                    50.0f,
                    1.0f,
                    { -25.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    80,
                    0,
                    50.0f,
                    1.0f,
                    { -25.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    8,
                    0,
                    60.0f,
                    0.1f,
                    { -25.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    15,
                    4,
                    55.0f,
                    0.05f,
                    { -50.0f, 20.0f, 20.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    15,
                    -4,
                    50.0f,
                    0.05f,
                    { 0.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    15,
                    0,
                    50.0f,
                    0.1f,
                    { -25.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    40,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_5);
            setSpeedQuake(i, 400);
            setScaleQuake(i, 2, 2, 50, 0);
            setTimerQuake(i, 280);
            break;
        }

        case 2230: {
            static OnePointCsFull cuts_of_this[6] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2143,
                    30,
                    0,
                    70.0f,
                    0.4f,
                    { 0.0f, 40.0f, 50.0f },
                    { 30.0f, 10.0f, -50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2222,
                    10,
                    0,
                    42.0f,
                    1.0f,
                    { 0.0f, 40.0f, 0.0f },
                    { 0.0f, 85.0f, 45.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 10.0f, 0.0f },
                    { 30.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            if (player->actor.world.pos.z < 1000.0f) {
                cuts_of_this[0].eyeTargetInit.x = -cuts_of_this[0].eyeTargetInit.x;
                cuts_of_this[2].eyeTargetInit.x = -cuts_of_this[2].eyeTargetInit.x;
            }

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 2340: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x2101,
                    20,
                    0,
                    50.0f,
                    1.0f,
                    { 3840.0f, 10.0f, 950.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2101,
                    50,
                    0,
                    55.0f,
                    1.0f,
                    { 4000.0f, 50.0f, 1000.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_5);
            setSpeedQuake(i, 400);
            setScaleQuake(i, 2, 2, 50, 0);
            setTimerQuake(i, 60);
            break;
        }

        case 2350: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x2142,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -25.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 5.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 10.0f, 0.0f },
                    { 0.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 2200: {
            static OnePointCsFull cuts_of_this0[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    40,
                    0,
                    -1.0f,
                    0.1f,
                    { 0.0f, 10.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };
            static OnePointCsFull cuts_of_this1[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    50,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 10.0f, 0.0f },
                    { -10.0f, 85.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };
            s16 sp82;
            s16 sp80;
            s16 sp7E;
            s16 sp7C;

            Actor_display_position_set(play, &player->actor, &sp82, &sp7E);
            Actor_display_position_set(play, actor, &sp80, &sp7C);
            if ((sp82 > 0) && (sp82 < 320) && (sp7E > 0) && (sp7E < 240) && (sp80 > 0) && (sp80 < 320) && (sp7C > 0) &&
                (sp7C < 240) &&
                !check_wall(&play->colCtx, &actor->focus.pos, &player->actor.focus.pos)) {
                cuts_of_this0[0].atTargetInit.x = (play->view.at.x + actor->focus.pos.x) * 0.5f;
                cuts_of_this0[0].atTargetInit.y = (play->view.at.y + actor->focus.pos.y) * 0.5f;
                cuts_of_this0[0].atTargetInit.z = (play->view.at.z + actor->focus.pos.z) * 0.5f;
                cuts_of_this0[0].eyeTargetInit = play->view.eye;
                cuts_of_this0[0].eyeTargetInit.y = player->actor.focus.pos.y + 20.0f;
                cuts_of_this0[0].fovTargetInit = mainCam->fov * 0.75f;

                csInfo->keyFrames = cuts_of_this0;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this0);
            } else {
                cuts_of_this1[0].atTargetInit.x = actor->focus.pos.x;
                cuts_of_this1[0].atTargetInit.y = actor->focus.pos.y - 5.0f;
                cuts_of_this1[0].atTargetInit.z = actor->focus.pos.z;
                spC0 = ((EnSw*)actor)->unk_364;
                PRINTF("%s(%d): xyz_t: %s (%f %f %f)\n", "../z_onepointdemo.c", 1671, "&cp", spC0.x, spC0.y, spC0.z);
                cuts_of_this1[0].eyeTargetInit.x = (actor->focus.pos.x + (120.0f * spC0.x)) - (fqrand() * 20.0f);
                cuts_of_this1[0].eyeTargetInit.y = actor->focus.pos.y + (120.0f * spC0.y) + 20.0f;
                cuts_of_this1[0].eyeTargetInit.z = (actor->focus.pos.z + (120.0f * spC0.z)) - (fqrand() * 20.0f);

                csInfo->keyFrames = cuts_of_this1;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this1);
            }
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 2290: {
            static OnePointCsFull cuts_of_this[6] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x4141,
                    2,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 5.0f, 10.0f },
                    { 0.0f, 0.0f, 45.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    18,
                    0,
                    45.0f,
                    1.0f,
                    { 0.0f, 5.0f, 10.0f },
                    { 0.0f, -10.0f, 50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_52),
                    0x4104,
                    80,
                    0,
                    70.0f,
                    0.05f,
                    { 0.0f, 0.0f, 60.0f },
                    { 0.0f, 250.0f, -50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0000,
                    20,
                    0,
                    70.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0421,
                    60,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, -30.0f, 20.0f },
                    { 10.0f, 5.0f, -50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };
            Actor* rideActor = player->rideActor;

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            if (rideActor != NULL) {
                rideActor->freezeTimer = 180;
            }

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 5120: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x4141,
                    1000,
                    0,
                    75.0f,
                    0.6f,
                    { 0.0f, 0.0f, 10.0f },
                    { 0.0f, 0.0f, 100.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4510: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0141,
                    40,
                    0,
                    75.0f,
                    1.0f,
                    { 0.0f, 60.0f, 0.0f },
                    { 0.0f, 0.0f, 100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    20,
                    0,
                    60.0f,
                    0.2f,
                    { 0.0f, -10.0f, -10.0f },
                    { 0.0f, 10.0f, -100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].eyeTargetInit = actor->world.pos;
            cuts_of_this[0].eyeTargetInit.y = player->actor.world.pos.y + 40.0f;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4500:
            spA0 = Actor_get_eye(actor);
            spC0 = spA0.pos;
            spC0.y = floor_at(&play->colCtx, &spC0) + 40.0f;
            spD0.r = 150.0f;
            spD0.yaw = spA0.rot.y;
            spD0.pitch = 0x3E8;

            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            subCam->roll = 0;
            subCam->fov = 50.0f;
            if (subCam->childCamId != CAM_ID_MAIN) {
                deleteOnepointDemo(play, subCam->childCamId);
            }
            break;

        case 2210: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0xC2C2,
                    40,
                    0,
                    70.0f,
                    1.0f,
                    { 80.0f, 0.0f, 20.0f },
                    { 20.0f, 0.0f, 80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0xC2C2,
                    120,
                    0,
                    70.0f,
                    0.1f,
                    { 80.0f, 0.0f, 20.0f },
                    { 20.0f, 0.0f, 80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_83),
                    0xC2C2,
                    30,
                    0,
                    50.0f,
                    1.0f,
                    { 60.0f, 0.0f, 20.0f },
                    { 60.0f, 0.0f, 60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_69),
                    0x4222,
                    30,
                    0,
                    60.0f,
                    0.1f,
                    { 0.0f, 50.0f, 0.0f },
                    { 5.0f, 30.0f, 220.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    75.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            spD0 = sglobe_by_2pos(&player->actor.world.pos, &actor->world.pos);
            cuts_of_this[0].eyeTargetInit.y = cuts_of_this[1].eyeTargetInit.y = cuts_of_this[2].eyeTargetInit.y =
                cuts_of_this[2].atTargetInit.y = CAM_BINANG_TO_DEG(spD0.yaw);
            if (fqrand() < 0.0f) {
                cuts_of_this[3].eyeTargetInit.x = -cuts_of_this[3].eyeTargetInit.x;
            }
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 1010:
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &childCam->at, &childCam->eye);
            Gama_play_set_camera_fovy(play, subCamId, childCam->fov);
            Gama_play_set_camera_sz(play, subCamId, childCam->roll);
            break;

        case 9601: // Leaving a crawlspace forwards
            Gama_play_change_camera_set(play, subCamId, CAM_SET_CS_3);
            Gama_play_change_camera_set(play, CAM_ID_MAIN, mainCam->prevSetting);
            set_onepoint_spline(subCam, op02Mode | 0x1000, op02nFrames,
                                            op02Lookat, op02Position);
            break;

        case 9602: // Leaving a crawlspace backwards
            Gama_play_change_camera_set(play, subCamId, CAM_SET_CS_3);
            Gama_play_change_camera_set(play, CAM_ID_MAIN, mainCam->prevSetting);
            set_onepoint_spline(subCam, op02Mode | 0x1000, op02nFrames,
                                            op02Lookat, op02bPosition);
            break;

        case 4175: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    40,
                    0,
                    45.0f,
                    1.0f,
                    { 820.0f, 1600.0f, -400.0f },
                    { 777.0f, 1577.0f, -577.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0142,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -50.0f, 80.0f, 0.0f },
                    { 900.0f, 1575.0f, 850.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0142,
                    89,
                    -4,
                    80.0f,
                    0.07f,
                    { -50.0f, 70.0f, 0.0f },
                    { 975.0f, 1575.0f, 770.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4180:
            spC0.x = -1881.0f;
            spC0.y = 766.0f;
            spC0.z = -330.0f;
            spB4.x = -1979.0f;
            spB4.y = 703.0f;
            spB4.z = -269.0f;
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 6;
            subCam->fov = 75.0f;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            break;

        case 3040: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    29,
                    0,
                    60.0f,
                    1.0f,
                    { -700.0f, 875.0f, -100.0f },
                    { -550.0f, 920.0f, -150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            cuts_of_this[0].timerInit = timer - 1;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3020: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_77),
                    0x4242,
                    1,
                    0,
                    65.0f,
                    1.0f,
                    { 60.0f, 30.0f, 0.0f },
                    { 50.0f, 20.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    -1,
                    0,
                    65.0f,
                    1.0f,
                    { -50.0f, 60.0f, 0.0f },
                    { -60.0f, 40.0f, 150.0f },
                },
            };

            cuts_of_this[1].timerInit = timer - 1;
            if (mainCam->play->state.frames & 1) {
                cuts_of_this[0].atTargetInit.x = -cuts_of_this[0].atTargetInit.x;
                cuts_of_this[0].eyeTargetInit.x = -cuts_of_this[0].eyeTargetInit.x;
                cuts_of_this[1].atTargetInit.x = -cuts_of_this[1].atTargetInit.x;
                cuts_of_this[1].eyeTargetInit.x = -cuts_of_this[1].eyeTargetInit.x;
            }
            tempRand = fqrand() * 15.0f;
            cuts_of_this[0].eyeTargetInit.x += tempRand;
            cuts_of_this[1].eyeTargetInit.x += tempRand;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            break;
        }

        case 3010: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    5,
                    0,
                    65.0f,
                    1.0f,
                    { -1185.0f, 655.0f, 1185.0f },
                    { -1255.0f, 735.0f, 1255.0f },
                },
            };

            cuts_of_this[0].timerInit = timer;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3070: {
            static OnePointCsFull cuts_of_this[10] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x4141,
                    20,
                    0,
                    30.0f,
                    1.0f,
                    { 0.0f, 120.0f, 0.0f },
                    { -10.0f, 140.0f, -90.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    1,
                    4,
                    75.0f,
                    1.0f,
                    { -1360.0f, -940.0f, -3343.0f },
                    { -1060.0f, -980.0f, -3325.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    129,
                    0,
                    75.0f,
                    0.5f,
                    { 0.0f, 50.0f, 0.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0303,
                    30,
                    0,
                    70.0f,
                    0.05f,
                    { 0.0f, 80.0f, 0.0f },
                    { -10.0f, 120.0f, 10.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_9),
                    0x0101,
                    40,
                    -5,
                    70.0f,
                    1.0f,
                    { -973.0f, -924.0f, -3263.0f },
                    { -1190.0f, -1010.0f, -3365.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    1,
                    0,
                    75.0f,
                    1.0f,
                    { -1355.0f, -700.0f, -3340.0f },
                    { -1040.0f, -940.0f, -3345.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    45.0f,
                    0.8f,
                    { -1370.0f, -875.0f, -3345.0f },
                    { -1230.0f, -885.0f, -3345.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    10,
                    0,
                    70.0f,
                    1.0f,
                    { -1370.0f, -875.0f, -3345.0f },
                    { -1210.0f, -900.0f, -3420.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    20,
                    0,
                    70.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 22000);
            setScaleQuake(i, 2, 0, 200, 0);
            setTimerQuake(i, 10);
            break;
        }

        case 3080: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    1,
                    -2,
                    75.0f,
                    1.0f,
                    { -1340.0f, -860.0f, -3345.0f },
                    { -1415.0f, -940.0f, -3520.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0142,
                    39,
                    2,
                    70.0f,
                    1.0f,
                    { 0.0f, -20.0f, 10.0f },
                    { -1140.0f, -1010.0f, -3560.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x0121,
                    20,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -20.0f, 20.0f },
                    { -1220.0f, -1005.0f, -3660.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3090: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_76),
                    0x0101,
                    5,
                    0,
                    40.0f,
                    1.0f,
                    { -1400.0f, -540.0f, -3327.0f },
                    { -1254.0f, -20.0f, -3357.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    70,
                    0,
                    75.0f,
                    0.75f,
                    { -1327.0f, 100.0f, -3342.0f },
                    { -1320.0f, 350.0f, -3540.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    60.0f,
                    0.75f,
                    { 0.0f, 10.0f, 0.0f },
                    { 0.0f, 20.0f, -150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3100:
            VEC_SET(spB4, 0.0f, -280.0f, -1400.0f);

            spA0 = Actor_get_eye(actor);
            spC0 = spA0.pos;
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_PIVOT_VERTICAL);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0;
            subCam->fov = 70.0f;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            break;

        case 3380:
        case 3065: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    65.0f,
                    1.0f,
                    { 0.0f, 350.0f, -1520.0f },
                    { 0.0f, 715.0f, -885.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    100,
                    0,
                    70.0f,
                    0.02f,
                    { 0.0f, 75.0f, -1335.0f },
                    { 0.0f, 20.0f, -1190.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 24000);
            setScaleQuake(i, 2, 0, 0, 0);
            setTimerQuake(i, 160);
            break;
        }

        case 3060: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    50,
                    10,
                    65.0f,
                    1.0f,
                    { 165.0f, 85.0f, -920.0f },
                    { 65.0f, -30.0f, -720.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3050: {
            Gama_play_change_camera_set(play, subCamId, CAM_SET_CS_3);
            player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_5);
            set_onepoint_spline(subCam, op00Mode | 0x2000, op00PlayFrames, op00Lookat, op00Position);
#if OOT_VERSION >= PAL_1_0
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
#endif
            xyz2sv(&mainCam->at, &op00Lookat[op00NCtlPoints - 2].pos);
            xyz2sv(&mainCam->eye, &op00Position[op00NCtlPoints - 2].pos);
            op00Lookat[op00NCtlPoints - 3].pos.x +=
                (op00Lookat[op00NCtlPoints - 2].pos.x - op00Lookat[op00NCtlPoints - 3].pos.x) / 2;
            op00Lookat[op00NCtlPoints - 3].pos.y +=
                (op00Lookat[op00NCtlPoints - 2].pos.y - op00Lookat[op00NCtlPoints - 3].pos.y) / 2;
            op00Lookat[op00NCtlPoints - 3].pos.z +=
                (op00Lookat[op00NCtlPoints - 2].pos.z - op00Lookat[op00NCtlPoints - 3].pos.z) / 2;
            op00Position[op00NCtlPoints - 3].pos.x +=
                (op00Position[op00NCtlPoints - 2].pos.x - op00Position[op00NCtlPoints - 3].pos.x) / 2;
            op00Position[op00NCtlPoints - 3].pos.y +=
                (op00Position[op00NCtlPoints - 2].pos.y - op00Position[op00NCtlPoints - 3].pos.y) / 2;
            op00Position[op00NCtlPoints - 3].pos.z +=
                (op00Position[op00NCtlPoints - 2].pos.z - op00Position[op00NCtlPoints - 3].pos.z) / 2;

#if OOT_VERSION < PAL_1_0
            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 30000);
            setScaleQuake(i, 3, 1, 1, 0);
            setTimerQuake(i, op00PlayFrames);

            i = startQuake(mainCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 30000);
            setScaleQuake(i, 3, 1, 1, 0);
            setTimerQuake(i, op00PlayFrames + 50);
#else
            i = startQuake(mainCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 30000);
            setScaleQuake(i, 2, 1, 1, 0);
            setTimerQuake(i, 200);
#endif
            break;
        }

        case 3120: {
            static OnePointCsFull cuts_of_this_allay[3][2] = {
                {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        20,
                        5,
                        60.0f,
                        1.0f,
                        { -700.0f, 940.0f, 300.0f },
                        { -765.0f, 1000.0f, 335.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        80,
                        -10,
                        70.0f,
                        0.1f,
                        { -540.0f, 875.0f, 245.0f },
                        { -585.0f, 900.0f, 335.0f },
                    },
                },
                {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        40,
                        -30,
                        70.0f,
                        1.0f,
                        { -80.0f, 115.0f, -180.0f },
                        { -5.0f, 240.0f, -190.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        60,
                        20,
                        70.0f,
                        0.1f,
                        { -100.0f, 350.0f, -175.0f },
                        { -5.0f, 240.0f, -190.0f },
                    },
                },
                {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        80,
                        5,
                        70.0f,
                        0.2f,
                        { 960.0f, 900.0f, 260.0f },
                        { 970.0f, 950.0f, 250.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        20,
                        5,
                        70.0f,
                        1.0f,
                        { 960.0f, 900.0f, 260.0f },
                        { 970.0f, 950.0f, 250.0f },
                    },
                },
            };

            csInfo->keyFrames = cuts_of_this_allay[-(timer + 101)];
            subCam->timer = 100;
            subCam->stateFlags |= CAM_STATE_CHECK_WATER;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this_allay[0]);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3130: {
            static OnePointCsFull cuts_of_this[12] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x2121,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -5.0f, 0.0f },
                    { 0.0f, 0.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x4242,
                    30,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 45.0f, 0.0f },
                    { 0.0f, 45.0f, 50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x2222,
                    40,
                    5,
                    50.0f,
                    1.0f,
                    { 0.0f, 50.0f, 0.0f },
                    { 0.0f, 50.0f, 50.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x4242,
                    40,
                    5,
                    60.0f,
                    1.0f,
                    { 30.0f, 30.0f, 15.0f },
                    { 70.0f, 30.0f, -40.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    30,
                    -5,
                    50.0f,
                    1.0f,
                    { 20.0f, 30.0f, -5.0f },
                    { 0.0f, 70.0f, 70.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2242,
                    40,
                    0,
                    45.0f,
                    1.0f,
                    { 0.0f, 30.0f, 30.0f },
                    { 25.0f, 60.0f, -60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, true, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x22C2,
                    140,
                    0,
                    60.0f,
                    0.04f,
                    { 0.0f, 0.0f, 30.0f },
                    { 25.0f, 60.0f, -60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_9, true, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2222,
                    20,
                    0,
                    60.0f,
                    0.8f,
                    { 0.0f, 50.0f, 0.0f },
                    { 0.0f, 60.0f, -60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            subCam->stateFlags |= CAM_STATE_CHECK_WATER;
            break;
        }

        case 3140: {
            static OnePointCsFull cuts_of_this[7] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    89,
                    0,
                    50.0f,
                    0.4f,
                    { 125.0f, 320.0f, -1500.0f },
                    { 125.0f, 500.0f, -1150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    40,
                    4,
                    55.0f,
                    1.0f,
                    { 0.0f, 375.0f, -1440.0f },
                    { 5.0f, 365.0f, -1315.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    40,
                    -4,
                    55.0f,
                    1.0f,
                    { 250.0f, 375.0f, -1440.0f },
                    { 235.0f, 365.0f, -1315.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    100,
                    0,
                    95.0f,
                    1.0f,
                    { 125.0f, 345.0f, -1500.0f },
                    { 125.0f, 255.0f, -1350.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    100,
                    0,
                    60.0f,
                    1.0f,
                    { 125.0f, 325.0f, -1500.0f },
                    { 125.0f, 480.0f, -1000.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3150:
            spC0.x = 1890.0f;
            spC0.y = 886.0f;
            spC0.z = -1432.0f;
            spB4.x = 1729.0f;
            spB4.y = 995.0f;
            spB4.z = -1405.0f;
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0x50;
            subCam->fov = 55.0f;
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            break;

        case 3170:
            spA0 = Actor_get_world(actor);
            spC0 = spA0.pos;
            spD0.pitch = -0x5DC;
            spC0.y += 50.0f;
            spD0.r = 250.0f;
            spA0 = Actor_get_world(&player->actor);
            spD0.yaw = get_y_angle_by_2pos(&spC0, &spA0.pos) - 0x7D0;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            Gama_play_copy_camera_position(play, CAM_ID_MAIN, subCamId);
            subCam->roll = -1;
            subCam->fov = 55.0f;
            player_demo_mode_set2(play, actor, PLAYER_CSACTION_1);
            break;

        case 3160:
            spA0 = Actor_get_world(actor);
            spC0 = spA0.pos;
            spD0.pitch = 0;
            spD0.yaw = spA0.rot.y;
            spD0.r = 150.0f;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0;
            subCam->fov = 55.0f;
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            break;

        case 3180:
            spA0 = Actor_get_shape(actor);
            spC0 = spA0.pos;
            spC0.y += 120.0f;
            spD0.r = 300.0f;
            spD0.yaw = spA0.rot.y;
            spD0.pitch = -0xAF0;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0;
            subCam->fov = 60.0f;
            player_demo_mode_set2(play, actor, PLAYER_CSACTION_1);
            break;

        case 3190:
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FOREST_DEFEAT_POE);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            player_demo_mode_set2(play, actor, PLAYER_CSACTION_12);
            break;

        case 3230:
            spC0.x = 120.0f;
            spC0.y = 265.0f;
            spC0.z = -1570.0f;
            spB4.x = 80.0f;
            spB4.y = 445.0f;
            spB4.z = -1425.0f;
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0x1E;
            subCam->fov = 75.0f;
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            spA0 = Actor_get_shape(actor);
            sp8C = Actor_get_eye(&player->actor);
            spC0.x = sp8C.pos.x;
            spC0.y = sp8C.pos.y + 70.0f;
            spC0.z = sp8C.pos.z;
            spD0 = sglobe_by_2pos(&spA0.pos, &sp8C.pos);
            spD0.pitch = 0x5DC;
            spD0.r = 120.0f;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_camera_setting(play, CAM_ID_MAIN, &spC0, &spB4);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 22000);
            setScaleQuake(i, 1, 0, 0, 0);
            setTimerQuake(i, 90);
            break;

        case 6010:
            spA0 = Actor_get_world(actor);
            spC0 = spA0.pos;
            spD0.pitch = 0;
            spC0.y += 70.0f;
            spD0.yaw = spA0.rot.y + 0x7FFF;
            spD0.r = 300.0f;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0;
            subCam->fov = 45.0f;
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            break;

        case 3220:
            spA0 = Actor_get_eye(actor);
            spC0 = spA0.pos;
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_PIVOT_VERTICAL);
            spA0 = Actor_get_world(&player->actor);
            spD0 = sglobe_by_2pos(&spC0, &spA0.pos);
            spD0.yaw += 0x3E8;
            spD0.r = 400.0f;
            spB4 = translate_by_sglobe(&spC0, &spD0);
            spB4.y = spA0.pos.y + 60.0f;
            Gama_play_camera_setting(play, subCamId, &spC0, &spB4);
            subCam->roll = 0;
            subCam->fov = 75.0f;
            player->actor.shape.rot.y = player->actor.world.rot.y = player->yaw = spD0.yaw + 0x7FFF;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            break;

        case 3240: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 1023.0f, 738.0f, -2628.0f },
                    { 993.0f, 770.0f, -2740.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    4,
                    0,
                    50.0f,
                    1.0f,
                    { 1255.0f, 350.0f, -1870.0f },
                    { 1240.0f, 575.0f, -2100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    -1,
                    0,
                    75.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[2].timerInit = timer - 5;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 6001:
            Gama_play_change_camera_set(play, subCamId, CAM_SET_CS_3);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            spA0 = Actor_get_world(actor);
            if (spA0.pos.z > -750.0f) {
                set_onepoint_spline(subCam, op05Mode, op05nFrames, op05Lookat, op05bPosition);
            } else {
                set_onepoint_spline(subCam, op05Mode, op05nFrames, op05Lookat, op05Position);
            }

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 0, 0, 20, 0);
            setTimerQuake(i, op05nFrames - 10);
            break;

        case 3400:
            Gama_play_change_camera_set(play, subCamId, CAM_SET_CS_3);
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            set_onepoint_spline(subCam, op04Mode | 0x2000, op04nFrames, op04Lookat, op04Position);
            xyz2sv(&mainCam->eye, &op04Position[op04nPoints - 2].pos);
            xyz2sv(&mainCam->at, &op04Lookat[op04nPoints - 2].pos);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 0x4E20);
            setScaleQuake(i, 1, 0, 50, 0);
            setTimerQuake(i, op04nFrames - 20);
            break;

        case 3390: {
            static OnePointCsFull cuts_of_this[9] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    40,
                    0,
                    70.0f,
                    1.0f,
                    { 4290.0f, -1332.0f, -1900.0f },
                    { 4155.0f, -1360.0f, -1840.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    70.0f,
                    1.0f,
                    { 4215.0f, -975.0f, -2095.0f },
                    { 4070.0f, -1000.0f, -2025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    5,
                    0,
                    70.0f,
                    1.0f,
                    { 4215.0f, -975.0f, -2095.0f },
                    { 4070.0f, -1000.0f, -2025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    80,
                    8,
                    75.0f,
                    1.0f,
                    { 4010.0f, -1152.0f, -1728.0f },
                    { 3997.0f, -1194.0f, -1629.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_57),
                    0x2121,
                    1,
                    8,
                    75.0f,
                    1.0f,
                    { 20.0f, 20.0f, 0.0f },
                    { 50.0f, 30.0f, 200.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    99,
                    2,
                    70.0f,
                    0.02f,
                    { -20.0f, 0.0f, 20.0f },
                    { 300.0f, 50.0f, -500.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_9, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_56),
                    0x2121,
                    149,
                    -20,
                    70.0f,
                    0.1f,
                    { 100.0f, 50.0f, -100.0f },
                    { 5000.0f, 1055.0f, -2250.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -20.0f, 0.0f },
                    { 0.0f, 20.0f, -150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            player->actor.shape.rot.y = player->actor.world.rot.y = player->yaw = -0x3FD9;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3310:
            Gama_play_change_camera_set(play, subCamId, CAM_SET_FIRE_STAIRCASE);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_copy_camera_position(play, subCamId, CAM_ID_MAIN);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 2, 0, 0, 0);
            setTimerQuake(i, timer);
            break;

        case 3290: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    50.0f,
                    0.5f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_2),
                    0x2121,
                    23,
                    0,
                    50.0f,
                    0.5f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;
            spA0 = Actor_get_eye(actor);
            player->actor.shape.rot.y = player->actor.world.rot.y = player->yaw = spA0.rot.y;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 12000);
            setScaleQuake(i, 0, 0, 1000, 0);
            setTimerQuake(i, 5);
            break;
        }

        case 3340: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    10,
                    0,
                    30.0f,
                    1.0f,
                    { -2130.0f, 2885.0f, -1055.0f },
                    { -2085.0f, 2875.0f, -1145.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    30,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 12000);
            setScaleQuake(i, 0, 0, 1000, 0);
            setTimerQuake(i, 5);
            break;
        }

        case 3360: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x42C2,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 220.0f, 0.0f },
                    { 0.0f, 220.0f, 240.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0080,
                    29,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 220.0f, 0.0f },
                    { 0.0f, 220.0f, 240.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x21A1,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 10.0f, -200.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3350: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    10,
                    5,
                    55.0f,
                    0.75f,
                    { 400.0f, -50.0f, 800.0f },
                    { 600.0f, -60.0f, 800.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    15,
                    10,
                    40.0f,
                    0.75f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 10.0f, 200.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    25,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;
            if (actor->world.pos.x > 0.0f) {
                cuts_of_this[1].rollTargetInit = -cuts_of_this[1].rollTargetInit;
                cuts_of_this[2].rollTargetInit = -cuts_of_this[2].rollTargetInit;
                cuts_of_this[1].atTargetInit.x = -cuts_of_this[1].atTargetInit.x;
                cuts_of_this[1].atTargetInit.y = 50.0f;
                cuts_of_this[1].eyeTargetInit.y = 80.0f;
                cuts_of_this[1].eyeTargetInit.x = -cuts_of_this[1].eyeTargetInit.x;
            }
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3330: {
            static OnePointCsFull cuts_of_this[7] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, -5.0f, 0.0f },
                    { 0.0f, 0.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    15,
                    0,
                    40.0f,
                    0.4f,
                    { 0.0f, 60.0f, -20.0f },
                    { 0.0f, 60.0f, 100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    20,
                    0,
                    40.0f,
                    1.0f,
                    { 0.0f, 60.0f, -20.0f },
                    { 0.0f, 60.0f, 100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    20,
                    0,
                    60.0f,
                    1.0f,
                    { 20.0f, 60.0f, 20.0f },
                    { 40.0f, 60.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    90,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3410: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    10,
                    45.0f,
                    1.0f,
                    { -1200.0f, 730.0f, -860.0f },
                    { -1100.0f, 500.0f, -1025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    10,
                    45.0f,
                    0.1f,
                    { -880.0f, 480.0f, -860.0f },
                    { -1100.0f, 500.0f, -1025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                    ONEPOINT_CS_INIT_FIELD_ACTORCAT(ACTORCAT_BG),
                    0x0101,
                    20,
                    10,
                    45.0f,
                    0.1f,
                    { -880.0f, 500.0f, -860.0f },
                    { -1100.0f, 500.0f, -1025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                    ONEPOINT_CS_INIT_FIELD_ACTORCAT(ACTORCAT_DOOR),
                    0x0101,
                    5,
                    10,
                    45.0f,
                    0.1f,
                    { -880.0f, 500.0f, -860.0f },
                    { -1100.0f, 500.0f, -1025.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 4, 0, 0, 0);
            setTimerQuake(i, 20);
            break;
        }

        case 3450: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    -2,
                    65.0f,
                    1.0f,
                    { -625.0f, 185.0f, -685.0f },
                    { -692.0f, 226.0f, -515.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 2, 0, 0, 0);
            setTimerQuake(i, 10);
            break;
        }

        case 3440: {
            static OnePointCsFull cuts_of_this[6] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    0,
                    55.0f,
                    1.0f,
                    { 60.0f, 1130.0f, -1430.0f },
                    { 60.0f, 1130.0f, -1190.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    18,
                    -13,
                    68.0f,
                    1.0f,
                    { 60.0f, 1130.0f, -1445.0f },
                    { 180.0f, 1170.0f, -1240.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    16,
                    18,
                    75.0f,
                    1.0f,
                    { 42.0f, 1040.0f, -1400.0f },
                    { -20.0f, 940.0f, -1280.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    4,
                    0,
                    60.0f,
                    1.0f,
                    { 60.0f, 1100.0f, -1465.0f },
                    { 60.0f, 1100.0f, -1180.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    32,
                    0,
                    70.0f,
                    1.0f,
                    { 60.0f, 1100.0f, -1030.0f },
                    { 60.0f, 1150.0f, -740.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            player->stateFlags1 |= PLAYER_STATE1_29;
            player->actor.freezeTimer = 90;

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 2, 0, 0, 0);
            setTimerQuake(i, 10);
            break;
        }

        case 3430: {
            static OnePointCsFull cuts_of_this[7] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    5,
                    0,
                    70.0f,
                    1.0f,
                    { 60.0f, 1800.0f, -920.0f },
                    { 60.0f, 1860.0f, -800.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    0,
                    70.0f,
                    0.1f,
                    { 60.0f, 1720.0f, -920.0f },
                    { 60.0f, 1780.0f, -800.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0142,
                    1,
                    0,
                    75.0f,
                    1.0f,
                    { 0.0f, 70.0f, 0.0f },
                    { 60.0f, 990.0f, -690.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0142,
                    119,
                    0,
                    75.0f,
                    0.05f,
                    { 0.0f, 70.0f, 0.0f },
                    { 60.0f, 990.0f, -690.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    20,
                    0,
                    60.0f,
                    0.1f,
                    { 0.0f, 70.0f, 0.0f },
                    { 0.0f, 100.0f, 200.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_1);
            setSpeedQuake(i, 32000);
            setScaleQuake(i, 1, 0, 10, 0);
            setTimerQuake(i, 20);
            break;
        }

        case 4100: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 4100.0f, 1200.0f, -1400.0f },
                    { 3900.0f, 1100.0f, -1400.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_59),
                    0x0101,
                    60,
                    4,
                    50.0f,
                    0.94f,
                    { 4100.0f, 965.0f, -1385.0f },
                    { 3790.0f, 825.0f, -1325.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    90,
                    -5,
                    130.0f,
                    0.02f,
                    { 4100.0f, 975.0f, -1375.0f },
                    { 3735.0f, 715.0f, -1325.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x2323,
                    2,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 60.0f, 0.0f },
                    { -10.0f, 15.0f, -200.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player->actor.shape.rot.y = player->actor.world.rot.y = player->yaw = 0x3FFC;
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            break;
        }

        case 4110: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    2,
                    45.0f,
                    1.0f,
                    { 975.0f, 225.0f, -1195.0f },
                    { 918.0f, 228.0f, -1228.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4120: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    0,
                    45.0f,
                    1.0f,
                    { -915.0f, -2185.0f, 6335.0f },
                    { -915.0f, -2290.0f, 6165.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    -1,
                    0,
                    80.0f,
                    0.8f,
                    { -920.0f, -2270.0f, 6140.0f },
                    { -920.0f, -2280.0f, 6070.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    0,
                    80.0f,
                    0.9f,
                    { -920.0f, -2300.0f, 6140.0f },
                    { -920.0f, -2300.0f, 6070.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            cuts_of_this[1].timerInit = 80;
            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4140: {
            static OnePointCsFull cuts_of_this[6] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    30,
                    0,
                    60.0f,
                    1.0f,
                    { 1400.0f, 100.0f, -170.0f },
                    { 1250.0f, 100.0f, -170.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    130,
                    0,
                    60.0f,
                    0.2f,
                    { 0.0f, -5.0f, 0.0f },
                    { -150.0f, -5.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0303,
                    69,
                    0,
                    85.0f,
                    1.0f,
                    { -40.0f, 0.0f, 0.0f },
                    { -40.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0303,
                    20,
                    0,
                    60.0f,
                    1.0f,
                    { 10.0f, 0.0f, 0.0f },
                    { 10.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            break;
        }

        case 4150: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0101,
                    20,
                    5,
                    30.0f,
                    1.0f,
                    { 800.0f, -40.0f, 2170.0f },
                    { 512.0f, 142.0f, 2020.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    -2,
                    70.0f,
                    0.8f,
                    { 800.0f, -40.0f, 2170.0f },
                    { 512.0f, 142.0f, 2020.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_8),
                    0x0101,
                    90,
                    2,
                    62.0f,
                    1.0f,
                    { 1140.0f, 125.0f, 1920.0f },
                    { 1255.0f, 150.0f, 1785.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 10.0f, 0.0f },
                    { 30.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4160: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    20,
                    -10,
                    70.0f,
                    1.0f,
                    { -930.0f, 765.0f, -3075.0f },
                    { -700.0f, 700.0f, -3075.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    80,
                    -10,
                    70.0f,
                    0.05f,
                    { -930.0f, 205.0f, -3075.0f },
                    { -700.0f, 140.0f, -3075.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    120,
                    0,
                    70.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4170: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    4,
                    50.0f,
                    1.0f,
                    { 0.0f, 400.0f, -1000.0f },
                    { -200.0f, 500.0f, -850.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4190: {
            static OnePointCsFull cuts_of_this[8] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    -15,
                    70.0f,
                    1.0f,
                    { 230.0f, 3675.0f, -4230.0f },
                    { -45.0f, 3650.0f, -4415.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -120.0f, 2187.0f, -3286.0f },
                    { -110.0f, 2162.0f, -3262.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_21, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    55,
                    0,
                    60.0f,
                    1.0f,
                    { -38.0f, 1467.0f, -1102.0f },
                    { 64.0f, 1423.0f, -1188.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    -15,
                    70.0f,
                    1.0f,
                    { 230.0f, 3675.0f, -4230.0f },
                    { -20.0f, 3650.0f, -4400.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4200: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    65.0f,
                    1.0f,
                    { 1095.0f, 2890.0f, -2980.0f },
                    { 1166.0f, 2695.0f, -2710.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    15,
                    65.0f,
                    1.0f,
                    { 566.0f, 4654.0f, -4550.0f },
                    { 606.0f, 5160.0f, -4740.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_8);
            changeCameraMode(mainCam, CAM_MODE_NORMAL);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4210: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    999,
                    0,
                    85.0f,
                    1.0f,
                    { -15.0f, 185.0f, 160.0f },
                    { -15.0f, 210.0f, 250.0f },
                },
            };

            player->actor.freezeTimer = timer;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 12000);
            setScaleQuake(i, 0, 1, 100, 0);
            setTimerQuake(i, timer - 80);
            break;
        }

        case 4220: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    999,
                    -2,
                    70.0f,
                    1.0f,
                    { -62.0f, 60.0f, -315.0f },
                    { -115.0f, 30.0f, -445.0f },
                },
            };
            static OnePointCsFull D_80122C64[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    999,
                    3,
                    70.0f,
                    1.0f,
                    { -40.0f, 80.0f, 375.0f },
                    { -85.0f, 45.0f, 485.0f },
                },
            };

            csInfo->keyFrames = (player->actor.world.pos.z < -15.0f) ? cuts_of_this : D_80122C64;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            player_demo_mode_set2(play, &player->actor, PLAYER_CSACTION_1);

            i = startQuake(subCam, QUAKE_TYPE_3);
            setSpeedQuake(i, 12000);
            setScaleQuake(i, 0, 1, 10, 0);
            setTimerQuake(i, timer - 10);
            break;
        }

        case 4221: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    999,
                    5,
                    60.0f,
                    1.0f,
                    { -70.0f, 140.0f, 25.0f },
                    { 10.0f, 180.0f, 195.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3260: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 1000.0f },
                    { 0.0f, 0.0f, 1100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    -1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, -100.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            cuts_of_this[1].timerInit = timer - 5;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 3261: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, -100.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    -1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 1000.0f },
                    { 0.0f, 0.0f, 1100.0f },
                },
            };

            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            cuts_of_this[1].timerInit = timer - 10;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 8010: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    -4,
                    50.0f,
                    1.0f,
                    { 230.0f, 65.0f, 300.0f },
                    { 50.0f, 50.0f, 225.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 8002: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 5.0f, -145.0f },
                    { 0.0f, 55.0f, 55.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_17, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    -1,
                    -1.0f,
                    -1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 8700: {
            static OnePointCsFull cuts_of_this_allay[2][7] = {
                {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x2222,
                        10,
                        5,
                        90.0f,
                        0.2f,
                        { 50.0f, 100.0f, 140.0f },
                        { -30.0f, 10.0f, -20.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        20,
                        0,
                        90.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x4343,
                        30,
                        -5,
                        50.0f,
                        0.2f,
                        { -10.0f, 80.0f, 10.0f },
                        { 20.0f, 20.0f, 120.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        -5,
                        60.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                        ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                        0x4343,
                        160,
                        10,
                        80.0f,
                        0.005f,
                        { -50.0f, 60.0f, 0.0f },
                        { -100.0f, 20.0f, 50.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0501,
                        50,
                        0,
                        60.0f,
                        1.0f,
                        { 0.0f, -10.0f, 0.0f },
                        { 0.0f, 10.0f, 80.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_19, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        -1,
                        -1.0f,
                        -1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                },
                {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x2222,
                        10,
                        -5,
                        90.0f,
                        0.2f,
                        { -50.0f, 100.0f, 140.0f },
                        { 30.0f, 10.0f, -20.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        20,
                        0,
                        90.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x4343,
                        30,
                        5,
                        50.0f,
                        0.2f,
                        { 10.0f, 80.0f, 10.0f },
                        { -20.0f, 20.0f, 120.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        5,
                        60.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                        ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                        0x4343,
                        160,
                        -10,
                        80.0f,
                        0.005f,
                        { 50.0f, 60.0f, 0.0f },
                        { 100.0f, 20.0f, 50.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0501,
                        50,
                        0,
                        60.0f,
                        1.0f,
                        { 0.0f, -10.0f, 0.0f },
                        { 0.0f, 10.0f, 80.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_19, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        -1,
                        -1.0f,
                        -1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                },
            };

            spA0 = Actor_get_eye(actor);
            sp8C = Actor_get_eye(&player->actor);
            cuts_of_this_allay[timer & 1][0].atTargetInit.y = ((spA0.pos.y - sp8C.pos.y) / 10.0f) + 90.0f;
            cuts_of_this_allay[timer & 1][5].atTargetInit = mainCam->at;

            csInfo->keyFrames = cuts_of_this_allay[timer & 1];
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this_allay[0]);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 1100: {
            s32 tempDiff = play->state.frames - magic_frame;

            if ((tempDiff > 3600) || (tempDiff < -3600)) {
                static OnePointCsFull cuts_of_this0[5] = {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        2,
                        8,
                        70.0f,
                        1.0f,
                        { -27.0f, -96.0f, 25.0f },
                        { 37.0f, -5.0f, 100.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        38,
                        4,
                        60.0f,
                        1.0f,
                        { 64.0f, -109.0f, 55.0f },
                        { 37.0f, 150.0f, 155.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        2,
                        8,
                        70.0f,
                        1.0f,
                        { 45.0f, 123.0f, 45.0f },
                        { 70.0f, 5.0f, 125.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        58,
                        4,
                        60.0f,
                        0.9f,
                        { 82.0f, 95.0f, 55.0f },
                        { 25.0f, -175.0f, 180.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        0,
                        60.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                };

                csInfo->keyFrames = cuts_of_this0;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this0);
            } else {
                static OnePointCsFull cuts_of_this1[3] = {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        20,
                        8,
                        70.0f,
                        1.0f,
                        { 65.0f, -150.0f, 50.0f },
                        { 30.0f, 10.0f, 90.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0xA2A2,
                        100,
                        0,
                        60.0f,
                        1.0f,
                        { 70.0f, -160.0f, 50.0f },
                        { 25.0f, 180.0f, 180.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, true),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0000,
                        1,
                        0,
                        60.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                };

                if (play->state.frames & 1) {
                    cuts_of_this1[0].rollTargetInit = -cuts_of_this1[0].rollTargetInit;
                    cuts_of_this1[0].atTargetInit.y = -cuts_of_this1[0].atTargetInit.y;
                    cuts_of_this1[0].eyeTargetInit.y = -cuts_of_this1[0].eyeTargetInit.y;
                    cuts_of_this1[1].atTargetInit.y = -cuts_of_this1[1].atTargetInit.y;
                }
                csInfo->keyFrames = cuts_of_this1;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this1);
            }
            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            magic_frame = play->state.frames;

            break;
        }

        case 9806:
            subCam->timer = -99;
            if (Game_play_change_camera_check(play)) {
                Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_TURN_AROUND);
                subCam->data2 = 0xC;
            } else {
                Gama_play_copy_camera_position(play, subCamId, CAM_ID_MAIN);
                Gama_play_change_camera_set(play, subCamId, CAM_SET_FREE2);
            }
            break;

        case 9908:
            if (Game_play_change_camera_check(play)) {
                static OnePointCsFull cuts_of_this1[4] = {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE),
                        0x4343,
                        1,
                        0,
                        50.0f,
                        1.0f,
                        { 0.0f, 20.0f, 0.0f },
                        { 0.0f, 5.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                        ONEPOINT_CS_INIT_FIELD_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE),
                        0x4343,
                        48,
                        0,
                        50.0f,
                        0.75f,
                        { 0.0f, 80.0f, 0.0f },
                        { 0.0f, 15.0f, -1.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                        ONEPOINT_CS_INIT_FIELD_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE),
                        0x4343,
                        1,
                        5,
                        45.0f,
                        1.0f,
                        { 0.0f, 0.0f, 30.0f },
                        { 30.0f, 120.0f, 60.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                        ONEPOINT_CS_INIT_FIELD_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE),
                        0x4343,
                        -1,
                        0,
                        -1.0f,
                        1.0f,
                        { -1.0f, -1.0f, -1.0f },
                        { -1.0f, -1.0f, -1.0f },
                    },
                };

                cuts_of_this1[0].eyeTargetInit.z = cuts_of_this1[1].eyeTargetInit.z = !LINK_IS_ADULT ? 100.0f : 120.0f;

                if (player->stateFlags1 & PLAYER_STATE1_27) {
                    cuts_of_this1[2].atTargetInit.z = 0.0f;
                }
                spA0 = Actor_get_shape(&player->actor);
                spD0 = sglobe_by_2pos(&spA0.pos, &mainCam->at);
                spD0.yaw -= spA0.rot.y;
                cuts_of_this1[3].atTargetInit = sglobe2world(&spD0);
                spD0 = sglobe_by_2pos(&spA0.pos, &mainCam->eye);
                spD0.yaw -= spA0.rot.y;
                cuts_of_this1[3].eyeTargetInit = sglobe2world(&spD0);
                cuts_of_this1[3].fovTargetInit = mainCam->fov;
                cuts_of_this1[3].timerInit = timer - 50;

                csInfo->keyFrames = cuts_of_this1;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this1);

                Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            } else {
                static OnePointCsFull cuts_of_this2[2] = {
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                        ONEPOINT_CS_INIT_FIELD_NONE,
                        0x0101,
                        1,
                        0,
                        60.0f,
                        1.0f,
                        { 0.0f, 0.0f, 0.0f },
                        { 0.0f, 0.0f, 0.0f },
                    },
                    {
                        ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                        ONEPOINT_CS_INIT_FIELD_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE),
                        0x0101,
                        49,
                        0,
                        50.0f,
                        0.05f,
                        { 0.0f, 0.0f, 0.0f },
                        { 0.0f, 0.0f, 0.0f },
                    },
                };

                cuts_of_this2[1].timerInit = timer - 1;
                cuts_of_this2[0].fovTargetInit = mainCam->fov;
                cuts_of_this2[0].atTargetInit = cuts_of_this2[1].atTargetInit = mainCam->at;
                cuts_of_this2[0].eyeTargetInit = cuts_of_this2[1].eyeTargetInit = mainCam->eye;

                csInfo->keyFrames = cuts_of_this2;
                csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this2);

                Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            }
            break;

        case 1000: {
            static OnePointCsFull cuts_of_this[1] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_69),
                    0x0101,
                    9999,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 8603: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    45,
                    -3,
                    65.0f,
                    1.0f,
                    { -52.0f, 84.0f, -846.0f },
                    { -159.0f, 33.0f, -729.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    15,
                    0,
                    60.0f,
                    1.0f,
                    { 10.0f, -5.0f, 0.0f },
                    { 0.0f, 0.0f, -150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 8604: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    45,
                    3,
                    65.0f,
                    1.0f,
                    { -16.0f, 87.0f, -829.0f },
                    { 98.0f, 24.0f, -714.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    15,
                    0,
                    60.0f,
                    1.0f,
                    { 10.0f, -5.0f, 0.0f },
                    { 0.0f, 0.0f, -150.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4000: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x4242,
                    40,
                    0,
                    40.0f,
                    1.0f,
                    { 0.0f, 50.0f, -40.0f },
                    { 0.0f, 60.0f, -160.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_77),
                    0x4242,
                    40,
                    0,
                    60.0f,
                    0.3f,
                    { 0.0f, 90.0f, -40.0f },
                    { 0.0f, 60.0f, -160.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_4, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x2121,
                    10,
                    0,
                    60.0f,
                    0.2f,
                    { 0.0f, -10.0f, 10.0f },
                    { 0.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4010: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_5),
                    0x0441,
                    10,
                    0,
                    70.0f,
                    1.0f,
                    { 0.0f, -10.0f, 20.0f },
                    { 0.0f, 0.0f, 120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4141,
                    30,
                    0,
                    50.0f,
                    0.1f,
                    { 0.0f, -10.0f, 20.0f },
                    { 0.0f, 10.0f, 80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    10,
                    0,
                    60.0f,
                    0.9f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4011: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_PLAYER_CS(PLAYER_CSACTION_1),
                    0x4141,
                    1,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, -10.0f, 20.0f },
                    { 0.0f, 10.0f, 60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0441,
                    39,
                    0,
                    70.0f,
                    0.1f,
                    { 0.0f, -10.0f, 20.0f },
                    { 0.0f, 0.0f, 100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_16, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    15,
                    0,
                    60.0f,
                    0.9f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4020: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    10,
                    0,
                    60.0f,
                    1.0f,
                    { -1110.0f, -180.0f, -840.0f },
                    { -985.0f, -220.0f, -840.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    70,
                    -45,
                    75.0f,
                    1.0f,
                    { -1060.0f, -160.0f, -840.0f },
                    { -1005.0f, -230.0f, -840.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    10,
                    -45,
                    75.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    180,
                    9,
                    80.0f,
                    1.0f,
                    { -1205.0f, -175.0f, -840.0f },
                    { -1305.0f, -230.0f, -828.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4021: {
            static OnePointCsFull cuts_of_this[4] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0142,
                    10,
                    0,
                    70.0f,
                    1.0f,
                    { 0.0f, 80.0f, 0.0f },
                    { -1650.0f, 200.0f, -2920.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0142,
                    110,
                    -2,
                    50.0f,
                    0.5f,
                    { 0.0f, 150.0f, 0.0f },
                    { -1320.0f, 170.0f, -2900.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_11, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    100,
                    2,
                    70.0f,
                    0.1f,
                    { 0.0f, 150.0f, 50.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    60,
                    2,
                    45.0f,
                    0.01f,
                    { 0.0f, 150.0f, 50.0f },
                    { 0.0f, 200.0f, -80.0f },
                },
            };

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 4022: {
            static OnePointCsFull cuts_of_this[5] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    20,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 50.0f, -10.0f },
                    { 0.0f, 0.0f, 100.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_10, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    80,
                    0,
                    75.0f,
                    1.0f,
                    { 2900.0f, 1300.0f, 530.0f },
                    { 2800.0f, 1190.0f, 540.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    10,
                    0,
                    75.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    55,
                    0,
                    75.0f,
                    1.0f,
                    { 2900.0f, 1300.0f, 530.0f },
                    { 1500.0f, 1415.0f, 650.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    100,
                    -45,
                    75.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            subCam->timer = cuts_of_this[0].timerInit + cuts_of_this[3].timerInit + cuts_of_this[1].timerInit +
                            cuts_of_this[2].timerInit + cuts_of_this[4].timerInit;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 9703: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    30,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 28.0f, 0.0f },
                    { 0.0f, 20.0f, 40.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_13, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    120,
                    0,
                    180.0f,
                    0.4f,
                    { 0.0f, -5.0f, 0.0f },
                    { 0.0f, 2.0f, 40.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;
            if (LINK_IS_ADULT) {
                cuts_of_this[1].atTargetInit.y = 60.0f;
                cuts_of_this[1].eyeTargetInit.y = 52.0f;
            }

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 9704: {
            static OnePointCsFull cuts_of_this[2] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    30,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    180,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 78.0f, 0.0f },
                    { 0.0f, 78.0f, 200.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 9705: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0101,
                    60,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    30,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, 28.0f, 0.0f },
                    { 0.0f, 20.0f, -45.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_13, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    120,
                    0,
                    180.0f,
                    0.4f,
                    { 0.0f, -5.0f, 0.0f },
                    { 0.0f, 2.0f, 45.0f },
                },
            };

            cuts_of_this[0].atTargetInit = play->view.at;
            cuts_of_this[0].eyeTargetInit = play->view.eye;
            cuts_of_this[0].fovTargetInit = play->view.fovy;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, player, CAM_SET_CS_C);
            break;
        }

        case 5110: {
            static OnePointCsFull cuts_of_this[3] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 20.0f, 0.0f },
                    { 0.0f, 40.0f, -120.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_9, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x4242,
                    0,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 20.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[1].timerInit = 10;

            csInfo->keyFrames = cuts_of_this;
            csInfo->keyFrameCount = ARRAY_COUNT(cuts_of_this);

            Gama_play_set_camera_owner(play, subCamId, (Player*)actor, CAM_SET_CS_C);
            break;
        }

        default:
            PRINTF(VT_COL(RED, WHITE) "onepointdemo camera: demo number not found !! (%d)\n" VT_RST, csId);
            break;
    }
    return 0;
}

s16 chainOnepointDemo(PlayState* play, s16 newCamId, s16 parentCamId) {
    s16 prevCamId = play->cameraPtrs[parentCamId]->childCamId;

    play->cameraPtrs[newCamId]->parentCamId = parentCamId;
    play->cameraPtrs[parentCamId]->childCamId = newCamId;

    return prevCamId;
}

/**
 * Removes a cutscene camera from the list. Returns the parent cam if the removed camera is active, otherwise returns
 * CAM_ID_NONE
 */
s32 clearOnepointDemo(PlayState* play, s16 subCamId) {
    Camera* subCam = play->cameraPtrs[subCamId];
    s32 nextCamId;

    if (subCam->camId == CHILD_CAM(subCam)->parentCamId) {
        CHILD_CAM(subCam)->parentCamId = subCam->parentCamId;
    }
    if (subCam->camId == PARENT_CAM(subCam)->childCamId) {
        PARENT_CAM(subCam)->childCamId = subCam->childCamId;
    }
    nextCamId = (play->activeCamId == subCamId) ? subCam->parentCamId : CAM_ID_NONE;
    subCam->parentCamId = CAM_ID_MAIN;
    subCam->childCamId = subCam->parentCamId;
    subCam->timer = -1;
    Gama_play_clear_camera(subCam->play, subCam->camId);
    return nextCamId;
}

#define vChildCamId temp2
#define vSubCamStatus temp1
#define vCurCamId temp2
#define vNextCamId temp3
#define vParentCamId temp1

/**
 * Creates a cutscene subcamera with the specified ID, duration, and targeted actor. The camera is placed into the
 * cutscene queue in front of the specified camera, then all lower priority demos in front of it are removed from the
 * queue.
 */
s16 makeOnepointDemo(PlayState* play, s16 csId, s16 timer, Actor* actor, s16 parentCamId) {
    Camera* subCam;
    s16 subCamId;
    s16 temp1;
    s16 temp2;
    s16 temp3;

    if (parentCamId == CAM_ID_NONE) {
        parentCamId = play->activeCamId;
    }
    subCamId = Gama_play_make_camera(play);
    if (subCamId == CAM_ID_NONE) {
        PRINTF(VT_COL(RED, WHITE) "onepoint demo: error: too many cameras ... give up! type=%d\n" VT_RST, csId);
        return CAM_ID_NONE;
    }

    // Inserts the cutscene camera into the cutscene queue in front of parentCam

    vChildCamId = play->cameraPtrs[parentCamId]->childCamId;
    vSubCamStatus = CAM_STAT_ACTIVE;
    if (vChildCamId >= CAM_ID_SUB_FIRST) {
        chainOnepointDemo(play, vChildCamId, subCamId);
        vSubCamStatus = CAM_STAT_WAIT;
    } else {
        alpha_change(HUD_VISIBILITY_NOTHING_ALT);
    }
    chainOnepointDemo(play, subCamId, parentCamId);

    subCam = play->cameraPtrs[subCamId];

    subCam->timer = timer;
    subCam->target = actor;

    subCam->at = play->view.at;
    subCam->eye = play->view.eye;
    subCam->fov = play->view.fovy;

    subCam->csId = csId;

    if (parentCamId == CAM_ID_MAIN) {
        Gama_play_set_camera_status(play, parentCamId, CAM_STAT_UNK3);
    } else {
        Gama_play_set_camera_status(play, parentCamId, CAM_STAT_WAIT);
    }
    set_onepointdemo(play, subCamId, csId, actor, timer);
    Gama_play_set_camera_status(play, subCamId, vSubCamStatus);

    // Removes all lower priority cutscenes in front of this cutscene from the queue.
    vCurCamId = subCamId;
    vNextCamId = play->cameraPtrs[subCamId]->childCamId;

    while (vNextCamId >= CAM_ID_SUB_FIRST) {
        if ((play->cameraPtrs[vNextCamId]->csId / 100) < (play->cameraPtrs[subCamId]->csId / 100)) {
            PRINTF(VT_COL(YELLOW, BLACK) "onepointdemo camera[%d]: killed 'coz low priority (%d < %d)\n" VT_RST,
                   vNextCamId, play->cameraPtrs[vNextCamId]->csId, play->cameraPtrs[subCamId]->csId);
            if (play->cameraPtrs[vNextCamId]->csId != 5010) {
                if ((vParentCamId = clearOnepointDemo(play, vNextCamId)) != CAM_ID_NONE) {
                    Gama_play_set_camera_status(play, vParentCamId, CAM_STAT_ACTIVE);
                }
            } else {
                vCurCamId = vNextCamId;
                deleteOnepointDemo(play, vNextCamId);
            }
        } else {
            vCurCamId = vNextCamId;
        }
        vNextCamId = play->cameraPtrs[vCurCamId]->childCamId;
    }
    return subCamId;
}

/**
 *  Ends the cutscene in subCamId by setting its timer to 0. For attention cutscenes, it is set to 5 instead.
 */
s16 deleteOnepointDemo(PlayState* play, s16 subCamId) {
    if (subCamId == CAM_ID_NONE) {
        subCamId = play->activeCamId;
    }
    if (play->cameraPtrs[subCamId] != NULL) {
        PRINTF("onepointdemo camera[%d]: delete timer=%d next=%d\n", subCamId, play->cameraPtrs[subCamId]->timer,
               play->cameraPtrs[subCamId]->parentCamId);
        if (play->cameraPtrs[subCamId]->csId == 5010) {
            play->cameraPtrs[subCamId]->timer = 5;
        } else {
            play->cameraPtrs[subCamId]->timer = 0;
        }
    }
    return subCamId;
}

#define vTargetCat temp1
#define vParentCamId temp1
#define vLastHigherCat temp2
#define vSubCamId temp2

/**
 *  Adds an attention cutscene to the cutscene queue.
 */
s32 makeActorAttentionDemo(PlayState* play, Actor* actor) {
    Camera* parentCam;
    s32 temp1;
    s32 temp2;
    s32 timer;

#if DEBUG_FEATURES
    if (stop_attention) {
        PRINTF(VT_COL(YELLOW, BLACK) "actor attention demo camera: canceled by other camera\n" VT_RST);
        return CAM_ID_NONE;
    }
#endif

    attention_part = -1;

    parentCam = play->cameraPtrs[CAM_ID_MAIN];
    if (parentCam->mode == CAM_MODE_FOLLOW_BOOMERANG) {
        PRINTF(VT_COL(YELLOW, BLACK) "actor attention demo camera: change mode BOOKEEPON -> NORMAL\n" VT_RST);
        changeCameraMode(parentCam, CAM_MODE_NORMAL);
    }

    // Finds the camera of the first actor attention demo with a lower category actor, or the first non-attention demo
    // after at least one attention demo.

    vLastHigherCat = -1;
    while (parentCam->childCamId != CAM_ID_MAIN) {
        parentCam = play->cameraPtrs[parentCam->childCamId];
        if (parentCam == NULL) {
            break;
        } else if (parentCam->setting != CAM_SET_CS_ATTENTION) {
            if (vLastHigherCat == -1) {
                continue;
            } else {
                break;
            }
        } else {
            vTargetCat = parentCam->target->category;
            if (actor->category > vTargetCat) {
                break;
            }
            vLastHigherCat = vTargetCat;
        }
    }
    // Actorcat is only undefined if the actor is in a higher category than all other attention cutscenes. In this case,
    // it goes in the first position of the list. Otherwise, it goes in the index found in the loop.
    vParentCamId = (vLastHigherCat == -1) ? CAM_ID_MAIN : parentCam->camId;

    switch (actor->category) {
        case ACTORCAT_SWITCH:
        case ACTORCAT_BG:
        case ACTORCAT_PLAYER:
        case ACTORCAT_PROP:
        case ACTORCAT_DOOR:
            timer = 30;
            break;
        case ACTORCAT_NPC:
        case ACTORCAT_ITEMACTION:
        case ACTORCAT_CHEST:
            timer = 100;
            break;
        case ACTORCAT_EXPLOSIVE:
        case ACTORCAT_ENEMY:
        case ACTORCAT_MISC:
        case ACTORCAT_BOSS:
        default:
            PRINTF(VT_COL(YELLOW, BLACK) "actor attention demo camera: %d: unkown part of actor %d\n" VT_RST,
                   play->state.frames, actor->category);
            timer = 30;
            break;
    }
    PRINTF(VT_FGCOL(CYAN) "%06u:" VT_RST " actor attention demo camera: request %d ", play->state.frames,
           actor->category);

    // If the previous attention cutscene has an actor in the same category, skip this actor.
    if (actor->category == vLastHigherCat) {
        PRINTF("→ " VT_FGCOL(MAGENTA) "×" VT_RST " (%d)\n", actor->id);
        return CAM_ID_NONE;
    }
    PRINTF("→ " VT_FGCOL(BLUE) "○" VT_RST " (%d)\n", actor->id);
    vSubCamId = makeOnepointDemo(play, 5010, timer, actor, vParentCamId);
    if (vSubCamId == CAM_ID_NONE) {
        PRINTF(VT_COL(RED, WHITE) "actor attention demo: give up! \n" VT_RST, actor->id);
        return CAM_ID_NONE;
    } else {
        s32* data = (s32*)&play->cameraPtrs[vSubCamId]->data1;

        *data = NA_SE_SY_CORRECT_CHIME;
        return vSubCamId;
    }
}

/**
 *  Adds an attention cutscene to the cutscene queue with the specified sound effect
 */
s32 makeActorAttentionDemoSE(PlayState* play, Actor* actor, s32 sfxId) {
    s32 subCamId = makeActorAttentionDemo(play, actor);

    if (subCamId != CAM_ID_NONE) {
        s32* data = (s32*)&play->cameraPtrs[subCamId]->data1;

        *data = sfxId;
    }
    return subCamId;
}

// unused
void allowActorAttentionDemo(void) {
    stop_attention = false;
}

// unused
void denyActorAttentionDemo(void) {
    stop_attention = true;
}

s32 checkPartrActorAttentionDemo(PlayState* play, s32 actorCategory) {
    Camera* parentCam = play->cameraPtrs[CAM_ID_MAIN];

    while (parentCam->childCamId != CAM_ID_MAIN) {
        parentCam = play->cameraPtrs[parentCam->childCamId];
        if ((parentCam == NULL) || (parentCam->setting != CAM_SET_CS_ATTENTION)) {
            break;
        } else if (actorCategory == parentCam->target->category) {
            return true;
        }
    }
    return false;
}

// unused, also empty.
void makeDoorDemo(PlayState* play, s32 arg1) {
}
