#ifndef SYS_MATRIX_H
#define SYS_MATRIX_H

#include "z64math.h"

struct GraphicsContext;
struct GameState;

typedef enum MatrixMode {
    /* 0 */ MTXMODE_NEW,  // generates a new matrix
    /* 1 */ MTXMODE_APPLY // applies transformation to the current matrix
} MatrixMode;

extern Mtx Mtx_clear;
extern MtxF MtxF_clear;

/* Stack operations */

void new_Matrix(struct GameState* gameState);
void Matrix_push(void);
void Matrix_pull(void);
void Matrix_get(MtxF* dest);
void Matrix_put(MtxF* src);

/* Basic operations */

void Matrix_mult(MtxF* mf, u8 mode);
void Matrix_translate(f32 x, f32 y, f32 z, u8 mode);
void Matrix_scale(f32 x, f32 y, f32 z, u8 mode);
void Matrix_rotateX(f32 x, u8 mode);
void Matrix_rotateY(f32 y, u8 mode);
void Matrix_rotateZ(f32 z, u8 mode);

/* Compound operations */

void Matrix_rotateXYZ(s16 x, s16 y, s16 z, u8 mode);
void Matrix_softcv3_mult(Vec3f* translation, Vec3s* rotation);
void Matrix_softcv3_load(f32 translateX, f32 translateY, f32 translateZ, Vec3s* rot);
void suMtxMakeTS(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, f32 translateX, f32 translateY,
                                  f32 translateZ);

/* Conversion and allocation operations */

Mtx* _MtxF_to_Mtx(MtxF* src, Mtx* dest);

#if DEBUG_FEATURES

Mtx* _Matrix_to_Mtx(Mtx* dest, const char* file, int line);
Mtx* _Matrix_to_Mtx_new(struct GraphicsContext* gfxCtx, const char* file, int line);
MtxF* Matrix_CheckFloats(MtxF* mf, const char* file, int line);

#define MATRIX_TO_MTX(gfxCtx, file, line) _Matrix_to_Mtx(gfxCtx, file, line)
#define MATRIX_FINALIZE(gfxCtx, file, line) _Matrix_to_Mtx_new(gfxCtx, file, line)
#define MATRIX_CHECK_FLOATS(mtx, file, line) Matrix_CheckFloats(mtx, file, line)

#else

Mtx* _Matrix_to_Mtx(Mtx* dest);
Mtx* _Matrix_to_Mtx_new(struct GraphicsContext* gfxCtx);

#define MATRIX_TO_MTX(gfxCtx, file, line) _Matrix_to_Mtx(gfxCtx)
#define MATRIX_FINALIZE(gfxCtx, file, line) _Matrix_to_Mtx_new(gfxCtx)
#define MATRIX_CHECK_FLOATS(mtx, file, line) (mtx)

#endif

/* Vector operations */

void Matrix_Position(Vec3f* src, Vec3f* dest);
void Matrix_MtxF_Position2(Vec3f* src, Vec3f* dest, MtxF* mf);

/* Copy and another conversion */

void Matrix_copy_MtxF(MtxF* dest, MtxF* src);
void Matrix_MtxtoMtxF(Mtx* src, MtxF* dest);

/* Miscellaneous */

void Matrix_reverse(MtxF* mf);
void Matrix_rotate_scale_exchange(MtxF* mf);
void Matrix_to_rotate_new(MtxF* mf, Vec3s* rotDest, s32 flag);
void Matrix_to_rotate2_new(MtxF* mf, Vec3s* rotDest, s32 flag);
void Matrix_rotateVector(f32 angle, Vec3f* axis, u8 mode);

#endif
