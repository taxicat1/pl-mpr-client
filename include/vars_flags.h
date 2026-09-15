#ifndef MPR_VARS_FLAGS_H
#define MPR_VARS_FLAGS_H

#include "savedata.h"

// The only flag that is used (to display contest info on summary screen)
#define FLAG_CONTEST_HALL_VISITED  2424

#define NUM_VARS   288
#define NUM_FLAGS  2912

typedef struct {
	u16  vars[NUM_VARS];
	u8   flags[NUM_FLAGS / 8];
} VarsFlags;

int VarsFlags_GetSaveSize(void);
void VarsFlags_Init(VarsFlags* varsFlags);
VarsFlags* SaveData_GetVarsFlags(SaveData* saveData);
BOOL VarsFlags_CheckFlag(VarsFlags* varsFlags, u16 flagID);
u8* VarsFlags_GetFlagChunk(VarsFlags* varsFlags, u16 flagID);

#endif /* MPR_VARS_FLAGS_H */
