#include "z_bg_toki_swd.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData tokinoma_demo_swd0_data[] = {
    CS_HEADER(9, 425),
    CS_PLAYER_CUE_LIST(1),
        CS_PLAYER_CUE(PLAYER_CUEID_12, 0, 256, 0x0000, 0x0000, 0x0000, 0, 54, 52, 0, 54, 52, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f)),
    CS_LIGHT_SETTING_LIST(1),
        CS_LIGHT_SETTING(0x0001, 110, 111, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFE3, 0xFFFFFFC5, 0x00000000, 0xFFFFFFE3, 0xFFFFFFC5, 0x00000000, 0x00000000, 0x00000000),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_SET_CSFLAG_1, 110, 111, 0x0000, 0x00000000, 0x00000000, 0x0000002E, 0xFFFFFFE6, 0x00000000, 0x0000002E, 0xFFFFFFE6, 0x00000000, 0x00000000, 0x00000000),
    CS_DESTINATION(CS_DEST_TEMPLE_OF_TIME_FROM_MASTER_SWORD, 230, 231),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 210, 230),
    CS_CAM_EYE_SPLINE(0, 241),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x018C),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x019D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x01AE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x02A8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x007A),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x0064),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x0074),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -42, 72, -39, 0x005F),
    CS_CAM_EYE_SPLINE(80, 406),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 95, -19, 0x2F73),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 95, -19, 0x6B2F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 95, -19, 0x6169),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 95, -19, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 95, -19, 0x0005),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 129, -34, 0x7961),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 173, -19, 0x742F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -17, 217, -20, 0x2E64),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -24, 328, -12, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -27, 509, -2, 0x2F73),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -41, 813, 10, 0x6B2F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -60, 1285, 38, 0x6D61),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -58, 1910, 91, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -53, 2526, 133, 0x0005),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -33, 3445, 193, 0x7961),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 0, 3960, 225, 0x742F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 7, 4317, 245, 0x6565),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 7, 4316, 245, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 7, 4316, 245, 0x2F73),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 7, 4316, 245, 0x6B2F),
    CS_CAM_AT_SPLINE(0, 270),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x2F73),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x6B2F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x6169),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x0005),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x7961),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x742F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x2E64),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -20, 85, -9, 0x0000),
    CS_CAM_AT_SPLINE(80, 425),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 112, 7, 0x2F73),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 112, 7, 0x6B2F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 112, 7, 0x6169),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 112, 7, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -1, 112, 7, 0x0005),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x7961),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 25, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x742F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 25, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x2E64),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x2F73),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x6B2F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x6D61),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x0005),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x7961),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x742F),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x6565),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x2F73),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 20, CS_FLOAT(0x42700000, 60.0f), -1, 117, 7, 0x6B2F),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on

#include "z_bg_toki_swd.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData tokinoma_demo_swd1_data[] = {
    CS_HEADER(9, 368),
    CS_CAM_EYE_SPLINE(0, 126),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -1, 101, -110, 0x616D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 101, -110, 0x6964),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 101, -110, 0x00FB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 101, -110, 0x0111),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 101, -109, 0x012D),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 71, -110, 0x0033),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 72, -110, 0x00FB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 72, -110, 0x00FB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 72, -110, 0x2F68),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42200000, 40.0f), -1, 72, -110, 0x612F),
    CS_CAM_EYE_SPLINE(58, 339),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -9, 106, -3, 0xA1BC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -9, 106, -3, 0xA5F3),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -9, 106, -3, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -9, 106, -3, 0xA5A2),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -67, 108, -53, 0xBAEE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -67, 108, -53, 0xC9B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -67, 108, -53, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -67, 108, -53, 0xA5A4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -67, 108, -53, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -83, 102, -42, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -116, 81, -19, 0xA5D0),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -118, 81, 54, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -106, 78, 116, 0xA5A6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -27, 75, 226, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 121, 82, 312, 0xC0AE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 296, 76, 374, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 565, 80, 257, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 614, 80, -1, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 594, 80, -145, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 475, 80, -380, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 323, 80, -513, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), 192, 80, -574, 0xA5D0),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -45, 80, -604, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -142, 80, -589, 0xA5A6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -228, 80, -562, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -376, 93, -477, 0xC0AE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -584, 95, -183, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -613, 95, -34, 0x0020),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -595, 95, 161, 0xA5EB),
    CS_CAM_AT_SPLINE(0, 155),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x4221999A, 40.4f), -1, 111, 5, 0xA1BC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42200000, 40.0f), -1, 111, 5, 0xA5F3),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42200000, 40.0f), -1, 111, 5, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42200000, 40.0f), -1, 111, 5, 0xA5A2),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42200000, 40.0f), -1, 111, 5, 0xBAEE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42200000, 40.0f), -1, 104, 8, 0xC9B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42200000, 40.0f), -1, 104, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42200000, 40.0f), -1, 104, 8, 0xA5A4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42200000, 40.0f), -1, 104, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42200000, 40.0f), -1, 104, 8, 0xA5E1),
    CS_CAM_AT_SPLINE(58, 368),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700001, 60.000004f), 0, 120, 12, 0xA1BC),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), 0, 120, 12, 0xA5F3),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), 0, 120, 12, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), 0, 120, 12, 0xA5A2),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 95, 11, 0xBAEE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 95, 11, 0xC9B8),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 95, 11, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 95, 11, 0xA5A4),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 5, CS_FLOAT(0x42700000, 60.0f), -1, 95, 11, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5D0),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5A6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xC0AE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5EB),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E1),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5D0),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5A6),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xC0AE),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5E5),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 15, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0x0020),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -3, 95, 8, 0xA5EB),
    CS_PLAYER_CUE_LIST(1),
        CS_PLAYER_CUE(PLAYER_CUEID_12, 0, 180, 0x0000, 0x0000, 0x0000, 0, 28, -10, 0, -14, 9, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0xBE6EEEEF, -0.23333333f), CS_FLOAT(0x0, 0.0f)),
    CS_LIGHT_SETTING_LIST(1),
        CS_LIGHT_SETTING(0x0001, 90, 91, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFF2, 0x00000024, 0x00000000, 0xFFFFFFF2, 0x00000024, 0x00000000, 0x00000000, 0x00000000),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_SET_CSFLAG_1, 90, 91, 0x0000, 0x00000000, 0x00000000, 0xFFFFFFF8, 0xFFFFFFDD, 0x00000000, 0xFFFFFFF8, 0xFFFFFFDD, 0x00000000, 0x00000000, 0x00000000),
    CS_TRANSITION(CS_TRANS_GRAY_FILL_IN, 190, 210),
    CS_DESTINATION(CS_DEST_TEMPLE_OF_TIME_FROM_MASTER_SWORD, 210, 211),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on

#include "z_bg_toki_swd.h"
#include "z64cutscene_commands.h"

// clang-format off
CutsceneData tokinoma_demo_first_data[] = {
    CS_HEADER(11, 3000),
    CS_UNK_DATA_LIST(0x00000021, 1),
        CS_UNK_DATA(0x00010000, 0x0BB80000, 0x00000000, 0x00000000, 0xFFFFFFF8, 0xFFFFFFFF, 0x00000000, 0xFFFFFFF8, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000),
    CS_PLAYER_CUE_LIST(3),
        CS_PLAYER_CUE(PLAYER_CUEID_5, 0, 1, 0x0000, 0x8000, 0x0000, 0, 0, 820, 0, 0, 820, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x1, 1e-45f)),
        CS_PLAYER_CUE(PLAYER_CUEID_2, 1, 16, 0x0000, 0x8000, 0x0000, 0, 0, 820, 0, 0, 720, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x1, 1e-45f)),
        CS_PLAYER_CUE(PLAYER_CUEID_5, 16, 176, 0x0000, 0x8000, 0x0000, 0, 0, 720, 0, 0, 720, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x0, 0.0f), CS_FLOAT(0x1, 1e-45f)),
    CS_ACTOR_CUE_LIST(CS_CMD_ACTOR_CUE_8_0, 3),
        CS_ACTOR_CUE(0x0004, 40, 70, 0x7D74, 0x0000, 0x0000, -1, 49, 719, 1, 47, 687, CS_FLOAT(0x3D888889, 0.06666667f), CS_FLOAT(0xBD888889, -0.06666667f), CS_FLOAT(0xBD888889, -0.06666667f)),
        CS_ACTOR_CUE(0x0004, 70, 220, 0x8010, 0x0000, 0x0000, 1, 47, 687, 0, 134, 21, CS_FLOAT(0xBBDA740E, -0.006666667f), CS_FLOAT(0x3F147AE1, 0.58f), CS_FLOAT(0x3BDA740E, 0.006666667f)),
        CS_ACTOR_CUE(0x0002, 220, 272, 0x8000, 0x0000, 0x0000, 0, 134, 21, 0, 101, 2, CS_FLOAT(0x0, 0.0f), CS_FLOAT(0xBF227627, -0.63461536f), CS_FLOAT(0x0, 0.0f)),
    CS_MISC_LIST(1),
        CS_MISC(CS_MISC_STOP_CUTSCENE, 340, 341, 0x0000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFD, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFD, 0x00000000, 0x00000000, 0x00000000),
    CS_TEXT_LIST(6),
        CS_TEXT_NONE(0, 50),
        CS_TEXT(0x70E6, 50, 60, CS_TEXT_NORMAL, 0x0000, 0x0000),
        CS_TEXT_NONE(60, 250),
        CS_TEXT(0x70E7, 250, 260, CS_TEXT_NORMAL, 0x0000, 0x0000),
        CS_TEXT_NONE(260, 290),
        CS_TEXT(0x70E8, 290, 320, CS_TEXT_NORMAL, 0x0000, 0x0000),
    CS_CAM_EYE_SPLINE(0, 251),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -59, 12, 690, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -59, 12, 690, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -59, 12, 690, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -59, 12, 690, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -59, 12, 690, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x2D70),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x0085),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x01E6),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -31, 24, 701, 0x0000),
    CS_CAM_EYE_SPLINE(90, 331),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700000, 60.0f), -154, 92, 236, 0x2D70),
    CS_CAM_EYE_SPLINE(220, 491),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 122, 39, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 122, 39, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 102, 39, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 90, 39, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x2D70),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 0, CS_FLOAT(0x42700001, 60.000004f), -2, 81, 62, 0x0085),
    CS_CAM_AT_SPLINE(0, 280),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 43, 52, 716, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 43, 52, 716, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), 43, 52, 716, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), 43, 52, 716, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), 43, 52, 716, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 10, CS_FLOAT(0x42700000, 60.0f), 63, 86, 722, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 63, 86, 722, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 63, 86, 722, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 63, 86, 722, 0x2D70),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 63, 85, 721, 0x0085),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 63, 85, 721, 0x01E6),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), 62, 85, 721, 0x0000),
    CS_CAM_AT_SPLINE(90, 360),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -105, 83, 366, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -104, 83, 364, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -63, 83, 339, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -16, 99, 255, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -30, 111, 177, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -80, 112, 121, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -80, 112, 121, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -80, 112, 121, 0x0000),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700000, 60.0f), -80, 112, 121, 0x2D70),
    CS_CAM_AT_SPLINE(220, 520),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 137, -55, 0x0222),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 137, -55, 0x00FA),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 117, -55, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 105, -55, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x0000),
        CS_CAM_POINT(CS_CAM_CONTINUE, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x2D70),
        CS_CAM_POINT(CS_CAM_STOP, 0x00, 30, CS_FLOAT(0x42700001, 60.000004f), 0, 98, -31, 0x0085),
    CS_END_OF_SCRIPT(),
    0, 0,
};
// clang-format on

/*
 * File: z_bg_toki_swd.c
 * Overlay: ovl_Bg_Toki_Swd
 * Description: Master Sword (Contains Cutscenes)
 */

#include "z_bg_toki_swd.h"
#include "assets/objects/object_toki_objects/object_toki_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Toki_Swd_actor_ct(Actor* thisx, PlayState* play);
void Bg_Toki_Swd_actor_dt(Actor* thisx, PlayState* play);
void Bg_Toki_Swd_actor_move(Actor* thisx, PlayState* play);
void Bg_Toki_Swd_actor_draw(Actor* thisx, PlayState* play2);

static void mode_wait(BgTokiSwd* this, PlayState* play);
static void mode_demo_wait(BgTokiSwd* this, PlayState* play);
void mode_no_draw(BgTokiSwd* this, PlayState* play);

extern CutsceneData tokinoma_demo_swd0_data[];
extern CutsceneData tokinoma_demo_swd1_data[];
extern CutsceneData tokinoma_demo_first_data[];

ActorProfile Bg_Toki_Swd_Profile = {
    /**/ ACTOR_BG_TOKI_SWD,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_TOKI_OBJECTS,
    /**/ sizeof(BgTokiSwd),
    /**/ Bg_Toki_Swd_actor_ct,
    /**/ Bg_Toki_Swd_actor_dt,
    /**/ Bg_Toki_Swd_actor_move,
    /**/ Bg_Toki_Swd_actor_draw,
};

static ColliderCylinderInit TokiSwdOcData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 70, 0, { 0 } },
};

static CollisionCheckInfoInit TokiSwdStatusData = { 10, 35, 100, MASS_IMMOVABLE };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 25, ICHAIN_STOP),
};

void Bg_Toki_Swd_actor_set_process(BgTokiSwd* this, BgTokiSwdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Toki_Swd_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgTokiSwd* this = (BgTokiSwd*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.shape.yOffset = 800.0f;
    Bg_Toki_Swd_actor_set_process(this, mode_wait);

    if (LINK_IS_ADULT) {
        this->actor.draw = NULL;
    }

    if (z_common_data.sceneLayer == 5) {
        play->roomCtx.drawParams[0] = 0xFF;
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &TokiSwdOcData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &TokiSwdStatusData);
}

void Bg_Toki_Swd_actor_dt(Actor* thisx, PlayState* play) {
    BgTokiSwd* this = (BgTokiSwd*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait(BgTokiSwd* this, PlayState* play) {
    if (!GET_EVENTCHKINF(EVENTCHKINF_REVEALED_MASTER_SWORD) && !IS_CUTSCENE_LAYER &&
        Actor_player_distance_direction_check(&this->actor, 800.0f, 0x7530) && !Game_play_demo_mode_check(play)) {
        SET_EVENTCHKINF(EVENTCHKINF_REVEALED_MASTER_SWORD);
        play->csCtx.script = tokinoma_demo_first_data;
        z_common_data.cutsceneTrigger = 1;
    }
    if (!LINK_IS_ADULT || GET_EVENTCHKINF(EVENTCHKINF_55)) {
        if (Actor_carry_check(&this->actor, play)) {
            if (!LINK_IS_ADULT) {
                item_get_setting(play, ITEM_SWORD_MASTER);
                play->csCtx.script = tokinoma_demo_swd0_data;
            } else {
                play->csCtx.script = tokinoma_demo_swd1_data;
            }
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_MASTER_SWORD);
            z_common_data.cutsceneTrigger = 1;
            this->actor.parent = NULL;
            Bg_Toki_Swd_actor_set_process(this, mode_demo_wait);
        } else {
            if (Actor_player_direction_check(&this->actor, 0x2000)) {
                Actor_carry_request(&this->actor, play);
            }
        }
    }
    if (z_common_data.sceneLayer == 5) {
        if (play->roomCtx.drawParams[0] > 0) {
            play->roomCtx.drawParams[0]--;
        } else {
            play->roomCtx.drawParams[0] = 0;
        }
    }
}

static void mode_demo_wait(BgTokiSwd* this, PlayState* play) {
    Player* player;

    // if sword has a parent it has been pulled/placed from the pedestal
    if (Actor_carry_check(&this->actor, play)) {
        if (!LINK_IS_ADULT) {
            Actor_SE_set(&this->actor, NA_SE_IT_SWORD_PUTAWAY_STN);
            this->actor.draw = NULL; // sword has been pulled, don't draw sword
        } else {
            this->actor.draw = Bg_Toki_Swd_actor_draw; // sword has been placed, draw the master sword
        }
        Bg_Toki_Swd_actor_set_process(this, mode_no_draw);
    } else {
        player = GET_PLAYER(play);
        player->interactRangeActor = &this->actor;
    }
}

void mode_no_draw(BgTokiSwd* this, PlayState* play) {
    if (eventbit_check(play, 1) && (play->roomCtx.drawParams[0] < 0xFF)) {
        play->roomCtx.drawParams[0] += 5;
    }
}

void Bg_Toki_Swd_actor_move(Actor* thisx, PlayState* play) {
    BgTokiSwd* this = (BgTokiSwd*)thisx;

    this->actionFunc(this, play);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void Bg_Toki_Swd_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgTokiSwd* this = (BgTokiSwd*)thisx;
    s32 pad[3];

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_toki_swd.c", 727);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    Actor_HiliteReflect_set_init(&this->actor, play, 0);

    gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, -(play->gameplayFrames % 0x80), 32, 32));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_toki_swd.c", 742);
    gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_001BD0);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_toki_swd.c", 776);
}
