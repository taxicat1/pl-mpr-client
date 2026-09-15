#include <nitro.h>
#include <string.h>

#include "dexmode_checker.h"

#include "pokedex.h"
#include "pokemon.h"
#include "savedata.h"


u32 SaveData_GetDexMode(SaveData* saveData) {
	PokedexCommon* dex = SaveData_GetPokedex(saveData);
	return IsNationalDexObtained(dex);
}


u32 IsNationalDexObtained(const PokedexCommon* dex) {
	BOOL isNationalDexObtained = Pokedex_IsNationalDexObtained(dex);
	return isNationalDexObtained == TRUE;
}


u32 GetDexNumber(u32 isNationalDexObtained, u32 monSpecies) {
	return (isNationalDexObtained == FALSE) ? Pokemon_SinnohDexNumber(monSpecies) : monSpecies;
}
