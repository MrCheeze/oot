#include "z64math.h"
#include "macros.h"

#if !PLATFORM_N64
s32 __fatan_mode;
#endif

/**
 * @param angle radians
 * @return tan(angle)
 */
f32 ftan(f32 angle) {
    f32 sin = sinf(angle);
    f32 cos = cosf(angle);

    return sin / cos;
}

f32 ffloor(f32 x) {
    return __ffloor(x);
}

f32 fceil(f32 x) {
    return __fceil(x);
}

#if PLATFORM_N64
f64 Math_FAbs(f64 x) {
    return x < 0.0 ? -x : x;
}

f32 Math_FAbsF(f32 x) {
    return x < 0.0f ? -x : x;
}
#endif

f32 frint(f32 x) {
    return __frint(x);
}

f32 ftrunc(f32 x) {
    return __ftrunc(x);
}

f32 fround(f32 x) {
    return __fround(x);
}

#if !PLATFORM_N64
/* Arctangent approximation using a Taylor series (one quadrant) */
f32 fatan_new_sub(f32 x) {
    static const f32 div_n[] = {
        -1.0f / 3, +1.0f / 5, -1.0f / 7, +1.0f / 9, -1.0f / 11, +1.0f / 13, -1.0f / 15, +1.0f / 17, 0.0f,
    };

    f32 poly = x;
    f32 sq = SQ(x);
    f32 exp = x * sq;
    const f32* c = div_n;
    f32 term;

    while (true) {
        term = *c++ * exp;
        if (poly + term == poly) {
            break;
        }
        poly += term;
        exp *= sq;
    }

    return poly;
}

/* Ditto for two quadrants */
f32 fatan_new(f32 x) {
    f32 t;
    f32 q;

    if (x > 0.0f) {
        t = x;
    } else if (x < 0.0f) {
        t = -x;
    } else if (x == 0.0f) {
        return 0.0f;
    } else {
        return float_quiet_nan_set;
    }

    if (t <= M_SQRT2 - 1.0f) {
        return fatan_new_sub(x);
    }

    if (t >= M_SQRT2 + 1.0f) {
        q = M_PI / 2 - fatan_new_sub(1.0f / t);
    } else {
        q = M_PI / 4 - fatan_new_sub((1.0f - t) / (1.0f + t));
    }

    if (x > 0.0f) {
        return q;
    } else {
        return -q;
    }
}
#endif

/* Arctangent approximation using a continued fraction */
f32 fatan_old(f32 x) {
    s32 sector;
    f32 z;
    f32 conv;
    f32 sq;
    s32 i;

#if PLATFORM_N64
    if (x > 1.0f) {
        sector = 1;
        x = 1.0f / x;
    } else if (x < -1.0f) {
        sector = -1;
        x = 1.0f / x;
    } else {
        sector = 0;
    }
#else
    if (x >= -1.0f && x <= 1.0f) {
        sector = 0;
    } else if (x > 1.0f) {
        sector = 1;
        x = 1.0f / x;
    } else if (x < -1.0f) {
        sector = -1;
        x = 1.0f / x;
    } else {
        return float_quiet_nan_set;
    }
#endif

    sq = SQ(x);
    conv = 0.0f;

#if PLATFORM_N64
    z = 24.0f;
    i = 24;
#else
    z = 8.0f;
    i = 8;
#endif

    while (i != 0) {
        conv = SQ(z) * sq / (2.0f * z + 1.0f + conv);
        z -= 1.0f;
        i--;
    }

#if PLATFORM_N64
    if (sector > 0) {
        return M_PI / 2 - (x / (1.0f + conv));
    } else if (sector < 0) {
        return -M_PI / 2 - (x / (1.0f + conv));
    } else {
        return x / (1.0f + conv);
    }
#else
    conv = x / (1.0f + conv);
    if (sector == 0) {
        return conv;
    } else if (sector > 0) {
        return M_PI / 2 - conv;
    } else {
        return -M_PI / 2 - conv;
    }
#endif
}

#if !PLATFORM_N64
/**
 * @return arctan(x) in radians, in (-pi/2,pi/2) range
 */
f32 fatan(f32 x) {
    if (!__fatan_mode) {
        return fatan_new(x);
    } else {
        return fatan_old(x);
    }
}
#endif

/**
 * @return angle to (x,y) from vector (1,0) around (0,0) in radians, in (-pi,pi] range
 */
f32 fatan2(f32 y, f32 x) {
#if PLATFORM_N64
    if (y == 0.0f && x == 0.0f) {
        return 0.0f;
    }

    if (x == 0.0f) {
        if (y < 0.0f) {
            return -M_PI / 2;
        } else {
            return M_PI / 2;
        }
    } else if (x < 0.0f) {
        if (y < 0.0f) {
            return -(M_PI - fatan_old(fabs(y / x)));
        } else {
            return M_PI - fatan_old(fabs(y / x));
        }
    } else { // x > 0.0f
        return fatan_old(y / x);
    }
#else
    if (x == 0.0f) {
        if (y == 0.0f) {
            return 0.0f;
        } else if (y > 0.0f) {
            return M_PI / 2;
        } else if (y < 0.0f) {
            return -M_PI / 2;
        } else {
            return float_quiet_nan_set;
        }
    } else if (x >= 0.0f) {
        return fatan(y / x);
    } else { // x < 0.0f
        if (y < 0.0f) {
            return fatan(y / x) - M_PI;
        } else {
            return M_PI - fatan(-(y / x));
        }
    }
#endif
}

/**
 * @return arcsin(x) in radians, in [-pi/2,pi/2] range
 */
f32 fasin(f32 x) {
    return fatan2(x, sqrtf(1.0f - SQ(x)));
}

/**
 * @return arccos(x) in radians, in [0,pi] range
 */
f32 facos(f32 x) {
    return M_PI / 2 - fasin(x);
}
