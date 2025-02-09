#ifndef Z_LIB_H
#define Z_LIB_H

#include "libc/stddef.h"
#include "padmgr.h"
#include "z64math.h"
#include "color.h"

void mem_clear(u8* dest, size_t len, u8 val);
f32 cos_s(s16 angle);
f32 sin_s(s16 angle);
s32 chase_angle(s16* pValue, s16 target, s16 step);
s32 chase_s(s16* pValue, s16 target, s16 step);
s32 chase_f(f32* pValue, f32 target, f32 step);
s32 chase_angle2(s16* pValue, s16 limit, s16 step);
s32 chase_s2(s16* pValue, s16 limit, s16 step);
s32 chase_s3(s16* pValue, s16 target, s16 step);
s32 chase_f2(f32* pValue, f32 limit, f32 step);
s32 chase_f3(f32* pValue, f32 target, f32 incrStep, f32 decrStep);
void stick_ratio_set(f32* outMagnitude, s16* outAngle, Input* input);
s16 get_random_timer(s16 base, s16 range);
void xyz_t_move(Vec3f* dest, Vec3f* src);
void xyz_t_move_s_xyz(Vec3f* dest, Vec3s* src);
void xyz_t_add(Vec3f* a, Vec3f* b, Vec3f* dest);
void xyz_t_sub(Vec3f* a, Vec3f* b, Vec3f* dest);
void xyz_t_sub_ss(Vec3f* dest, Vec3s* a, Vec3s* b);
void xyz_t_mult_v(Vec3f* vec, f32 scaleF);
f32 search_position_distance(Vec3f* a, Vec3f* b);
f32 search_position_distance2(Vec3f* a, Vec3f* b, Vec3f* dest);
f32 search_position_distanceXZ(Vec3f* a, Vec3f* b);
s16 search_position_angleY(Vec3f* origin, Vec3f* point);
s16 search_position_angleX(Vec3f* a, Vec3f* b);
f32 add_calc(f32* pValue, f32 target, f32 fraction, f32 step, f32 minStep);
void add_calc2(f32* pValue, f32 target, f32 fraction, f32 step);
void add_calc0(f32* pValue, f32 fraction, f32 step);
f32 add_calc_a(f32* pValue, f32 target, f32 fraction, f32 step, f32 minStep);
s16 add_calc_short_angle2(s16* pValue, s16 target, s16 scale, s16 step, s16 minStep);
void adds(s16* pValue, s16 target, s16 scale, s16 step);
void rgba_t_move(Color_RGBA8* dst, Color_RGBA8* src);
void Na_StartSystemSe_F(u16 sfxId);
void Na_StartFixSe_F(u16 sfxId);
void Na_StartObjectSe_F(Vec3f* projectedPos, u16 sfxId);

#endif
