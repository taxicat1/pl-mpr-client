#include <nitro.h>

#include "field_overworld_state.h"


int FieldOverworldState_GetSaveSize(void) {
	return sizeof(FieldOverworldState);
}


int FieldOverworldSave_GetSaveSize(void) {
	return sizeof(FieldOverworldSave);
}


void FieldOverworldSave_Init(FieldOverworldSave* fieldSave) {
	MI_CpuClear32(fieldSave, sizeof(FieldOverworldSave));
}


void FieldOverworldState_Init(FieldOverworldState* fieldState) {
	// Unimplemented
	return;
}
