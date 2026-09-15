#include <nitro.h>

#include "pal_park_transfers.h"

#include "assert.h"


int MigratedPokemon_GetSaveSize(void) {
	return sizeof(MigratedPokemon);
}


void MigratedPokemon_Init(MigratedPokemon* transferData) {
	MI_CpuClearFast(transferData->mons, sizeof(MigratedPokemon));
	for (int i = 0; i < CATCHING_SHOW_MONS; i++) {
		Pokemon_Init(&transferData->mons[i]);
		GF_ASSERT(Pokemon_GetValue(&transferData->mons[i], MON_DATA_SPECIES_EXISTS, NULL) == FALSE);
	}
}
