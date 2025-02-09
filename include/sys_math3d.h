#ifndef SYS_MATH3D_H
#define SYS_MATH3D_H

#include "ultra64.h"
#include "z64math.h"

struct PlayState;

s32 Math3D_2PlaneLine_LineNearPos(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                                      f32 planeBC, f32 planeBDist, Vec3f* linePointA, Vec3f* linePointB,
                                      Vec3f* closestPoint);
void Math3D_lineVsPosSuisenCross(InfiniteLine* line, Vec3f* pos, Vec3f* closestPoint);
s32 Math3D2PlaneLinePosNearPos(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA,
                                          f32 planeBB, f32 planeBC, f32 planeBDist, Vec3f* point, Vec3f* closestPoint);
void Math3DInDivPos2(Vec3f* v0, Vec3f* v1, f32 ratio, Vec3f* ret);
f32 M3D_getCos(Vec3f* a, Vec3f* b);
s32 M3D_getCos2(Vec3f* a, Vec3f* b, f32* dst);
void M3D_getRefVec(Vec3f* vec, Vec3f* normal, Vec3f* reflVec);
s32 Math3D_Check2DInArea(f32 upperLeftX, f32 lowerRightX, f32 upperLeftY, f32 lowerRightY, f32 x, f32 y);
f32 Math3DVecLengthSquare2D(f32 a, f32 b);
f32 Math3DLengthSquare2D(f32 x0, f32 y0, f32 x1, f32 y1);
f32 Math3DVecLengthSquare(Vec3f* vec);
f32 Math3DVecLength(Vec3f* vec);
f32 Math3DLengthSquare(Vec3f* a, Vec3f* b);
void Math3DVectorProduct2Vec(Vec3f* a, Vec3f* b, Vec3f* ret);
void Math3DVectorProductXYZ(Vec3f* va, Vec3f* vb, Vec3f* vc, Vec3f* normal);
f32 Math3DLength(Vec3f* a, Vec3f* b);
s32 Math3DCheckBoxPos_CheckPosition_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3DCheckBoxEdgePlane_Bevel2DCheck_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3DCheckBoxEdgePlane_Bevel3DCheck_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3DCheckBoxCrossLine_xyz_t(Vec3f* min, Vec3f* max, Vec3f* a, Vec3f* b);
void Math3DPlanePosAngleY(Vec3f* pointOnPlane, s16 angle, f32* a, f32* c, f32* d);
void Math3DPlane(Vec3f* va, Vec3f* vb, Vec3f* vc, f32* nx, f32* ny, f32* nz, f32* originDist);
f32 Math3DLengthPlaneAndPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p);
f32 Math3DSignedLengthPlaneAndPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p);
s32 Math3DTriangleCrossYCheck(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x);
s32 Math3DTriangleCrossYLine_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                         f32 x, f32* yIntersect, f32 chkDist);
s32 Math3DTriangleCrossYLine_sukimaR_VP0(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist,
                                              f32 z, f32 x, f32* yIntersect, f32 chkDist);
s32 Math3DTriangleCrossYLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                       f32 x, f32* yIntersect, f32 y0, f32 y1);
s32 Math3DTrianglePFCrossYLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 z, f32 x, f32 chkDist);
s32 Math3DTriangleCrossXLine(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                     f32 z, f32* xIntersect);
s32 Math3DTriangleCrossXLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                       f32 z, f32* xIntersect, f32 x0, f32 x1);
s32 Math3DTrianglePFCrossXLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 y, f32 z, f32 chkDist);
s32 Math3DTriangleCrossZLine(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                     f32 y, f32* zIntersect);
s32 Math3DTriangleCrossZLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                       f32 y, f32* zIntersect, f32 z0, f32 z1);
s32 Math3DTrianglePFCrossZLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 x, f32 y, f32 chkDist);
s32 Math3DCheckPlaneCrossLine_xyz_t_cp(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA, Vec3f* linePointB,
                          Vec3f* intersect, s32 fromFront);
void Math3D_triangle_ct(TriNorm* tri, Vec3f* va, Vec3f* vb, Vec3f* vc);
s32 Math3D_pointVsLineSegmentLengthSquare2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32* lineLenSq);
s32 Math3D_sphereCrossLineSegment(Sphere16* sphere, Linef* line);
s32 Math3D_sphereCrossTriangle3_cp(Sphere16* sphere, TriNorm* tri, Vec3f* intersectPoint);
s32 Math3D_pipeCrossLine(Cylinder16* cyl, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersectA, Vec3f* intersectB);
s32 Math3D_pipeCrossTriangle(Cylinder16* cyl, TriNorm* tri);
s32 Math3D_pipeCrossTriangle_cp(Cylinder16* cyl, TriNorm* tri, Vec3f* intersect);
s32 Math3D_sphereCrossSphere(Sphere16* sphereA, Sphere16* sphereB);
s32 Math3D_sphereCrossSphere_cl(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize);
s32 Math3D_sphereCrossSphere_cl_cc(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize, f32* centerDist);
s32 Math3D_sphereVsPipe_cl(Sphere16* sph, Cylinder16* cyl, f32* overlapSize);
s32 Math3D_sphereVsPipe_cl_cc(Sphere16* sph, Cylinder16* cyl, f32* overlapSize, f32* centerDist);
s32 Math3D_pipeVsPipe_cl(Cylinder16* ca, Cylinder16* cb, f32* overlapSize);
s32 Math3D_pipeVsPipe_cl_cc(Cylinder16* ca, Cylinder16* cb, f32* overlapSize, f32* centerDist);
s32 Math3D_triangleCrossTriangle_cp(TriNorm* ta, TriNorm* tb, Vec3f* intersect);
s32 Math3D_sphereCrossXZLine(Sphere16* sphere, f32 x, f32 z);
s32 Math3D_sphereCrossXYLine(Sphere16* sphere, f32 x, f32 y);
s32 Math3D_sphereCrossYZLine(Sphere16* sphere, f32 y, f32 z);

void Math3D_DrawSphere(struct PlayState* play, Sphere16* sph);
void Math3D_DrawCylinder(struct PlayState* play, Cylinder16* cyl);

#endif
