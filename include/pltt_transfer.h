#ifndef MPR_PLTT_TRANSFER_H
#define MPR_PLTT_TRANSFER_H

#include <nitro/types.h>
#include <nnsys.h>

#include "constants/heap.h"

typedef struct {
	NNSG2dPaletteData*  data;
	NNS_G2D_VRAM_TYPE   vramType;
	u32                 plttIndex;
	u32                 resourceID;
} PlttTransferTaskTemplate;

void PlttTransfer_Init(int capacity, HeapID heapID);
void PlttTransfer_Free(void);
void PlttTransfer_Clear(void);
BOOL PlttTransfer_RequestWholeRange(const PlttTransferTaskTemplate* template);
void PlttTransfer_ReplacePlttData(int resourceID, NNSG2dPaletteData* data);
BOOL PlttTransfer_HasTask(int resourceID);
void PlttTransfer_ResetTask(int resourceID);
void PlttTransfer_ResetAllTasks(void);
NNSG2dImagePaletteProxy* PlttTransfer_GetPaletteProxy(int resourceID);
NNSG2dImagePaletteProxy* PlttTransfer_ToggleExtPalette(int resourceID, NNSG2dImageProxy* imageProxy);
u32 PlttTransfer_GetPlttOffset(const NNSG2dImagePaletteProxy* paletteProxy, NNS_G2D_VRAM_TYPE vramType);

#endif /* MPR_PLTT_TRANSFER_H */
