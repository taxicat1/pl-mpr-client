#ifndef MPR_DEXMODE_CHECKER_H
#define MPR_DEXMODE_CHECKER_H

#include "pokedex.h"
#include "savedata.h"

u32 SaveData_GetDexMode(SaveData* saveData);
u32 IsNationalDexObtained(const PokedexCommon* dex);
u32 GetDexNumber(u32 isNationalDexObtained, u32 monSpecies);

#endif /* MPR_DEXMODE_CHECKER_H */
