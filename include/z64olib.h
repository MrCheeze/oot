#ifndef Z64OLIB_H
#define Z64OLIB_H

#include "z64math.h"

f32 distance_between(Vec3f* a, Vec3f* b);
f32 distance_2d(Vec3f* a, Vec3f* b);
f32 never_zero(f32 val, f32 min);
f32 limiter(f32 val, f32 max);
Vec3f unitvector_by_2pos(Vec3f* a, Vec3f* b);
Vec3f sglobe2world(VecGeo* geo);
VecSph world2spolar(Vec3f* vec);
VecGeo world2sglobe(Vec3f* vec);
VecGeo sglobe_by_2pos(Vec3f* a, Vec3f* b);
Vec3f radianxy_by_2pos(Vec3f* a, Vec3f* b);

#endif
