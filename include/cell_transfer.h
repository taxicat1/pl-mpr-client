#ifndef MPR_CELL_TRANSFER_H
#define MPR_CELL_TRANSFER_H

#include <nnsys.h>

#include "constants/heap.h"

NNSG2dCellTransferState* CellTransfer_New(int capacity, HeapID heapID);
void CellTransfer_Free(NNSG2dCellTransferState* transferStates);

#endif /* MPR_CELL_TRANSFER_H */
