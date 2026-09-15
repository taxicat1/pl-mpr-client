#ifndef MPR_RENDER_OAM_H
#define MPR_RENDER_OAM_H

#include <nnsys.h>

#include "constants/graphics.h"
#include "constants/heap.h"

typedef struct {
	int  mainOamStart;
	int  mainOamCount;
	int  mainAffineOamStart;
	int  mainAffineOamCount;
	int  subOamStart;
	int  subOamCount;
	int  subAffineOamStart;
	int  subAffineOamCount;
} RenderOamTemplate;

void RenderOam_Init(
	int mainOamStart,
	int mainOamCount,
	int mainAffineOamStart,
	int mainAffineOamCount,
	int subOamStart,
	int subOamCount,
	int subAffineOamStart,
	int subAffineOamCount,
	HeapID heapID
);
void RenderOam_InitSurface(NNSG2dRenderSurface* surface, NNSG2dViewRect* viewRect, NNSG2dSurfaceType surfaceType, NNSG2dRendererInstance* renderer);

void RenderOam_Transfer(void);
void RenderOam_Free(void);
void RenderOam_ClearMain(HeapID heapID);
void RenderOam_ClearSub(HeapID heapID);

#endif /* MPR_RENDER_OAM_H */
