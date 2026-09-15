#ifndef MPR_PARTY_H
#define MPR_PARTY_H

#include "constants/heap.h"
#include "constants/pokemon.h"
#include "struct_defs/pokemon.h"

#include "savedata.h"

typedef struct {
	int      capacity;
	int      currentCount;
	Pokemon  pokemon[MAX_PARTY_SIZE];
} Party;

int Party_GetSaveSize(void);
void Party_Init(Party* party);
void Party_InitWithCapacity(Party* party, int capacity);
BOOL Party_AddPokemon(Party* party, Pokemon* mon);
BOOL Party_RemovePokemonBySlotIndex(Party* party, int slot);
int Party_GetCurrentCount(const Party* party);
Pokemon* Party_GetPokemonBySlotIndex(Party* party, int slot);
void Party_AddPokemonBySlotIndex(Party* party, int slot, Pokemon* mon);
BOOL Party_HasSpecies(Party* party, int species);
Party* SaveData_GetParty(SaveData* saveData);

#endif /* MPR_PARTY_H */
