#ifndef MPR_POKEDEX_LANGUAGE_H
#define MPR_POKEDEX_LANGUAGE_H

#define NUM_LANGUAGES  6

#include <nitro/types.h>

int PokedexLanguage_LanguageToIndex(int language);
int PokedexLanguage_SpeciesAlternateLanguageIdx(u16 species);

#endif /* MPR_POKEDEX_LANGUAGE_H */
