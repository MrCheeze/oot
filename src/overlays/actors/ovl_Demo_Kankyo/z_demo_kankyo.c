#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_iki_adult_soto[] = {
    CS_HEADER(2, 164),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 135),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 79, 59, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 78, 60, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 32, 78, 60, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 30, 79, 59, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 30, 78, 60, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 30, 78, 60, 0x0207),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 30, 78, 60, 0x0000),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 164),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 11, 50, 23, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 11, 48, 22, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA7, 60.324856f), 11, 48, 22, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA7, 60.324856f), 57, 101, 21, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 101, 21, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 101, 21, 0x0207),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 101, 21, 0x0000),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_kaeri_adult_soto[] = {
    CS_HEADER(5, 1167),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1138),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA1BC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA5CB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA1BC),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 31, 82, 61, 0xA5E1),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 55, 99, 31, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 55, 99, 31, 0xA1BC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42714CA8, 60.32486f), 55, 99, 31, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA8, 60.32486f), 17, 59, 31, 0xA5CB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 17, 59, 31, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42714CA8, 60.32486f), 17, 59, 31, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 17, 59, 31, 0xA1BC),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 17, 59, 31, 0xA5E1),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_OUT, 36, 46),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 30, 35),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 95, 97, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFE3, 0xFFFFFFF9, 0x00000000, 0xFFFFFFE3, 0xFFFFFFF9, 0x00000000, 0x00000000, 0x00000000),
    CS_END_OF_SCRIPT(),
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_iki_adult_naka[] = {
    CS_HEADER(2, 118),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 89),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428EA647, 71.32476f), 53, 53, 40, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 53, 53, 40, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 53, 53, 40, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 102, 48, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 102, 48, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 102, 48, 0x0207),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 102, 48, 0x0047),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 118),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D0CAF, 70.52477f), 11, 29, 10, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428CCCC2, 70.39992f), 11, 29, 10, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D3328, 70.599915f), 11, 29, 10, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D3328, 70.599915f), 29, 71, 25, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 71, 25, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 71, 25, 0x0207),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 71, 25, 0x0047),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_kaeri_adult_naka[] = {
    CS_HEADER(5, 1120),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_OUT, 36, 46),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 30, 35),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1091),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 42, 89, 50, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 42, 89, 50, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 42, 89, 50, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 42, 89, 50, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 42, 89, 50, 0x29D0),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 24, 66, 29, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 24, 66, 29, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42714CA8, 60.32486f), 24, 66, 29, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 24, 66, 29, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 24, 66, 29, 0x29D0),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 95, 96, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFEE, 0xFFFFFFF3, 0x00000000, 0xFFFFFFEE, 0xFFFFFFF3, 0x00000000, 0x00000000, 0x00000000),
    CS_END_OF_SCRIPT(),
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_iki_child_soto[] = {
    CS_HEADER(2, 1164),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1135),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 68, 59, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 32, 68, 60, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 69, 59, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 64, 59, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 64, 59, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 64, 59, 0x0207),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 64, 59, 0xB46C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CA7, 60.324856f), 31, 64, 59, 0x05BC),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1164),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 12, 40, 22, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 11, 38, 22, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA7, 60.324856f), 11, 39, 22, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA7, 60.324856f), 57, 86, 21, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 86, 21, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42714CA7, 60.324856f), 57, 86, 21, 0x0207),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 86, 21, 0xB46C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CA7, 60.324856f), 57, 86, 21, 0x05BC),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_kaeri_child_soto[] = {
    CS_HEADER(5, 1167),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_OUT, 36, 46),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 30, 35),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1138),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 30, 63, 61, 0xA8A5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 30, 63, 61, 0xA3D9),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA8, 60.32486f), 30, 63, 61, 0xF3A5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA9, 60.324863f), 29, 61, 59, 0xA5D5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA9, 60.324863f), 29, 61, 59, 0xA5E7),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA9, 60.324863f), 29, 61, 59, 0xA5EC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CA9, 60.324863f), 29, 61, 59, 0xBCA5),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CA9, 60.324863f), 29, 61, 59, 0xEEC0),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 54, 79, 31, 0xA8A5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA8, 60.32486f), 54, 80, 31, 0xA3D9),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42714CA8, 60.32486f), 54, 79, 31, 0xF3A5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x42714CA9, 60.324863f), 15, 42, 30, 0xA5D5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA9, 60.324863f), 15, 42, 30, 0xA5E7),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42714CA9, 60.324863f), 15, 42, 30, 0xA5EC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CA9, 60.324863f), 15, 42, 30, 0xBCA5),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CA9, 60.324863f), 15, 42, 30, 0xEEC0),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 95, 96, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFEF, 0xFFFFFFCD, 0x00000000, 0xFFFFFFEF, 0xFFFFFFCD, 0x00000000, 0x00000000, 0x00000000),
    CS_END_OF_SCRIPT(),
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_iki_child_naka[] = {
    CS_HEADER(2, 1118),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1089),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428EA647, 71.32476f), 53, 53, 40, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 53, 53, 40, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 53, 53, 40, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 89, 47, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 89, 47, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 89, 47, 0x0207),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 89, 47, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x428D3328, 70.599915f), 58, 89, 47, 0x0000),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1118),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D0CAF, 70.52477f), 11, 29, 10, 0x010F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428CCCC2, 70.39992f), 11, 29, 10, 0x0120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D3328, 70.599915f), 11, 29, 10, 0x0131),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 7, CS_FLOAT(0x428D3328, 70.599915f), 29, 58, 25, 0x01F4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 58, 25, 0x01F6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x428D3328, 70.599915f), 29, 58, 25, 0x0207),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 58, 25, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x428D3328, 70.599915f), 29, 58, 25, 0x0000),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on
#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData okarina_kaeri_child_naka[] = {
    CS_HEADER(5, 1120),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_OUT, 36, 46),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 30, 35),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(0, 1091),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CAA, 60.324867f), 41, 75, 49, 0x1F1C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CAA, 60.324867f), 41, 75, 49, 0x1F8C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CAA, 60.324867f), 41, 75, 49, 0x1FFC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42714CAA, 60.324867f), 41, 75, 49, 0x206C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42714CAA, 60.324867f), 41, 75, 49, 0x20DC),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(0, 1120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CAA, 60.324867f), 24, 52, 29, 0x1F1C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CAA, 60.324867f), 24, 52, 29, 0x1F8C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42714CAA, 60.324867f), 24, 52, 29, 0x1FFC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42714CAA, 60.324867f), 24, 52, 29, 0x206C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42714CAA, 60.324867f), 24, 52, 29, 0x20DC),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 95, 96, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFDF, 0x00000019, 0x00000000, 0xFFFFFFDF, 0x00000019, 0x00000000, 0x00000000, 0x00000000),
    CS_END_OF_SCRIPT(),
};
// clang-format on
#include "z_demo_kankyo.h"
#include "global.h"
#include "versions.h"
#include "z64cutscene_commands.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_efc_star_field/object_efc_star_field.h"
#include "assets/objects/object_toki_objects/object_toki_objects.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:128"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Kankyo_actor_ct(Actor* thisx, PlayState* play);
void Demo_Kankyo_actor_dt(Actor* thisx, PlayState* play);
void Demo_Kankyo_actor_move(Actor* thisx, PlayState* play);
void Demo_Kankyo_actor_draw(Actor* thisx, PlayState* play);

static void move_wait(DemoKankyo* this, PlayState* play);
void cloud_move(DemoKankyo* this, PlayState* play);
void rock_move(DemoKankyo* this, PlayState* play);
void move_oka16(DemoKankyo* this, PlayState* play);
void tobira_move(DemoKankyo* this, PlayState* play);
void move_nonproc(DemoKankyo* this, PlayState* play);
void tobira_move2(DemoKankyo* this, PlayState* play);

void Demo_Starfield_draw(Actor* thisx, PlayState* play);
void Demo_Rock_draw(Actor* thisx, PlayState* play);
void Demo_cloud_draw(Actor* thisx, PlayState* play);
void Demo_tokidoor_draw(Actor* thisx, PlayState* play);
void Demo_madopika_draw(Actor* thisx, PlayState* play);
void Demo_okawarp_draw(Actor* thisx, PlayState* play);
void Demo_Takara_draw(Actor* thisx, PlayState* play);

// adult warp songs cutscenes
extern CutsceneData okarina_iki_adult_soto[];
extern CutsceneData okarina_kaeri_adult_soto[];
// adult warp songs cutscenes in temple of time
extern CutsceneData okarina_iki_adult_naka[];
extern CutsceneData okarina_kaeri_adult_naka[];
// child warp songs cutscenes
extern CutsceneData okarina_iki_child_soto[];
extern CutsceneData okarina_kaeri_child_soto[];
// child warp songs cutscenes in temple of time
extern CutsceneData okarina_iki_child_naka[];
extern CutsceneData okarina_kaeri_child_naka[];

ActorProfile Demo_Kankyo_Profile = {
    /**/ ACTOR_DEMO_KANKYO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(DemoKankyo),
    /**/ Demo_Kankyo_actor_ct,
    /**/ Demo_Kankyo_actor_dt,
    /**/ Demo_Kankyo_actor_move,
    /**/ Demo_Kankyo_actor_draw,
};

static s16 demo_kankyo_shape_data[] = {
    OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD,
    OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_GAMEPLAY_KEEP,  OBJECT_GI_MELODY,      OBJECT_GI_MELODY,
    OBJECT_GI_MELODY,      OBJECT_GI_MELODY,      OBJECT_GI_MELODY,      OBJECT_TOKI_OBJECTS,   OBJECT_TOKI_OBJECTS,
    OBJECT_GAMEPLAY_KEEP,  OBJECT_GAMEPLAY_KEEP,  OBJECT_GAMEPLAY_KEEP,
};

// unused, presumed to be floats
static f32 topmode[] = {
    0.0f,
    150.0f,
};

static s16 SFIELDSPEED;
static s16 SFIELDSCALE;
static s16 end_timer;

void Demo_Kankyo_actor_set_process(DemoKankyo* this, DemoKankyoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Demo_Kankyo_actor_ct(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, demo_kankyo_shape_data[this->actor.params]);

    PRINTF("bank_ID = %d\n", objectSlot);
    if (objectSlot < 0) {
        ASSERT(0, "0", "../z_demo_kankyo.c", 521);
    } else {
        this->requiredObjectSlot = objectSlot;
    }

    switch (this->actor.params) {
        case DEMOKANKYO_BLUE_RAIN:
        case DEMOKANKYO_BLUE_RAIN_2:
            switch (play->sceneId) {
                case SCENE_CUTSCENE_MAP:
                    play->roomCtx.curRoom.segment = NULL;
                    SFIELDSPEED = 10;
                    SFIELDSCALE = 8;
                    break;
                case SCENE_TEMPLE_OF_TIME:
                    SFIELDSPEED = 14;
                    SFIELDSCALE = 8;
                    break;
                case SCENE_HYRULE_FIELD:
                    SFIELDSPEED = 1;
                    SFIELDSCALE = 5;
                    break;
                default:
                    Actor_delete(&this->actor);
                    break;
            }
            break;
        case DEMOKANKYO_ROCK_1:
        case DEMOKANKYO_ROCK_2:
        case DEMOKANKYO_ROCK_3:
        case DEMOKANKYO_ROCK_4:
        case DEMOKANKYO_ROCK_5:
            play->roomCtx.curRoom.segment = NULL;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = fqrand() * 0.5f + 0.5f;
            this->unk_150[0].unk_0.x = fqrand() * 3.0f + 1.0f;
            this->unk_150[0].unk_0.y = fqrand() * 3.0f + 1.0f;
            this->unk_150[0].unk_0.z = fqrand() * 3.0f + 1.0f;
            break;
        case DEMOKANKYO_CLOUDS:
            for (i = 0; i < 30; i++) {
                this->unk_150[i].unk_20 = fqrand() * 65535.0f;
                this->unk_150[i].unk_18 = fqrand() * 100.0f + 60.0f;
            }
            break;
        case DEMOKANKYO_DOOR_OF_TIME:
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            this->unk_150[0].unk_18 = 0.0f;
            if (!GET_EVENTCHKINF(EVENTCHKINF_OPENED_DOOR_OF_TIME)) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_TOKI, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x0000);
            } else {
                play->roomCtx.drawParams[1] = 0xFF;
                Actor_delete(&this->actor);
            }
            break;
        case DEMOKANKYO_LIGHT_PLANE:
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            this->unk_150[0].unk_18 = 0.0f;
            break;
        case DEMOKANKYO_WARP_OUT:
        case DEMOKANKYO_WARP_IN:
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ITEMACTION);
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
            this->actor.room = -1;
            this->warpTimer = 35;
            this->sparkleCounter = 0;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                Nai_FxFlagEntry(NA_SE_EV_SARIA_MELODY, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            break;
        case DEMOKANKYO_SPARKLES:
            this->warpTimer = 35;
            this->sparkleCounter = 0;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            break;
        default:
            break;
    }
    for (i = 0; i < 30; i++) {
        this->unk_150[i].unk_22 = 0;
    }
    Demo_Kankyo_actor_set_process(this, move_wait);
}

void Demo_Kankyo_actor_dt(Actor* thisx, PlayState* play) {
    if (thisx) {}
}

static void move_wait(DemoKankyo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.objectSlot == this->requiredObjectSlot) {
        switch (this->actor.params) {
            case DEMOKANKYO_ROCK_1:
            case DEMOKANKYO_ROCK_2:
            case DEMOKANKYO_ROCK_3:
            case DEMOKANKYO_ROCK_4:
            case DEMOKANKYO_ROCK_5:
                Demo_Kankyo_actor_set_process(this, rock_move);
                break;
            case DEMOKANKYO_CLOUDS:
                Demo_Kankyo_actor_set_process(this, cloud_move);
                break;
            case DEMOKANKYO_DOOR_OF_TIME:
                if (eventbit_check(play, 2)) {
                    Demo_Kankyo_actor_set_process(this, tobira_move);
                }
                break;
            case DEMOKANKYO_WARP_OUT:
                play->envCtx.screenFillColor[0] = 0xFF;
                play->envCtx.screenFillColor[1] = 0xFF;
                play->envCtx.screenFillColor[2] = 0xFF;
                play->envCtx.fillScreen = false;
                if (this->warpTimer < 21 && this->warpTimer >= 15) {
                    f32 temp = (this->warpTimer - 15.0f) / 5.0f;

                    play->envCtx.fillScreen = true;
                    play->envCtx.screenFillColor[3] = 255 - 255 * temp;
                }
                if (this->warpTimer < 15 && this->warpTimer >= 4) {
                    f32 temp = (this->warpTimer - 4.0f) / 10.0f;

                    play->envCtx.fillScreen = true;
                    play->envCtx.screenFillColor[3] = 255 * temp;
                }
                if (this->warpTimer == 15) {
                    player->actor.draw = NULL;
                }
                if ((u32)this->warpTimer != 0) {
                    this->warpTimer--;
                }
                if (this->warpTimer == 1) {
                    if (play->sceneId == SCENE_TEMPLE_OF_TIME) {
                        end_timer = 25;
                        if (!LINK_IS_ADULT) {
                            play->csCtx.script = okarina_iki_child_naka;
                        } else {
                            play->csCtx.script = okarina_iki_adult_naka;
                        }
                    } else {
                        end_timer = 32;
                        if (!LINK_IS_ADULT) {
                            play->csCtx.script = okarina_iki_child_soto;
                        } else {
                            play->csCtx.script = okarina_iki_adult_soto;
                        }
                    }
                    if (Game_play_change_camera_check(play)) {
                        z_common_data.cutsceneTrigger = 1;
                    }
                    Demo_Kankyo_actor_set_process(this, move_nonproc);
                }
                break;
            case DEMOKANKYO_WARP_IN:
                if (play->sceneId == SCENE_TEMPLE_OF_TIME) {
                    if (!LINK_IS_ADULT) {
                        play->csCtx.script = okarina_kaeri_child_naka;
                    } else {
                        play->csCtx.script = okarina_kaeri_adult_naka;
                    }
                } else {
                    if (!LINK_IS_ADULT) {
                        play->csCtx.script = okarina_kaeri_child_soto;
                    } else {
                        play->csCtx.script = okarina_kaeri_adult_soto;
                    }
                }
                z_common_data.cutsceneTrigger = 1;
                Demo_Kankyo_actor_set_process(this, move_oka16);
                break;
            case DEMOKANKYO_BLUE_RAIN:
            case DEMOKANKYO_SPARKLES:
                break;
        }
    }
}

void move_nonproc(DemoKankyo* this, PlayState* play) {
}

void move_oka16(DemoKankyo* this, PlayState* play) {
#if OOT_VERSION < PAL_1_0
    Nai_FxFlagEntry(NA_SE_EV_LINK_WARP_OUT, &_dummy_zero_f, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
#endif
    Demo_Kankyo_actor_set_process(this, move_nonproc);
}

static void DemoEffMoveProc_local(DemoKankyo* this, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];
    f32 lerp;

    startPos.x = cue->startPos.x;
    startPos.y = cue->startPos.y;
    startPos.z = cue->startPos.z;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    lerp = get_parcent(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    this->actor.world.pos.x = ((endPos.x - startPos.x) * lerp) + startPos.x;
    this->actor.world.pos.y = ((endPos.y - startPos.y) * lerp) + startPos.y;
    this->actor.world.pos.z = ((endPos.z - startPos.z) * lerp) + startPos.z;
}

void rock_move(DemoKankyo* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[this->actor.params - DEMOKANKYO_ROCK_1] != NULL) {
        DemoEffMoveProc_local(this, play, this->actor.params - DEMOKANKYO_ROCK_1);
    }

    this->unk_150[0].unk_C.x += this->unk_150[0].unk_0.x;
    this->unk_150[0].unk_C.y += this->unk_150[0].unk_0.y;
    this->unk_150[0].unk_C.z += this->unk_150[0].unk_0.z;
}

void cloud_move(DemoKankyo* this, PlayState* play) {
    u8 i;

    for (i = 0; i < 30; i++) {
        this->unk_150[i].unk_20 += (s16)this->unk_150[i].unk_18;
    }
}

void tobira_move(DemoKankyo* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    this->unk_150[0].unk_18 += 1.0f;
    if (this->unk_150[0].unk_18 >= 102.0f) {
        Actor_SE_set(&this->actor, NA_SE_EV_STONEDOOR_STOP);
        SET_EVENTCHKINF(EVENTCHKINF_OPENED_DOOR_OF_TIME);
        Actor_delete(this->actor.child);
        Demo_Kankyo_actor_set_process(this, tobira_move2);
    }
}

void tobira_move2(DemoKankyo* this, PlayState* play) {
    Actor_delete(this->actor.child);
}

void Demo_Kankyo_actor_move(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    this->actionFunc(this, play);
}

void Demo_Kankyo_actor_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    if (this->actor.objectSlot == this->requiredObjectSlot) {
        switch (this->actor.params) {
            case DEMOKANKYO_BLUE_RAIN:
            case DEMOKANKYO_BLUE_RAIN_2:
                if (play->sceneId == SCENE_TEMPLE_OF_TIME) {
                    if (!eventbit_check(play, 1)) {
                        break;
                    } else if (!Actor_player_distance_direction_check(&this->actor, 300.0f, 0x7530)) {
                        break;
                    } else {
                        if (!LINK_IS_ADULT) {
                            if (play->csCtx.curFrame < 170 || play->csCtx.state == CS_STATE_IDLE) {
                                break;
                            }
                        } else {
                            if (play->csCtx.curFrame < 120 || play->csCtx.state == CS_STATE_IDLE) {
                                break;
                            }
                        }
                    }
                }
                Demo_Starfield_draw(thisx, play);
                break;
            case DEMOKANKYO_ROCK_1:
            case DEMOKANKYO_ROCK_2:
            case DEMOKANKYO_ROCK_3:
            case DEMOKANKYO_ROCK_4:
            case DEMOKANKYO_ROCK_5:
                Demo_Rock_draw(thisx, play);
                break;
            case DEMOKANKYO_CLOUDS:
                Demo_cloud_draw(thisx, play);
                break;
            case DEMOKANKYO_DOOR_OF_TIME:
                Demo_tokidoor_draw(thisx, play);
                break;
            case DEMOKANKYO_LIGHT_PLANE:
                Demo_madopika_draw(thisx, play);
                break;
            case DEMOKANKYO_WARP_OUT:
            case DEMOKANKYO_WARP_IN:
                Demo_okawarp_draw(thisx, play);
                break;
            case DEMOKANKYO_SPARKLES:
                Demo_Takara_draw(thisx, play);
                break;
        }
    }
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
    }
}

// transform relating to blue rain
void Demo_Starfield_init(Actor* thisx, PlayState* play, s16 i) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    switch (play->sceneId) {
        case SCENE_CUTSCENE_MAP:
            this->unk_150[i].unk_0.x = (fqrand() - 0.5f) * 500.0f;
            this->unk_150[i].unk_0.y = 500.0f;
            this->unk_150[i].unk_0.z = (fqrand() - 0.5f) * 500.0f;
            break;
        case SCENE_TEMPLE_OF_TIME:
            this->unk_150[i].unk_C.x = 0.0f;
            this->unk_150[i].unk_C.y = 0.0f;
            this->unk_150[i].unk_C.z = 0.0f;
            this->unk_150[i].unk_0.x = (fqrand() - 0.5f) * 180.0f;
            this->unk_150[i].unk_0.y = 10.0f;
            this->unk_150[i].unk_0.z = (fqrand() - 0.5f) * 180.0f;
            break;
        case SCENE_HYRULE_FIELD:
            this->unk_150[i].unk_0.x = (fqrand() - 0.5f) * 600.0f;
            this->unk_150[i].unk_0.y = -500.0f;
            this->unk_150[i].unk_0.z = (fqrand() - 0.5f) * 600.0f;
            break;
    }
    this->unk_150[i].unk_18 = fqrand() * (SFIELDSPEED * 4.0f) + SFIELDSPEED;
}

void Demo_Starfield_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    f32 temp_f12_2;
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 norm;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    s16 j;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1186);

    for (i = 0; i < 30; i++) {
        s32 pad[2];

        dx = play->view.at.x - play->view.eye.x;
        dy = play->view.at.y - play->view.eye.y;
        dz = play->view.at.z - play->view.eye.z;
        norm = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

        if (play->sceneId != SCENE_TEMPLE_OF_TIME) {
            this->unk_150[i].unk_C.x = play->view.eye.x + (dx / norm) * 350.0f;
            this->unk_150[i].unk_C.y = play->view.eye.y + (dy / norm) * 80.0f;
            this->unk_150[i].unk_C.z = play->view.eye.z + (dz / norm) * 350.0f;
        }

        switch (this->unk_150[i].unk_22) {
            case 0:
                Demo_Starfield_init(thisx, play, i);
                if (z_common_data.save.entranceIndex == ENTR_CUTSCENE_MAP_0) {
                    this->unk_150[i].unk_0.y = fqrand() * 500.0f;
                } else {
                    this->unk_150[i].unk_0.y = fqrand() * -500.0f;
                }
                this->unk_150[i].unk_22++;
                break;
            case 1:
                temp_f12_2 = play->view.eye.y + (dy / norm) * 150.0f;
                if (z_common_data.save.entranceIndex == ENTR_CUTSCENE_MAP_0) {
                    this->unk_150[i].unk_0.y -= this->unk_150[i].unk_18;
                } else {
                    this->unk_150[i].unk_0.y += this->unk_150[i].unk_18;
                }
                if (z_common_data.save.entranceIndex == ENTR_CUTSCENE_MAP_0) {
                    if (this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y < temp_f12_2 - 300.0f) {
                        this->unk_150[i].unk_22++;
                    }
                } else if (z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_0) {
                    if (temp_f12_2 + 300.0f < this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y) {
                        this->unk_150[i].unk_22++;
                    }
                } else {
                    if (1000.0f < this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y) {
                        this->unk_150[i].unk_22++;
                    }
                }
                break;
            case 2:
                Demo_Starfield_init(thisx, play, i);
                this->unk_150[i].unk_22--;
                break;
        }

        Matrix_translate(this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x,
                         this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y,
                         this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z, MTXMODE_NEW);

        if (z_common_data.save.entranceIndex != ENTR_CUTSCENE_MAP_0) {
            Matrix_rotateX(M_PI, MTXMODE_APPLY);
        }

        gDPPipeSync(POLY_XLU_DISP++);

        if (z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_0) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 255);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 255);
        }

        Matrix_scale(SFIELDSCALE * 0.001f, SFIELDSCALE * 0.001f, SFIELDSCALE * 0.001f, MTXMODE_APPLY);

        for (j = 0; j < 5; j++) {
            s32 pad1;

            if (play->sceneId != SCENE_TEMPLE_OF_TIME) {
                if (this->unk_150[i].unk_0.x >= 0.0f) {
                    translateX = -j * 1500.0f;
                } else {
                    translateX = j * 1500.0f;
                }
                if (this->unk_150[i].unk_0.z >= 0.0f) {
                    translateZ = -j * 1500.0f;
                } else {
                    translateZ = j * 1500.0f;
                }
                if (j % 2 != 0) {
                    translateY = j * 4000.0f;
                } else {
                    translateY = -j * 4000.0f;
                }
            } else {
                translateX = 0.0f;
                translateY = j * 10.0f;
                translateZ = 0.0f;
            }

            Matrix_translate(translateX, translateY, translateZ, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1344);
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_star_field_DL_000080);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1358);
}

void Demo_Rock_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1376);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateX(DEG_TO_RAD(this->unk_150[0].unk_C.x), MTXMODE_APPLY);
    Matrix_rotateY(DEG_TO_RAD(this->unk_150[0].unk_C.y), MTXMODE_APPLY);
    Matrix_rotateZ(DEG_TO_RAD(this->unk_150[0].unk_C.z), MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 55, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 155, 255, 55, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1404);
    gSPDisplayList(POLY_OPA_DISP++, object_efc_star_field_DL_000DE0);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1409);
}

void Demo_cloud_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    s32 pad;
    f32 dx;
    f32 dy;
    f32 dz;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1425);

    for (i = 0; i < 30; i++) {
        dx = -(sin_s(this->unk_150[i].unk_20 - 0x8000) * 120.0f) * (30.0f + (i / 30.0f) * 10.0f);
        dy = cos_s(this->unk_150[i].unk_20 - 0x8000) * 5.0f + 1200.0f;
        dz = (cos_s(this->unk_150[i].unk_20 - 0x8000) * 120.0f) * (30.0f + (i / 30.0f) * 10.0f);

        Matrix_translate(play->view.eye.x + dx, play->view.eye.y + dy + ((i - 12.0f) * 300.0f), play->view.eye.z + dz,
                         MTXMODE_NEW);
        Matrix_scale(125.0f, 60.0f, 125.0f, MTXMODE_APPLY);

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 210, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
        gDPSetColorDither(POLY_XLU_DISP++, G_AD_NOTPATTERN | G_CD_MAGICSQ);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1461);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gDust5Tex));

        texture_z_cld_poly_xlu_nd(play->state.gfxCtx);

        gSPMatrix(POLY_XLU_DISP++, &D_01000000, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffDustDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1474);
}

void Demo_tokidoor_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1487);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_translate(-this->unk_150[0].unk_18, 0.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1492);
    gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007440);
    Matrix_translate(this->unk_150[0].unk_18 + this->unk_150[0].unk_18, 0.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1497);
    gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007578);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1501);
}

void Demo_madopika_draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1514);

    if (play->csCtx.state == CS_STATE_IDLE || IS_CUTSCENE_LAYER) {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, play->state.frames & 0x7F, 64, 32));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 1529);
        gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_008390);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1534);
}

static void fv2xyz(Vec3f* src, Vec3f* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

static Vec3f translate_by_sglobe(Vec3f* a, VecGeo* geo) {
    Vec3f sum;
    Vec3f b = sglobe2world(geo);

    sum.x = a->x + b.x;
    sum.y = a->y + b.y;
    sum.z = a->z + b.z;

    return sum;
}

static void fvrel2xyz(PosRot* posRot, Vec3f* vec, Vec3f* dst) {
    VecGeo geo;
    Vec3f vecCopy;

    fv2xyz(vec, &vecCopy);
    geo = world2sglobe(&vecCopy);
    geo.yaw += posRot->rot.y;
    *dst = translate_by_sglobe(&posRot->pos, &geo);
}

void Demo_okawarp_draw(Actor* thisx, PlayState* play) {
    static Color_RGB8 tamacol[] = {
        { 0, 200, 0 },    // minuet
        { 255, 50, 0 },   // bolero
        { 0, 150, 255 },  // serenade
        { 255, 150, 0 },  // requiem
        { 200, 50, 255 }, // nocturne
        { 200, 255, 0 },  // prelude
    };

    static CutsceneCameraPoint Position[] = {
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0x0000, 0x0000 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0017, 0x0024 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x001C, 0x0032, 0xFFFF } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0001, 0x0018, 0xFFD9 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0xFFFA, 0x0003 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0025, 0x0037 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x004F, 0x0066, 0x0029 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x00A6, 0x00AD, 0x0006 } },
        { CS_CAM_CONTINUE, 0, 5, 45.0f, { 0x010D, 0x015A, 0xFF4C } },
        { CS_CAM_CONTINUE, 0, 5, 45.0f, { 0x019F, 0x0245, 0xFE35 } },
        { CS_CAM_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
        { CS_CAM_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
    };

    static CutsceneCameraPoint Position2[] = {
        { CS_CAM_CONTINUE, 0, 5, 45.0f, { 0x019F, 0x0245, 0xFE35 } },
        { CS_CAM_CONTINUE, 0, 5, 45.0f, { 0x010D, 0x015A, 0xFF4C } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x00A6, 0x00AD, 0x0006 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x004F, 0x0066, 0x0029 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0025, 0x0037 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0xFFFA, 0x0003 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0001, 0x0018, 0xFFD9 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x001C, 0x0032, 0xFFFF } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0017, 0x0024 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0x0000, 0x0000 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
        { CS_CAM_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
        { CS_CAM_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
        { CS_CAM_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
    };

    static f32 z;
    static f32 fovy;
    // the following 2 vars are unused
    static u32 key;
    static u32 frame;
    static Vec3f pos0;

    PlayState* play2 = (PlayState*)play;
    s16 i;
    f32 temp_f22;
    DemoKankyo* this = (DemoKankyo*)thisx;
    Player* player = GET_PLAYER(play2);
    Vec3f camPos;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    PosRot posRot;
    u8 linkAge = z_common_data.save.linkAge;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 1824);

    if (this->sparkleCounter < 30) {
        this->sparkleCounter += 2;
    }
    for (i = this->sparkleCounter - 1; i >= 0; i--) {
        temp_f22 = 1.0f - (i / (f32)this->sparkleCounter);

        switch (this->unk_150[i].unk_22) {
            case 0:
                this->unk_150[i].unk_20 = 0;
                this->unk_150[i].unk_1C = 0;
                this->unk_150[i].unk_0.x = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_0.y = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_0.z = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_23 = 0;
                this->unk_150[i].unk_22++;
                FALLTHROUGH;
            case 1:
                if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                    if (Grou_Dospline(&camPos, &z, &fovy, Position, &this->unk_150[i].unk_20,
                                      &this->unk_150[i].unk_1C) != 0) {
                        this->unk_150[i].unk_22++;
                    }
                    if (play2->sceneId == SCENE_TEMPLE_OF_TIME && play2->csCtx.curFrame == 25) {
                        this->unk_150[i].unk_22++;
                    }
                } else {
#if OOT_VERSION >= PAL_1_0
                    Nai_FxFlagEntry(NA_SE_EV_LINK_WARP_OUT - SFX_FLAG, &_dummy_zero_f, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
#endif
                    if (Grou_Dospline(&camPos, &z, &fovy, Position2, &this->unk_150[i].unk_20,
                                      &this->unk_150[i].unk_1C) != 0) {
                        this->unk_150[i].unk_22++;
                    }
                    if (end_timer < play2->csCtx.curFrame && this->actor.params == DEMOKANKYO_WARP_OUT) {
                        this->unk_150[i].unk_22++;
                    }
                }
                posRot = Actor_get_world(&player->actor);
                fvrel2xyz(&posRot, &camPos, &pos0);
                break;
            case 2:
                if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                    if (i == 0) {
                        OcaWarpJumpProc(play2);
                        this->unk_150[i].unk_22++;
                    }
                } else if (i + 1 == this->sparkleCounter && play2->csCtx.state == CS_STATE_IDLE) {
                    total_event_timer_end(play2);
                    Actor_delete(&this->actor);
                }
                break;
        }

        this->unk_150[i].unk_C.x = pos0.x;
        this->unk_150[i].unk_C.y = pos0.y;
        this->unk_150[i].unk_C.z = pos0.z;

        switch (this->unk_150[i].unk_23) {
            case 0:
                this->unk_150[i].unk_18 = fqrand();
                this->unk_150[i].unk_23++;
                FALLTHROUGH;
            case 1:
                add_calc(&this->unk_150[i].unk_18, 1.0f, 0.5f, 0.4f, 0.2f);
                if (this->unk_150[i].unk_18 >= 1.0f) {
                    this->unk_150[i].unk_23 = 2;
                }
                break;
            case 2:
                add_calc(&this->unk_150[i].unk_18, 0.0f, 0.5f, 0.3f, 0.2f);
                if (this->unk_150[i].unk_18 <= 0.0f) {
                    this->unk_150[i].unk_0.x = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_0.y = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_0.z = (s16)((fqrand() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_18 = 0.0f;
                    this->unk_150[i].unk_23 = 1;
                }
                break;
        }

        translateX = this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x;
        translateY = this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y;
        translateZ = this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z;

        if (this->unk_150[i].unk_22 < 2) {
            if (linkAge != LINK_AGE_ADULT) {
                Matrix_translate(translateX, translateY, translateZ, MTXMODE_NEW);
            } else {
                if (translateY) {}
                Matrix_translate(translateX, translateY + 15.0f, translateZ, MTXMODE_NEW);
            }
            Matrix_scale(this->unk_150[i].unk_18 * (0.018f * temp_f22), this->unk_150[i].unk_18 * (0.018f * temp_f22),
                         this->unk_150[i].unk_18 * (0.018f * temp_f22), MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                gDPSetEnvColor(POLY_XLU_DISP++, tamacol[play2->msgCtx.lastPlayedSong].r,
                               tamacol[play2->msgCtx.lastPlayedSong].g,
                               tamacol[play2->msgCtx.lastPlayedSong].b, 255);
            } else {
                s8 respawnData = z_common_data.respawn[RESPAWN_MODE_RETURN].data;

                gDPSetEnvColor(POLY_XLU_DISP++, tamacol[respawnData].r,
                               tamacol[respawnData].g, tamacol[respawnData].b, 255);
            }
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            Matrix_mult(&play2->billboardMtxF, MTXMODE_APPLY);
            Matrix_rotateZ(DEG_TO_RAD(this->unk_150[i].unk_24), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 2011);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
            this->unk_150[i].unk_24 += 0x190;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 2019);
}

void Demo_Takara_draw(Actor* thisx, PlayState* play) {
    static Color_RGB8 tamacol[] = {
        { 0, 200, 0 },   { 255, 50, 0 },  { 0, 150, 255 }, { 255, 150, 0 }, // only this one is used
        { 0, 255, 255 }, { 200, 255, 0 },
    };

    static CutsceneCameraPoint Position[] = {
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0000, 0xFFE9 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD3, 0x0000, 0x000A } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFE8, 0x0001, 0x0027 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0015, 0x0000, 0x002B } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002F, 0x0005, 0x000E } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0031, 0x0005, 0xFFF5 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0020, 0x0005, 0xFFDA } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF5, 0x0005, 0xFFD1 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0006, 0xFFEA } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD5, 0x0009, 0x000D } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFE9, 0x0009, 0x0027 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0014, 0x000B, 0x0029 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002D, 0x000B, 0x000F } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002E, 0x000B, 0xFFF0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x001E, 0x000B, 0xFFDA } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x000E, 0xFFD3 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFDA, 0x000E, 0xFFEB } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0010, 0x0008 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFE9, 0x0010, 0x0024 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0011, 0x0010, 0x0028 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002C, 0x0010, 0x000D } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002C, 0x0012, 0xFFF5 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x001F, 0x0011, 0xFFDE } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFFB, 0x0014, 0xFFD5 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFDD, 0x0014, 0xFFEC } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFDA, 0x0017, 0x0008 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFE8, 0x0014, 0x001F } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x000C, 0x0018, 0x0026 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0027, 0x0018, 0x000D } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0027, 0x001B, 0xFFF6 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x001C, 0x001A, 0xFFE2 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x000E, 0xFFD4 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD9, 0x001B, 0xFFEF } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x001B, 0x000A } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFE6, 0x001B, 0x0022 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x000F, 0x001F, 0x002C } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0032, 0x0020, 0x0009 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0030, 0x0021, 0xFFF0 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x001C, 0x0025, 0xFFD9 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x0028, 0xFFD4 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD8, 0x002B, 0xFFF5 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x002B, 0x0006 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFDF, 0x002B, 0x0019 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x000E, 0x002E, 0x002C } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x0032, 0x002E, 0x0003 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0x002A, 0x0030, 0xFFE7 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
        { CS_CAM_CONTINUE, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
        { CS_CAM_STOP, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
        { CS_CAM_STOP, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
    };

    static f32 z;
    static f32 fovy;
    // the following 3 vars are unused
    static u32 key;
    static u32 frame;
    static u32 D_8098CFB4;
    static Vec3f pos0;

    DemoKankyo* this = (DemoKankyo*)thisx;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    Vec3f camPos;
    f32 temp_f20;
    f32 scale;
    s16 i;
    PosRot posRot;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 2434);

    if (this->sparkleCounter < 20) {
        this->sparkleCounter++;
    }

    for (i = this->sparkleCounter - 1; i >= 0; i--) {
        temp_f20 = 1.0f - (i / (f32)this->sparkleCounter);

        switch (this->unk_150[i].unk_22) {
            case 0:
                this->unk_150[i].unk_20 = 0;
                this->unk_150[i].unk_1C = 0;
                this->unk_150[i].unk_0.x = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_0.y = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_0.z = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_23 = 0;
                this->unk_150[i].unk_22++;
                FALLTHROUGH;
            case 1:
                if (Grou_Dospline(&camPos, &z, &fovy, Position,
                                  &this->unk_150[i].unk_20, &this->unk_150[i].unk_1C) != 0) {
                    this->unk_150[i].unk_22++;
                }
                posRot = Actor_get_world(&this->actor);
                fvrel2xyz(&posRot, &camPos, &pos0);
                break;
            case 2:
                if (i + 1 == this->sparkleCounter && play->csCtx.state == CS_STATE_IDLE) {
                    Actor_delete(&this->actor);
                }
                break;
        }

        this->unk_150[i].unk_C.x = pos0.x;
        this->unk_150[i].unk_C.y = pos0.y;
        this->unk_150[i].unk_C.z = pos0.z;

        switch (this->unk_150[i].unk_23) {
            case 0:
                this->unk_150[i].unk_18 = fqrand();
                this->unk_150[i].unk_23++;
                FALLTHROUGH;
            case 1:
                add_calc(&this->unk_150[i].unk_18, 1.0f, 0.5f, 0.4f, 0.2f);
                if (1.0f <= this->unk_150[i].unk_18) {
                    this->unk_150[i].unk_23 = 2;
                }
                break;
            case 2:
                add_calc(&this->unk_150[i].unk_18, 0.0f, 0.5f, 0.3f, 0.2f);
                if (this->unk_150[i].unk_18 <= 0.0f) {
                    this->unk_150[i].unk_0.x = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_0.y = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_0.z = (s16)((fqrand() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_18 = 0.0f;
                    this->unk_150[i].unk_23 = 1;
                }
                break;
        }

        translateX = this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x;
        translateY = this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y;
        translateZ = this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z;

        if (this->unk_150[i].unk_22 < 2) {
            Matrix_translate(translateX, translateY, translateZ, MTXMODE_NEW);
            scale = this->unk_150[i].unk_18 * (0.02f * temp_f20);
            Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, tamacol[3].r, tamacol[3].g, tamacol[3].b,
                           255);
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_rotateZ(DEG_TO_RAD(this->unk_150[i].unk_24), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kankyo.c", 2572);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
            this->unk_150[i].unk_24 += 0x190;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kankyo.c", 2579);
}
