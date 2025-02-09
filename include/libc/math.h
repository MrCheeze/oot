#ifndef MATH_H
#define MATH_H

#include "ultra64/ultratypes.h"

#define M_PI 3.14159265358979323846f
#define M_SQRT2 1.41421356237309504880f
#define MAXFLOAT 3.40282347e+38f
#define SHT_MAX 32767.0f
#define SHT_MINV (1.0f / SHT_MAX)

float __ffloor(float);
double __floor(double);
long __iffloor(float);
long __ifloor(double);

float __fceil(float);
double __ceil(double);
long __ifceil(float);
long __iceil(double);

float __ftrunc(float);
double __trunc(double);
long __iftrunc(float);
long __itrunc(double);

float __fround(float);
double __round(double);
long __ifround(float);
long __iround(double);

float __frint(float);
double __rint(double);
long __ifrint(float);
long __irint(double);

f32 fabsf(f32 f);
#ifdef __sgi
#pragma intrinsic(fabsf)
#else
#define fabsf(f) __builtin_fabsf((f32)(f))
#endif

f64 fabs(f64 f);
#ifdef __sgi
#pragma intrinsic(fabs)
#else
#define fabs(f) __builtin_fabs((f64)(f))
#endif

f32 sqrtf(f32 f);
// IDO has a sqrtf intrinsic, but in N64 versions it's not used for some files.
// For these files we define NO_SQRTF_INTRINSIC to use the sqrtf function instead.
#if defined(__sgi) && !defined(NO_SQRTF_INTRINSIC)
#pragma intrinsic(sqrtf)
#endif

f64 sqrt(f64 f);
#ifdef __sgi
#pragma intrinsic(sqrt)
#endif

#if !PLATFORM_N64
extern float float_positive_infinity;
extern float float_negative_infinity;
extern float float_positive_zero;
extern float float_negative_zero;
extern float float_quiet_nan;
extern float float_quiet_nan_set;
extern float float_signaling_nan;
#endif

#endif
