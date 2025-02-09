#ifndef SYS_MATH_H
#define SYS_MATH_H

#include "ultra64.h"

f32 ffact(f32 n);
f32 ifact(s32 n);
f32 powi(f32 base, s32 exp);
f32 sinf_table(f32 angle);
f32 cosf_table(f32 angle);
s16 atans_table(f32 x, f32 y);
f32 atanf_table(f32 x, f32 y);

#endif
