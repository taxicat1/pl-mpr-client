#include <nitro.h>

#include "record_mixed_rng.h"

#include "constants/charcode.h"

RecordMixedRNG* sRNGCollection; // Unused


int RecordMixedRNG_GetSaveSize() {
	return sizeof(RecordMixedRNG) * RECORD_MIXED_RNG_MAX;
}


void RecordMixedRNG_Init(RecordMixedRNG* rngCollection) {
	MI_CpuClearFast(rngCollection, sizeof(RecordMixedRNG) * RECORD_MIXED_RNG_MAX);
	
	for (int entry = 0; entry < RECORD_MIXED_RNG_MAX; entry++) {
		rngCollection[entry].groupName[0] = CHAR_EOS;
		rngCollection[entry].playerName[0] = CHAR_EOS;
	}
	
	sRNGCollection = rngCollection;
}
