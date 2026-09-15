#include <nitro.h>
#include <string.h>

#include "party.h"

#include "assert.h"
#include "heap.h"
#include "pokemon.h"
#include "savedata.h"


static inline void Party_AssertValidSlot(Party* party, int slot) {
	GF_ASSERT(slot >= 0);                   
	GF_ASSERT(slot < party->currentCount);
	GF_ASSERT(slot < party->capacity);    
}


int Party_GetSaveSize(void) {
	return sizeof(Party);
}


void Party_Init(Party* party) {
	Party_InitWithCapacity(party, MAX_PARTY_SIZE);
}


void Party_InitWithCapacity(Party* party, int capacity) {
	int i;
	
	GF_ASSERT(capacity <= MAX_PARTY_SIZE);
	memset(party, 0, sizeof(Party));
	
	party->currentCount = 0;
	party->capacity = capacity;
	
	for (i = 0; i < MAX_PARTY_SIZE; i++) {
		Pokemon_Init(&party->pokemon[i]);
	}
}


BOOL Party_AddPokemon(Party* party, Pokemon* mon) {
	if (party->currentCount >= party->capacity) {
		return FALSE;
	}
	
	party->pokemon[party->currentCount] = *mon;
	party->currentCount++;
	return TRUE;
}


BOOL Party_RemovePokemonBySlotIndex(Party* party, int slot) {
	Party_AssertValidSlot(party, slot);
	GF_ASSERT(party->currentCount > 0);
	
	int i;
	for (i = slot; i < party->currentCount - 1; i++) {
		party->pokemon[i] = party->pokemon[i + 1];
	}
	
	Pokemon_Init(&party->pokemon[i]);
	party->currentCount--;
	return TRUE;
}


int Party_GetCurrentCount(const Party* party) {
	return party->currentCount;
}


Pokemon* Party_GetPokemonBySlotIndex(Party* party, int slot) {
	Party_AssertValidSlot(party, slot);
	return &party->pokemon[slot];
}


void Party_AddPokemonBySlotIndex(Party* party, int slot, Pokemon* mon) {
	Party_AssertValidSlot(party, slot);
	
	int addOrRemoveSlots = Pokemon_GetValue(&(party->pokemon[slot]), MON_DATA_SPECIES_EXISTS, NULL) - Pokemon_GetValue(mon, MON_DATA_SPECIES_EXISTS, NULL);
	party->pokemon[slot] = *mon;
	party->currentCount += addOrRemoveSlots;
}


BOOL Party_HasSpecies(Party* party, int species) {
	int i;
	for (i = 0; i < party->currentCount; i++) {
		if (Pokemon_GetValue(&party->pokemon[i], MON_DATA_SPECIES, NULL) == species) {
			break;
		}
	}
	
	return i != party->currentCount;
}


Party* SaveData_GetParty(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_PARTY);
}
