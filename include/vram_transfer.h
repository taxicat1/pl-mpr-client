#ifndef MPR_VRAM_TRANSFER_H
#define MPR_VRAM_TRANSFER_H

#include <nnsys/gfd.h>

#include "constants/heap.h"

void VramTransfer_New(u32 capacity, HeapID heapID);
void VramTransfer_Free(void);
BOOL VramTransfer_Request(NNS_GFD_DST_TYPE type, u32 destAddr, void* buf, u32 size);
void VramTransfer_Process(void);

#endif /* MPR_VRAM_TRANSFER_H */