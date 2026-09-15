#ifndef MPR_POKEDEX_H
#define MPR_POKEDEX_H

#include <stddef.h>

#include "constants/species.h"
#include "constants/forms.h"

#include "heap.h"
#include "pokemon.h"
#include "savedata.h"

#define DEX_SIZE_U32  ((int)((NATIONAL_DEX_COUNT - 1) / 32) + 1)

#define DEX_MAGIC_NUMBER   0xBEEFCAFE

#define PT_LANGUAGE_COUNT  (MAX_SPECIES + 1)
#define DP_LANGUAGE_COUNT  14

typedef struct {
	u32  magic;
	u32  caughtPokemon[DEX_SIZE_U32];
	u32  seenPokemon[DEX_SIZE_U32];
	u32  recordedGenders[2][DEX_SIZE_U32];
	u32  spindaForm;
	u8   shellosFormsSeen;
	u8   gastrodonFormsSeen;
	u8   burmyFormsSeen;
	u8   wormadamFormsSeen;
	u8   unownFormsSeen[UNOWN_FORM_COUNT];
	u8   recordedLanguages[PT_LANGUAGE_COUNT];
	u8   canDetectForms;
	u8   canDetectLanguages;
	u8   pokedexObtained;
	u8   nationalDexObtained;
	u32  rotomFormsSeen;
	u8   shayminFormsSeen;
	u8   giratinaFormsSeen;
} PokedexPt;

typedef struct {
	u32  magic;
	u32  caughtPokemon[DEX_SIZE_U32];
	u32  seenPokemon[DEX_SIZE_U32];
	u32  recordedGenders[2][DEX_SIZE_U32];
	u32  spindaForm;
	u8   shellosFormsSeen;
	u8   gastrodonFormsSeen;
	u8   burmyFormsSeen;
	u8   wormadamFormsSeen;
	u8   unownFormsSeen[UNOWN_FORM_COUNT];
	u8   canDetectForms;
	u8   recordedLanguages[DP_LANGUAGE_COUNT];
	u8   canDetectLanguages;
	u8   pokedexObtained;
	u8   nationalDexObtained;
} PokedexDP;

typedef struct {
	u32  magic;
	u32  caughtPokemon[DEX_SIZE_U32];
	u32  seenPokemon[DEX_SIZE_U32];
	u32  recordedGenders[2][DEX_SIZE_U32];
	u32  spindaForm;
	u8   shellosFormsSeen;
	u8   gastrodonFormsSeen;
	u8   burmyFormsSeen;
	u8   wormadamFormsSeen;
	u8   unownFormsSeen[UNOWN_FORM_COUNT];
} PokedexCommon;

int Pokedex_GetSaveSize(void);
int Pokedex_GetSaveSizeDP(void);
void Pokedex_Copy(const void* src, void* dest);
void Pokedex_CopyAsDP(const void* src, PokedexDP* dest);
void Pokedex_Init(PokedexCommon* pokedex);
BOOL Pokedex_HasSeenSpecies(const PokedexCommon* pokedex, u16 species);
void Pokedex_Capture(PokedexCommon* pokedex, Pokemon* mon);
BOOL Pokedex_IsNationalDexObtained(const PokedexCommon* pokedex);
PokedexCommon* SaveData_GetPokedex(SaveData* saveData);

#endif /* MPR_POKEDEX_H */
