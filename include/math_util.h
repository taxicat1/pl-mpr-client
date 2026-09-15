#ifndef MPR_MATH_UTIL_H
#define MPR_MATH_UTIL_H

#include <nitro/fx/fx.h>

#include "constants/heap.h"

#define LCRNG_MULTIPLIER  1103515245L
#define LCRNG_INCREMENT   24691
#define LCRNG_DIVISOR     65536L

#define F32_DEG_TO_IDX(degrees)  FX_DEG_TO_IDX(FX32_CONST(degrees))

// Does almost the same as F32_DEG_TO_IDX but takes integers as input instead of float
// Use if FX_DEG_TO_IDX(FX32_CONST(degrees)) doesn't match
#define DEG_TO_IDX(degrees)  (((degrees) * 0xFFFF) / 360)

typedef enum {
    AFFINE_MODE_NORMAL = 0,
    AFFINE_MODE_MAX_256,
    AFFINE_MODE_MAX_360
} AffineTransformationMatrixMode;

typedef struct {
    f32  x;
    f32  y;
} Vec2F32;

typedef struct {
    s16  x;
    s16  y;
} Point2D;

extern const fx32 gFxSinCosTable[];

fx32 CalcSineDegrees(u16 degrees);
fx32 CalcSineDegrees_Wraparound(u16 degrees);
void LCRNG_SetSeed(u32 seed);
u16 LCRNG_Next(void);
void MTRNG_SetSeed(u32 seed);
u32 MTRNG_Next(void);
void CreateAffineTransformationMatrix(MtxFx22* matrix, u16 degrees, fx32 xScale, fx32 yScale, u8 mode);

#endif /* MPR_MATH_UTIL_H */
