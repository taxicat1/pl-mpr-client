#ifndef MPR_HEAP_H
#define MPR_HEAP_H

#include <nnsys.h>

#include "constants/heap.h"

typedef struct {
	u32        size;
	OSArenaId  arena;
} HeapParam;

void Heap_InitSystem(const HeapParam* templates, u32 nTemplates, u32 totalNumHeaps, u32 preSize);
BOOL Heap_Create(HeapID parent, HeapID child, u32 size);
void Heap_Destroy(HeapID heapID);
void* Heap_Alloc(u32 heapID, u32 size);
void* Heap_AllocAtEnd(u32 heapID, u32 size);
void Heap_Free(void* ptr);
void Heap_FreeExplicit(u32 heapID, void* ptr);
BOOL Heap_Check(u32 heapID);

#endif /* MPR_HEAP_H */
