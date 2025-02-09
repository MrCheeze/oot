#ifndef Z64SKIN_MATRIX_H
#define Z64SKIN_MATRIX_H

#include "ultra64.h"
#include "z64math.h"

struct GraphicsContext;

void Skin_Matrix_PrjMulVector(MtxF* mf, Vec3f* src, Vec3f* xyzDest, f32* wDest);
void Skin_Matrix_MulVector(MtxF* mf, Vec3f* src, Vec3f* dest);
void Skin_Matrix_MulMatrix(MtxF* mfA, MtxF* mfB, MtxF* dest);
void Skin_Matrix_getUnitMatrixPointer(MtxF** mfp);
void Skin_Matrix_Copy(MtxF* src, MtxF* dest);
s32 Skin_Matrix_InverseMatrix(MtxF* src, MtxF* dest);
void Skin_Matrix_SetScale(MtxF* mf, f32 x, f32 y, f32 z);
void Skin_Matrix_SetRotateXyz_s(MtxF* mf, s16 x, s16 y, s16 z);
void Skin_Matrix_SetTranslate(MtxF* mf, f32 x, f32 y, f32 z);
void Skin_Matrix_SetSRzxyTMatrix(MtxF* dest, f32 scaleX, f32 scaleY, f32 scaleZ, s16 rotX, s16 rotY, s16 rotZ,
                                           f32 translateX, f32 translateY, f32 translateZ);
void Skin_Matrix_SetRtMatrix(MtxF* dest, s16 rotX, s16 rotY, s16 rotZ, f32 translateX, f32 translateY,
                                      f32 translateZ);
Mtx* Skin_Matrix_to_Mtx_new(struct GraphicsContext* gfxCtx, MtxF* src);
void Skin_Matrix_SetFreeVecRotMatrix(MtxF* mf, s16 angle, f32 axisX, f32 axisY, f32 axisZ);

#endif
