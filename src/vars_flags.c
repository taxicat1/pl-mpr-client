#include <nitro.h>
#include <string.h>

#include "vars_flags.h"

#include "assert.h"

#define GET_FLAG_BIT(flagID) (1 << (flagID % 8))
#define NUM_UNKNOWN_FLAGS  64

static u8 sUnknownFlags[NUM_UNKNOWN_FLAGS / 8];


int VarsFlags_GetSaveSize(void) {
	return sizeof(VarsFlags);
}


void VarsFlags_Init(VarsFlags* varsFlags) {
	memset(varsFlags, 0, sizeof(VarsFlags));
}


VarsFlags* SaveData_GetVarsFlags(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_VARS_FLAGS);
}


BOOL VarsFlags_CheckFlag(VarsFlags* varsFlags, u16 flagID) {
	u8* flagChunk = VarsFlags_GetFlagChunk(varsFlags, flagID);
	return flagChunk && *flagChunk & GET_FLAG_BIT(flagID);
}


#define VARS_START  16384

u8 *VarsFlags_GetFlagChunk(VarsFlags* varsFlags, u16 flagID) {
	if (flagID == 0) {
		return NULL;
	} else if (flagID < VARS_START) {
		GF_ASSERT(flagID / 8 < NUM_FLAGS / 8);
		return &varsFlags->flags[flagID / 8];
	} else {
		GF_ASSERT((flagID - VARS_START) / 8 < NUM_UNKNOWN_FLAGS / 8);
		return &(sUnknownFlags[(flagID - VARS_START) / 8]);
	}
}