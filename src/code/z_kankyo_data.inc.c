typedef struct TimeBasedLightEntry {
    /* 0x00 */ u16 startTime;
    /* 0x02 */ u16 endTime;
    /* 0x04 */ u8 lightSetting;
    /* 0x05 */ u8 nextLightSetting;
} TimeBasedLightEntry; // size = 0x6

TimeBasedLightEntry sTimeBasedLightConfigs[][7] = {
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, 3, 3 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(6, 0), 3, 0 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, 0, 1 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), 1, 1 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, 1, 2 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(19, 0) + 1, 2, 3 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, 3, 3 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, 7, 7 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(6, 0), 7, 4 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, 4, 5 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), 5, 5 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, 5, 6 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(19, 0) + 1, 6, 7 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, 7, 7 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, 11, 11 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(6, 0), 11, 8 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, 8, 9 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), 9, 9 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, 9, 10 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(19, 0) + 1, 10, 11 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, 11, 11 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, 15, 15 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(6, 0), 15, 12 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, 12, 13 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), 13, 13 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, 13, 14 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(19, 0) + 1, 14, 15 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, 15, 15 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, 23, 23 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(6, 0), 23, 20 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, 20, 21 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), 21, 21 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, 21, 22 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(19, 0) + 1, 22, 23 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, 23, 23 },
    },
};

TimeBasedSkyboxEntry gTimeBasedSkyboxConfigs[][9] = {
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, false, 3, 3 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(5, 0) + 1, true, 3, 0 },
        { CLOCK_TIME(5, 0) + 1, CLOCK_TIME(6, 0), false, 0, 0 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, true, 0, 1 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), false, 1, 1 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, true, 1, 2 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(18, 0) + 1, false, 2, 2 },
        { CLOCK_TIME(18, 0) + 1, CLOCK_TIME(19, 0) + 1, true, 2, 3 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, false, 3, 3 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0) + 1, false, 7, 7 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(5, 0) + 1, true, 7, 4 },
        { CLOCK_TIME(5, 0) + 1, CLOCK_TIME(6, 0), false, 4, 4 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0) + 1, true, 4, 5 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), false, 5, 5 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, true, 5, 6 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(18, 0) + 1, false, 6, 6 },
        { CLOCK_TIME(18, 0) + 1, CLOCK_TIME(19, 0) + 1, true, 6, 7 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, false, 7, 7 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(2, 0) + 1, false, 3, 3 },
        { CLOCK_TIME(2, 0) + 1, CLOCK_TIME(4, 0) + 1, true, 3, 0 },
        { CLOCK_TIME(4, 0) + 1, CLOCK_TIME(8, 0) + 1, false, 0, 0 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(10, 0), true, 0, 1 },
        { CLOCK_TIME(10, 0), CLOCK_TIME(14, 0) + 1, false, 1, 1 },
        { CLOCK_TIME(14, 0) + 1, CLOCK_TIME(16, 0), true, 1, 2 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(20, 0) + 1, false, 2, 2 },
        { CLOCK_TIME(20, 0) + 1, CLOCK_TIME(22, 0), true, 2, 3 },
        { CLOCK_TIME(22, 0), CLOCK_TIME(24, 0) - 1, false, 3, 3 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(5, 0) + 1, false, 11, 11 },
        { CLOCK_TIME(5, 0) + 1, CLOCK_TIME(6, 0), true, 11, 8 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(7, 0), false, 8, 8 },
        { CLOCK_TIME(7, 0), CLOCK_TIME(8, 0) + 1, true, 8, 9 },
        { CLOCK_TIME(8, 0) + 1, CLOCK_TIME(16, 0), false, 9, 9 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0) + 1, true, 9, 10 },
        { CLOCK_TIME(17, 0) + 1, CLOCK_TIME(18, 0) + 1, false, 10, 10 },
        { CLOCK_TIME(18, 0) + 1, CLOCK_TIME(19, 0) + 1, true, 10, 11 },
        { CLOCK_TIME(19, 0) + 1, CLOCK_TIME(24, 0) - 1, false, 11, 11 },
    },
};

SkyboxFile gNormalSkyFiles[] = {
    {
        ROM_FILE(vr_fine0_static),
        ROM_FILE(vr_fine0_pal_static),
    },
    {
        ROM_FILE(vr_fine1_static),
        ROM_FILE(vr_fine1_pal_static),
    },
    {
        ROM_FILE(vr_fine2_static),
        ROM_FILE(vr_fine2_pal_static),
    },
    {
        ROM_FILE(vr_fine3_static),
        ROM_FILE(vr_fine3_pal_static),
    },
    {
        ROM_FILE(vr_cloud0_static),
        ROM_FILE(vr_cloud0_pal_static),
    },
    {
        ROM_FILE(vr_cloud1_static),
        ROM_FILE(vr_cloud1_pal_static),
    },
    {
        ROM_FILE(vr_cloud2_static),
        ROM_FILE(vr_cloud2_pal_static),
    },
    {
        ROM_FILE(vr_cloud3_static),
        ROM_FILE(vr_cloud3_pal_static),
    },
    {
        ROM_FILE(vr_holy0_static),
        ROM_FILE(vr_holy0_pal_static),
    },
};
