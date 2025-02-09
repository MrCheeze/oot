#include "z_en_zl4.h"

static CutsceneCameraDirection meetdemo_fixed_data[] = {
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

static CutsceneCameraMove meetdemo_spline_data[] = {
    { ABLookat, ABPosition, 0 }, { ACLookat, ACPosition, 0 }, { AGLookat, AGPosition, 0 },
    { AJLookat, AJPosition, 0 }, { AKLookat, AKPosition, 0 }, { BALookat, BAPosition, 0 },
    { BBLookat, BBPosition, 0 }, { BCLookat, BCPosition, 0 }, { BGLookat, BGPosition, 0 },
    { BHLookat, BHPosition, 0 }, { BJLookat, BJPosition, 0 },
};

static void start_fixed_demo_camera(PlayState* play, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(play);

    changeCameraSet(activeCam, CAM_SET_FREE0);
    activeCam->at = meetdemo_fixed_data[index].at;
    activeCam->eye = activeCam->eyeNext = meetdemo_fixed_data[index].eye;
    activeCam->roll = meetdemo_fixed_data[index].roll;
    activeCam->fov = meetdemo_fixed_data[index].fov;
}

static void start_spline_demo_camera(PlayState* play, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(play);
    Player* player = GET_PLAYER(play);

    changeCameraSet(activeCam, CAM_SET_CS_0);
    setCameraResetSpline(activeCam);
    setCameraDemoSplineInfo(activeCam, meetdemo_spline_data[index].atPoints, meetdemo_spline_data[index].eyePoints, player,
                       meetdemo_spline_data[index].relativeToPlayer);
}
