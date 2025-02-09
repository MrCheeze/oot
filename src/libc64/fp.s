#include "ultra64/asm.h"
#include "ultra64/regdef.h"

.data

#if !PLATFORM_N64

DATA(float_positive_infinity)
    .word 0x7F800000
ENDDATA(float_positive_infinity)

DATA(float_negative_infinity)
    .word 0xFF800000
ENDDATA(float_negative_infinity)

DATA(float_positive_zero)
    .word 0x00000000
ENDDATA(float_positive_zero)

DATA(float_negative_zero)
    .word 0x80000000
ENDDATA(float_negative_zero)

DATA(float_quiet_nan)
    .word 0x7FBFFFFF
ENDDATA(float_quiet_nan)

DATA(float_quiet_nan_set)
    .word 0x7F810000
ENDDATA(float_quiet_nan_set)

DATA(float_signaling_nan)
    .word 0x7FFFFFFF
ENDDATA(float_signaling_nan)

#endif

.text

LEAF(__ffloor)
    floor.w.s   fa0, fa0
    cvt.s.w     fv0, fa0
    jr          ra
END(__ffloor)

LEAF(__floor)
    floor.w.d   fa0, fa0
    cvt.d.w     fv0, fa0
    jr          ra
END(__floor)

LEAF(__iffloor)
    floor.w.s   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__iffloor)

LEAF(__ifloor)
    floor.w.d   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__ifloor)

LEAF(__fceil)
    ceil.w.s    fa0, fa0
    cvt.s.w     fv0, fa0
    jr          ra
END(__fceil)

LEAF(__ceil)
    ceil.w.d    fa0, fa0
    cvt.d.w     fv0, fa0
    jr          ra
END(__ceil)

LEAF(__ifceil)
    ceil.w.s    ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__ifceil)

LEAF(__iceil)
    ceil.w.d    ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__iceil)

LEAF(__ftrunc)
    trunc.w.s   fa0, fa0
    cvt.s.w     fv0, fa0
    jr          ra
END(__ftrunc)

LEAF(__trunc)
    trunc.w.d   fa0, fa0
    cvt.d.w     fv0, fa0
    jr          ra
END(__trunc)

LEAF(__iftrunc)
    trunc.w.s   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__iftrunc)

LEAF(__itrunc)
    trunc.w.d   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__itrunc)

LEAF(__fround)
    round.w.s   fa0, fa0
    cvt.s.w     fv0, fa0
    jr          ra
END(__fround)

LEAF(__round)
    round.w.d   fa0, fa0
    cvt.d.w     fv0, fa0
    jr          ra
END(__round)

LEAF(__ifround)
    round.w.s   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__ifround)

LEAF(__iround)
    round.w.d   ft0, fa0
    mfc1        v0, ft0
    jr          ra
END(__iround)

LEAF(__frint)
    li.s        ft0, 0.5
    add.s       fv0, fa0, ft0
    floor.w.s   fv0, fv0
    cvt.s.w     fv0, fv0
    jr          ra
END(__frint)

LEAF(__rint)
    li.d        ft0, 0.5
    add.d       fv0, fa0, ft0
    floor.w.d   fv0, fv0
    cvt.d.w     fv0, fv0
    jr          ra
END(__rint)

LEAF(__ifrint)
    li.s        ft0, 0.5
    add.s       fv0, fa0, ft0
    floor.w.s   fv0, fv0
    mfc1        v0, fv0
    jr          ra
END(__ifrint)

LEAF(__irint)
    li.d        ft0, 0.5
    add.d       fv0, fa0, ft0
    floor.w.d   fv0, fv0
    mfc1        v0, fv0
    jr          ra
END(__irint)
