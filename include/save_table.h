#ifndef MPR_SAVE_TABLE_HELPERS_H
#define MPR_SAVE_TABLE_HELPERS_H

#include "constants/save_table.h"
#include "pc_boxes.h"

typedef int (*SaveEntrySizeFunc)(void);
typedef void (*SaveEntryInitFunc)(void*);

typedef struct {
	int  dataID;
	u32  blockID;
	
	SaveEntrySizeFunc  sizeFunc;
	SaveEntryInitFunc  initFunc;
} SaveTableEntry;

extern const SaveTableEntry gSaveTable[];
extern const int gSaveTableSize; // =NELEMS(gSaveTable)

PCBoxes* SaveData_GetPCBoxes(SaveData* saveData);

#endif /* MPR_SAVE_TABLE_HELPERS_H */
