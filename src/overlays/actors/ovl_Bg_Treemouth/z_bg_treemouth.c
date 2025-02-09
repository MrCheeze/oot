#include "z_bg_treemouth.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData gDekuTreeMeetingCs[] = {
    CS_HEADER(12, 3000),
    CS_UNK_DATA_LIST(0x00000015, 1),
        CS_UNK_DATA(0x00010000, 0x0BB80000, 0x00000000, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x00000000, 0x00000000),
    CS_PLAYER_CUE_LIST(2),
        CS_PLAYER_CUE(PLAYER_CUEID_2, 0, 33, 0x54B2, 0x0000, 0x0000, 2614, 0, -451, 2808, 0, -559, CS_FLOAT(0x40BC1F08, 5.878788f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0xC0BC1F08, -5.878788f)),
        CS_PLAYER_CUE(PLAYER_CUEID_4, 33, 42, 0x5945, 0x0000, 0x0000, 2808, 0, -559, 2857, 0, -594, CS_FLOAT(0x40AE38E4, 5.4444447f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0xC0AE38E4, -5.4444447f)),
    CS_CAM_EYE_SPLINE(0, 1091),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x59A8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x20B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x6430),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x0000),
    CS_CAM_EYE_SPLINE(60, 1271),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x44B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2753, 46, -354, 0x8080),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2705, 67, -302, 0x2D9A),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2596, 127, -195, 0x005E),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2596, 127, -195, 0xFFFF),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2596, 127, -195, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2596, 127, -195, 0x5B80),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x423CCCC1, 47.199955f), 2596, 127, -195, 0x7805),
    CS_CAM_AT_SPLINE(0, 1120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2788, 23, -453, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x44B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x8080),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x2D9A),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x005E),
    CS_CAM_AT_SPLINE(60, 1300),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2788, 23, -453, 0x20BA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x44B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2787, 23, -453, 0x8080),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2777, 72, -378, 0x2D9A),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2673, 127, -267, 0x005E),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2673, 127, -267, 0xFFFF),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x423CCCC1, 47.199955f), 2673, 127, -267, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2673, 127, -267, 0x5B80),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x423CCCC1, 47.199955f), 2673, 127, -267, 0x7805),
    CS_TEXT_LIST(4),
        CS_TEXT_NONE(0, 40),
        CS_TEXT(0x107D, 40, 60, CS_TEXT_NORMAL, 0xFFFF, 0xFFFF),
        CS_TEXT_NONE(60, 160),
        CS_TEXT(0x1015, 160, 170, CS_TEXT_NORMAL, 0xFFFF, 0xFFFF),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 180, 200, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFF7, 0xFFFFFFDB, 0x00000000, 0xFFFFFFF7, 0xFFFFFFDB, 0x00000000, 0x00000000, 0x00000000),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_0_6, 1),
        CS_ACTOR_CUE(0x0001, 0, 3000, 0x0000, 0x0000, 0x0000, 0, -41, -28, 0, -41, -28, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_8_0, 4),
        CS_ACTOR_CUE(0x0001, 0, 1, 0x0000, 0x0000, 0x0000, 2668, 46, -490, 2668, 46, -490, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
        CS_ACTOR_CUE(0x0004, 1, 50, 0x5479, 0x0000, 0x0000, 2668, 46, -490, 2890, 43, -612, CS_FLOAT(0x4090FAC7, 4.5306125f), CS_FLOAT(0xBD7AC688, -0.06122449f), CS_FLOAT(0xC090FAC7, -4.5306125f)),
        CS_ACTOR_CUE(0x0004, 50, 100, 0x505C, 0x0000, 0x0000, 2890, 43, -612, 3109, 121, -705, CS_FLOAT(0x408C28F6, 4.38f), CS_FLOAT(0x3FC7AE14, 1.56f), CS_FLOAT(0xC08C28F6, -4.38f)),
        CS_ACTOR_CUE(0x0001, 100, 2084, 0x0000, 0x0000, 0x0000, 3109, 121, -705, 3109, 121, -705, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_START_SEQ_LIST(1),
        CS_START_SEQ(NA_BGM_DEKU_TREE_CS, 140, 141, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFBA, 0x00000094, 0x00000000, 0xFFFFFFBA, 0x00000094),
    CS_FADE_OUT_SEQ_LIST(1),
        CS_FADE_OUT_SEQ(CS_FADE_OUT_BGM_MAIN, 0, 20, 0x0000, 0x00000000, 0xFFFFFFA2, 0x00000000, 0x00000027, 0xFFFFFFA2, 0x00000000, 0x00000027),
    CS_END_OF_SCRIPT(),
};

CutsceneData gDekuTreeChoiceCs[] = {
    CS_HEADER(9, 3000),
    CS_UNK_DATA_LIST(0x00000015, 1),
        CS_UNK_DATA(0x00010000, 0x0BB80000, 0x00000000, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x00000000, 0x00000000),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 90, 172, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFE3, 0x00000012, 0x00000000, 0xFFFFFFE3, 0x00000012, 0x00000000, 0x00000000, 0x00000000),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_0_6, 1),
        CS_ACTOR_CUE(0x0001, 0, 3000, 0x0000, 0x0000, 0x0000, 0, -27, 26, 0, -27, 26, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_CAM_EYE_SPLINE(0, 1091),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 3740, -141, -530, 0x7065),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x6167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x6D5D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0xF348),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x9D94),
    CS_CAM_EYE_SPLINE_REL_TO_PLAYER(60, 1151),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -4, 5, 49, 0x7065),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -4, 5, 49, 0x6167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -4, 5, 49, 0x6D5D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -4, 5, 49, 0xF348),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -4, 5, 49, 0x9D94),
    CS_CAM_AT_SPLINE(0, 1120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3777, -89, -605, 0x7065),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3777, -89, -605, 0x6167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x4223FFED, 40.999928f), 3776, -89, -605, 0x6D5D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3776, -89, -605, 0xF348),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3776, -89, -604, 0x9D94),
    CS_CAM_AT_SPLINE_REL_TO_PLAYER(60, 1180),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 1, 35, -36, 0x7065),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 1, 35, -36, 0x6167),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x42700000, 60.0f), 1, 35, -36, 0x6D5D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 1, 35, -36, 0xF348),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 1, 35, -36, 0x9D94),
    CS_TEXT_LIST(2),
        CS_TEXT_NONE(0, 20),
        CS_TEXT(0x1016, 20, 80, CS_TEXT_NORMAL, 0xFFFF, 0xFFFF),
    CS_START_SEQ_LIST(1),
        CS_START_SEQ(NA_BGM_DEKU_TREE_CS, 0, 1, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFAC, 0x0000007E, 0x00000000, 0xFFFFFFAC, 0x0000007E),
    CS_END_OF_SCRIPT(),
};

CutsceneData gDekuTreeMouthOpeningCs[] = {
    CS_HEADER(8, 3000),
    CS_UNK_DATA_LIST(0x00000015, 1),
        CS_UNK_DATA(0x00010000, 0x0BB80000, 0x00000000, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x00000000, 0x00000000),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_0_6, 2),
        CS_ACTOR_CUE(0x0001, 0, 20, 0x0000, 0x0000, 0x0000, 42, 0, 77, 42, 0, 77, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
        CS_ACTOR_CUE(0x0003, 20, 357, 0x0000, 0x0000, 0x0000, 42, 0, 77, 42, 0, 77, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_CAM_EYE_SPLINE(0, 1151),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 3740, -141, -530, 0x00EA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x00FB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x010C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x0198),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x019A),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3740, -141, -530, 0x01AB),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x4223FFED, 40.999928f), 3739, -141, -530, 0x01BC),
    CS_CAM_AT_SPLINE(0, 1180),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3777, -89, -605, 0x00EA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3777, -89, -605, 0x00FB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3777, -89, -605, 0x010C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3763, -126, -621, 0x0198),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x4223FFED, 40.999928f), 3763, -126, -621, 0x019A),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3763, -126, -621, 0x01AB),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x4223FFED, 40.999928f), 3765, -118, -617, 0x01BC),
    CS_TEXT_LIST(2),
        CS_TEXT_NONE(0, 20),
        CS_TEXT(0x1017, 20, 60, CS_TEXT_NORMAL, 0xFFFF, 0xFFFF),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 100, 150, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFF5, 0x00000010, 0x00000000, 0xFFFFFFF5, 0x00000010, 0x00000000, 0x00000000, 0x00000000),
    CS_STOP_SEQ_LIST(1),
        CS_STOP_SEQ(NA_BGM_DEKU_TREE_CS, 90, 91, 0x0000, 0x00000000, 0xFFFFFFAC, 0x00000000, 0x00000034, 0xFFFFFFAC, 0x00000000, 0x00000034),
    CS_START_SEQ_LIST(1),
        CS_START_SEQ(NA_BGM_KOKIRI, 99, 100, 0x0000, 0x00000000, 0xFFFFFFD0, 0x00000000, 0x00000041, 0xFFFFFFD0, 0x00000000, 0x00000041),
    CS_END_OF_SCRIPT(),
};

CutsceneData gDekuTreeAskAgainCs[] = {
    CS_HEADER(8, 3000),
    CS_UNK_DATA_LIST(0x00000015, 1),
        CS_UNK_DATA(0x00010000, 0x0BB80000, 0x00000000, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x0000000F, 0xFFFFFFDE, 0x00000000, 0x00000000, 0x00000000),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_0_6, 1),
        CS_ACTOR_CUE(0x0001, 0, 119, 0x0000, 0x0000, 0x0000, 42, 0, 77, 42, 0, 77, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_CAM_EYE_SPLINE(0, 1091),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4297331E, 75.59984f), 3716, 790, -1171, 0x54EC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4297331E, 75.59984f), 3716, 790, -1171, 0x555C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4297331E, 75.59984f), 3716, 790, -1171, 0x55CC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x4297331E, 75.59984f), 3716, 790, -1171, 0x563C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x4297331E, 75.59984f), 3716, 790, -1171, 0x56AC),
    CS_CAM_AT_SPLINE(0, 1120),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4297331E, 75.59984f), 3769, 718, -1186, 0x54EC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4297331E, 75.59984f), 3769, 718, -1186, 0x555C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 1000, CS_FLOAT(0x4297331E, 75.59984f), 3769, 718, -1186, 0x55CC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x4297331E, 75.59984f), 3769, 718, -1186, 0x563C),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x4297331E, 75.59984f), 3769, 718, -1186, 0x56AC),
    CS_TEXT_LIST(2),
        CS_TEXT_NONE(0, 20),
        CS_TEXT(0x1018, 20, 60, CS_TEXT_NORMAL, 0xFFFF, 0xFFFF),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 80, 110, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFEA, 0x00000006, 0x00000000, 0xFFFFFFEA, 0x00000006, 0x00000000, 0x00000000, 0x00000000),
    CS_STOP_SEQ_LIST(1),
        CS_STOP_SEQ(NA_BGM_DEKU_TREE_CS, 70, 71, 0x0000, 0x00000000, 0xFFFFFFB7, 0x00000000, 0x00000044, 0xFFFFFFB7, 0x00000000, 0x00000044),
    CS_START_SEQ_LIST(1),
        CS_START_SEQ(NA_BGM_KOKIRI, 79, 80, 0x0000, 0x00000000, 0xFFFFFFE8, 0x00000000, 0x0000003A, 0xFFFFFFE8, 0x00000000, 0x0000003A),
    CS_END_OF_SCRIPT(),
};
// clang-format on

/*
 * File: z_bg_treemouth.c
 * Overlay: ovl_Bg_Treemouth
 * Description: Great Deku Tree's Mouth
 */

#include "z_bg_treemouth.h"
#include "versions.h"
#include "assets/objects/object_spot04_objects/object_spot04_objects.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgTreemouth_Init(Actor* thisx, PlayState* play);
void BgTreemouth_Destroy(Actor* thisx, PlayState* play);
void BgTreemouth_Update(Actor* thisx, PlayState* play);
void BgTreemouth_Draw(Actor* thisx, PlayState* play);

void func_808BC65C(BgTreemouth* this, PlayState* play);
void func_808BC6F8(BgTreemouth* this, PlayState* play);
void func_808BC80C(BgTreemouth* this, PlayState* play);
void func_808BC864(BgTreemouth* this, PlayState* play);
void BgTreemouth_DoNothing(BgTreemouth* this, PlayState* play);
void func_808BC8B8(BgTreemouth* this, PlayState* play);
void func_808BC9EC(BgTreemouth* this, PlayState* play);
void func_808BCAF0(BgTreemouth* this, PlayState* play);

extern CutsceneData gDekuTreeMeetingCs[];
extern CutsceneData gDekuTreeChoiceCs[];
extern CutsceneData gDekuTreeMouthOpeningCs[];
extern CutsceneData gDekuTreeAskAgainCs[];

ActorProfile Bg_Treemouth_Profile = {
    /**/ ACTOR_BG_TREEMOUTH,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT04_OBJECTS,
    /**/ sizeof(BgTreemouth),
    /**/ BgTreemouth_Init,
    /**/ BgTreemouth_Destroy,
    /**/ BgTreemouth_Update,
    /**/ BgTreemouth_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 8000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

// unused
static f32 D_808BD9C4[] = {
    -2746.0f, 545.0f, 4694.0f, -2654.0f, 146.0f, 4534.0f,
};

void BgTreemouth_SetupAction(BgTreemouth* this, BgTreemouthActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgTreemouth_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgTreemouth* this = (BgTreemouth*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gDekuTreeMouthCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Actor_SetFocus(thisx, 50.0f);

    if (!IS_CUTSCENE_LAYER && !LINK_IS_ADULT) {
        BgTreemouth_SetupAction(this, func_808BC8B8);
    } else if (LINK_IS_ADULT || (gSaveContext.sceneLayer == 7)) {
        this->unk_168 = 0.0f;
        BgTreemouth_SetupAction(this, BgTreemouth_DoNothing);
    } else {
        this->unk_168 = 1.0f;
        BgTreemouth_SetupAction(this, func_808BC6F8);
    }

    thisx->textId = 0x905;
}

void BgTreemouth_Destroy(Actor* thisx, PlayState* play) {
    BgTreemouth* this = (BgTreemouth*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808BC65C(BgTreemouth* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[0];
        if (cue != NULL) {
            if (cue->id == 2) {
                BgTreemouth_SetupAction(this, func_808BC80C);
            } else if (cue->id == 3) {
                Audio_PlaySfxGeneral(NA_SE_EV_WOODDOOR_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                BgTreemouth_SetupAction(this, func_808BC6F8);
            }
        }
    }
}

void func_808BC6F8(BgTreemouth* this, PlayState* play) {
    Vec3f sp34;

    if (this->unk_168 < 1.0f) {
        this->unk_168 += 0.01f;
    } else {
        this->unk_168 = 1.0f;
    }

    if ((gSaveContext.sceneLayer == 6) && (play->csCtx.curFrame > 700) && (play->state.frames % 8 == 0)) {
        sp34.x = (Rand_ZeroOne() * 1158.0f) + 3407.0f;
        sp34.y = 970.0f;
        sp34.z = (Rand_ZeroOne() * 2026.0f) + -2163.0f;
        EffectSsHahen_SpawnBurst(play, &sp34, 0.8f, 0, 50, 30, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
    }
}

void func_808BC80C(BgTreemouth* this, PlayState* play) {
    this->unk_168 += 0.05f;
    if (this->unk_168 >= 0.8f) {
        BgTreemouth_SetupAction(this, func_808BC864);
    }
}

void func_808BC864(BgTreemouth* this, PlayState* play) {
    this->unk_168 -= 0.03f;
    if (this->unk_168 <= 0.0f) {
        BgTreemouth_SetupAction(this, func_808BC65C);
    }
}

void func_808BC8B8(BgTreemouth* this, PlayState* play) {
    if (!Flags_GetEventChkInf(EVENTCHKINF_05) || LINK_IS_ADULT) {
        if (!LINK_IS_ADULT) {
            if (Flags_GetEventChkInf(EVENTCHKINF_0C)) {
                if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 1658.0f, 0x7530)) {
                    this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    if (this->dyna.actor.isLockedOn) {
                        this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                        play->csCtx.script = gDekuTreeChoiceCs;
                        gSaveContext.cutsceneTrigger = 1;
                        BgTreemouth_SetupAction(this, func_808BC9EC);
                    }
                }
            } else if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 1658.0f, 0x4E20)) {
                Flags_SetEventChkInf(EVENTCHKINF_0C);
                play->csCtx.script = gDekuTreeMeetingCs;
                gSaveContext.cutsceneTrigger = 1;
                BgTreemouth_SetupAction(this, func_808BC9EC);
            }
        }
    } else {
        this->unk_168 = 1.0f;
    }
}

void func_808BC9EC(BgTreemouth* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->csCtx.state == CS_STATE_STOP) {
        if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 350.0f, 0x7530)) {
            player->actor.world.pos.x = 3827.0f;
            player->actor.world.pos.y = -161.0f;
            player->actor.world.pos.z = -1142.0f;
        }

        play->csCtx.curFrame = 0;

        play->csCtx.camEyeSplinePointsAppliedFrame = CS_CAM_DATA_NOT_APPLIED;
        gCamAtSplinePointsAppliedFrame = CS_CAM_DATA_NOT_APPLIED;
        gCamEyePointAppliedFrame = CS_CAM_DATA_NOT_APPLIED;
        gCamAtPointAppliedFrame = CS_CAM_DATA_NOT_APPLIED;

        play->csCtx.camAtReady = false;
        play->csCtx.camEyeReady = false;

        play->csCtx.state = CS_STATE_RUN;

        if (play->msgCtx.choiceIndex == 0) {
            play->csCtx.script = gDekuTreeMouthOpeningCs;
            Flags_SetEventChkInf(EVENTCHKINF_05);
            BgTreemouth_SetupAction(this, func_808BCAF0);
        } else {
            play->csCtx.script = gDekuTreeAskAgainCs;
            play->csCtx.curFrame = 0;
            BgTreemouth_SetupAction(this, func_808BC8B8);
        }
    }
}

void func_808BCAF0(BgTreemouth* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[0];
        if (cue != NULL) {
            if (cue->id == 2) {
                BgTreemouth_SetupAction(this, func_808BC80C);
            } else if (cue->id == 3) {
                Audio_PlaySfxGeneral(NA_SE_EV_WOODDOOR_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                BgTreemouth_SetupAction(this, func_808BC6F8);
            }
        }
    }
}

void BgTreemouth_DoNothing(BgTreemouth* this, PlayState* play) {
}

void BgTreemouth_Update(Actor* thisx, PlayState* play) {
    BgTreemouth* this = (BgTreemouth*)thisx;
    f32 unk_168;

    this->actionFunc(this, play);
    unk_168 = this->unk_168;
    thisx->world.pos.x = (unk_168 * -160.0f) + 4029.0f;
    thisx->world.pos.y = (unk_168 * -399.0f) + 136.0f;
    thisx->world.pos.z = (unk_168 * 92.0f) + -1255.0f;
}

void BgTreemouth_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    u16 alpha = 500;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_treemouth.c", 893);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

#if OOT_VERSION < PAL_1_0
    if (!IS_CUTSCENE_LAYER)
#else
    if (!IS_CUTSCENE_LAYER || LINK_IS_ADULT)
#endif
    {
        if (GET_EVENTCHKINF(EVENTCHKINF_07)) {
            alpha = 2150;
        }
    } else { // needed to match
    }

    if (gSaveContext.sceneLayer == 6) {
        alpha = (play->roomCtx.drawParams[0] + 0x1F4);
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, alpha * 0.1f);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_treemouth.c", 932);
    gSPDisplayList(POLY_OPA_DISP++, gDekuTreeMouthDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_treemouth.c", 937);
}
