#include <nitro.h>
#include <string.h>

#include "pokedex.h"

#include "assert.h"
#include "game_version.h"
#include "pokedex_language.h"
#include "pokemon.h"
#include "savedata.h"
#include "constants/forms.h"
#include "constants/species.h"

static int NumFormsSeen_TwoForms(const PokedexCommon* pokedexData, u32 species);
static BOOL FormSeen_TwoForms(const PokedexCommon* pokedexData, u32 species, u8 form);
static void UpdateForms_TwoForms(PokedexCommon* pokedexData, u32 species, int form);
static int NumFormsSeen_ThreeForms(const PokedexCommon* pokedexData, u32 species);
static BOOL FormSeen_ThreeForms(const PokedexCommon* pokedexData, u32 species, u8 form);
static void UpdateForms_ThreeForms(PokedexCommon* pokedexData, u32 species, int form);
static void UpdateFormArray_Deoxys(PokedexCommon* pokedexData, u8 form, u8 bitIndex);
static u32 NumFormsSeen_Deoxys(const PokedexCommon* pokedexData);
static BOOL FormSeen_Deoxys(const PokedexCommon* pokedexData, u32 form);
static void UpdateForms_Deoxys(PokedexCommon* pokedexData, u16 species, Pokemon* mon);
static void InitDeoxys(PokedexCommon* pokedexData);
static void UpdateForm(PokedexCommon* pokedexData, u16 species, Pokemon* mon);
static void UpdateLanguage(PokedexCommon* pokedexData, u16 species, u32 language);


int Pokedex_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(PokedexDP);
	} else {
		return sizeof(PokedexPt);
	}
}


int Pokedex_GetSaveSizeDP(void) {
	return sizeof(PokedexDP);
}


void Pokedex_Copy(const void* src, void* dest) {
	MI_CpuCopy8(src, dest, Pokedex_GetSaveSize());
}


void Pokedex_CopyAsDP(const void* src, PokedexDP* dest) {
	if (gIsDiamondPearl) {
		Pokedex_Copy(src, dest);
	} else {
		MI_CpuClear16(dest, sizeof(PokedexDP));
		
		// canDetectForms is the first differing field
		MI_CpuCopy16(src, dest, (u32)&dest->canDetectForms - (u32)dest);
		
		// It and all fields afterward copied manually
		PokedexPt* ptSrc = (PokedexPt*)src;
		dest->canDetectForms      = ptSrc->canDetectForms;
		dest->canDetectLanguages  = ptSrc->canDetectLanguages;
		dest->pokedexObtained     = ptSrc->pokedexObtained;
		dest->nationalDexObtained = ptSrc->nationalDexObtained;
	}
}


static inline BOOL ReadBit_2Forms(const u8* array, u16 bitIndex) {
	bitIndex--;
	return 0 != (array[bitIndex >> 3] & (1 << (bitIndex & 7)));
}


static inline void ActivateBit_2Forms(u8 *array, u16 bitIndex) {
	bitIndex--;
	array[bitIndex >> 3] |= (1 << (bitIndex & 7));
}


static inline void SetBit_2Forms(u8* array, u8 value, u16 bitIndex) {
	GF_ASSERT(value < 2);
	
	bitIndex--;
	array[bitIndex >> 3] &= ~(1 << (bitIndex & 7));
	array[bitIndex >> 3] |= (value << (bitIndex & 7));
}


static inline u32 ReadBit_3Forms(const u8* array, u16 bitIndex) {
	return (array[bitIndex >> 2] >> ((bitIndex & 3) * 2)) & 3;
}


static inline void SetBit_3Forms(u8* array, u8 value, u16 bitIndex) {
	GF_ASSERT(value < 4);
	
	array[bitIndex >> 2] &= ~(3 << ((bitIndex & 3) * 2));
	array[bitIndex >> 2] |= (value << ((bitIndex & 3) * 2));
}


static inline void Write_SeenSpecies(PokedexCommon *pokedexData, u16 species) {
	ActivateBit_2Forms((u8*)pokedexData->seenPokemon, species);
}


static inline void Write_CaughtSpecies(PokedexCommon *pokedexData, u16 species) {
	ActivateBit_2Forms((u8*)pokedexData->caughtPokemon, species);
}


static inline void SetBit_Gender(PokedexCommon* pokedexData, u8 gender, u8 isSeen, u16 bitIndex) {
	if (isSeen == FALSE) {
		SetBit_2Forms((u8*)pokedexData->recordedGenders[1], gender, bitIndex);
	}
	
	SetBit_2Forms((u8*)pokedexData->recordedGenders[isSeen], gender, bitIndex);
}


static inline void UpdateGender(PokedexCommon* pokedexData, u8 gender, u8 isSeen, u16 bitIndex) {
	GF_ASSERT(gender <= GENDER_NONE);
	
	if (gender == GENDER_NONE) {
		gender = GENDER_MALE;
	}
	
	SetBit_Gender(pokedexData, gender, isSeen, bitIndex);
}


static inline u8 GetGender(const PokedexCommon* pokedexData, u16 species, u8 bitIndex) {
	return ReadBit_2Forms((const u8*)pokedexData->recordedGenders[bitIndex], species);
}


static inline int NumFormsSeen_Unown(const PokedexCommon* pokedexData) {
	int formIndex;
	for (formIndex = 0; formIndex < UNOWN_FORM_COUNT; formIndex++) {
		if (pokedexData->unownFormsSeen[formIndex] == 0xFF) {
			break;
		}
	}
	
	return formIndex;
}


static inline BOOL UnownFormSeen(const PokedexCommon* pokedexData, u8 form) {
	for (int formIndex = 0; formIndex < UNOWN_FORM_COUNT; formIndex++) {
		if (pokedexData->unownFormsSeen[formIndex] == form) {
			return TRUE;
		}
	}
	
	return FALSE;
}


static inline void SetUnownForm(PokedexCommon* pokedexData, int form) {
	if (UnownFormSeen(pokedexData, form)) {
		return;
	}
	
	int numUnownSeen = NumFormsSeen_Unown(pokedexData);
	if (numUnownSeen < UNOWN_FORM_COUNT) {
		pokedexData->unownFormsSeen[numUnownSeen] = form;
	}
}


static inline void SetForm_Spinda(PokedexCommon* pokedexData, u16 species, u32 personality) {
	if (species == SPECIES_SPINDA) {
		pokedexData->spindaForm = personality;
	}
}


static inline u32 ReadBit_Deoxys(const u32* array, u8 bitIndex) {
	u32 bitOffset = 24 + (bitIndex * 4);
	return (array[DEX_SIZE_U32 - 1] >> bitOffset) & 0x0F;
}


static inline void WriteBit_Deoxys(u32* array, u8 value, u8 bitIndex) {
	u32 bitOffset = (24 + (bitIndex * 4));
	u32 emptyBits = ~(0x0F << bitOffset);
	array[DEX_SIZE_U32 - 1] &= emptyBits;
	array[DEX_SIZE_U32 - 1] |= (value << bitOffset);
}


static inline u32 GetForm_Deoxys(const PokedexCommon* pokedexData, u8 formIndex) {
	u32 form;
	if (formIndex < 2) {
		form = ReadBit_Deoxys(pokedexData->caughtPokemon, formIndex);
	} else {
		form = ReadBit_Deoxys(pokedexData->seenPokemon, formIndex - 2);
	}
	
	return form;
}


static inline void CheckPokedexIntegrity(const PokedexCommon* pokedexData) {
	GF_ASSERT(pokedexData->magic == DEX_MAGIC_NUMBER);
}


static inline BOOL SpeciesInvalid(u16 species) {
	if (species == SPECIES_NONE || species > NATIONAL_DEX_COUNT) {
		GF_ASSERT(FALSE);
		return TRUE;
	} else {
		return FALSE;
	}
}


static inline BOOL SpeciesSeen(const PokedexCommon* pokedexData, u16 species) {
	return ReadBit_2Forms((const u8*)pokedexData->seenPokemon, species);
}


static int NumFormsSeen_TwoForms(const PokedexCommon* pokedexData, u32 species) {
	GF_ASSERT((species == SPECIES_SHELLOS) || (species == SPECIES_GASTRODON));
	
	if (Pokedex_HasSeenSpecies(pokedexData, species) == FALSE) {
		return 0;
	}
	
	const u8* formArray;
	if (species == SPECIES_SHELLOS) {
		formArray = &pokedexData->shellosFormsSeen;
	} else {
		formArray = &pokedexData->gastrodonFormsSeen;
	}
	
	u32 form_1 = ReadBit_2Forms(formArray, 1);
	u32 form_2 = ReadBit_2Forms(formArray, 2);
	
	if (form_1 == form_2) {
		return 1;
	}
	
	return 2;
}


static BOOL FormSeen_TwoForms(const PokedexCommon* pokedexData, u32 species, u8 form) {
	GF_ASSERT((species == SPECIES_SHELLOS) || (species == SPECIES_GASTRODON));
	
	if (Pokedex_HasSeenSpecies(pokedexData, species) == FALSE) {
		return FALSE;
	}
	
	const u8* formArray;
	if (species == SPECIES_SHELLOS) {
		formArray = &pokedexData->shellosFormsSeen;
	} else {
		formArray = &pokedexData->gastrodonFormsSeen;
	}
	
	u32 numFormsSeen = NumFormsSeen_TwoForms(pokedexData, species);
	
	for (u32 formIndex = 0; formIndex < numFormsSeen; formIndex++) {
		u32 currentForm = ReadBit_2Forms(formArray, formIndex + 1);
		
		if (currentForm == form) {
			return TRUE;
		}
	}
	
	return FALSE;
}


static void UpdateForms_TwoForms(PokedexCommon* pokedexData, u32 species, int form) {
	GF_ASSERT((species == SPECIES_SHELLOS) || (species == SPECIES_GASTRODON));
	
	if (FormSeen_TwoForms(pokedexData, species, form)) {
		return;
	}
	
	u8* formArray;
	if (species == SPECIES_SHELLOS) {
		formArray = &pokedexData->shellosFormsSeen;
	} else {
		formArray = &pokedexData->gastrodonFormsSeen;
	}
	
	int numFormsSeen = NumFormsSeen_TwoForms(pokedexData, species);
	
	if (numFormsSeen < 2) {
		SetBit_2Forms(formArray, form, numFormsSeen + 1);
		
		if (numFormsSeen == 0) {
			SetBit_2Forms(formArray, form, numFormsSeen + 2);
		}
	}
}


static int NumFormsSeen_ThreeForms(const PokedexCommon* pokedexData, u32 species) {
	GF_ASSERT(species == SPECIES_BURMY || species == SPECIES_WORMADAM);
	
	if (Pokedex_HasSeenSpecies(pokedexData, species) == FALSE) {
		return 0;
	}
	
	const u8* formArray;
	if (species == SPECIES_BURMY) {
		formArray = &pokedexData->burmyFormsSeen;
	} else {
		formArray = &pokedexData->wormadamFormsSeen;
	}
	
	int formIndex;
	for (formIndex = 0; formIndex < 3; formIndex++) {
		u32 currentForm = ReadBit_3Forms(formArray, formIndex);
		
		if (currentForm == 3) {
			break;
		}
	}
	
	return formIndex;
}


static BOOL FormSeen_ThreeForms(const PokedexCommon* pokedexData, u32 species, u8 form) {
	GF_ASSERT(species == SPECIES_BURMY || species == SPECIES_WORMADAM);
	
	if (Pokedex_HasSeenSpecies(pokedexData, species) == FALSE) {
		return FALSE;
	}
	
	const u8* formArray;
	if (species == SPECIES_BURMY) {
		formArray = &pokedexData->burmyFormsSeen;
	} else {
		formArray = &pokedexData->wormadamFormsSeen;
	}
	
	for (int formIndex = 0; formIndex < 3; formIndex++) {
		u32 currentForm = ReadBit_3Forms(formArray, formIndex);
		
		if (currentForm == form) {
			return TRUE;
		}
	}
	
	return FALSE;
}


static void UpdateForms_ThreeForms(PokedexCommon* pokedexData, u32 species, int form) {
	GF_ASSERT(species == SPECIES_BURMY || species == SPECIES_WORMADAM);
	
	if (FormSeen_ThreeForms(pokedexData, species, form)) {
		return;
	}
	
	u8* formArray;
	if (species == SPECIES_BURMY) {
		formArray = &pokedexData->burmyFormsSeen;
	} else {
		formArray = &pokedexData->wormadamFormsSeen;
	}
	
	int numFormsSeen = NumFormsSeen_ThreeForms(pokedexData, species);
	
	if (numFormsSeen < 3) {
		SetBit_3Forms(formArray, form, numFormsSeen);
	}
}


static void UpdateFormArray_Deoxys(PokedexCommon* pokedexData, u8 form, u8 bitIndex) {
	GF_ASSERT(bitIndex < DEOXYS_FORM_COUNT);
	GF_ASSERT(form <= 0x0F);
	
	// Deoxys forms are stored in spare bits in these arrays
	if (bitIndex < 2) {
		WriteBit_Deoxys(pokedexData->caughtPokemon, form, bitIndex);
	} else {
		WriteBit_Deoxys(pokedexData->seenPokemon, form, bitIndex - 2);
	}
}


static u32 NumFormsSeen_Deoxys(const PokedexCommon* pokedexData) {
	int formIndex;
	for (formIndex = 0; formIndex < DEOXYS_FORM_COUNT; formIndex++) {
		if (GetForm_Deoxys(pokedexData, formIndex) == 0x0F) {
			break;
		}
	}
	
	return formIndex;
}


static BOOL FormSeen_Deoxys(const PokedexCommon* pokedexData, u32 form) {
	for (int formIndex = 0; formIndex < DEOXYS_FORM_COUNT; formIndex++) {
		if (GetForm_Deoxys(pokedexData, formIndex) == form) {
			return TRUE;
		}
	}
	
	return FALSE;
}


static void UpdateForms_Deoxys(PokedexCommon* pokedexData, u16 species, Pokemon* mon) {
	u8 form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
	if (species == SPECIES_DEOXYS) {
		form = Pokemon_SanitizeFormId(species, form);
		if (FormSeen_Deoxys(pokedexData, form) == FALSE) {
			u32 newIndex = NumFormsSeen_Deoxys(pokedexData);
			UpdateFormArray_Deoxys(pokedexData, form, newIndex);
		}
	}
}


static void InitDeoxys(PokedexCommon* pokedexData) {
	for (int formIndex = 0; formIndex < DEOXYS_FORM_COUNT; formIndex++) {
		UpdateFormArray_Deoxys(pokedexData, 0x0F, formIndex);
	}
}


static void UpdateForm(PokedexCommon* pokedexData, u16 species, Pokemon* mon) {
	int form;
	if (species == SPECIES_UNOWN) {
		form = Pokemon_GetForm(mon);
		form = Pokemon_SanitizeFormId(species, form);
		SetUnownForm(pokedexData, form);
	
	} else if (species == SPECIES_BURMY) {
		form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
		form = Pokemon_SanitizeFormId(species, form);
		UpdateForms_ThreeForms(pokedexData, species, form);
	
	} else if (species == SPECIES_WORMADAM) {
		form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
		form = Pokemon_SanitizeFormId(species, form);
		UpdateForms_ThreeForms(pokedexData, species, form);
	
	} else if (species == SPECIES_SHELLOS) {
		form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
		form = Pokemon_SanitizeFormId(species, form);
		UpdateForms_TwoForms(pokedexData, species, form);
		
	} else if (species == SPECIES_GASTRODON) {
		form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
		form = Pokemon_SanitizeFormId(species, form);
		UpdateForms_TwoForms(pokedexData, species, form);
	
	} else if (species == SPECIES_DEOXYS) {
		UpdateForms_Deoxys(pokedexData, species, mon);
	}
	
	// Shaymin, Giratina, Rotom unimplemented
}


static void UpdateLanguage(PokedexCommon* pokedexData, u16 species, u32 language) {
	if (gIsDiamondPearl) {
		PokedexDP* pokedexDataDP = (PokedexDP*)pokedexData;
		
		int bitIndex = PokedexLanguage_SpeciesAlternateLanguageIdx(species);
		int languageIndex = PokedexLanguage_LanguageToIndex(language);
		
		if (bitIndex == DP_LANGUAGE_COUNT || languageIndex == NUM_LANGUAGES) {
			return;
		}
		
		pokedexDataDP->recordedLanguages[bitIndex] |= (1 << languageIndex);
	} else {
		PokedexPt* pokedexDataPt = (PokedexPt*)pokedexData;
		
		int bitIndex = species;
		int languageIndex = PokedexLanguage_LanguageToIndex(language);
		
		if (languageIndex == NUM_LANGUAGES) {
			return;
		}
		
		pokedexDataPt->recordedLanguages[bitIndex] |= (1 << languageIndex);
	}
}


void Pokedex_Init(PokedexCommon* pokedexData) {
	// Clear smaller of the two structure sizes
	memset(pokedexData, 0, sizeof(PokedexDP));
	
	pokedexData->magic = DEX_MAGIC_NUMBER;
	
	if (gIsDiamondPearl) {
		((PokedexDP*)pokedexData)->nationalDexObtained = FALSE;
	} else {
		((PokedexPt*)pokedexData)->nationalDexObtained = FALSE;
	}
	
	memset(pokedexData->unownFormsSeen, 0xFF, sizeof(u8) * UNOWN_FORM_COUNT);
	
	pokedexData->shellosFormsSeen   = 0xFF;
	pokedexData->gastrodonFormsSeen = 0xFF;
	pokedexData->burmyFormsSeen     = 0xFF;
	pokedexData->wormadamFormsSeen  = 0xFF;
	InitDeoxys(pokedexData);
}


BOOL Pokedex_HasSeenSpecies(const PokedexCommon* pokedex, u16 species) {
	CheckPokedexIntegrity(pokedex);
	
	if (SpeciesInvalid(species)) {
		return FALSE;
	}
	
	return SpeciesSeen(pokedex, species);
}


void Pokedex_Capture(PokedexCommon* pokedexData, Pokemon* mon) {
	u16 species     = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
	u32 language    = Pokemon_GetValue(mon, MON_DATA_LANGUAGE, NULL);
	u32 personality = Pokemon_GetValue(mon, MON_DATA_PERSONALITY, NULL);
	u32 gender      = Pokemon_GetGender(mon);
	
	CheckPokedexIntegrity(pokedexData);
	
	if (SpeciesInvalid(species)) {
		return;
	}
	
	if (!SpeciesSeen(pokedexData, species)) {
		SetForm_Spinda(pokedexData, species, personality);
		UpdateGender(pokedexData, gender, FALSE, species);
	} else {
		u32 displayedGender = GetGender(pokedexData, species, 0);
		
		if (displayedGender != gender) {
			UpdateGender(pokedexData, gender, TRUE, species);
		}
	}
	
	UpdateForm(pokedexData, species, mon);
	UpdateLanguage(pokedexData, species, language);
	
	Write_CaughtSpecies(pokedexData, species);
	Write_SeenSpecies(pokedexData, species);
}


BOOL Pokedex_IsNationalDexObtained(const PokedexCommon* pokedex) {
	CheckPokedexIntegrity(pokedex);
	
	if (gIsDiamondPearl) {
		return ((PokedexDP*)pokedex)->nationalDexObtained;
	} else {
		return ((PokedexPt*)pokedex)->nationalDexObtained;
	}
}


PokedexCommon* SaveData_GetPokedex(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_POKEDEX);
}
