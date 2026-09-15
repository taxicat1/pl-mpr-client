#ifndef MPR_POKEMON_MEMO_H
#define MPR_POKEMON_MEMO_H

#include <nitro/types.h>

#include "string_gf.h"
#include "pokemon.h"
#include "trainer_info.h"
#include "message.h"
#include "string_template.h"

typedef struct {
	int      lineNum;
	String*  text;
} PokemonMemoText;

typedef struct {
	HeapID           heapID;
	MessageLoader*   messageLoader;
	StringTemplate*  stringTemplate;
	Pokemon*         mon;
	BOOL             monOTMatches;
	
	PokemonMemoText  nature;
	PokemonMemoText  metLocation;
	PokemonMemoText  characteristic;
	PokemonMemoText  flavorPreference;
	PokemonMemoText  eggHatchingStatus;
} PokemonMemo;

typedef enum {
	MET_TYPE_NORMAL = 0,
	MET_TYPE_IN_GAME_TRADE,
	MET_TYPE_PAL_PARK,
	MET_TYPE_DAY_CARE_EGG,
	MET_TYPE_EVENT,
	MET_TYPE_LINK_TRADE,
	MET_TYPE_EGG_HATCHED
} PokemonMemoMetType;

PokemonMemo* PokemonMemo_New(Pokemon* mon, BOOL monOTMatches, HeapID heapID);
void PokemonMemo_Free(PokemonMemo* memo);

#endif /* MPR_POKEMON_MEMO_H */
