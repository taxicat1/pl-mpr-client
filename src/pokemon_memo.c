#include <nitro.h>

#include "pokemon_memo.h"

#include "heap.h"
#include "game_version.h"
#include "met_location_util.h"
#include "string_gf.h"
#include "string_template.h"
#include "constants/natures.h"

#include "fs/msgdata/location_names.h"
#include "fs/msgdata/special_met_location_names.h"
#include "fs/msgdata/mystery_gift_event_names.h"
#include "fs/msgdata/pokemon_summary_screen.h"

typedef enum {
	MEMO_TYPE_STANDARD = 0,
	MEMO_TYPE_STANDARD_APPARENT,
	
	MEMO_TYPE_IN_GAME_TRADE,
	
	MEMO_TYPE_TRADED_HATCHED_EGG,
	MEMO_TYPE_TRADED_HATCHED_EGG_APPARENT,
	
	MEMO_TYPE_DAYCARE_HATCHED_EGG,
	MEMO_TYPE_DAYCARE_HATCHED_EGG_APPARENT,
	
	MEMO_TYPE_FATEFUL_ENCOUNTER,
	MEMO_TYPE_FATEFUL_ENCOUNTER_APPARENT,
	
	MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG,
	MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG_APPARENT,
	
	MEMO_TYPE_POKEMON_RANGER,
	MEMO_TYPE_POKEMON_RANGER_APPARENT,
	
	MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG,
	MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG_APPARENT,
	
	MEMO_TYPE_PAL_PARK,
	
	MEMO_TYPE_DAYCARE_EGG,
	MEMO_TYPE_DAYCARE_EGG_APPARENT,
	MEMO_TYPE_GIFT_EGG,
	MEMO_TYPE_GIFT_EGG_APPARENT,
	MEMO_TYPE_RANGER_EGG
} PokemonMemoType;

typedef enum {
	FIELD_EGG_MET = 0,
	FIELD_MON_MET
} PokemonMetField;

#define MESSAGE_ID_NATURES_START  POKEMON_SUMMARY_SCREEN_00024

#define LINE_BUFFER_SIZE  72

static void SetNatureTemplate(PokemonMemo* memo);
static void SetMetLocationTemplate(PokemonMemo* memo, int messageID);
static void SetAlternateMetLocationTemplate(PokemonMemo* memo, int messageID);
static void SetEggMetLocationTemplate(PokemonMemo* memo, int messageID, PokemonMetField field);
static void SetCharacteristicTemplate(PokemonMemo* memo);
static void SetFlavorPreferenceTemplate(PokemonMemo* memo);
static void SetEggStatusTemplate(PokemonMemo* memo);
static PokemonMemoType DetermineMonMemoType(Pokemon* mon, BOOL monOTMatches, HeapID heapID);


PokemonMemo* PokemonMemo_New(Pokemon* mon, BOOL monOTMatches, HeapID heapID) {
	PokemonMemo* memo = Heap_Alloc(heapID, sizeof(PokemonMemo));
	memo->heapID = heapID;
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	memo->messageLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_POKEMON_SUMMARY_SCREEN, memo->heapID);
	memo->stringTemplate = StringTemplate_New(9, 32, memo->heapID);
	memo->mon = mon;
	memo->monOTMatches = monOTMatches;
	
	memo->nature.lineNum = 0;
	memo->nature.text = NULL;
	
	memo->metLocation.lineNum = 0;
	memo->metLocation.text = NULL;
	
	memo->characteristic.lineNum = 0;
	memo->characteristic.text = NULL;
	
	memo->flavorPreference.lineNum = 0;
	memo->flavorPreference.text = NULL;
	
	memo->eggHatchingStatus.lineNum = 0;
	memo->eggHatchingStatus.text = NULL;
	
	switch (DetermineMonMemoType(memo->mon, memo->monOTMatches, memo->heapID)) {
		case MEMO_TYPE_STANDARD:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00049);
			
			memo->characteristic.lineNum = 6;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 7;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_STANDARD_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00050);
			
			memo->characteristic.lineNum = 6;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 7;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_IN_GAME_TRADE:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00051);
			
			memo->characteristic.lineNum = 6;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 7;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_TRADED_HATCHED_EGG:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00052);
			
			memo->characteristic.lineNum = 8;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 9;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_TRADED_HATCHED_EGG_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00053);
			
			memo->characteristic.lineNum = 8;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 9;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_DAYCARE_HATCHED_EGG:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00054);
			
			memo->characteristic.lineNum = 8;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 9;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_DAYCARE_HATCHED_EGG_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00055);
			
			memo->characteristic.lineNum = 8;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 9;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_FATEFUL_ENCOUNTER:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00056);
			
			memo->characteristic.lineNum = 7;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 8;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_FATEFUL_ENCOUNTER_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00057);
			
			memo->characteristic.lineNum = 7;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 8;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00058);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00059);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_POKEMON_RANGER:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00060);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_POKEMON_RANGER_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00061);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00062);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG_APPARENT:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00063);
			
			memo->characteristic.lineNum = 9;
			SetCharacteristicTemplate(memo);
			break;
		
		case MEMO_TYPE_PAL_PARK:
			memo->nature.lineNum = 1;
			SetNatureTemplate(memo);
			
			memo->metLocation.lineNum = 2;
			SetAlternateMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00064);
			
			memo->characteristic.lineNum = 6;
			SetCharacteristicTemplate(memo);
			
			memo->flavorPreference.lineNum = 7;
			SetFlavorPreferenceTemplate(memo);
			break;
		
		case MEMO_TYPE_DAYCARE_EGG:
			memo->metLocation.lineNum = 1;
			SetEggMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00101, FIELD_EGG_MET);
			
			memo->eggHatchingStatus.lineNum = 6;
			SetEggStatusTemplate(memo);
			break;
		
		case MEMO_TYPE_DAYCARE_EGG_APPARENT:
			memo->metLocation.lineNum = 1;
			SetEggMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00102, FIELD_MON_MET);
			
			memo->eggHatchingStatus.lineNum = 6;
			SetEggStatusTemplate(memo);
			break;
		
		case MEMO_TYPE_GIFT_EGG:
			memo->metLocation.lineNum = 1;
			SetEggMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00103, FIELD_EGG_MET);
			
			memo->eggHatchingStatus.lineNum = 6;
			SetEggStatusTemplate(memo);
			break;
		
		case MEMO_TYPE_GIFT_EGG_APPARENT:
			memo->metLocation.lineNum = 1;
			SetEggMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00103, FIELD_MON_MET);
			
			memo->eggHatchingStatus.lineNum = 6;
			SetEggStatusTemplate(memo);
			break;
		
		case MEMO_TYPE_RANGER_EGG:
			memo->metLocation.lineNum = 1;
			SetEggMetLocationTemplate(memo, POKEMON_SUMMARY_SCREEN_00104, FIELD_EGG_MET);
			
			memo->eggHatchingStatus.lineNum = 6;
			SetEggStatusTemplate(memo);
			break;
	}
	
	return memo;
}


void PokemonMemo_Free(PokemonMemo* memo) {
	if (memo->nature.text != NULL) {
		Heap_Free(memo->nature.text);
	}
	
	if (memo->metLocation.text != NULL) {
		Heap_Free(memo->metLocation.text);
	}
	
	if (memo->characteristic.text != NULL) {
		Heap_Free(memo->characteristic.text);
	}
	
	if (memo->flavorPreference.text != NULL) {
		Heap_Free(memo->flavorPreference.text);
	}
	
	if (memo->eggHatchingStatus.text != NULL) {
		Heap_Free(memo->eggHatchingStatus.text);
	}
	
	StringTemplate_Free(memo->stringTemplate);
	MessageLoader_Free(memo->messageLoader);
	Heap_Free(memo);
}


static void SetNatureTemplate(PokemonMemo* memo) {
	int nature = Pokemon_GetNature(memo->mon);
	if (nature > NATURE_QUIRKY) {
		return;
	}
	
	memo->nature.text = String_Init(LINE_BUFFER_SIZE, memo->heapID);
	MessageLoader_GetString(memo->messageLoader, MESSAGE_ID_NATURES_START + nature, memo->nature.text);
}


static void SetMetLocationTemplate(PokemonMemo* memo, int messageID) {
	String* tmp = String_Init(LINE_BUFFER_SIZE * 8, memo->heapID);
	
	memo->metLocation.text = String_Init(LINE_BUFFER_SIZE * 8, memo->heapID);
	
	MessageLoader_GetString(memo->messageLoader, messageID, tmp);
	
	// Year is formatted as 20__
	StringTemplate_SetNumber(memo->stringTemplate, 0, Pokemon_GetValue(memo->mon, MON_DATA_MET_YEAR, NULL), 2, PADDING_MODE_ZEROES, CHARSET_MODE_EN);
	StringTemplate_SetMonthName(memo->stringTemplate, 1, Pokemon_GetValue(memo->mon, MON_DATA_MET_MONTH, NULL));
	StringTemplate_SetNumber(memo->stringTemplate, 2, Pokemon_GetValue(memo->mon, MON_DATA_MET_DAY, NULL), 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
	
	StringTemplate_SetNumber(memo->stringTemplate, 3, Pokemon_GetValue(memo->mon, MON_DATA_MET_LEVEL, NULL), 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
	
	StringTemplate_SetMetLocationName(memo->stringTemplate, 4, Pokemon_GetValue(memo->mon, MON_DATA_MET_LOCATION, NULL));
	
	StringTemplate_SetNumber(memo->stringTemplate, 5, Pokemon_GetValue(memo->mon, MON_DATA_EGG_YEAR, NULL), 2, PADDING_MODE_ZEROES, CHARSET_MODE_EN);
	StringTemplate_SetMonthName(memo->stringTemplate, 6, Pokemon_GetValue(memo->mon, MON_DATA_EGG_MONTH, NULL));
	StringTemplate_SetNumber(memo->stringTemplate, 7, Pokemon_GetValue(memo->mon, MON_DATA_EGG_DAY, NULL), 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
	
	StringTemplate_SetMetLocationName(memo->stringTemplate, 8, Pokemon_GetValue(memo->mon, MON_DATA_EGG_LOCATION, NULL));
	
	StringTemplate_Format(memo->stringTemplate, memo->metLocation.text, tmp);
	String_Free(tmp);
}


static void SetAlternateMetLocationTemplate(PokemonMemo* memo, int messageID) {
	String* tmp = String_Init(LINE_BUFFER_SIZE * 4, memo->heapID);
	
	memo->metLocation.text = String_Init(LINE_BUFFER_SIZE * 4, memo->heapID);
	
	MessageLoader_GetString(memo->messageLoader, messageID, tmp);
	
	// Year is formatted as 20__
	StringTemplate_SetNumber(memo->stringTemplate, 0, Pokemon_GetValue(memo->mon, MON_DATA_MET_YEAR,  NULL), 2, PADDING_MODE_ZEROES, CHARSET_MODE_EN);
	StringTemplate_SetMonthName(memo->stringTemplate, 1, Pokemon_GetValue(memo->mon, MON_DATA_MET_MONTH, NULL));
	StringTemplate_SetNumber(memo->stringTemplate, 2, Pokemon_GetValue(memo->mon, MON_DATA_MET_DAY,   NULL), 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
	StringTemplate_SetNumber(memo->stringTemplate, 3, Pokemon_GetValue(memo->mon, MON_DATA_MET_LEVEL, NULL), 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
	
	switch (Pokemon_GetValue(memo->mon, MON_DATA_MET_GAME, NULL)) {
		default:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Empty));
			break;
		
		case VERSION_FIRERED:
		case VERSION_LEAFGREEN:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Kanto));
			break;
		
		case VERSION_HEARTGOLD:
		case VERSION_SOULSILVER:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Johto));
			break;
		
		case VERSION_RUBY:
		case VERSION_SAPPHIRE:
		case VERSION_EMERALD:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Hoenn));
			break;
		
		case VERSION_GAMECUBE:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_DistantLand));
			break;
		
		case VERSION_DIAMOND:
		case VERSION_PEARL:
			StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Empty));
			break;
		
		case VERSION_PLATINUM:
			if (!gIsDiamondPearl) {
				StringTemplate_SetMetLocationName(memo->stringTemplate, 4, MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Empty));
			}
			break;
	}
	
	StringTemplate_Format(memo->stringTemplate, memo->metLocation.text, tmp);
	String_Free(tmp);
}


static void SetEggMetLocationTemplate(PokemonMemo* memo, int messageID, PokemonMetField field) {
	String* tmp = String_Init(LINE_BUFFER_SIZE * 5, memo->heapID);
	
	memo->metLocation.text = String_Init(LINE_BUFFER_SIZE * 5, memo->heapID);
	
	MessageLoader_GetString(memo->messageLoader, messageID, tmp);
	
	if (field == FIELD_EGG_MET) {
		// Year is formatted as 20__
		StringTemplate_SetNumber(memo->stringTemplate, 5, Pokemon_GetValue(memo->mon, MON_DATA_EGG_YEAR, NULL), 2, PADDING_MODE_ZEROES, CHARSET_MODE_EN);
		StringTemplate_SetMonthName(memo->stringTemplate, 6, Pokemon_GetValue(memo->mon, MON_DATA_EGG_MONTH, NULL));
		StringTemplate_SetNumber(memo->stringTemplate, 7, Pokemon_GetValue(memo->mon, MON_DATA_EGG_DAY, NULL), 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
		StringTemplate_SetMetLocationName(memo->stringTemplate, 8, Pokemon_GetValue(memo->mon, MON_DATA_EGG_LOCATION, NULL));
	} else {
		// Year is formatted as 20__
		StringTemplate_SetNumber(memo->stringTemplate, 5, Pokemon_GetValue(memo->mon, MON_DATA_MET_YEAR, NULL), 2, PADDING_MODE_ZEROES, CHARSET_MODE_EN);
		StringTemplate_SetMonthName(memo->stringTemplate, 6, Pokemon_GetValue(memo->mon, MON_DATA_MET_MONTH, NULL));
		StringTemplate_SetNumber(memo->stringTemplate, 7, Pokemon_GetValue(memo->mon, MON_DATA_MET_DAY, NULL), 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
		StringTemplate_SetMetLocationName(memo->stringTemplate, 8, Pokemon_GetValue(memo->mon, MON_DATA_MET_LOCATION, NULL));
	}
	
	StringTemplate_Format(memo->stringTemplate, memo->metLocation.text, tmp);
	String_Free(tmp);
}


static void SetCharacteristicTemplate(PokemonMemo* memo) {
	static const u16 sIVCharacteristicTable[6][5] = {
		{
			POKEMON_SUMMARY_SCREEN_LovesToEat,
			POKEMON_SUMMARY_SCREEN_OftenDozesOff,
			POKEMON_SUMMARY_SCREEN_OftenScattersThings,
			POKEMON_SUMMARY_SCREEN_ScattersThingsOften,
			POKEMON_SUMMARY_SCREEN_LikesToRelax
		},
		{
			POKEMON_SUMMARY_SCREEN_ProudOfItsPower,
			POKEMON_SUMMARY_SCREEN_LikesToThrashAbout,
			POKEMON_SUMMARY_SCREEN_ALittleQuickTempered,
			POKEMON_SUMMARY_SCREEN_LikesToFight,
			POKEMON_SUMMARY_SCREEN_QuickTempered
		},
		{
			POKEMON_SUMMARY_SCREEN_SturdyBody,
			POKEMON_SUMMARY_SCREEN_CapableOfTakingHits,
			POKEMON_SUMMARY_SCREEN_HighlyPersistant,
			POKEMON_SUMMARY_SCREEN_GoodEndurance,
			POKEMON_SUMMARY_SCREEN_GoodPerserverance
		},
		{
			POKEMON_SUMMARY_SCREEN_LikesToRun,
			POKEMON_SUMMARY_SCREEN_AlertToSounds,
			POKEMON_SUMMARY_SCREEN_ImpetuousAndSilly,
			POKEMON_SUMMARY_SCREEN_SomewhatOfAClown,
			POKEMON_SUMMARY_SCREEN_QuickToFlee
		},
		{
			POKEMON_SUMMARY_SCREEN_HighlyCurious,
			POKEMON_SUMMARY_SCREEN_Mischievous,
			POKEMON_SUMMARY_SCREEN_ThoroughlyCunning,
			POKEMON_SUMMARY_SCREEN_OftenLostInThought,
			POKEMON_SUMMARY_SCREEN_VeryFinicky,
		},
		{
			POKEMON_SUMMARY_SCREEN_Strongwilled,
			POKEMON_SUMMARY_SCREEN_SomewhatVain,
			POKEMON_SUMMARY_SCREEN_StronglyDefiant,
			POKEMON_SUMMARY_SCREEN_HatesToLose,
			POKEMON_SUMMARY_SCREEN_SomewhatStubborn
		}
	};
	
	memo->characteristic.text = String_Init(LINE_BUFFER_SIZE, memo->heapID);
	
	int ivs[6];
	ivs[0] = Pokemon_GetValue(memo->mon, MON_DATA_HP_IV,    NULL);
	ivs[1] = Pokemon_GetValue(memo->mon, MON_DATA_ATK_IV,   NULL);
	ivs[2] = Pokemon_GetValue(memo->mon, MON_DATA_DEF_IV,   NULL);
	ivs[3] = Pokemon_GetValue(memo->mon, MON_DATA_SPEED_IV, NULL);
	ivs[4] = Pokemon_GetValue(memo->mon, MON_DATA_SPATK_IV, NULL);
	ivs[5] = Pokemon_GetValue(memo->mon, MON_DATA_SPDEF_IV, NULL);
	
	int highestIVIdx, highestIV;
	switch (Pokemon_GetValue(memo->mon, MON_DATA_PERSONALITY, NULL) % 6) {
		default:
		case 0:
			highestIVIdx = 0;
			highestIV = ivs[0];
			
			if (highestIV < ivs[1]) {
				highestIVIdx = 1;
				highestIV = ivs[1];
			}
			
			if (highestIV < ivs[2]) {
				highestIVIdx = 2;
				highestIV = ivs[2];
			}
			
			if (highestIV < ivs[3]) {
				highestIVIdx = 3;
				highestIV = ivs[3];
			}
			
			if (highestIV < ivs[4]) {
				highestIVIdx = 4;
				highestIV = ivs[4];
			}
			
			if (highestIV < ivs[5]) {
				highestIVIdx = 5;
				highestIV = ivs[5];
			}
			break;
		
		case 1:
			highestIVIdx = 1;
			highestIV = ivs[1];
			
			if (highestIV < ivs[2]) {
				highestIVIdx = 2;
				highestIV = ivs[2];
			}
			
			if (highestIV < ivs[3]) {
				highestIVIdx = 3;
				highestIV = ivs[3];
			}
			
			if (highestIV < ivs[4]) {
				highestIVIdx = 4;
				highestIV = ivs[4];
			}
			
			if (highestIV < ivs[5]) {
				highestIVIdx = 5;
				highestIV = ivs[5];
			}
			
			if (highestIV < ivs[0]) {
				highestIVIdx = 0;
				highestIV = ivs[0];
			}
			break;
		
		case 2:
			highestIVIdx = 2;
			highestIV = ivs[2];
			
			if (highestIV < ivs[3]) {
				highestIVIdx = 3;
				highestIV = ivs[3];
			}
			
			if (highestIV < ivs[4]) {
				highestIVIdx = 4;
				highestIV = ivs[4];
			}
			
			if (highestIV < ivs[5]) {
				highestIVIdx = 5;
				highestIV = ivs[5];
			}
			
			if (highestIV < ivs[0]) {
				highestIVIdx = 0;
				highestIV = ivs[0];
			}
			
			if (highestIV < ivs[1]) {
				highestIVIdx = 1;
				highestIV = ivs[1];
			}
			break;
		
		case 3:
			highestIVIdx = 3;
			highestIV = ivs[3];
			
			if (highestIV < ivs[4]) {
				highestIVIdx = 4;
				highestIV = ivs[4];
			}
			
			if (highestIV < ivs[5]) {
				highestIVIdx = 5;
				highestIV = ivs[5];
			}
			
			if (highestIV < ivs[0]) {
				highestIVIdx = 0;
				highestIV = ivs[0];
			}
			
			if (highestIV < ivs[1]) {
				highestIVIdx = 1;
				highestIV = ivs[1];
			}
			
			if (highestIV < ivs[2]) {
				highestIVIdx = 2;
				highestIV = ivs[2];
			}
			break;
		
		case 4:
			highestIVIdx = 4;
			highestIV = ivs[4];
			
			if (highestIV < ivs[5]) {
				highestIVIdx = 5;
				highestIV = ivs[5];
			}
			
			if (highestIV < ivs[0]) {
				highestIVIdx = 0;
				highestIV = ivs[0];
			}
			
			if (highestIV < ivs[1]) {
				highestIVIdx = 1;
				highestIV = ivs[1];
			}
			
			if (highestIV < ivs[2]) {
				highestIVIdx = 2;
				highestIV = ivs[2];
			}
			
			if (highestIV < ivs[3]) {
				highestIVIdx = 3;
				highestIV = ivs[3];
			}
			break;
		
		case 5:
			highestIVIdx = 5;
			highestIV = ivs[5];
			
			if (highestIV < ivs[0]) {
				highestIVIdx = 0;
				highestIV = ivs[0];
			}
			
			if (highestIV < ivs[1]) {
				highestIVIdx = 1;
				highestIV = ivs[1];
			}
			
			if (highestIV < ivs[2]) {
				highestIVIdx = 2;
				highestIV = ivs[2];
			}
			
			if (highestIV < ivs[3]) {
				highestIVIdx = 3;
				highestIV = ivs[3];
			}
			
			if (highestIV < ivs[4]) {
				highestIVIdx = 4;
				highestIV = ivs[4];
			}
			break;
	}
	
	int characteristicMessageID = sIVCharacteristicTable[highestIVIdx][highestIV % 5];
	MessageLoader_GetString(memo->messageLoader, characteristicMessageID, memo->characteristic.text);
}


static void SetFlavorPreferenceTemplate(PokemonMemo* memo) {
	static const u16 sFlavorPreferenceTable[6] = {
		POKEMON_SUMMARY_SCREEN_HappilyEatsAnything,
		POKEMON_SUMMARY_SCREEN_LikesSpicyFood,
		POKEMON_SUMMARY_SCREEN_LikesDryFood,
		POKEMON_SUMMARY_SCREEN_LikesSweetFood,
		POKEMON_SUMMARY_SCREEN_LikesBitterFood,
		POKEMON_SUMMARY_SCREEN_LikesSourFood
	};
	
	memo->flavorPreference.text = String_Init(LINE_BUFFER_SIZE, memo->heapID);
	
	int preference = 0;
	for (int flavorIdx = FLAVOR_SPICY; flavorIdx < FLAVOR_MAX; flavorIdx++) {
		if (Pokemon_GetFlavorAffinity(memo->mon, flavorIdx) == TRUE) {
			preference = flavorIdx + 1;
		}
	}
	
	int flavorMessageID = sFlavorPreferenceTable[preference];
	MessageLoader_GetString(memo->messageLoader, flavorMessageID, memo->flavorPreference.text);
}


static void SetEggStatusTemplate(PokemonMemo* memo) {
	// Friendship stores the egg hatching progress
	int eggStatus = Pokemon_GetValue(memo->mon, MON_DATA_FRIENDSHIP, NULL);
	
	memo->eggHatchingStatus.text = String_Init(LINE_BUFFER_SIZE * 4, memo->heapID);
	
	int eggStatusMessageID;
	if (eggStatus <= 5) {
		eggStatusMessageID = POKEMON_SUMMARY_SCREEN_EggSoundsInside;
	} else if (eggStatus <= 10) {
		eggStatusMessageID = POKEMON_SUMMARY_SCREEN_EggMovesOccasionally;
	} else if (eggStatus <= 40) {
		eggStatusMessageID = POKEMON_SUMMARY_SCREEN_EggDoesntSeemClose;
	} else {
		eggStatusMessageID = POKEMON_SUMMARY_SCREEN_EggLongTime;
	}
	
	MessageLoader_GetString(memo->messageLoader, eggStatusMessageID, memo->eggHatchingStatus.text);
}


static PokemonMemoType DetermineMonMemoType(Pokemon* mon, BOOL monOTMatches, HeapID heapID) {
	PokemonMemoType result = MEMO_TYPE_STANDARD;
	
	// Is this an egg, or a regular Pokemon?
	if (Pokemon_GetValue(mon, MON_DATA_IS_EGG, NULL) == FALSE) {
		// Mon is not an egg. Was it hatched from one?
		if (Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == 0) {
			// Mon was never an egg. Did it come from the Pal Park, a fateful encounter, or an in-game trade?
			if (Pokemon_GetValue(mon, MON_DATA_MET_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_NORMAL, LOCATION_NAME_PalPark)) {
				result = MEMO_TYPE_PAL_PARK;
			} else if (Pokemon_GetValue(mon, MON_DATA_FATEFUL_ENCOUNTER, NULL) == TRUE) {
				if (monOTMatches == TRUE) {
					result = MEMO_TYPE_FATEFUL_ENCOUNTER;
				} else {
					result = MEMO_TYPE_FATEFUL_ENCOUNTER_APPARENT;
				}
			} else if (Pokemon_GetValue(mon, MON_DATA_MET_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Trade)) {
				result = MEMO_TYPE_IN_GAME_TRADE;
			} else {
				if (monOTMatches == TRUE) {
					result = MEMO_TYPE_STANDARD;
				} else {
					result = MEMO_TYPE_STANDARD_APPARENT;
				}
			}
		} else {
			// Mon was hatched from an egg. Was obtaining the egg a fateful encounter?
			if (Pokemon_GetValue(mon, MON_DATA_FATEFUL_ENCOUNTER, NULL) == TRUE) {
				// Was a fateful encounter egg. Was the egg obtained in a trade, from Pokemon Ranger, or somewhere else?
				if (Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Trade2)) {
					if (monOTMatches == TRUE) {
						result = MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG;
					} else {
						result = MEMO_TYPE_TRADED_FATEFUL_ENCOUTER_HATCHED_EGG_APPARENT;
					}
				} else if (Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_MYSTERY_GIFT, MYSTERY_GIFT_EVENT_NAME_PokemonRanger)) {
					if (monOTMatches == TRUE) {
						result = MEMO_TYPE_POKEMON_RANGER;
					} else {
						result = MEMO_TYPE_POKEMON_RANGER_APPARENT;
					}
				} else {
					if (monOTMatches == TRUE) {
						result = MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG;
					} else {
						result = MEMO_TYPE_FATEFUL_ENCOUNTER_HATCHED_EGG_APPARENT;
					}
				}
			} else {
				// Mon was hatched from an egg and not obtained from a fateful encounter. Did it come from the Day-Care, an in-game gift, or a trade?
				if (
					Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Trade) ||
					Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Daycare) ||
					Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_TravelingMan) || 
					Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Riley) ||
					(!gIsDiamondPearl && (Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_SPECIAL, SPECIAL_METLOC_NAME_Cynthia)))
				) {
					if (monOTMatches == TRUE) {
						result = MEMO_TYPE_DAYCARE_HATCHED_EGG;
					} else {
						result = MEMO_TYPE_DAYCARE_HATCHED_EGG_APPARENT;
					}
				} else {
					if (monOTMatches == TRUE) {
						result = MEMO_TYPE_TRADED_HATCHED_EGG;
					} else {
						result = MEMO_TYPE_TRADED_HATCHED_EGG_APPARENT;
					}
				}
			}
		}
	} else {
		// Mon is an egg. Are we the OT?
		if (monOTMatches == TRUE) {
			// We are the OT. Is the egg from Pokemon Ranger, Mystery Gift, or the Day-Care?
			if (Pokemon_GetValue(mon, MON_DATA_FATEFUL_ENCOUNTER, NULL) == TRUE) {
				if (Pokemon_GetValue(mon, MON_DATA_EGG_LOCATION, NULL) == (MetLoc_GetByTypeAndEntry(MET_LOCATION_TYPE_MYSTERY_GIFT, MYSTERY_GIFT_EVENT_NAME_PokemonRanger))) {
					result = MEMO_TYPE_RANGER_EGG;
				} else {
					result = MEMO_TYPE_GIFT_EGG;
				}
			} else {
				result = MEMO_TYPE_DAYCARE_EGG;
			}
		} else {
			// We are not the OT. Is this a fateful encounter egg, or from the Day-Care?
			if (Pokemon_GetValue(mon, MON_DATA_FATEFUL_ENCOUNTER, NULL) == TRUE) {
				result = MEMO_TYPE_GIFT_EGG_APPARENT;
			} else {
				result = MEMO_TYPE_DAYCARE_EGG_APPARENT;
			}
		}
	}
	
	return result;
}
