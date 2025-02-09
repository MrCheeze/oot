#include "sys_math.h"

f32 FACT_TBL[] = { 1.0f,    1.0f,     2.0f,      6.0f,       24.0f,       120.0f,      720.0f,
                        5040.0f, 40320.0f, 362880.0f, 3628800.0f, 39916800.0f, 479001600.0f };

f32 ffact(f32 n) {
    f32 ret = 1.0f;
    s32 i;

    for (i = n; i > 1; i--) {
        ret *= i;
    }
    return ret;
}

f32 ifact(s32 n) {
    f32 ret;
    s32 i;

    if ((u32)n > 12U) {
        ret = FACT_TBL[12];
        for (i = 13; i <= n; i++) {
            ret *= i;
        }
    } else {
        ret = FACT_TBL[n];
    }
    return ret;
}

f32 powi(f32 base, s32 exp) {
    f32 ret = 1.0f;

    while (exp > 0) {
        exp--;
        ret *= base;
    }
    return ret;
}

/**
 * @param angle radians
 * @return sin(angle)
 */
f32 sinf_table(f32 angle) {
    return sins((s16)(angle * (0x7FFF / M_PI))) * SHT_MINV;
}

/**
 * @param angle radians
 * @return cos(angle)
 */
f32 cosf_table(f32 angle) {
    return coss((s16)(angle * (0x7FFF / M_PI))) * SHT_MINV;
}
