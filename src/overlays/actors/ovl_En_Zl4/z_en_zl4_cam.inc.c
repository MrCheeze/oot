#include "z_en_zl4.h"

static CutsceneCameraDirection sCamDirections[] = {
    { { -490.0f, 120.0f, 0.0f }, { -440.0f, 117.0f, 0.0f }, 0, 45 },
    { { -484.0f, 122.0f, -29.0f }, { -480.0f, 116.0f, 18.0f }, 0, 80 },
    { { -413.0f, 136.0f, -72.0f }, { -403.0f, 141.0f, -89.0f }, 0, 25 },
    { { -454.0f, 120.0f, 0.0f }, { -434.0f, 121.0f, 0.0f }, 0, 20 },
    { { -453.0f, 107.0f, -13.0f }, { -440.0f, 102.0f, -25.0f }, 0, 40 },
    { { -454.0f, 105.0f, 50.0f }, { -453.0f, 105.0f, 66.0f }, 0, 60 },
    { { -496.0f, 119.0f, 0.0f }, { -443.0f, 115.0f, 0.0f }, 0, 45 },
    { { -482.0f, 119.0f, 0.0f }, { -440.0f, 115.0f, 0.0f }, 0, 30 },
    { { -551.0f, 119.0f, 7.0f }, { -587.0f, 115.0f, 14.0f }, 0, 25 },
    { { -489.0f, 129.0f, 0.0f }, { -470.0f, 128.0f, 0.0f }, 0, 40 },
    { { -525.0f, 126.0f, 0.0f }, { -509.0f, 126.0f, 0.0f }, 0, 10 },
    { { -491.0f, 120.0f, -7.0f }, { -509.0f, 115.0f, -7.0f }, 0, 75 },
    { { -485.0f, 119.0f, -35.0f }, { -484.0f, 116.0f, 15.0f }, 0, 45 },
    { { -484.0f, 123.0f, -6.0f }, { -435.0f, 122.0f, -11.0f }, 0, 45 },
};

#include "../ovl_En_Zl1/z_en_girlB_demo.inc.c"

static CutsceneCameraMove sCamMove[] = {
    { D_80B4D72C, D_80B4D7AC, 0 }, { D_80B4D82C, D_80B4D8CC, 0 }, { D_80B4D96C, D_80B4DA4C, 0 },
    { D_80B4DB2C, D_80B4DBBC, 0 }, { D_80B4DC4C, D_80B4DD3C, 0 }, { D_80B4DE2C, D_80B4DF0C, 0 },
    { D_80B4DFEC, D_80B4E08C, 0 }, { D_80B4E12C, D_80B4E1BC, 0 }, { D_80B4E24C, D_80B4E2CC, 0 },
    { D_80B4E34C, D_80B4E3CC, 0 }, { D_80B4E44C, D_80B4E4CC, 0 },
};

void EnZl4_SetActiveCamDir(PlayState* play, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(play);

    Camera_RequestSetting(activeCam, CAM_SET_FREE0);
    activeCam->at = sCamDirections[index].at;
    activeCam->eye = activeCam->eyeNext = sCamDirections[index].eye;
    activeCam->roll = sCamDirections[index].roll;
    activeCam->fov = sCamDirections[index].fov;
}

void EnZl4_SetActiveCamMove(PlayState* play, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(play);
    Player* player = GET_PLAYER(play);

    Camera_RequestSetting(activeCam, CAM_SET_CS_0);
    Camera_ResetAnim(activeCam);
    Camera_SetCSParams(activeCam, sCamMove[index].atPoints, sCamMove[index].eyePoints, player,
                       sCamMove[index].relativeToPlayer);
}
