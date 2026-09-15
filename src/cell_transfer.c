#include <nitro.h>
#include <nnsys.h>

#include "cell_transfer.h"

#include "heap.h"
#include "vram_transfer.h"

static BOOL RegisterTransferTaskCB(NNS_GFD_DST_TYPE type, u32 destAddr, void* buf, u32 size);


NNSG2dCellTransferState* CellTransfer_New(int capacity, HeapID heapID) {
	NNSG2dCellTransferState* transferStates = Heap_Alloc(heapID, sizeof(NNSG2dCellTransferState) * capacity);
	NNS_G2dInitCellTransferStateManager(transferStates, capacity, RegisterTransferTaskCB);
	return transferStates;
}


void CellTransfer_Free(NNSG2dCellTransferState* transferStates) {
	Heap_Free(transferStates);
}


static BOOL RegisterTransferTaskCB(NNS_GFD_DST_TYPE type, u32 destAddr, void* buf, u32 size) {
	return VramTransfer_Request(type, destAddr, buf, size);
}
