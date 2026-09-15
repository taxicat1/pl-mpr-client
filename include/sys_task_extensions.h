#ifndef MPR_SYS_TASK_EXTENSIONS_H
#define MPR_SYS_TASK_EXTENSIONS_H

#include "constants/heap.h"

#include "sys_task_manager.h"

SysTask* SysTask_StartAndAllocateParam(SysTaskFunc callback, int paramSize, u32 priority, HeapID heapID);
void SysTask_FinishAndFreeParam(SysTask* task);

#endif /* MPR_SYS_TASK_EXTENSIONS_H */
