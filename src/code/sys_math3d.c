#include "ultra64.h"
#include "z_lib.h"
#include "z64math.h"
#include "terminal.h"
#include "macros.h"
#include "sys_math3d.h"

#pragma increment_block_number "gc-eu:97 gc-eu-mq:97 gc-jp:97 gc-jp-ce:97 gc-jp-mq:97 gc-us:97 gc-us-mq:97 ique-cn:97" \
                               "ntsc-1.0:80 ntsc-1.1:80 ntsc-1.2:79 pal-1.0:80 pal-1.1:80 hiratsu3:97"

s32 Math3D_2LineNear2Pos(Vec3f* lineAPointA, Vec3f* lineAPointB, Vec3f* lineBPointA, Vec3f* lineBPointB,
                                      Vec3f* lineAClosestToB, Vec3f* lineBClosestToA);
s32 Math3DCheckTriangleCrossLine_xyz_t_cp(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA,
                            Vec3f* linePointB, Vec3f* intersect, s32 fromFront);
s32 Math3D2PlaneCrossLine(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                               f32 planeBC, f32 planeBDist, InfiniteLine* intersect);
s32 Math3DCheck3PointMinMaxAreaInside2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32 centerX, f32 centerY, f32 radius);
s32 Math3DCheck3PointMinMaxAreaInside3D(Vec3f* v0, Vec3f* v1, Vec3f* v2, Vec3f* center, f32 radius);

/**
 * Creates an infinite line along the intersection of the plane defined from `planeAA`x + `planeAB`y + `planeAB`z +
 * `planeADist` = 0 and `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0, and finds the closest point on that
 * intersection to the line segment `linePointA and linePointB`, outputs the intersection to `closestPoint`
 */
s32 Math3D_2PlaneLine_LineNearPos(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                                      f32 planeBC, f32 planeBDist, Vec3f* linePointA, Vec3f* linePointB,
                                      Vec3f* closestPoint) {
    static InfiniteLine l1;
    static Linef ls1;

    Vec3f sp34; // unused

    if (!Math3D2PlaneCrossLine(planeAA, planeAB, planeAC, planeADist, planeBA, planeBB, planeBC, planeBDist,
                                    &l1)) {
        // The planes are parallel
        return false;
    }

    // create a line segment on the plane.
    xyz_t_move(&ls1.a, &l1.point);
    ls1.b.x = (l1.dir.x * 100.0f) + l1.point.x;
    ls1.b.y = (l1.dir.y * 100.0f) + l1.point.y;
    ls1.b.z = (l1.dir.z * 100.0f) + l1.point.z;

    // closestPoint is a point on p2cl, sp34 is a point on linePointA, linePointB
    if (!Math3D_2LineNear2Pos(&ls1.a, &ls1.b, linePointA, linePointB,
                                           closestPoint, &sp34)) {
        return false;
    }
    return true;
}

/**
 * Finds the two points on lines A and B where the lines are closest together.
 */
s32 Math3D_2LineNear2Pos(Vec3f* lineAPointA, Vec3f* lineAPointB, Vec3f* lineBPointA, Vec3f* lineBPointB,
                                      Vec3f* lineAClosestToB, Vec3f* lineBClosestToA) {
    f32 sqMag;
    f32 scaleB;
    f32 lineAx;
    f32 lineAy;
    f32 lineAz;
    f32 lineBx;
    f32 lineBy;
    f32 lineBz;
    f32 compAAlongB;
    f32 compBAAlongB;
    Vec3f lineAPerpB;
    Vec3f lineBAPerpB;
    f32 tA;
    f32 tB;

    lineAx = lineAPointB->x - lineAPointA->x;
    lineAy = lineAPointB->y - lineAPointA->y;
    lineAz = lineAPointB->z - lineAPointA->z;

    lineBx = lineBPointB->x - lineBPointA->x;
    lineBy = lineBPointB->y - lineBPointA->y;
    lineBz = lineBPointB->z - lineBPointA->z;

    sqMag = SQ(lineBx) + SQ(lineBy) + SQ(lineBz);
    if (IS_ZERO(sqMag)) {
        return false;
    }

    scaleB = 1.0f / sqMag;

    compAAlongB = ((lineAx * lineBx) + (lineAy * lineBy) + (lineAz * lineBz)) * scaleB;

    compBAAlongB = ((lineBx * (lineAPointA->x - lineBPointA->x)) + (lineBy * (lineAPointA->y - lineBPointA->y)) +
                    (lineBz * (lineAPointA->z - lineBPointA->z))) *
                   scaleB;

    lineAPerpB.x = lineAx - (lineBx * compAAlongB);
    lineAPerpB.y = lineAy - (lineBy * compAAlongB);
    lineAPerpB.z = lineAz - (lineBz * compAAlongB);

    sqMag = SQXYZ(lineAPerpB);
    if (IS_ZERO(sqMag)) {
        return false;
    }

    lineBAPerpB.x = (lineAPointA->x - lineBPointA->x) - (lineBx * compBAAlongB);
    lineBAPerpB.y = (lineAPointA->y - lineBPointA->y) - (lineBy * compBAAlongB);
    lineBAPerpB.z = (lineAPointA->z - lineBPointA->z) - (lineBz * compBAAlongB);

    tA = -DOTXYZ(lineAPerpB, lineBAPerpB) / sqMag;
    lineAClosestToB->x = (lineAx * tA) + lineAPointA->x;
    lineAClosestToB->y = (lineAy * tA) + lineAPointA->y;
    lineAClosestToB->z = (lineAz * tA) + lineAPointA->z;

    tB = (compAAlongB * tA) + compBAAlongB;
    lineBClosestToA->x = (lineBx * tB) + lineBPointA->x;
    lineBClosestToA->y = (lineBy * tB) + lineBPointA->y;
    lineBClosestToA->z = (lineBz * tB) + lineBPointA->z;

    return true;
}

/**
 * Determines the closest point on the line `line` to `pos`, by forming a line perpendicular from
 * `point` to `line` closest point is placed in `closestPoint`
 */
void Math3D_lineVsPosSuisenCross(InfiniteLine* line, Vec3f* pos, Vec3f* closestPoint) {
    f32 dirVectorLengthSq;
    f32 t;

    dirVectorLengthSq = Math3DVecLengthSquare(&line->dir);
    if (IS_ZERO(dirVectorLengthSq)) {
        PRINTF_COLOR_WARNING();
        PRINTF(T("Math3D_lineVsPosSuisenCross():直線の長さがありません\n",
                 "Math3D_lineVsPosSuisenCross(): No straight line length\n"));
        PRINTF(T("cross = pos を返します。\n", "Returns cross = pos.\n"));
        PRINTF_RST();
        xyz_t_move(closestPoint, pos);
        //! @bug Missing early return
    }

    t = (((pos->x - line->point.x) * line->dir.x) + ((pos->y - line->point.y) * line->dir.y) +
         ((pos->z - line->point.z) * line->dir.z)) /
        dirVectorLengthSq;
    closestPoint->x = (line->dir.x * t) + line->point.x;
    closestPoint->y = (line->dir.y * t) + line->point.y;
    closestPoint->z = (line->dir.z * t) + line->point.z;
}

void Math2D2PlaneCrossLineProcWork(f32 planeAAxis1Norm, f32 planeAAxis2Norm, f32 planeBAxis1Norm,
                                      f32 planeBAxis2Norm, f32 axis3Direction, f32 planeADist, f32 planeBDist,
                                      f32* axis1Point, f32* axis2Point) {
    *axis1Point = ((planeAAxis2Norm * planeBDist) - (planeBAxis2Norm * planeADist)) / axis3Direction;
    *axis2Point = ((planeBAxis1Norm * planeADist) - (planeAAxis1Norm * planeBDist)) / axis3Direction;
}

/**
 * Creates a line between the intersections of two planes defined from `planeAA`x + `planeAB`y + `planeAC`z +
 * `planeADist` = 0 and `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0, and outputs the line to `intersect`.
 * Returns false if the planes are parallel.
 */
s32 Math3D2PlaneCrossLine(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                               f32 planeBC, f32 planeBDist, InfiniteLine* intersect) {
    char pad[4];
    Vec3f planeANormal;
    Vec3f planeBNormal;
    f32 dirX;
    f32 dirY;
    f32 dirZ;

    VEC_SET(planeANormal, planeAA, planeAB, planeAC);
    VEC_SET(planeBNormal, planeBA, planeBB, planeBC);

    Math3DVectorProduct2Vec(&planeANormal, &planeBNormal, &intersect->dir);

    if (IS_ZERO(intersect->dir.x) && IS_ZERO(intersect->dir.y) && IS_ZERO(intersect->dir.z)) {
        // planes are parallel
        return false;
    }

    dirX = fabsf(intersect->dir.x);
    dirY = fabsf(intersect->dir.y);
    dirZ = fabsf(intersect->dir.z);

    if ((dirX >= dirY) && (dirX >= dirZ)) {
        Math2D2PlaneCrossLineProcWork(planeAB, planeAC, planeBB, planeBC, intersect->dir.x, planeADist, planeBDist,
                                         &intersect->point.y, &intersect->point.z);
        intersect->point.x = 0.0f;
    } else if ((dirY >= dirX) && (dirY >= dirZ)) {
        Math2D2PlaneCrossLineProcWork(planeAC, planeAA, planeBC, planeBA, intersect->dir.y, planeADist, planeBDist,
                                         &intersect->point.z, &intersect->point.x);
        intersect->point.y = 0.0f;
    } else {
        Math2D2PlaneCrossLineProcWork(planeAA, planeAB, planeBA, planeBB, intersect->dir.z, planeADist, planeBDist,
                                         &intersect->point.x, &intersect->point.y);
        intersect->point.z = 0.0f;
    }
    return true;
}

/**
 * Gets the closest point on the line formed from the intersection of of the planes defined from
 * `planeAA`x + `planeAB`y + `planeAC`z + `planeADist` = 0 and
 * `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0
 * the point on the intersection line closest to `point` is placed in `closestPoint`
 * returns false if the planes are parallel.
 */
s32 Math3D2PlaneLinePosNearPos(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA,
                                          f32 planeBB, f32 planeBC, f32 planeBDist, Vec3f* point, Vec3f* closestPoint) {
    static InfiniteLine p2cl;

    if (!Math3D2PlaneCrossLine(planeAA, planeAB, planeAC, planeADist, planeBA, planeBB, planeBC, planeBDist,
                                    &p2cl)) {
        return false;
    }
    Math3D_lineVsPosSuisenCross(&p2cl, point, closestPoint);
    return true;
}

/**
 * Finds a point on the line from starting point `v0`, and directional vector `dir`
 * which is `dist` length from the starting point.  Result is placed in `ret`
 */
void Math3DInDivPos1(Vec3f* v0, Vec3f* dir, f32 dist, Vec3f* ret) {
    ret->x = (dir->x * dist) + v0->x;
    ret->y = (dir->y * dist) + v0->y;
    ret->z = (dir->z * dist) + v0->z;
}

/**
 * Splits the line segment from end points `v0` and `v1`, and splits that segment
 * by `ratio` of `v0`:`v1`, places the resulting point on the line in `ret`
 */
void Math3DInDivPos2(Vec3f* v0, Vec3f* v1, f32 ratio, Vec3f* ret) {
    Vec3f diff;

    xyz_t_sub(v1, v0, &diff);
    Math3DInDivPos1(v0, &diff, ratio, ret);
}

/**
 * Calculates the cosine between vectors `a` and `b`
 */
f32 M3D_getCos(Vec3f* a, Vec3f* b) {
    f32 ret;

    M3D_getCos2(a, b, &ret);
    return ret;
}

/**
 * Calculates the cosine between vectors `a` and `b` and places the result in `ret`
 * returns true if the cosine cannot be calculated because the product of the magnitudes is zero
 */
s32 M3D_getCos2(Vec3f* a, Vec3f* b, f32* dst) {
    f32 magProduct;

    magProduct = Math3DVecLength(a) * Math3DVecLength(b);
    if (IS_ZERO(magProduct)) {
        *dst = 0.0f;
        return true;
    }
    *dst = ((a->x * b->x) + (a->y * b->y) + (a->z * b->z)) / magProduct;
    return false;
}

/**
 * Reflects vector `vec` across the mult_vec vector `mult_vec`, reflection vector is placed in
 * `reflVec`
 */
void M3D_getRefVec(Vec3f* vec, Vec3f* mult_vec, Vec3f* reflVec) {

    f32 normScaleY;
    Vec3f negVec;
    f32 normScaleZ;
    f32 normScaleX;
    f32 vecDotNorm;

    negVec.x = vec->x * -1.0f;
    negVec.y = vec->y * -1.0f;
    negVec.z = vec->z * -1.0f;

    vecDotNorm = M3D_getCos(&negVec, mult_vec);

    normScaleX = mult_vec->x * vecDotNorm;
    normScaleY = mult_vec->y * vecDotNorm;
    normScaleZ = mult_vec->z * vecDotNorm;

    reflVec->x = ((normScaleX + vec->x) + (normScaleX + vec->x)) + negVec.x;
    reflVec->y = ((normScaleY + vec->y) + (normScaleY + vec->y)) + negVec.y;
    reflVec->z = ((normScaleZ + vec->z) + (normScaleZ + vec->z)) + negVec.z;
}

/**
 * Checks if the point (`x`,`y`) is contained within the square formed from (`upperLeftX`,`upperLeftY`) to
 * (`lowerRightX`,`lowerRightY`)
 */
s32 Math3D_Check2DInArea(f32 upperLeftX, f32 lowerRightX, f32 upperLeftY, f32 lowerRightY, f32 x, f32 y) {
    if (x >= upperLeftX && x <= lowerRightX && y >= upperLeftY && y <= lowerRightY) {
        return true;
    }
    return false;
}

/**
 * Checks if the square formed around the circle with center (`centerX`,`centerY`) with radius `radius`
 * touches any portion of the square formed around the triangle with vertices (`x0`,`y0`), (`x1`,`y1`),
 * and (`x2`,`y2`)
 */
s32 Math3DCheck3PointMinMaxAreaInside2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32 centerX, f32 centerY, f32 radius) {
    f32 minX;
    f32 maxX;
    f32 minY;
    f32 maxY;

    minX = maxX = x0;
    minY = maxY = y0;

    if (x1 < minX) {
        minX = x1;
    } else if (maxX < x1) {
        maxX = x1;
    }

    if (y1 < minY) {
        minY = y1;
    } else if (maxY < y1) {
        maxY = y1;
    }

    if (x2 < minX) {
        minX = x2;
    } else if (maxX < x2) {
        maxX = x2;
    }

    if (y2 < minY) {
        minY = y2;
    } else if (maxY < y2) {
        maxY = y2;
    }

    if ((minX - radius) <= centerX && (maxX + radius) >= centerX && (minY - radius) <= centerY &&
        (maxY + radius) >= centerY) {
        return true;
    }
    return false;
}

/**
 * Checks if the cube formed around the triangle formed from `v0`, `v1`, and `v2`
 * has any portion touching the cube formed around the sphere with center `center`
 * and radius of `radius`
 */
s32 Math3DCheck3PointMinMaxAreaInside3D(Vec3f* v0, Vec3f* v1, Vec3f* v2, Vec3f* center, f32 radius) {
    f32 minX;
    f32 maxX;
    f32 minY;
    f32 maxY;
    f32 minZ;
    f32 maxZ;

    minX = maxX = v0->x;
    minY = maxY = v0->y;
    minZ = maxZ = v0->z;

    if (v1->x < minX) {
        minX = v1->x;
    } else if (maxX < v1->x) {
        maxX = v1->x;
    }

    if (v1->y < minY) {
        minY = v1->y;
    } else if (maxY < v1->y) {
        maxY = v1->y;
    }

    if (v1->z < minZ) {
        minZ = v1->z;
    } else if (maxZ < v1->z) {
        maxZ = v1->z;
    }

    if (v2->x < minX) {
        minX = v2->x;
    } else if (maxX < v2->x) {
        maxX = v2->x;
    }

    if (v2->y < minY) {
        minY = v2->y;
    } else if (maxY < v2->y) {
        maxY = v2->y;
    }

    if (v2->z < minZ) {
        minZ = v2->z;
    } else if (maxZ < v2->z) {
        maxZ = v2->z;
    }

    if ((center->x >= (minX - radius)) && (center->x <= (maxX + radius)) && (center->y >= (minY - radius)) &&
        (center->y <= (maxY + radius)) && (center->z >= (minZ - radius)) && (center->z <= (maxZ + radius))) {
        return true;
    }

    return false;
}

/**
 * Returns the distance squared between `a` and `b` on a single axis
 */
f32 Math3DVecLengthSquare2D(f32 a, f32 b) {
    return SQ(a) + SQ(b);
}

/**
 * Returns the distance between `a` and `b` on a single axis
 */
f32 Math3DVecLength2D(f32 a, f32 b) {
    return sqrtf(Math3DVecLengthSquare2D(a, b));
}

/**
 * Returns the distance squared between (`x0`,`y0`) and (`x1`,`x2`)
 */
f32 Math3DLengthSquare2D(f32 x0, f32 y0, f32 x1, f32 y1) {
    return Math3DVecLengthSquare2D(x0 - x1, y0 - y1);
}

/**
 * Returns the distance between points (`x0`,`y0`) and (`x1`,`y1`)
 */
f32 Math3DLength2D(f32 x0, f32 y0, f32 x1, f32 y1) {
    return sqrtf(Math3DLengthSquare2D(x0, y0, x1, y1));
}

/**
 * Returns the magnitude (length) squared of `vec`
 */
f32 Math3DVecLengthSquare(Vec3f* vec) {
    return SQ(vec->x) + SQ(vec->y) + SQ(vec->z);
}

/**
 * Returns the magnitude (length) of `vec`
 */
f32 Math3DVecLength(Vec3f* vec) {
    return sqrt(Math3DVecLengthSquare(vec));
}

/**
 * Returns the distance between `a` and `b` squared.
 */
f32 Math3DLengthSquare(Vec3f* a, Vec3f* b) {
    Vec3f diff;

    xyz_t_sub(a, b, &diff);
    return Math3DVecLengthSquare(&diff);
}

/*
 * Calculates the distance between points `a` and `b`
 */
f32 Math3DLength(Vec3f* a, Vec3f* b) {
    return search_position_distance(a, b);
}

/*
 * Calculates the distance between `a` and `b`.
 */
f32 Math3DLength_s_f(Vec3s* a, Vec3f* b) {
    Vec3f diff;

    diff.x = a->x - b->x;
    diff.y = a->y - b->y;
    diff.z = a->z - b->z;
    return Math3DVecLength(&diff);
}

/**
 * Gets the Z portion of the cross product of vectors `a - (`dx`,`dy`,z) and `b` - (`dx`,`dy`,z)
 */
f32 Math3DVectorProductXY(Vec3f* a, Vec3f* b, f32 dx, f32 dy) {
    return ((a->x - dx) * (b->y - dy)) - ((a->y - dy) * (b->x - dx));
}

/**
 * Gets the X portion of the cross product of vectors `a - (x,`dy`,`dz`) and `b` - (x,`dy`,`dz`)
 */
f32 Math3DVectorProductYZ(Vec3f* a, Vec3f* b, f32 dy, f32 dz) {
    return ((a->y - dy) * (b->z - dz)) - ((a->z - dz) * (b->y - dy));
}

/**
 * Gets the Y portion of the cross product of vectors `a - (`dx`,y,`dz`) and `b` - (`dx`,y,`dz`)
 */
f32 Math3DVectorProductZX(Vec3f* a, Vec3f* b, f32 dz, f32 dx) {
    return ((a->z - dz) * (b->x - dx)) - ((a->x - dx) * (b->z - dz));
}

/**
 * Gets the Cross Product of vectors `a` and `b` and places the result in `ret`
 */
void Math3DVectorProduct2Vec(Vec3f* a, Vec3f* b, Vec3f* ret) {
    ret->x = (a->y * b->z) - (a->z * b->y);
    ret->y = (a->z * b->x) - (a->x * b->z);
    ret->z = (a->x * b->y) - (a->y * b->x);
}

/*
 * Calculates the mult_vec vector to a surface with sides `vb` - `va` and `vc` - `va`
 * outputs the mult_vec to `mult_vec`
 */
void Math3DVectorProductXYZ(Vec3f* va, Vec3f* vb, Vec3f* vc, Vec3f* mult_vec) {
    static Vec3f vec01;
    static Vec3f vec02;

    xyz_t_sub(vb, va, &vec01);
    xyz_t_sub(vc, va, &vec02);
    Math3DVectorProduct2Vec(&vec01, &vec02, mult_vec);
}

/**
 * Creates flags relative to the faces of a cube.
 */
s32 Math3DCheckBoxPos_CheckPosition_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if (point->x > max->x) {
        ret = 1;
    }

    if (point->x < min->x) {
        ret |= 2;
    }

    if (point->y > max->y) {
        ret |= 4;
    }

    if (point->y < min->y) {
        ret |= 8;
    }

    if (point->z > max->z) {
        ret |= 0x10;
    }

    if (point->z < min->z) {
        ret |= 0x20;
    }

    return ret;
}

/**
 * Creates flags of `point` relative to the edges of a cube
 */
s32 Math3DCheckBoxEdgePlane_Bevel2DCheck_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if ((-min->x + max->y) < (-point->x + point->y)) {
        ret |= 1;
    }

    if ((-point->x + point->y) < (-max->x + min->y)) {
        ret |= 2;
    }

    if ((max->x + max->y) < (point->x + point->y)) {
        ret |= 4;
    }

    if ((point->x + point->y) < (min->x + min->y)) {
        ret |= 8;
    }

    if ((-min->z + max->y) < (-point->z + point->y)) {
        ret |= 0x10;
    }

    if ((-point->z + point->y) < (-max->z + min->y)) {
        ret |= 0x20;
    }

    if ((max->z + max->y) < (point->z + point->y)) {
        ret |= 0x40;
    }

    if ((point->z + point->y) < (min->z + min->y)) {
        ret |= 0x80;
    }

    if ((-min->z + max->x) < (-point->z + point->x)) {
        ret |= 0x100;
    }

    if ((-point->z + point->x) < (-max->z + min->x)) {
        ret |= 0x200;
    }

    if ((max->z + max->x) < (point->z + point->x)) {
        ret |= 0x400;
    }

    if ((point->z + point->x) < (min->z + min->x)) {
        ret |= 0x800;
    }
    return ret;
}

/**
 * Creates flags for `point` relative to the vertices of a cube
 */
s32 Math3DCheckBoxEdgePlane_Bevel3DCheck_xyz_t(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if ((max->x + max->y + max->z) < (point->x + point->y + point->z)) {
        ret = 1;
    }

    if ((-min->x + max->y + max->z) < (-point->x + point->y + point->z)) {
        ret |= 2;
    }

    if ((-min->x + max->y - min->z) < (-point->x + point->y - point->z)) {
        ret |= 4;
    }

    if ((max->x + max->y - min->z) < (point->x + point->y - point->z)) {
        ret |= 8;
    }

    if ((max->x - min->y + max->z) < (point->x - point->y + point->z)) {
        ret |= 0x10;
    }

    //! @bug: The next 2 conditions are the same check.
    if ((-min->x - min->y + max->z) < (-point->x - point->y + point->z)) {
        ret |= 0x20;
    }

    if ((-min->x - min->y + max->z) < (-point->x - point->y + point->z)) {
        ret |= 0x40;
    }

    if ((-min->x - min->y - min->z) < (-point->x - point->y - point->z)) {
        ret |= 0x80;
    }
    return ret;
}

/**
 * Checks if a line segment with endpoints `a` and `b` intersect a cube
 */
s32 Math3DCheckBoxCrossLine_xyz_t(Vec3f* min, Vec3f* max, Vec3f* a, Vec3f* b) {
    static Vec3f tp0;
    static Vec3f tp1;
    static Vec3f tp2;
    static Vec3f cross_pos;

    s32 flags[2];

    flags[0] = flags[1] = 0;
    flags[0] = Math3DCheckBoxPos_CheckPosition_xyz_t(a, min, max);
    if (!flags[0]) {
        return true;
    }

    flags[1] = Math3DCheckBoxPos_CheckPosition_xyz_t(b, min, max);
    if (!flags[1]) {
        return true;
    }

    if (flags[0] & flags[1]) {
        return false;
    }

    flags[0] |= (Math3DCheckBoxEdgePlane_Bevel2DCheck_xyz_t(a, min, max) << 8);
    flags[1] |= (Math3DCheckBoxEdgePlane_Bevel2DCheck_xyz_t(b, min, max) << 8);
    if (flags[0] & flags[1]) {
        return false;
    }

    flags[0] |= (Math3DCheckBoxEdgePlane_Bevel3DCheck_xyz_t(a, min, max) << 0x18);
    flags[1] |= (Math3DCheckBoxEdgePlane_Bevel3DCheck_xyz_t(b, min, max) << 0x18);
    if (flags[0] & flags[1]) {
        return false;
    }

    // face 1
    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = min->x;
    tp1.y = min->y;
    tp1.z = max->z;
    tp2.x = min->x;
    tp2.y = max->y;
    tp2.z = max->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, -1.0f, 0.0f, 0.0f, min->x, a, b, &cross_pos, 0)) {
        return true;
    }

    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = min->x;
    tp1.y = max->y;
    tp1.z = max->z;
    tp2.x = min->x;
    tp2.y = max->y;
    tp2.z = min->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, -1.0f, 0.0f, 0.0f, min->x, a, b, &cross_pos, 0)) {
        return true;
    }

    // face 2
    tp0.x = min->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = min->x;
    tp1.y = min->y;
    tp1.z = max->z;
    tp2.x = max->x;
    tp2.y = max->y;
    tp2.z = max->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 0.0f, 1.0f, -max->z, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = max->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = min->x;
    tp1.y = min->y;
    tp1.z = max->z;
    tp2.x = max->x;
    //! @bug trVtx1.y should be tp2.y, prevents a tri on the cube from being checked.
    tp1.y = min->y;
    tp2.z = max->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 0.0f, 1.0f, -max->z, a, b, &cross_pos, 0)) {
        return true;
    }

    // face 3
    tp0.x = max->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = min->x;
    tp1.y = max->y;
    tp1.z = min->z;
    tp2.x = min->x;
    tp2.y = max->y;
    tp2.z = max->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 1.0f, 0.0f, -max->y, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = max->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = max->x;
    tp1.y = max->y;
    tp1.z = min->z;
    tp2.x = min->x;
    tp2.y = max->y;
    tp2.z = min->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 1.0f, 0.0f, -max->y, a, b, &cross_pos, 0)) {
        return true;
    }

    // face 4
    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = min->x;
    tp1.y = max->y;
    tp1.z = min->z;
    tp2.x = max->x;
    tp2.y = max->y;
    tp2.z = min->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 0.0f, -1.0f, min->z, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = max->x;
    tp1.y = max->y;
    tp1.z = min->z;
    tp2.x = max->x;
    tp2.y = min->y;
    tp2.z = min->z;

    // face 5
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, 0.0f, -1.0f, min->z, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = max->x;
    tp1.y = min->y;
    tp1.z = min->z;
    tp2.x = max->x;
    tp2.y = min->y;
    tp2.z = max->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, -1.0f, 0.0f, min->y, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = min->x;
    tp0.y = min->y;
    tp0.z = min->z;
    tp1.x = max->x;
    tp1.y = min->y;
    tp1.z = max->z;
    tp2.x = min->x;
    tp2.y = min->y;
    tp2.z = max->z;

    // face 6
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 0.0f, -1.0f, 0.0f, min->y, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = max->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = max->x;
    tp1.y = min->y;
    tp1.z = min->z;
    tp2.x = max->x;
    tp2.y = max->y;
    tp2.z = min->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 1.0f, 0.0f, 0.0f, -max->x, a, b, &cross_pos, 0)) {
        return true;
    }
    tp0.x = max->x;
    tp0.y = max->y;
    tp0.z = max->z;
    tp1.x = max->x;
    tp1.y = min->y;
    tp1.z = max->z;
    tp2.x = max->x;
    tp2.y = min->y;
    tp2.z = min->z;
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tp0, &tp1, &tp2, 1.0f, 0.0f, 0.0f, -max->x, a, b, &cross_pos, 0)) {
        return true;
    }

    return false;
}

/**
 * Checks if a line segment with endpoints `a` and `b` intersect a cube
 */
s32 Math3DCheckBoxCrossLine(Vec3s* min, Vec3s* max, Vec3s* a, Vec3s* b) {
    static Vec3f min_p;
    static Vec3f max_p;
    static Vec3f lps_p;
    static Vec3f lpe_p;

    min_p.x = min->x;
    min_p.y = min->y;
    min_p.z = min->z;
    max_p.x = max->x;
    max_p.y = max->y;
    max_p.z = max->z;
    lps_p.x = a->x;
    lps_p.y = a->y;
    lps_p.z = a->z;
    lpe_p.x = b->x;
    lpe_p.y = b->y;
    lpe_p.z = b->z;
    return Math3DCheckBoxCrossLine_xyz_t(&min_p, &max_p, &lps_p, &lpe_p);
}

/**
 * Rotates the xz plane around the y axis `angle` degrees.
 * outputs the plane equation `a``pointOnPlane->x` + 0y + `c``pointOnPlane->z`+`d` = 0
 */
void Math3DPlanePosAngleY(Vec3f* pointOnPlane, s16 angle, f32* a, f32* c, f32* d) {
    *a = sin_s(angle) * 32767.0f;
    *c = cos_s(angle) * 32767.0f;
    *d = -((*a * pointOnPlane->x) + (*c * pointOnPlane->z));
}

/*
 * Defines a plane from vertices `va`, `vb`, and `vc`.  Normal components are output to
 * `nx`, `ny`, and `nz`.  Distance from the origin is output to `originDist`
 * Satisfies the plane equation NxVx + NyVy + NzVz + D = 0
 */
void Math3DPlane(Vec3f* va, Vec3f* vb, Vec3f* vc, f32* nx, f32* ny, f32* nz, f32* originDist) {
    static Vec3f mult_vec;

    f32 normMagnitude;
    f32 normMagInv;

    Math3DVectorProductXYZ(va, vb, vc, &mult_vec);
    normMagnitude = sqrtf(SQ(mult_vec.x) + SQ(mult_vec.y) + SQ(mult_vec.z));
    if (!IS_ZERO(normMagnitude)) {
        normMagInv = 1.0f / normMagnitude;
        *nx = mult_vec.x * normMagInv;
        *ny = mult_vec.y * normMagInv;
        *nz = mult_vec.z * normMagInv;
        *originDist = -((*nx * va->x) + (*ny * va->y) + (*nz * va->z));
    } else {
        *originDist = 0.0f;
        *nz = 0.0f;
        *ny = 0.0f;
        *nx = 0.0f;
    }
}

/*
 * Returns the answer to the plane equation with elements specified by arguments.
 */
f32 Math3DPlaneFunc(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* pointOnPlane) {
    return (nx * pointOnPlane->x) + (ny * pointOnPlane->y) + (nz * pointOnPlane->z) + originDist;
}

/*
 * Returns the answer to the plane equation
 */
f32 T_PolygonF_planeFunc(Plane* plane, Vec3f* pointOnPlane) {
    return (plane->normal.x * pointOnPlane->x) + (plane->normal.y * pointOnPlane->y) +
           (plane->normal.z * pointOnPlane->z) + plane->originDist;
}

/*
 * Calculates the absolute distance from a point `p` to the plane defined as
 * `nx`, `ny`, `nz`, and `originDist`
 */
f32 Math3DLengthPlaneAndPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p) {
    if (DEBUG_FEATURES && IS_ZERO(sqrtf(SQ(nx) + SQ(ny) + SQ(nz)))) {
        PRINTF_COLOR_WARNING();
        PRINTF(T("Math3DLengthPlaneAndPos():法線size がゼロ近いです%f %f %f\n",
                 "Math3DLengthPlaneAndPos(): Normal size is near zero %f %f %f\n"),
               nx, ny, nz);
        PRINTF_RST();
        return 0.0f;
    }
    return fabsf(Math3DSignedLengthPlaneAndPos(nx, ny, nz, originDist, p));
}

/*
 * Calculates the signed distance from a point `p` to a plane defined as
 * `nx`, `ny`, `nz`, and `originDist`
 */
f32 Math3DSignedLengthPlaneAndPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p) {
    f32 normMagnitude;

    normMagnitude = sqrtf(SQ(nx) + SQ(ny) + SQ(nz));
    if (IS_ZERO(normMagnitude)) {
        PRINTF_COLOR_WARNING();
        PRINTF(T("Math3DSignedLengthPlaneAndPos():法線size がゼロ近いです%f %f %f\n",
                 "Math3DSignedLengthPlaneAndPos(): Normal size is close to zero %f %f %f\n"),
               nx, ny, nz);
        PRINTF_RST();
        return 0.0f;
    }
    return Math3DPlaneFunc(nx, ny, nz, originDist, p) / normMagnitude;
}

/**
 * Checks if the point defined by (`z`,`x`) is within distance of the triangle defined from `v0`,`v1`, and `v2`
 */
s32 Math3DTriangleCrossYCheck_general(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 chkDist, f32 ny) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    // first check if the point is within range of the triangle.
    if (!Math3DCheck3PointMinMaxAreaInside2D(v0->z, v0->x, v1->z, v1->x, v2->z, v2->x, z, x, chkDist)) {
        return false;
    }

    // check if the point is within `chkDist` units of any vertex of the triangle.
    chkDistSq = SQ(chkDist);
    if (((SQ(v0->z - z) + SQ(v0->x - x)) < chkDistSq) || ((SQ(v1->z - z) + SQ(v1->x - x)) < chkDistSq) ||
        ((SQ(v2->z - z) + SQ(v2->x - x)) < chkDistSq)) {

        return true;
    }

    // Calculate the determinant of each face of the triangle to the point.
    // If all the of determinants are within abs(`detMax`), return true.
    detv0v1 = ((v0->z - z) * (v1->x - x)) - ((v0->x - x) * (v1->z - z));
    detv1v2 = ((v1->z - z) * (v2->x - x)) - ((v1->x - x) * (v2->z - z));
    detv2v0 = ((v2->z - z) * (v0->x - x)) - ((v2->x - x) * (v0->z - z));

    if (((detMax >= detv0v1) && (detMax >= detv1v2) && (detMax >= detv2v0)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(ny) > 0.5f) {
        // Do a check on each face of the triangle, if the point is within `chkDist` units return true.
        if (Math3D_pointVsLineSegmentLengthSquare2D(z, x, v0->z, v0->x, v1->z, v1->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(z, x, v1->z, v1->x, v2->z, v2->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(z, x, v2->z, v2->x, v0->z, v0->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }
    return false;
}

s32 Math3DTriangleCrossYCheck_chparam(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 ny) {
    return Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, detMax, 1.0f, ny);
}

s32 Math3DTriangleCrossYCheck(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x) {
    return Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 300.0f, 1.0f, 0.6f);
}

/**
 * Performs the triangle and point check parallel to the Y axis, outputs the y coordinate of the point to `yIntersect`
 */
s32 Math3DTriangleCrossYLine_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                         f32 x, f32* yIntersect, f32 chkDist) {
    if (IS_ZERO(ny)) {
        return false;
    }

    if (Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 300.0f, chkDist, ny)) {
        *yIntersect = (f32)((((-nx * x) - (nz * z)) - originDist) / ny);
        return true;
    }

    return false;
}

s32 Math3DTriangleCrossYLine_sukimaR_VP0(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist,
                                              f32 z, f32 x, f32* yIntersect, f32 chkDist) {
    if (IS_ZERO(ny)) {
        return false;
    }

    if (Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 0.0f, chkDist, ny)) {
        *yIntersect = (f32)((((-nx * x) - (nz * z)) - originDist) / ny);
        return true;
    }

    return false;
}

s32 Math3DTriangleCrossYLine_noCross(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 ny, f32 z, f32 x) {
    if (IS_ZERO(ny)) {
        return false;
    }
    if (Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 300.0f, 1.0f, ny)) {
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossYLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                       f32 x, f32* yIntersect, f32 y0, f32 y1) {
    f32 pointADist;
    f32 pointBDist;
    Vec3f pos;

    if (IS_ZERO(ny)) {
        return false;
    }

    pos.x = x;
    pos.y = y0;
    pos.z = z;

    pointADist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);
    pos.y = y1;
    pointBDist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);
    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        return false;
    }

    if (Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 300.0f, 1.0f, ny)) {
        *yIntersect = (((-nx * x) - (nz * z)) - originDist) / ny;
        return true;
    }

    return false;
}

s32 Math3DTrianglePFCrossYLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 z, f32 x, f32 chkDist) {
    if (IS_ZERO(plane->normal.y)) {
        return false;
    }

    if (Math3DTriangleCrossYCheck_general(v0, v1, v2, z, x, 0.0f, chkDist, plane->normal.y)) {
        return true;
    }

    return false;
}

s32 Math3DTriangleCrossXCheck_general(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 chkDist, f32 nx) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    if (!Math3DCheck3PointMinMaxAreaInside2D(v0->y, v0->z, v1->y, v1->z, v2->y, v2->z, y, z, chkDist)) {
        return false;
    }

    chkDistSq = SQ(chkDist);

    if (((SQ(v0->y - y) + SQ(v0->z - z)) < chkDistSq) || ((SQ(v1->y - y) + SQ(v1->z - z)) < chkDistSq) ||
        ((SQ(v2->y - y) + SQ(v2->z - z)) < chkDistSq)) {
        return true;
    }

    detv0v1 = ((v0->y - y) * (v1->z - z)) - ((v0->z - z) * (v1->y - y));
    detv1v2 = ((v1->y - y) * (v2->z - z)) - ((v1->z - z) * (v2->y - y));
    detv2v0 = ((v2->y - y) * (v0->z - z)) - ((v2->z - z) * (v0->y - y));

    if (((detv0v1 <= detMax) && (detv1v2 <= detMax) && (detv2v0 <= detMax)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(nx) > 0.5f) {

        if (Math3D_pointVsLineSegmentLengthSquare2D(y, z, v0->y, v0->z, v1->y, v1->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(y, z, v1->y, v1->z, v2->y, v2->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(y, z, v2->y, v2->z, v0->y, v0->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }
    return false;
}

s32 Math3DTriangleCrossXCheck_chparam(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 nx) {
    return Math3DTriangleCrossXCheck_general(v0, v1, v2, y, z, detMax, 1.0f, nx);
}

s32 Math3DTriangleCrossXLine(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                     f32 z, f32* xIntersect) {
    if (IS_ZERO(nx)) {
        return false;
    }

    if (Math3DTriangleCrossXCheck_general(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        *xIntersect = (((-ny * y) - (nz * z)) - originDist) / nx;
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossXLine_noCross(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 y, f32 z) {
    if (IS_ZERO(nx)) {
        return false;
    }
    if (Math3DTriangleCrossXCheck_general(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossXLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                       f32 z, f32* xIntersect, f32 x0, f32 x1) {
    static Vec3f pos;

    f32 pointADist;
    f32 pointBDist;

    if (IS_ZERO(nx)) {
        return false;
    }

    pos.x = x0;
    pos.y = y;
    pos.z = z;
    pointADist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);

    pos.x = x1;
    pointBDist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);

    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        return false;
    }

    if (Math3DTriangleCrossXCheck_general(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        *xIntersect = (((-ny * y) - (nz * z)) - originDist) / nx;
        return true;
    }
    return false;
}

s32 Math3DTrianglePFCrossXLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 y, f32 z, f32 chkDist) {
    if (IS_ZERO(plane->normal.x)) {
        return false;
    }
    if (Math3DTriangleCrossXCheck_general(v0, v1, v2, y, z, 0.0f, chkDist, plane->normal.x)) {
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossZCheck_general(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 x, f32 y, f32 detMax, f32 chkDist, f32 nz) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    if (!Math3DCheck3PointMinMaxAreaInside2D(v0->x, v0->y, v1->x, v1->y, v2->x, v2->y, x, y, chkDist)) {
        return false;
    }

    chkDistSq = SQ(chkDist);

    if (((SQ(x - v0->x) + SQ(y - v0->y)) < chkDistSq) || ((SQ(x - v1->x) + SQ(y - v1->y)) < chkDistSq) ||
        ((SQ(x - v2->x) + SQ(y - v2->y)) < chkDistSq)) {
        // Distance from any vertex to a point is less than chkDist
        return true;
    }

    detv0v1 = ((v0->x - x) * (v1->y - y)) - ((v0->y - y) * (v1->x - x));
    detv1v2 = ((v1->x - x) * (v2->y - y)) - ((v1->y - y) * (v2->x - x));
    detv2v0 = ((v2->x - x) * (v0->y - y)) - ((v2->y - y) * (v0->x - x));

    if (((detMax >= detv0v1) && (detMax >= detv1v2) && (detMax >= detv2v0)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(nz) > 0.5f) {

        if (Math3D_pointVsLineSegmentLengthSquare2D(x, y, v0->x, v0->y, v1->x, v1->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(x, y, v1->x, v1->y, v2->x, v2->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_pointVsLineSegmentLengthSquare2D(x, y, v2->x, v2->y, v0->x, v0->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }

    return false;
}

s32 Math3DTriangleCrossZCheck_chparam(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 x, f32 y, f32 detMax, f32 nz) {
    return Math3DTriangleCrossZCheck_general(v0, v1, v2, x, y, detMax, 1.0f, nz);
}

s32 Math3DTriangleCrossZLine(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                     f32 y, f32* zIntersect) {

    if (IS_ZERO(nz)) {
        return false;
    }

    if (Math3DTriangleCrossZCheck_general(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        *zIntersect = (f32)((((-nx * x) - (ny * y)) - originDist) / nz);
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossZLine_noCross(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nz, f32 x, f32 y) {
    if (IS_ZERO(nz)) {
        return false;
    }
    if (Math3DTriangleCrossZCheck_general(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        return true;
    }
    return false;
}

s32 Math3DTriangleCrossZLine_scope(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                       f32 y, f32* zIntersect, f32 z0, f32 z1) {
    static Vec3f pos;

    f32 pointADist;
    f32 pointBDist;

    if (IS_ZERO(nz)) {
        return false;
    }
    pos.x = x;
    pos.y = y;
    pos.z = z0;
    pointADist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);

    pos.z = z1;
    pointBDist = Math3DPlaneFunc(nx, ny, nz, originDist, &pos);
    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        // points on the line segment are on the same side of the plane
        return false;
    }

    if (Math3DTriangleCrossZCheck_general(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        *zIntersect = (((-nx * x) - (ny * y)) - originDist) / nz;
        return true;
    }
    return false;
}

s32 Math3DTrianglePFCrossZLine_noCross_sukimaR(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 x, f32 y, f32 chkDist) {
    if (IS_ZERO(plane->normal.z)) {
        return false;
    }
    if (Math3DTriangleCrossZCheck_general(v0, v1, v2, x, y, 0.0f, chkDist, plane->normal.z)) {
        return true;
    }
    return false;
}

s32 Math3DCheckInfPlaneCrossInfLine_proc(f32 pointADist, f32 pointBDist, Vec3f* pointA, Vec3f* pointB, Vec3f* intersect) {
    f32 distDiff;

    distDiff = pointADist - pointBDist;
    if (IS_ZERO(distDiff)) {
        // both points lie on the plane.
        *intersect = *pointB;
        return false;
    }

    if (pointADist == 0.0f) {
        // pointA is on the plane
        *intersect = *pointA;
    } else if (pointBDist == 0.0f) {
        // pointB is on the plane
        *intersect = *pointB;
    } else {
        // place the point at the intersection point.
        Math3DInDivPos2(pointA, pointB, pointADist / distDiff, intersect);
    }
    return true;
}

/**
 * Determines if the line segment from `linePointA` to `linePointB` crosses the plane
 * from `nx` + `ny` + `nz` + `originDist` = 0.  If fromFront is set, then detection will only
 * be true if point A crosses from the front of the plane
 */
s32 Math3DCheckPlaneCrossLine_xyz_t_cp(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA, Vec3f* linePointB,
                          Vec3f* intersect, s32 fromFront) {
    f32 pointADist;
    f32 pointBDist;

    pointADist = Math3DPlaneFunc(nx, ny, nz, originDist, linePointA);
    pointBDist = Math3DPlaneFunc(nx, ny, nz, originDist, linePointB);

    if ((pointADist * pointBDist) > 0.0f) {
        *intersect = *linePointB;
        return false;
    }

    if (fromFront && (pointADist < 0.0f) && (pointBDist > 0.0f)) {
        *intersect = *linePointB;
        return false;
    }

    return Math3DCheckInfPlaneCrossInfLine_proc(pointADist, pointBDist, linePointA, linePointB, intersect);
}

/*
 * Determines if the line formed by `linePointA` and `linePointB` intersect with Triangle formed from
 * vertices `v0`, `v1`, and `v2` with mult_vec vector `nx`, `ny`, and `nz` with plane distance from origin
 * `originDist` Outputs the intersection point at to `intersect`
 * Returns 1 if the line intersects with the triangle, 0 otherwise
 */
s32 Math3DCheckTriangleCrossLine_xyz_t_cp(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA,
                            Vec3f* linePointB, Vec3f* intersect, s32 fromFront) {

    if (!Math3DCheckPlaneCrossLine_xyz_t_cp(nx, ny, nz, originDist, linePointA, linePointB, intersect, fromFront)) {
        return false;
    }

    if (((nx == 0.0f) || (Math3DTriangleCrossXLine_noCross(v0, v1, v2, nx, intersect->y, intersect->z))) &&
        ((ny == 0.0f) || (Math3DTriangleCrossYLine_noCross(v0, v1, v2, ny, intersect->z, intersect->x))) &&
        ((nz == 0.0f) || (Math3DTriangleCrossZLine_noCross(v0, v1, v2, nz, intersect->x, intersect->y)))) {
        return true;
    }

    *intersect = *linePointB;
    return false;
}

/*
 * Creates a TriNorm output to `tri`, and calculates the mult_vec vector and plane from vertices
 * `va`, `vb`, and `vc`
 */
void Math3D_triangle_ct(TriNorm* tri, Vec3f* va, Vec3f* vb, Vec3f* vc) {
    tri->vtx[0] = *va;
    tri->vtx[1] = *vb;
    tri->vtx[2] = *vc;
    Math3DPlane(va, vb, vc, &tri->plane.normal.x, &tri->plane.normal.y, &tri->plane.normal.z,
                    &tri->plane.originDist);
}

/*
 * Determines if point `point` lies within `sphere`
 */
s32 Math3D_sphereCollisionPoint(Sphere16* sphere, Vec3f* point) {
    if (Math3DLength_s_f(&sphere->center, point) < sphere->radius) {
        return true;
    }
    return false;
}

/**
 * Determines the distance from point (`x0`,`y0`) to the line formed from (`x1`,`y1`) and (`x2`,`y2`)
 * Distance squared is output to `lineLenSq`, returns true if the point perpendicular from (`x0`,`y0`)
 * is contained within the segment between (`x1`,`y1`) and (`x2`,`y2`)
 */
s32 Math3D_pointVsLineSegmentLengthSquare2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32* lineLenSq) {
    static Vec3f h;

    f32 perpendicularRatio;
    f32 xDiff;
    f32 distSq;
    f32 yDiff;
    s32 ret = false;

    xDiff = x2 - x1;
    yDiff = y2 - y1;
    distSq = SQ(xDiff) + SQ(yDiff);
    if (IS_ZERO(distSq)) {
        *lineLenSq = 0.0f;
        return false;
    }

    perpendicularRatio = (((x0 - x1) * xDiff) + (y0 - y1) * yDiff) / distSq;
    if (perpendicularRatio >= 0.0f && perpendicularRatio <= 1.0f) {
        ret = true;
    }
    h.x = (xDiff * perpendicularRatio) + x1;
    h.y = (yDiff * perpendicularRatio) + y1;
    *lineLenSq = SQ(h.x - x0) + SQ(h.y - y0);
    return ret;
}

/**
 * Determines if the line `line` is touching the sphere `sphere` at any point in the line.
 */
s32 Math3D_sphereCrossLineSegment(Sphere16* sphere, Linef* line) {
    static Vec3f h;

    Vec3f lineDiff;
    f32 temp_f0_2;
    f32 lineLenSq;

    if ((Math3D_sphereCollisionPoint(sphere, &line->a)) || (Math3D_sphereCollisionPoint(sphere, &line->b))) {
        // either point of the line is in the sphere.
        return true;
    }
    lineDiff.x = line->b.x - line->a.x;
    lineDiff.y = line->b.y - line->a.y;
    lineDiff.z = line->b.z - line->a.z;

    lineLenSq = SQ(lineDiff.x) + SQ(lineDiff.y) + SQ(lineDiff.z);
    if (IS_ZERO(lineLenSq)) {
        // line length is "0"
        return false;
    }
    temp_f0_2 = ((((sphere->center.x - line->a.x) * lineDiff.x) + ((sphere->center.y - line->a.y) * lineDiff.y)) +
                 ((sphere->center.z - line->a.z) * lineDiff.z)) /
                lineLenSq;
    if ((temp_f0_2 < 0.0f) || (temp_f0_2 > 1.0f)) {
        return false;
    }

    h.x = (lineDiff.x * temp_f0_2) + line->a.x;
    h.y = (lineDiff.y * temp_f0_2) + line->a.y;
    h.z = (lineDiff.z * temp_f0_2) + line->a.z;

    if (SQ(h.x - sphere->center.x) + SQ(h.y - sphere->center.y) +
            SQ(h.z - sphere->center.z) <=
        SQ((f32)sphere->radius)) {
        return true;
    }
    return false;
}

/**
 * Gets the surface point of `sphere` intersecting with `tri` generated from the line formed from the
 * sphere's surface to the midpoint of the line formed from the first two vertices of the tri
 */
void Math3D_sphereCrossTriangleCalc_cp(Sphere16* sphere, TriNorm* tri, Vec3f* cross_pos) {
    static Vec3f tr_center;
    static Vec3f sp_center;

    f32 dist;
    f32 splitRatio;

    tr_center.x = ((tri->vtx[0].x + tri->vtx[1].x) * 0.5f);
    tr_center.y = ((tri->vtx[0].y + tri->vtx[1].y) * 0.5f);
    tr_center.z = ((tri->vtx[0].z + tri->vtx[1].z) * 0.5f);
    sp_center.x = sphere->center.x;
    sp_center.y = sphere->center.y;
    sp_center.z = sphere->center.z;
    dist = Math3DLength(&tr_center, &sp_center);
    // Distance from the sphere's center to the center of the line formed from v0->v1
    if (IS_ZERO(dist)) {
        cross_pos->x = sp_center.x;
        cross_pos->y = sp_center.y;
        cross_pos->z = sp_center.z;
        return;
    }
    splitRatio = sphere->radius / dist;
    Math3DInDivPos2(&sp_center, &tr_center, splitRatio, cross_pos);
}

/**
 * Determines if `sphere` and `tri` and touching, and outputs the intersection point to `cross_pos`
 */
s32 Math3D_sphereCrossTriangle3_cp(Sphere16* sphere, TriNorm* tri, Vec3f* cross_pos) {
    static Linef ls;
    static Vec3f center;
    static Vec3f cross;

    f32 radius;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 planeDist;

    center.x = sphere->center.x;
    center.y = sphere->center.y;
    center.z = sphere->center.z;
    radius = sphere->radius;

    if (!Math3DCheck3PointMinMaxAreaInside3D(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], &center, radius)) {
        return false;
    }

    planeDist = Math3DLengthPlaneAndPos(tri->plane.normal.x, tri->plane.normal.y, tri->plane.normal.z,
                                       tri->plane.originDist, &center);
    if (radius < planeDist) {
        // the point that lies within the plane of the triangle which is perpendicular to the sphere's center is more
        // than the radius of the sphere, the plane never crosses the sphere.
        return false;
    }

    // tests if any of the edges of the triangle are intersecting the sphere
    ls.a = tri->vtx[0];
    ls.b = tri->vtx[1];
    if (Math3D_sphereCrossLineSegment(sphere, &ls)) {
        Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
        return true;
    }

    ls.a = tri->vtx[1];
    ls.b = tri->vtx[2];
    if (Math3D_sphereCrossLineSegment(sphere, &ls)) {
        Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
        return true;
    }

    ls.a = tri->vtx[2];
    ls.b = tri->vtx[0];
    if (Math3D_sphereCrossLineSegment(sphere, &ls)) {
        Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
        return true;
    }

    nx = tri->plane.normal.x * planeDist;
    ny = tri->plane.normal.y * planeDist;
    nz = tri->plane.normal.z * planeDist;

    if (Math3DPlaneFunc(tri->plane.normal.x, tri->plane.normal.y, tri->plane.normal.z, tri->plane.originDist,
                      &center) > 0.0f) {
        cross.x = center.x - nx;
        cross.y = center.y - ny;
        cross.z = center.z - nz;
    } else {
        cross.x = center.x + nx;
        cross.y = center.y + ny;
        cross.z = center.z + nz;
    }

    if (fabsf(tri->plane.normal.y) > 0.5f) {
        if (Math3DTriangleCrossYCheck_chparam(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], cross.z, cross.x,
                                               0.0f, tri->plane.normal.y)) {
            Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
            return true;
        }
    } else if (fabsf(tri->plane.normal.x) > 0.5f) {
        if (Math3DTriangleCrossXCheck_chparam(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], cross.y, cross.z,
                                               0.0f, tri->plane.normal.x)) {
            Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
            return true;
        }
    } else if (Math3DTriangleCrossZCheck_chparam(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], cross.x,
                                                  cross.y, 0.0f, tri->plane.normal.z)) {
        Math3D_sphereCrossTriangleCalc_cp(sphere, tri, cross_pos);
        return true;
    }
    return false;
}

/*
 * Checks if point `point` is within cylinder `cyl`
 * Returns 1 if the point is inside the cylinder, 0 otherwise.
 */
s32 Math3D_pipeVsPos(Cylinder16* cyl, Vec3f* point) {
    f32 bottom;
    f32 top;
    f32 x;
    f32 z;

    x = cyl->pos.x - point->x;
    z = cyl->pos.z - point->z;
    bottom = (f32)cyl->pos.y + cyl->yShift;
    top = cyl->height + bottom;

    if ((SQ(x) + SQ(z)) < SQ(cyl->radius) && (bottom < point->y) && (point->y < top)) {
        return true;
    } else {
        return false;
    }
}

s32 Math3D_pipeCrossLine(Cylinder16* cyl, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersectA, Vec3f* intersectB) {
    Vec3f cylToPtA;
    Vec3f cylToPtB;
    Vec3f ptAToPtB;
    f32 fracA;
    f32 fracB;
    f32 fracBase;
    f32 zero = 0.0f;
    f32 pad;
    f32 cylRadiusSq;
    f32 radSqDiff;
    f32 distCent2;
    f32 dot2AB;
    s32 sideIntA;
    s32 sideIntB;
    s32 intBeyondA;
    s32 intBeyondB;
    s32 intFlags = 0;
    Vec3f intPts[4];
    s32 count;
    s32 i;

    if (Math3D_pipeVsPos(cyl, linePointA) && Math3D_pipeVsPos(cyl, linePointB)) {
        // both points are in the cylinder
        *intersectA = *linePointA;
        *intersectB = *linePointB;
        return 2;
    }

    cylToPtA.x = linePointA->x - cyl->pos.x;
    cylToPtA.y = linePointA->y - cyl->pos.y - cyl->yShift;
    cylToPtA.z = linePointA->z - cyl->pos.z;
    cylToPtB.x = linePointB->x - cyl->pos.x;
    cylToPtB.y = linePointB->y - cyl->pos.y - cyl->yShift;
    cylToPtB.z = linePointB->z - cyl->pos.z;
    xyz_t_sub(&cylToPtB, &cylToPtA, &ptAToPtB);
    cylRadiusSq = SQ(cyl->radius);

    /**
     * This section checks for intersections with the cylinder's base and top
     */
    if (!IS_ZERO(ptAToPtB.y)) {
        // fraction of length along AB to reach y = 0
        fracBase = -cylToPtA.y / ptAToPtB.y;
        if ((0.0f <= fracBase) && (fracBase <= 1.0f)) {
            f32 baseIntX = (ptAToPtB.x * fracBase) + cylToPtA.x;
            f32 baseIntZ = (ptAToPtB.z * fracBase) + cylToPtA.z;

            if (SQ(baseIntX) + SQ(baseIntZ) < cylRadiusSq) {
                // adds base intersection point to intPts and sets its flag
                intPts[0].x = cyl->pos.x + baseIntX;
                intPts[0].y = (f32)cyl->pos.y + cyl->yShift;
                intPts[0].z = cyl->pos.z + baseIntZ;
                intFlags |= 1;
            }
        }
        // fraction of length along AB to reach y = cyl->height
        fracA = (cyl->height - cylToPtA.y) / ptAToPtB.y;
        if ((0.0f <= fracA) && (fracA <= 1.0f)) {
            f32 topIntX = ptAToPtB.x * fracA + cylToPtA.x;
            f32 topIntZ = ptAToPtB.z * fracA + cylToPtA.z;

            if (SQ(topIntX) + SQ(topIntZ) < cylRadiusSq) {
                // adds top intersection point to intPts and sets its flag
                intPts[1].x = cyl->pos.x + topIntX;
                intPts[1].y = (f32)cyl->pos.y + cyl->yShift + cyl->height;
                intPts[1].z = cyl->pos.z + topIntZ;
                intFlags |= 2;
            }
        }
    }
    /**
     * This section finds the points of intersection of the infinite line containing AB with the side of the infinite
     * cylinder containing cyl. Intersection points beyond the bounds of the segment and cylinder are filtered out
     * afterward.
     */
    radSqDiff = SQXZ(cylToPtA) - cylRadiusSq;
    if (!IS_ZERO(2.0f * SQXZ(ptAToPtB))) {
        dot2AB = 2.0f * DOTXZ(ptAToPtB, cylToPtA);
        if (SQ(dot2AB) < 4.0f * SQXZ(ptAToPtB) * radSqDiff) {
            // Line's closest xz-approach is outside cylinder. No intersections.
            return 0;
        }
        if (SQ(dot2AB) - (4.0f * SQXZ(ptAToPtB) * radSqDiff) > zero) {
            sideIntA = sideIntB = 1;
        } else {
            // Line is tangent in xz-plane. At most 1 side intersection.
            sideIntA = 1;
            sideIntB = 0;
        }
        distCent2 = sqrtf(SQ(dot2AB) - (4.0f * SQXZ(ptAToPtB) * radSqDiff));
        if (sideIntA == 1) {
            // fraction of length along AB for side intersection closer to A
            fracA = (distCent2 - dot2AB) / (2.0f * SQXZ(ptAToPtB));
        }
        if (sideIntB == 1) {
            // fraction of length along AB for side intersection closer to B
            fracB = (-dot2AB - distCent2) / (2.0f * SQXZ(ptAToPtB));
        }
    } else if (!IS_ZERO(2.0f * DOTXZ(ptAToPtB, cylToPtA))) {
        // Used if the line segment is nearly vertical. Unclear what it's calculating.
        fracA = -radSqDiff / (2.0f * DOTXZ(ptAToPtB, cylToPtA));
        sideIntA = 1;
        sideIntB = 0;
    } else {
        return 0;
    }
    // checks for intersection points outside the bounds of the segment
    if (!sideIntB) {
        if (fracA < 0.0f || 1.0f < fracA) {
            return 0;
        }
    } else {
        intBeyondA = fracA < 0.0f || 1.0f < fracA;
        intBeyondB = fracB < 0.0f || 1.0f < fracB;
        if (intBeyondA && intBeyondB) {
            return 0;
        }
        if (intBeyondA) {
            sideIntA = 0;
        }
        if (intBeyondB) {
            sideIntB = 0;
        }
    }
    // checks for intersection points outside the bounds of the cylinder
    if ((sideIntA == 1) &&
        ((fracA * ptAToPtB.y + cylToPtA.y) < 0.0f || cyl->height < (fracA * ptAToPtB.y + cylToPtA.y))) {
        sideIntA = 0;
    }
    if ((sideIntB == 1) &&
        ((fracB * ptAToPtB.y + cylToPtA.y) < 0.0f || cyl->height < (fracB * ptAToPtB.y + cylToPtA.y))) {
        sideIntB = 0;
    }
    if (sideIntA == 0 && sideIntB == 0) {
        return 0;
    }
    // Adds intersection points to intPts and sets side A and side B flags
    if (sideIntA == 1 && sideIntB == 1) {
        intPts[2].x = (fracA * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracA * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracA * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
        intPts[3].x = (fracB * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[3].y = (fracB * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[3].z = (fracB * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 8;
    } else if (sideIntA == 1) {
        intPts[2].x = (fracA * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracA * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracA * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
    } else if (sideIntB == 1) {
        intPts[2].x = (fracB * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracB * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracB * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
    }

    /**
     * Places the found intersection points into intersectA and intersectB. IntersectA is always closer to point A
     */
    for (count = 0, i = 0; i < 4; i++) {
        if (intFlags & (1 << i)) {
            if (count == 0) {
                *intersectA = intPts[i];
            } else if (count == 1) {
                if (Math3DLengthSquare(intersectA, linePointA) < Math3DLengthSquare(intersectA, &intPts[i])) {
                    *intersectB = intPts[i];
                } else {
                    *intersectB = *intersectA;
                    *intersectA = intPts[i];
                }
                break;
            }
            count++;
        }
    }
    return count;
}

/*
 * Determines if `cyl` and `tri` are touching.  The point of intersection
 * is placed in `intersect` Returns 1 if they are touching, 0 otherwise.
 */
s32 Math3D_pipeCrossTriangle_cp(Cylinder16* cyl, TriNorm* tri, Vec3f* intersect) {
    static Sphere16 sph1;
    static Sphere16 sph2;
    static Vec3f cross1;
    static Vec3f cross2;

    f32 yIntersect;
    f32 cylTop;
    f32 cylBottom;
    f32 minDistSq;
    f32 radiusTodistFromCylYIntersectTov0v1;
    f32 distFromPointAToIntersectASq;
    Vec3f cylIntersectCenter;
    Vec3f midpointv0v1;
    Vec3f diffMidpointIntersect;
    s32 pad;

    cylBottom = (f32)cyl->pos.y + cyl->yShift;
    cylTop = cyl->height + cylBottom;

    if (((tri->vtx[0].y < cylBottom) && (tri->vtx[1].y < cylBottom) && (tri->vtx[2].y < cylBottom)) ||
        ((cylTop < tri->vtx[0].y) && (cylTop < tri->vtx[1].y) && (cylTop < tri->vtx[2].y))) {
        // If all of the vertices are below or all of the vertices are above the cylinder.
        return false;
    }

    minDistSq = 1.e38f;
    if (Math3D_pipeCrossLine(cyl, &tri->vtx[0], &tri->vtx[1], &cross1, &cross2)) {
        distFromPointAToIntersectASq = Math3DLengthSquare(&cross1, &tri->vtx[0]);
        minDistSq = distFromPointAToIntersectASq;
        *intersect = cross1;
    }

    if (Math3D_pipeCrossLine(cyl, &tri->vtx[2], &tri->vtx[1], &cross1, &cross2)) {
        distFromPointAToIntersectASq = Math3DLengthSquare(&cross1, &tri->vtx[2]);
        if (distFromPointAToIntersectASq < minDistSq) {
            *intersect = cross1;
            minDistSq = distFromPointAToIntersectASq;
        }
    }

    if (Math3D_pipeCrossLine(cyl, &tri->vtx[0], &tri->vtx[2], &cross1, &cross2)) {
        distFromPointAToIntersectASq = Math3DLengthSquare(&cross1, &tri->vtx[0]);
        if (distFromPointAToIntersectASq < minDistSq) {
            *intersect = cross1;
            minDistSq = distFromPointAToIntersectASq;
        }
    }

    if (minDistSq != 1.e38f) {
        return true;
    }

    if (Math3DTriangleCrossYLine_scope(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], tri->plane.normal.x,
                                           tri->plane.normal.y, tri->plane.normal.z, tri->plane.originDist, cyl->pos.z,
                                           cyl->pos.x, &yIntersect, cylBottom, cylTop)) {
        f32 distFromCylYIntersectTov0v1;

        cylIntersectCenter.x = cyl->pos.x;
        cylIntersectCenter.y = yIntersect;
        cylIntersectCenter.z = cyl->pos.z;

        midpointv0v1.x = (tri->vtx[0].x + tri->vtx[1].x) * 0.5f;
        midpointv0v1.y = (tri->vtx[0].y + tri->vtx[1].y) * 0.5f;
        midpointv0v1.z = (tri->vtx[0].z + tri->vtx[1].z) * 0.5f;

        xyz_t_sub(&midpointv0v1, &cylIntersectCenter, &diffMidpointIntersect);
        distFromCylYIntersectTov0v1 = sqrtf(SQ(diffMidpointIntersect.x) + SQ(diffMidpointIntersect.z));

        if (IS_ZERO(distFromCylYIntersectTov0v1)) {
            xyz_t_move(intersect, &midpointv0v1);
            return true;
        }

        radiusTodistFromCylYIntersectTov0v1 = cyl->radius / distFromCylYIntersectTov0v1;
        Math3DInDivPos1(&cylIntersectCenter, &diffMidpointIntersect, radiusTodistFromCylYIntersectTov0v1,
                                   intersect);
        return true;
    }

    sph1.center.x = sph2.center.x = cyl->pos.x;
    sph1.center.z = sph2.center.z = cyl->pos.z;
    sph1.center.y = cylTop;
    sph2.center.y = cylBottom;
    sph1.radius = sph2.radius = cyl->radius;

    if ((Math3D_sphereCrossTriangle3_cp(&sph1, tri, intersect)) ||
        (Math3D_sphereCrossTriangle3_cp(&sph2, tri, intersect))) {
        return true;
    }
    return false;
}

/*
 * Determines if `cyl` and `tri` are touching.
 */
s32 Math3D_pipeCrossTriangle(Cylinder16* cyl, TriNorm* tri) {
    Vec3f intersect;

    return Math3D_pipeCrossTriangle_cp(cyl, tri, &intersect);
}

/*
 * Determines if two spheres are touching.
 */
s32 Math3D_sphereCrossSphere(Sphere16* sphereA, Sphere16* sphereB) {
    f32 overlapSize;

    return Math3D_sphereCrossSphere_cl(sphereA, sphereB, &overlapSize);
}

/*
 * Determines if two spheres are touching.  The amount that they're overlapping is placed in `overlapSize`
 */
s32 Math3D_sphereCrossSphere_cl(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize) {
    f32 centerDist;

    return Math3D_sphereCrossSphere_cl_cc(sphereA, sphereB, overlapSize, &centerDist);
}

/*
 * Determines if two spheres are touching  The distance from the centers is placed in `centerDist`,
 * and the amount that they're overlapping is placed in `overlapSize`
 */
s32 Math3D_sphereCrossSphere_cl_cc(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize, f32* centerDist) {
    Vec3f diff;

    diff.x = (f32)sphereA->center.x - (f32)sphereB->center.x;
    diff.y = (f32)sphereA->center.y - (f32)sphereB->center.y;
    diff.z = (f32)sphereA->center.z - (f32)sphereB->center.z;

    *centerDist = sqrt(SQ(diff.x) + SQ(diff.y) + SQ(diff.z));

    *overlapSize = (((f32)sphereA->radius + (f32)sphereB->radius) - *centerDist);
    if (*overlapSize > 0.008f) {
        return true;
    }

    *overlapSize = 0.0f;
    return false;
}

/**
 * Checks if `sph` and `cyl` are touching, output the amount of xz overlap to `overlapSize`
 */
s32 Math3D_sphereVsPipe_cl(Sphere16* sph, Cylinder16* cyl, f32* overlapSize) {
    f32 centerDist;

    return Math3D_sphereVsPipe_cl_cc(sph, cyl, overlapSize, &centerDist);
}

/**
 * Checks if `sph` and `cyl` are touching, output the xz distance of the centers to `centerDist`, and the amount of
 * xz overlap to `overlapSize`
 */
s32 Math3D_sphereVsPipe_cl_cc(Sphere16* sph, Cylinder16* cyl, f32* overlapSize, f32* centerDist) {
    static Cylinderf pipeF;
    static Spheref sphereF;

    f32 x;
    f32 z;
    f32 combinedRadius;
    f32 cylBottom;
    f32 cylTop;
    f32 sphBottom;
    f32 sphTop;

    if (sph->radius <= 0 || cyl->radius <= 0) {
        // either radius is 0
        return false;
    }
    sphereF.center.y = sph->center.y;
    sphereF.radius = sph->radius;
    pipeF.pos.y = cyl->pos.y;
    pipeF.yShift = cyl->yShift;
    pipeF.height = cyl->height;
    x = (f32)sph->center.x - cyl->pos.x;
    z = (f32)sph->center.z - cyl->pos.z;
    combinedRadius = (f32)sph->radius + cyl->radius;
    *centerDist = sqrtf(SQ(x) + SQ(z));
    if (combinedRadius < *centerDist) {
        // if the combined radii is less than the distance to the centers, they cannot be touching.
        return false;
    }

    cylBottom = (pipeF.pos.y + pipeF.yShift);
    cylTop = cylBottom + pipeF.height;
    sphBottom = sphereF.center.y - sphereF.radius;
    sphTop = sphereF.center.y + sphereF.radius;

    if ((sphTop >= cylBottom) && (sphBottom <= cylTop)) {
        // if the cylinder and sphere are intersecting on the xz plane, check if they're intersecting on
        // the y axis.
        *overlapSize = combinedRadius - *centerDist;
        return true;
    }
    return false;
}

/**
 * Checks if `ca` and `cb` are touching, output the amount of xz overlap to `overlapSize`
 */
s32 Math3D_pipeVsPipe_cl(Cylinder16* ca, Cylinder16* cb, f32* overlapSize) {
    f32 xzDist;

    return Math3D_pipeVsPipe_cl_cc(ca, cb, overlapSize, &xzDist);
}

/**
 * Checks if `ca` and `cb` are touching, output the xz distance of the centers to `centerDist`, and the amount of
 * xz overlap to `overlapSize`
 */
s32 Math3D_pipeVsPipe_cl_cc(Cylinder16* ca, Cylinder16* cb, f32* overlapSize, f32* centerDist) {
    static Cylinderf pipe1f;
    static Cylinderf pipe2f;

    xyz_t_move_s_xyz(&pipe1f.pos, &ca->pos);
    pipe1f.radius = ca->radius;
    pipe1f.yShift = ca->yShift;
    pipe1f.height = ca->height;

    xyz_t_move_s_xyz(&pipe2f.pos, &cb->pos);
    pipe2f.radius = cb->radius;
    pipe2f.yShift = cb->yShift;
    pipe2f.height = cb->height;

    *centerDist = sqrtf(SQ(pipe1f.pos.x - pipe2f.pos.x) + SQ(pipe1f.pos.z - pipe2f.pos.z));

    // The combined radii are within the xz distance
    if ((pipe1f.radius + pipe2f.radius) < *centerDist) {
        return false;
    }

    // top of ca < bottom of cb or top of cb < bottom of ca
    if (((pipe1f.pos.y + pipe1f.yShift) + pipe1f.height) < (pipe2f.pos.y + pipe2f.yShift) ||
        (((pipe2f.pos.y + pipe2f.yShift) + pipe2f.height) < (pipe1f.pos.y + pipe1f.yShift))) {
        return false;
    }

    *overlapSize = pipe1f.radius + pipe2f.radius - *centerDist;
    return true;
}

/*
 * Determines if triangle `ta` intersects with triangle `tb` the point of
 * intersection is output to `intersect.
 * Returns 1 is the triangles intersect, 0 otherwise
 */
s32 Math3D_triangleCrossTriangle_cp(TriNorm* ta, TriNorm* tb, Vec3f* intersect) {
    f32 dist0;
    f32 dist1;
    f32 dist2;

    dist0 = T_PolygonF_planeFunc(&ta->plane, &tb->vtx[0]);
    dist1 = T_PolygonF_planeFunc(&ta->plane, &tb->vtx[1]);
    dist2 = T_PolygonF_planeFunc(&ta->plane, &tb->vtx[2]);

    if (((dist0 > 0.0f) && (dist1 > 0.0f) && (dist2 > 0.0f)) ||
        (((dist0 < 0.0f) && (dist1 < 0.0f)) && (dist2 < 0.0f))) {
        return false;
    }

    dist0 = T_PolygonF_planeFunc(&tb->plane, &ta->vtx[0]);
    dist1 = T_PolygonF_planeFunc(&tb->plane, &ta->vtx[1]);
    dist2 = T_PolygonF_planeFunc(&tb->plane, &ta->vtx[2]);

    if ((((dist0 > 0.0f) && (dist1 > 0.0f)) && (dist2 > 0.0f)) ||
        ((dist0 < 0.0f) && (dist1 < 0.0f) && (dist2 < 0.0f))) {
        return false;
    }

    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[0], &ta->vtx[1], intersect, 0)) {
        return true;
    }
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[1], &ta->vtx[2], intersect, 0)) {
        return true;
    }
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[2], &ta->vtx[0], intersect, 0)) {
        return true;
    }
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[0], &tb->vtx[1], intersect,
                                0) == 1) {
        return true;
    }
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[1], &tb->vtx[2], intersect,
                                0) == 1) {
        return true;
    }
    if (Math3DCheckTriangleCrossLine_xyz_t_cp(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[2], &tb->vtx[0], intersect,
                                0) == 1) {
        return true;
    }
    return false;
}

s32 Math3D_sphereCrossXZLine(Sphere16* sphere, f32 x, f32 z) {
    f32 xDiff;
    f32 zDiff;

    xDiff = sphere->center.x - x;
    zDiff = sphere->center.z - z;
    if ((SQ(xDiff) + SQ(zDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}

s32 Math3D_sphereCrossXYLine(Sphere16* sphere, f32 x, f32 y) {
    f32 xDiff;
    f32 yDiff;

    xDiff = sphere->center.x - x;
    yDiff = sphere->center.y - y;
    if ((SQ(xDiff) + SQ(yDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}

s32 Math3D_sphereCrossYZLine(Sphere16* sphere, f32 y, f32 z) {
    f32 yDiff;
    f32 zDiff;

    yDiff = sphere->center.y - y;
    zDiff = sphere->center.z - z;
    if ((SQ(yDiff) + SQ(zDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}
