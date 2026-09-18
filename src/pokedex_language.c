#include <nitro.h>

#include "pokedex_language.h"

#include "common.h"
#include "pokedex_language_dp.h"
#include "constants/versions.h"


static const u8 sLanguageArray[] = {
	LANGUAGE_JAPANESE,
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GERMAN,
	LANGUAGE_ITALIAN,
	LANGUAGE_SPANISH
};


int PokedexLanguage_LanguageToIndex(int language) {
	int languageIndex;
	for (languageIndex = 0; languageIndex < NUM_LANGUAGES; languageIndex++) {
		if (language == sLanguageArray[languageIndex]) {
			break;
		}
	}
	
	return languageIndex;
}


int PokedexLanguage_SpeciesAlternateLanguageIdx(u16 species) {
	// See pokedex_language_dp.c
	int pokeIndex;
	for (pokeIndex = 0; pokeIndex < POKEDEX_LANGUAGE_DP_NUM_POKES; pokeIndex++) {
		if (species == gSpeciesWithAlternateLanguagesDP[pokeIndex]) {
			break;
		}
	}
	
	return pokeIndex;
}
