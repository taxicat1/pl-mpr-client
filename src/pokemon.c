#include <nitro.h>

#include "pokemon.h"

#include "common.h"
#include "assert.h"
#include "math_util.h"
#include "game_version.h"
#include "math_util.h"
#include "heap.h"
#include "charcode_util.h"
#include "seal_case.h"
#include "item.h"
#include "message.h"
#include "message_util.h"
#include "move_table.h"
#include "struct_defs/species_sprite_data.h"
#include "constants/charcode.h"
#include "constants/natures.h"
#include "constants/species.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/exp_rates.h"
#include "constants/item_hold_effects.h"
#include "constants/forms.h"
#include "constants/gender_ratios.h"
#include "constants/pokemon.h"

#define FATEFUL_ENCOUNTER_LOCATION  3002

typedef enum {
	DATA_BLOCK_A = 0,
	DATA_BLOCK_B,
	DATA_BLOCK_C,
	DATA_BLOCK_D
} PokemonDataBlockID;

enum {
	FLAVOR_DISLIKED = -1,
	FLAVOR_NEUTRAL  =  0,
	FLAVOR_LIKED    =  1
};

static const s8 sNatureFlavorAffinities[][5] = {
	[NATURE_HARDY] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_LONELY] = {
		[FLAVOR_SPICY]  = FLAVOR_LIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_DISLIKED
	},
	
	[NATURE_BRAVE] = {
		[FLAVOR_SPICY]  = FLAVOR_LIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_DISLIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_ADAMANT] = {
		[FLAVOR_SPICY]  = FLAVOR_LIKED,
		[FLAVOR_DRY]    = FLAVOR_DISLIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_NAUGHTY] = {
		[FLAVOR_SPICY]  = FLAVOR_LIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_DISLIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_BOLD] = {
		[FLAVOR_SPICY]  = FLAVOR_DISLIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_LIKED
	},
	
	[NATURE_DOCILE] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_RELAXED] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_DISLIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_LIKED
	},
	
	[NATURE_IMPISH] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_DISLIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_LIKED
	},
	
	[NATURE_LAX] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_DISLIKED,
		[FLAVOR_SOUR]   = FLAVOR_LIKED
	},
	
	[NATURE_TIMID] = {
		[FLAVOR_SPICY]  = FLAVOR_DISLIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_LIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_HASTY] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_LIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_DISLIKED
	},
	
	[NATURE_SERIOUS] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_JOLLY] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_DISLIKED,
		[FLAVOR_SWEET]  = FLAVOR_LIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_NAIVE] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_LIKED,
		[FLAVOR_BITTER] = FLAVOR_DISLIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_MODEST] = {
		[FLAVOR_SPICY]  = FLAVOR_DISLIKED,
		[FLAVOR_DRY]    = FLAVOR_LIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_MILD] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_LIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_DISLIKED
	},
	
	[NATURE_QUIET] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_LIKED,
		[FLAVOR_SWEET]  = FLAVOR_DISLIKED,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_BASHFUL] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_RASH] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_LIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_DISLIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_CALM] = {
		[FLAVOR_SPICY]  = FLAVOR_DISLIKED,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_LIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_GENTLE] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_LIKED,
		[FLAVOR_SOUR]   = FLAVOR_DISLIKED
	},
	
	[NATURE_SASSY] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_DISLIKED,
		[FLAVOR_BITTER] = FLAVOR_LIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL,
	},
	[NATURE_CAREFUL] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_DISLIKED,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_LIKED,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	},
	
	[NATURE_QUIRKY] = {
		[FLAVOR_SPICY]  = FLAVOR_NEUTRAL,
		[FLAVOR_DRY]    = FLAVOR_NEUTRAL,
		[FLAVOR_SWEET]  = FLAVOR_NEUTRAL,
		[FLAVOR_BITTER] = FLAVOR_NEUTRAL,
		[FLAVOR_SOUR]   = FLAVOR_NEUTRAL
	}
};

static void Pokemon_Instantiate(BoxPokemon *boxMon, int monSpecies, int monLevel, int monIVs, BOOL useMonPersonalityParam, u32 monPersonality, int monOTIDSource, u32 monOTID);
static u32 Pokemon_GetDataInternal(Pokemon* mon, PokemonDataParam param, void* dest);
static u32 BoxPokemon_GetDataInternal(BoxPokemon* boxMon, PokemonDataParam param, void* dest);
static void Pokemon_SetDataInternal(Pokemon *mon, PokemonDataParam param, const void* value);
static void BoxPokemon_SetDataInternal(BoxPokemon* boxMon, PokemonDataParam param, const void* value);
static void Pokemon_LoadExperienceTableOf(ExpRate monExpRate, u32* monExpTable);
static u32 Pokemon_GetExpRateBaseExpAt(ExpRate monExpRate, int monLevel);
static u16 Pokemon_GetNatureStatValue(u8 monNature, u16 monStatValue, u8 statType);
static u8 BoxPokemon_IsShiny(BoxPokemon* boxMon);
static void BoxPokemon_SetDefaultMoves(BoxPokemon* boxMon);
static u16 BoxPokemon_AddMove(BoxPokemon* boxMon, u16 moveID);
static void BoxPokemon_ReplaceMove(BoxPokemon* boxMon, u16 moveID);
static void BoxPokemon_SetMoveSlot(BoxPokemon* boxMon, u16 moveID, u8 moveSlot);
static s8 BoxPokemon_GetFlavorAffinity(BoxPokemon* boxMon, Flavor flavor);
static BOOL BoxPokemon_IsInfectedWithPokerus(BoxPokemon* boxMon);
static BOOL BoxPokemon_HasCuredPokerus(BoxPokemon* boxMon);
static void BoxPokemon_CalcAbility(BoxPokemon* boxMon);
static void SpeciesData_LoadSpecies(int monSpecies, SpeciesData* speciesData);
static void SpeciesData_LoadForm(int monSpecies, int monForm, SpeciesData* speciesData);
static void Pokemon_EncryptData(void* data, u32 bytes, u32 seed);
static void Pokemon_DecryptData(void* data, u32 bytes, u32 seed);
static u16 LCRNG_NextFrom(u32* seed);
static u16 Pokemon_GetDataChecksum(void* data, u32 bytes);
static void* BoxPokemon_GetDataBlock(BoxPokemon* boxMon, u32 personality, PokemonDataBlockID dataBlockID);
static int Pokemon_GetFormNarcIndex(int monSpecies, int monForm);


static inline BOOL Pokemon_InlineIsPersonalityShiny(u32 monOTID, u32 monPersonality) {
	return (((monOTID & 0xFFFF0000) >> 16) ^ (monOTID & 0xFFFF) ^ ((monPersonality & 0xFFFF0000) >> 16) ^ (monPersonality & 0xFFFF)) < 8;
}


void Pokemon_Init(Pokemon* mon) {
	MI_CpuClearFast(mon, sizeof(Pokemon));
	Pokemon_EncryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
	Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
}


void BoxPokemon_Init(BoxPokemon* boxMon) {
	MI_CpuClearFast(boxMon, sizeof(BoxPokemon));
	Pokemon_EncryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
}


int Pokemon_GetWorkSize(void) {
	return sizeof(Pokemon);
}


Pokemon* Pokemon_New(HeapID heapID) {
	Pokemon* mon = Heap_Alloc(heapID, sizeof(Pokemon));
	Pokemon_Init(mon);
	return mon;
}


BOOL Pokemon_EnterDecryptionContext(Pokemon* mon) {
	BOOL wasDecrypted = FALSE;
	
	if (mon->box.partyDecrypted == FALSE) {
		wasDecrypted = TRUE;
		GF_ASSERT(mon->box.boxDecrypted == FALSE);
		
		mon->box.partyDecrypted = TRUE;
		mon->box.boxDecrypted = TRUE;
		
		Pokemon_DecryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		Pokemon_DecryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
	}
	
	return wasDecrypted;
}


BOOL Pokemon_ExitDecryptionContext(Pokemon* mon, BOOL encrypt) {
	BOOL wasEncrypted = FALSE;
	
	if (mon->box.partyDecrypted == TRUE && encrypt == TRUE) {
		wasEncrypted = TRUE;
		mon->box.partyDecrypted = FALSE;
		mon->box.boxDecrypted = FALSE;
		
		Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		mon->box.checksum = Pokemon_GetDataChecksum(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4);
		Pokemon_EncryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
	}
	
	return wasEncrypted;
}


BOOL BoxPokemon_EnterDecryptionContext(BoxPokemon* boxMon) {
	BOOL wasDecrypted = FALSE;
	
	if (boxMon->boxDecrypted == FALSE) {
		wasDecrypted = TRUE;
		boxMon->boxDecrypted = TRUE;
		Pokemon_DecryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
	}
	
	return wasDecrypted;
}


BOOL BoxPokemon_ExitDecryptionContext(BoxPokemon* boxMon, BOOL encrypt) {
	BOOL wasEncrypted = FALSE;
	
	if (boxMon->boxDecrypted == TRUE && encrypt == TRUE) {
		wasEncrypted = TRUE;
		
		boxMon->boxDecrypted = FALSE;
		boxMon->checksum = Pokemon_GetDataChecksum(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4);
		
		Pokemon_EncryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
	}
	
	return wasEncrypted;
}


void Pokemon_InitWith(Pokemon *mon, int monSpecies, int monLevel, int monIVs, BOOL useMonPersonalityParam, u32 monPersonality, int monOTIDSource, u32 monOTID) {
	Pokemon_Init(mon);
	
	Pokemon_Instantiate(&mon->box, monSpecies, monLevel, monIVs, useMonPersonalityParam, monPersonality, monOTIDSource, monOTID);
	Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), 0);
	Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
	Pokemon_SetValue(mon, MON_DATA_LEVEL, &monLevel);
	
	Mail* mail = Mail_New(HEAP_ID_SYSTEM);
	Pokemon_SetValue(mon, MON_DATA_MAIL, mail);
	Heap_Free(mail);
	
	u32 capsuleId = 0;
	Pokemon_SetValue(mon, MON_DATA_BALL_CAPSULE_ID, &capsuleId);
	
	BallCapsule capsule;
	MI_CpuClearFast(&capsule, sizeof(BallCapsule));
	Pokemon_SetValue(mon, MON_DATA_BALL_CAPSULE, &capsule);
	
	Pokemon_CalcLevelAndStats(mon);
}


static void Pokemon_Instantiate(BoxPokemon *boxMon, int monSpecies, int monLevel, int monIVs, BOOL useMonPersonalityParam, u32 monPersonality, int monOTIDSource, u32 monOTID) {
	BoxPokemon_Init(boxMon);
	
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	if (!useMonPersonalityParam) {
		monPersonality = (LCRNG_Next() | (LCRNG_Next() << 16));
	}
	
	BoxPokemon_SetValue(boxMon, MON_DATA_PERSONALITY, &monPersonality);
	
	if (monOTIDSource == OTID_NOT_SHINY) {
		do {
			monOTID = (LCRNG_Next() | (LCRNG_Next() << 16));
		} while (Pokemon_InlineIsPersonalityShiny(monOTID, monPersonality));
	} else if (monOTIDSource != OTID_SET) {
		monOTID = 0;
	}
	
	BoxPokemon_SetValue(boxMon, MON_DATA_OT_ID, &monOTID);
	BoxPokemon_SetValue(boxMon, MON_DATA_LANGUAGE, &gGameLanguage);
	BoxPokemon_SetValue(boxMon, MON_DATA_SPECIES, &monSpecies);
	BoxPokemon_SetValue(boxMon, MON_DATA_SPECIES_NAME, NULL);
	
	u32 dat1, dat2;
	
	dat1 = Pokemon_GetSpeciesBaseExpAt(monSpecies, monLevel);
	BoxPokemon_SetValue(boxMon, MON_DATA_EXPERIENCE, &dat1);
	
	dat1 = SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_BASE_FRIENDSHIP);
	BoxPokemon_SetValue(boxMon, MON_DATA_FRIENDSHIP, &dat1);
	
	BoxPokemon_SetValue(boxMon, MON_DATA_MET_LEVEL, &monLevel);
	BoxPokemon_SetValue(boxMon, MON_DATA_MET_GAME, &gGameVersion);
	
	dat1 = ITEM_POKE_BALL;
	BoxPokemon_SetValue(boxMon, MON_DATA_POKEBALL, &dat1);
	
	if (monIVs < INIT_IVS_RANDOM) {
		BoxPokemon_SetValue(boxMon, MON_DATA_HP_IV, &monIVs);
		BoxPokemon_SetValue(boxMon, MON_DATA_ATK_IV, &monIVs);
		BoxPokemon_SetValue(boxMon, MON_DATA_DEF_IV, &monIVs);
		BoxPokemon_SetValue(boxMon, MON_DATA_SPEED_IV, &monIVs);
		BoxPokemon_SetValue(boxMon, MON_DATA_SPATK_IV, &monIVs);
		BoxPokemon_SetValue(boxMon, MON_DATA_SPDEF_IV, &monIVs);
	} else {
		dat1 = LCRNG_Next();
		dat2 = (dat1 & (0x1f << 0)) >> 0;
		BoxPokemon_SetValue(boxMon, MON_DATA_HP_IV, &dat2);
		
		dat2 = (dat1 & (0x1f << 5)) >> 5;
		BoxPokemon_SetValue(boxMon, MON_DATA_ATK_IV, &dat2);
		
		dat2 = (dat1 & (0x1f << 10)) >> 10;
		BoxPokemon_SetValue(boxMon, MON_DATA_DEF_IV, &dat2);
		
		dat1 = LCRNG_Next();
		dat2 = (dat1 & (0x1f << 0)) >> 0;
		BoxPokemon_SetValue(boxMon, MON_DATA_SPEED_IV, &dat2);
		
		dat2 = (dat1 & (0x1f << 5)) >> 5;
		BoxPokemon_SetValue(boxMon, MON_DATA_SPATK_IV, &dat2);
		
		dat2 = (dat1 & (0x1f << 10)) >> 10;
		BoxPokemon_SetValue(boxMon, MON_DATA_SPDEF_IV, &dat2);
	}
	
	dat1 = SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_ABILITY_1);
	dat2 = SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_ABILITY_2);
	
	if (dat2 != ABILITY_NONE) {
		if (monPersonality & 1) {
			BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &dat2);
		} else {
			BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &dat1);
		}
	} else {
		BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &dat1);
	}
	
	dat1 = BoxPokemon_GetGender(boxMon);
	
	BoxPokemon_SetValue(boxMon, MON_DATA_GENDER, &dat1);
	BoxPokemon_SetDefaultMoves(boxMon);
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


void Pokemon_CalcLevelAndStats(Pokemon* mon) {
	BOOL reencrypt = Pokemon_EnterDecryptionContext(mon);
	
	int monLevel = Pokemon_GetLevel(mon);
	Pokemon_SetValue(mon, MON_DATA_LEVEL, &monLevel);
	Pokemon_CalcStats(mon);
	
	Pokemon_ExitDecryptionContext(mon, reencrypt);
}


void Pokemon_CalcStats(Pokemon* mon) {
	int monMaxHp;
	int monCurrentHp;
	int monHpIV, monAtkIV, monDefIV, monSpeedIV, monSpAtkIV, monSpDefIV;
	int monHpEV, monAtkEV, monDefEV, monSpeedEV, monSpAtkEV, monSpDefEV;
	
	BOOL reencrypt = Pokemon_EnterDecryptionContext(mon);
	
	int monLevel = Pokemon_GetValue(mon, MON_DATA_LEVEL, NULL);
	
	monMaxHp = Pokemon_GetValue(mon, MON_DATA_MAX_HP, NULL);
	monCurrentHp = Pokemon_GetValue(mon, MON_DATA_HP, NULL);
	
	monHpIV = Pokemon_GetValue(mon, MON_DATA_HP_IV, NULL);
	monHpEV = Pokemon_GetValue(mon, MON_DATA_HP_EV, NULL);
	monAtkIV = Pokemon_GetValue(mon, MON_DATA_ATK_IV, NULL);
	monAtkEV = Pokemon_GetValue(mon, MON_DATA_ATK_EV, NULL);
	monDefIV = Pokemon_GetValue(mon, MON_DATA_DEF_IV, NULL);
	monDefEV = Pokemon_GetValue(mon, MON_DATA_DEF_EV, NULL);
	monSpeedIV = Pokemon_GetValue(mon, MON_DATA_SPEED_IV, NULL);
	monSpeedEV = Pokemon_GetValue(mon, MON_DATA_SPEED_EV, NULL);
	monSpAtkIV = Pokemon_GetValue(mon, MON_DATA_SPATK_IV, NULL);
	monSpAtkEV = Pokemon_GetValue(mon, MON_DATA_SPATK_EV, NULL);
	monSpDefIV = Pokemon_GetValue(mon, MON_DATA_SPDEF_IV, NULL);
	monSpDefEV = Pokemon_GetValue(mon, MON_DATA_SPDEF_EV, NULL);
	
	int monForm = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
	int monSpecies = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
	SpeciesData *speciesData = Heap_Alloc(HEAP_ID_SYSTEM, sizeof(SpeciesData));
	
	SpeciesData_LoadForm(monSpecies, monForm, speciesData);
	
	int newMaxHp;
	if (monSpecies == SPECIES_SHEDINJA) {
		newMaxHp = 1;
	} else {
		newMaxHp = ((2 * speciesData->baseStats.hp + monHpIV + monHpEV / 4) * monLevel / 100 + monLevel + 10);
	}
	
	Pokemon_SetValue(mon, MON_DATA_MAX_HP, &newMaxHp);
	
	int newAtk = ((2 * speciesData->baseStats.attack + monAtkIV + monAtkEV / 4) * monLevel / 100 + 5);
	newAtk = Pokemon_GetNatureStatValue(Pokemon_GetNature(mon), newAtk, STAT_ATTACK);
	
	Pokemon_SetValue(mon, MON_DATA_ATK, &newAtk);
	
	int newDef = ((2 * speciesData->baseStats.defense + monDefIV + monDefEV / 4) * monLevel / 100 + 5);
	newDef = Pokemon_GetNatureStatValue(Pokemon_GetNature(mon), newDef, STAT_DEFENSE);
	
	Pokemon_SetValue(mon, MON_DATA_DEF, &newDef);
	
	int newSpeed = ((2 * speciesData->baseStats.speed + monSpeedIV + monSpeedEV / 4) * monLevel / 100 + 5);
	newSpeed = Pokemon_GetNatureStatValue(Pokemon_GetNature(mon), newSpeed, STAT_SPEED);
	
	Pokemon_SetValue(mon, MON_DATA_SPEED, &newSpeed);
	
	int newSpAtk = ((2 * speciesData->baseStats.spAttack + monSpAtkIV + monSpAtkEV / 4) * monLevel / 100 + 5);
	newSpAtk = Pokemon_GetNatureStatValue(Pokemon_GetNature(mon), newSpAtk, STAT_SPECIAL_ATTACK);
	
	Pokemon_SetValue(mon, MON_DATA_SP_ATK, &newSpAtk);
	
	int newSpDef = ((2 * speciesData->baseStats.spDefense + monSpDefIV + monSpDefEV / 4) * monLevel / 100 + 5);
	newSpDef = Pokemon_GetNatureStatValue(Pokemon_GetNature(mon), newSpDef, STAT_SPECIAL_DEFENSE);
	
	Pokemon_SetValue(mon, MON_DATA_SP_DEF, &newSpDef);
	Heap_Free(speciesData);
	
	if (monCurrentHp != 0 || monMaxHp == 0) {
		if (monSpecies == SPECIES_SHEDINJA) {
			monCurrentHp = 1;
		} else if (monCurrentHp == 0) {
			monCurrentHp = newMaxHp;
		} else {
			monCurrentHp += newMaxHp - monMaxHp;
		}
	}
	
	if (monCurrentHp) {
		Pokemon_SetValue(mon, MON_DATA_HP, &monCurrentHp);
	}
	
	Pokemon_ExitDecryptionContext(mon, reencrypt);
}


u32 Pokemon_GetValue(Pokemon* mon, PokemonDataParam param, void* dest) {
	if (mon->box.partyDecrypted == FALSE) {
		Pokemon_DecryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		Pokemon_DecryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
		
		u16 checksum = Pokemon_GetDataChecksum(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4);
		
		if (checksum != mon->box.checksum) {
			GF_ASSERT(checksum == mon->box.checksum);
			mon->box.checksumFailed = TRUE;
		}
	}
	
	u32 result = Pokemon_GetDataInternal(mon, param, dest);
	
	if (mon->box.partyDecrypted == FALSE) {
		Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		Pokemon_EncryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
	}
	
	return result;
}


static u32 Pokemon_GetDataInternal(Pokemon* mon, PokemonDataParam param, void* dest) {
	u32 result = 0;
	
	switch (param) {
		case MON_DATA_STATUS:
			result = mon->party.status;
			break;
		
		case MON_DATA_LEVEL:
			result = mon->party.level;
			break;
		
		case MON_DATA_BALL_CAPSULE_ID:
			result = mon->party.ballCapsuleID;
			break;
		
		case MON_DATA_HP:
			result = mon->party.hp;
			break;
		
		case MON_DATA_MAX_HP:
			result = mon->party.maxHP;
			break;
		
		case MON_DATA_ATK:
			result = mon->party.attack;
			break;
		
		case MON_DATA_DEF:
			result = mon->party.defense;
			break;
		
		case MON_DATA_SPEED:
			result = mon->party.speed;
			break;
		
		case MON_DATA_SP_ATK:
			result = mon->party.spAtk;
			break;
		
		case MON_DATA_SP_DEF:
			result = mon->party.spDef;
			break;
		
		case MON_DATA_MAIL:
			Mail_Copy(&mon->party.mail, dest);
			result = TRUE;
			break;
		
		case MON_DATA_BALL_CAPSULE:
			BallCapsule_Copy(&mon->party.ballCapsule, dest);
			result = TRUE;
			break;
		
		default:
			result = BoxPokemon_GetDataInternal(&mon->box, param, dest);
			break;
	}
	
	return result;
}


u32 BoxPokemon_GetValue(BoxPokemon* boxMon, PokemonDataParam param, void* dest) {
	if (boxMon->boxDecrypted == FALSE) {
		Pokemon_DecryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
		u16 checksum = Pokemon_GetDataChecksum(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4);
		
		if (checksum != boxMon->checksum) {
			GF_ASSERT(checksum == boxMon->checksum);
			boxMon->checksumFailed = TRUE;
		}
	}
	
	u32 result = BoxPokemon_GetDataInternal(boxMon, param, dest);
	
	if (boxMon->boxDecrypted == FALSE) {
		Pokemon_EncryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
	}
	
	return result;
}


static inline u32 GetRibbon(u64 mask, PokemonDataParam param, PokemonDataParam ribbonStart) {
	return (mask & (1ULL << (param - ribbonStart))) != 0;
}


static u32 BoxPokemon_GetDataInternal(BoxPokemon* boxMon, PokemonDataParam param, void* dest) {
	u32 result = 0;
	
	PokemonDataBlockA* monDataBlockA = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_A);
	PokemonDataBlockB* monDataBlockB = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_B);
	PokemonDataBlockC* monDataBlockC = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_C);
	PokemonDataBlockD* monDataBlockD = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_D);
	
	switch (param) {
		default:
			result = 0;
			break;
		
		case MON_DATA_PERSONALITY:
			result = boxMon->personality;
			break;
		
		case MON_DATA_IS_PARTY_DECRYPTED:
			result = boxMon->partyDecrypted;
			break;
		
		case MON_DATA_IS_BOX_DECRYPTED:
			result = boxMon->boxDecrypted;
			break;
		
		case MON_DATA_CHECKSUM_FAILED:
			result = boxMon->checksumFailed;
			break;
		
		case MON_DATA_CHECKSUM:
			result = boxMon->checksum;
			break;
		
		case MON_DATA_SPECIES_EXISTS:
			result = (monDataBlockA->species != SPECIES_NONE);
			break;
		
		case MON_DATA_SANITY_IS_EGG:
			if (boxMon->checksumFailed) {
				result = boxMon->checksumFailed;
			} else {
				result = monDataBlockB->isEgg;
			}
			break;
		
		case MON_DATA_SPECIES_OR_EGG:
			result = monDataBlockA->species;
			if (result == SPECIES_NONE) {
				break;
			} else if (monDataBlockB->isEgg || boxMon->checksumFailed) {
				result = SPECIES_EGG;
			}
			
			break;
		
		case MON_DATA_LEVEL:
			result = Pokemon_GetSpeciesLevelAt(monDataBlockA->species, monDataBlockA->exp);
			break;
		
		case MON_DATA_SPECIES:
			if (boxMon->checksumFailed) {
				result = SPECIES_EGG;
			} else {
				result = monDataBlockA->species;
			}
			
			break;
		
		case MON_DATA_HELD_ITEM:
			result = monDataBlockA->heldItem;
			break;
		
		case MON_DATA_OT_ID:
			result = monDataBlockA->otID;
			break;
		
		case MON_DATA_EXPERIENCE:
			result = monDataBlockA->exp;
			break;
		
		case MON_DATA_FRIENDSHIP:
			result = monDataBlockA->friendship;
			break;
		
		case MON_DATA_ABILITY:
			result = monDataBlockA->ability;
			break;
		
		case MON_DATA_MARKINGS:
			result = monDataBlockA->markings;
			break;
		
		case MON_DATA_LANGUAGE:
			result = monDataBlockA->originLanguage;
			break;
		
		case MON_DATA_HP_EV:
			result = monDataBlockA->hpEV;
			break;
		
		case MON_DATA_ATK_EV:
			result = monDataBlockA->atkEV;
			break;
		
		case MON_DATA_DEF_EV:
			result = monDataBlockA->defEV;
			break;
		
		case MON_DATA_SPEED_EV:
			result = monDataBlockA->speedEV;
			break;
		
		case MON_DATA_SPATK_EV:
			result = monDataBlockA->spAtkEV;
			break;
		
		case MON_DATA_SPDEF_EV:
			result = monDataBlockA->spDefEV;
			break;
		
		case MON_DATA_COOL:
			result = monDataBlockA->cool;
			break;
		
		case MON_DATA_BEAUTY:
			result = monDataBlockA->beauty;
			break;
		
		case MON_DATA_CUTE:
			result = monDataBlockA->cute;
			break;
		
		case MON_DATA_SMART:
			result = monDataBlockA->smart;
			break;
		
		case MON_DATA_TOUGH:
			result = monDataBlockA->tough;
			break;
		
		case MON_DATA_SHEEN:
			result = monDataBlockA->sheen;
			break;
		
		case MON_DATA_SINNOH_CHAMP_RIBBON:
		case MON_DATA_ABILITY_RIBBON:
		case MON_DATA_GREAT_ABILITY_RIBBON:
		case MON_DATA_DOUBLE_ABILITY_RIBBON:
		case MON_DATA_MULTI_ABILITY_RIBBON:
		case MON_DATA_PAIR_ABILITY_RIBBON:
		case MON_DATA_WORLD_ABILITY_RIBBON:
		case MON_DATA_ALERT_RIBBON:
		case MON_DATA_SHOCK_RIBBON:
		case MON_DATA_DOWNCAST_RIBBON:
		case MON_DATA_CARELESS_RIBBON:
		case MON_DATA_RELAX_RIBBON:
		case MON_DATA_SNOOZE_RIBBON:
		case MON_DATA_SMILE_RIBBON:
		case MON_DATA_GORGEOUS_RIBBON:
		case MON_DATA_ROYAL_RIBBON:
		case MON_DATA_GORGEOUS_ROYAL_RIBBON:
		case MON_DATA_FOOTPRINT_RIBBON:
		case MON_DATA_RECORD_RIBBON:
		case MON_DATA_HISTORY_RIBBON:
		case MON_DATA_LEGEND_RIBBON:
		case MON_DATA_RED_RIBBON:
		case MON_DATA_GREEN_RIBBON:
		case MON_DATA_BLUE_RIBBON:
		case MON_DATA_FESTIVAL_RIBBON:
		case MON_DATA_CARNIVAL_RIBBON:
		case MON_DATA_CLASSIC_RIBBON:
		case MON_DATA_PREMIER_RIBBON:
		case MON_DATA_UNUSED_RIBBON_53:
			result = GetRibbon(monDataBlockA->ribbonsDS1, param, MON_DATA_SINNOH_CHAMP_RIBBON);
			break;
		
		case MON_DATA_MOVE1:
		case MON_DATA_MOVE2:
		case MON_DATA_MOVE3:
		case MON_DATA_MOVE4:
			result = monDataBlockB->moves[param - MON_DATA_MOVE1];
			break;
		
		case MON_DATA_MOVE1_PP:
		case MON_DATA_MOVE2_PP:
		case MON_DATA_MOVE3_PP:
		case MON_DATA_MOVE4_PP:
			result = monDataBlockB->moveCurrentPPs[param - MON_DATA_MOVE1_PP];
			break;
		
		case MON_DATA_MOVE1_PP_UPS:
		case MON_DATA_MOVE2_PP_UPS:
		case MON_DATA_MOVE3_PP_UPS:
		case MON_DATA_MOVE4_PP_UPS:
			result = monDataBlockB->movePPUps[param - MON_DATA_MOVE1_PP_UPS];
			break;
		
		case MON_DATA_MOVE1_MAX_PP:
		case MON_DATA_MOVE2_MAX_PP:
		case MON_DATA_MOVE3_MAX_PP:
		case MON_DATA_MOVE4_MAX_PP:
			result = MoveTable_CalcMaxPP(monDataBlockB->moves[param - MON_DATA_MOVE1_MAX_PP], monDataBlockB->movePPUps[param - MON_DATA_MOVE1_MAX_PP]);
			break;
		
		case MON_DATA_HP_IV:
			result = monDataBlockB->hpIV;
			break;
		
		case MON_DATA_ATK_IV:
			result = monDataBlockB->atkIV;
			break;
		
		case MON_DATA_DEF_IV:
			result = monDataBlockB->defIV;
			break;
		
		case MON_DATA_SPEED_IV:
			result = monDataBlockB->speedIV;
			break;
		
		case MON_DATA_SPATK_IV:
			result = monDataBlockB->spAtkIV;
			break;
		
		case MON_DATA_SPDEF_IV:
			result = monDataBlockB->spDefIV;
			break;
		
		case MON_DATA_IS_EGG:
			if (boxMon->checksumFailed) {
				result = boxMon->checksumFailed;
			} else {
				result = monDataBlockB->isEgg;
			}
			break;
		
		case MON_DATA_HAS_NICKNAME:
			result = monDataBlockB->hasNickname;
			break;
		
		case MON_DATA_COOL_RIBBON:
		case MON_DATA_COOL_RIBBON_SUPER:
		case MON_DATA_COOL_RIBBON_HYPER:
		case MON_DATA_COOL_RIBBON_MASTER:
		case MON_DATA_BEAUTY_RIBBON:
		case MON_DATA_BEAUTY_RIBBON_SUPER:
		case MON_DATA_BEAUTY_RIBBON_HYPER:
		case MON_DATA_BEAUTY_RIBBON_MASTER:
		case MON_DATA_CUTE_RIBBON:
		case MON_DATA_CUTE_RIBBON_SUPER:
		case MON_DATA_CUTE_RIBBON_HYPER:
		case MON_DATA_CUTE_RIBBON_MASTER:
		case MON_DATA_SMART_RIBBON:
		case MON_DATA_SMART_RIBBON_SUPER:
		case MON_DATA_SMART_RIBBON_HYPER:
		case MON_DATA_SMART_RIBBON_MASTER:
		case MON_DATA_TOUGH_RIBBON:
		case MON_DATA_TOUGH_RIBBON_SUPER:
		case MON_DATA_TOUGH_RIBBON_HYPER:
		case MON_DATA_TOUGH_RIBBON_MASTER:
		case MON_DATA_CHAMPION_RIBBON:
		case MON_DATA_WINNING_RIBBON:
		case MON_DATA_VICTORY_RIBBON:
		case MON_DATA_ARTIST_RIBBON:
		case MON_DATA_EFFORT_RIBBON:
		case MON_DATA_MARINE_RIBBON:
		case MON_DATA_LAND_RIBBON:
		case MON_DATA_SKY_RIBBON:
		case MON_DATA_COUNTRY_RIBBON:
		case MON_DATA_NATIONAL_RIBBON:
		case MON_DATA_EARTH_RIBBON:
		case MON_DATA_WORLD_RIBBON:
			result = GetRibbon(monDataBlockB->ribbonsGBA, param, MON_DATA_COOL_RIBBON);
			break;
		
		case MON_DATA_FATEFUL_ENCOUNTER:
			result = monDataBlockB->fatefulEncounter;
			break;
		
		case MON_DATA_GENDER:
			result = monDataBlockB->gender;
			break;
		
		case MON_DATA_FORM:
			result = monDataBlockB->form;
			break;
		
		case MON_DATA_UNUSED_113:
			result = monDataBlockB->unused1;
			break;
		
		case MON_DATA_UNUSED_114:
			result = monDataBlockB->unused2;
			break;
		
		case MON_DATA_NICKNAME:
			if (boxMon->checksumFailed) {
				MessageLoader_GetSpeciesName(SPECIES_BAD_EGG, HEAP_ID_SYSTEM, dest);
			} else {
				charcode_t* buf = dest;
				for (result = 0; result < MON_NAME_LEN; result++) {
					buf[result] = monDataBlockC->nickname[result];
				}
				
				buf[result] = CHAR_EOS;
			}
			break;
		
		case MON_DATA_NICKNAME_STRING_AND_FLAG:
			result = monDataBlockB->hasNickname;
			// Fallthrough
		
		case MON_DATA_NICKNAME_STRING:
			if (boxMon->checksumFailed) {
				String *string = MessageUtil_SpeciesName(SPECIES_BAD_EGG, HEAP_ID_SYSTEM);
				String_Copy(dest, string);
				String_Free(string);
			} else {
				String_CopyChars(dest, monDataBlockC->nickname);
			}
			break;
		
		case MON_DATA_UNUSED_121:
			result = monDataBlockC->unused;
			break;
		
		case MON_DATA_MET_GAME:
			result = monDataBlockC->originGame;
			break;
		
		case MON_DATA_SUPER_COOL_RIBBON:
		case MON_DATA_SUPER_COOL_RIBBON_GREAT:
		case MON_DATA_SUPER_COOL_RIBBON_ULTRA:
		case MON_DATA_SUPER_COOL_RIBBON_MASTER:
		case MON_DATA_SUPER_BEAUTY_RIBBON:
		case MON_DATA_SUPER_BEAUTY_RIBBON_GREAT:
		case MON_DATA_SUPER_BEAUTY_RIBBON_ULTRA:
		case MON_DATA_SUPER_BEAUTY_RIBBON_MASTER:
		case MON_DATA_SUPER_CUTE_RIBBON:
		case MON_DATA_SUPER_CUTE_RIBBON_GREAT:
		case MON_DATA_SUPER_CUTE_RIBBON_ULTRA:
		case MON_DATA_SUPER_CUTE_RIBBON_MASTER:
		case MON_DATA_SUPER_SMART_RIBBON:
		case MON_DATA_SUPER_SMART_RIBBON_GREAT:
		case MON_DATA_SUPER_SMART_RIBBON_ULTRA:
		case MON_DATA_SUPER_SMART_RIBBON_MASTER:
		case MON_DATA_SUPER_TOUGH_RIBBON:
		case MON_DATA_SUPER_TOUGH_RIBBON_GREAT:
		case MON_DATA_SUPER_TOUGH_RIBBON_ULTRA:
		case MON_DATA_SUPER_TOUGH_RIBBON_MASTER:
		case MON_DATA_UNUSED_RIBBON_143:
			result = GetRibbon(monDataBlockC->ribbonsDS2, param, MON_DATA_SUPER_COOL_RIBBON);
			break;
		
		case MON_DATA_OT_NAME:
			{
				charcode_t* otName = dest;
				for (result = 0; result < TRAINER_NAME_LEN; result++) {
					otName[result] = monDataBlockD->otName[result];
				}
				
				otName[result] = CHAR_EOS;
				break;
			}
		
		case MON_DATA_OT_NAME_STRING:
			String_CopyChars(dest, monDataBlockD->otName);
			break;
		
		case MON_DATA_EGG_YEAR:
			result = monDataBlockD->eggYear;
			break;
		
		case MON_DATA_EGG_MONTH:
			result = monDataBlockD->eggMonth;
			break;
		
		case MON_DATA_EGG_DAY:
			result = monDataBlockD->eggDay;
			break;
		
		case MON_DATA_MET_YEAR:
			result = monDataBlockD->metYear;
			break;
		
		case MON_DATA_MET_MONTH:
			result = monDataBlockD->metMonth;
			break;
		
		case MON_DATA_MET_DAY:
			result = monDataBlockD->metDay;
			break;
		
		case MON_DATA_EGG_LOCATION:
			if (gIsDiamondPearl) {
				result = monDataBlockD->EggLocation_DP;
			} else {
				// Handle Pt locations
				if (monDataBlockD->EggLocation_DP == FATEFUL_ENCOUNTER_LOCATION && monDataBlockB->EggLocation_PtHGSS) {
					result = monDataBlockB->EggLocation_PtHGSS;
				} else {
					result = monDataBlockD->EggLocation_DP;
				}
			}
			break;
		
		case MON_DATA_MET_LOCATION:
			if (gIsDiamondPearl) {
				result = monDataBlockD->MetLocation_DP;
			} else {
				// Handle Pt locations
				if (monDataBlockD->MetLocation_DP == FATEFUL_ENCOUNTER_LOCATION && monDataBlockB->MetLocation_PtHGSS) {
					result = monDataBlockB->MetLocation_PtHGSS;
				} else {
					result = monDataBlockD->MetLocation_DP;
				}
			}
			break;
		
		case MON_DATA_POKERUS:
			result = monDataBlockD->pokerus;
			break;
		
		case MON_DATA_POKEBALL:
			result = monDataBlockD->pokeball;
			break;
		
		case MON_DATA_MET_LEVEL:
			result = monDataBlockD->metLevel;
			break;
		
		case MON_DATA_OT_GENDER:
			result = monDataBlockD->otGender;
			break;
		
		case MON_DATA_MET_TERRAIN:
			result = monDataBlockD->metTerrain;
			break;
		
		case MON_DATA_UNUSED_159:
			result = monDataBlockD->unused;
			break;
		
		case MON_DATA_COMBINED_IVS:
			result = (monDataBlockB->hpIV    <<  0) |
			         (monDataBlockB->atkIV   <<  5) |
			         (monDataBlockB->defIV   << 10) |
			         (monDataBlockB->speedIV << 15) |
			         (monDataBlockB->spAtkIV << 20) |
			         (monDataBlockB->spDefIV << 25);
			break;
		
		case MON_DATA_NO_PRINT_GENDER:
			if ((monDataBlockA->species == SPECIES_NIDORAN_F || monDataBlockA->species == SPECIES_NIDORAN_M) && monDataBlockB->hasNickname == FALSE) {
				result = FALSE;
			} else {
				result = TRUE;
			}
			break;
		
		case MON_DATA_TYPE_1:
		case MON_DATA_TYPE_2:
			if (monDataBlockA->species == SPECIES_ARCEUS && monDataBlockA->ability == ABILITY_MULTITYPE) {
				result = Pokemon_GetArceusTypeOf(Item_LoadParam(monDataBlockA->heldItem, ITEM_PARAM_HOLD_EFFECT, HEAP_ID_SYSTEM));
			} else {
				result = SpeciesData_GetFormValue(monDataBlockA->species, monDataBlockB->form, SPECIES_DATA_TYPE_1 + (param - MON_DATA_TYPE_1));
			}
			break;
		
		case MON_DATA_SPECIES_NAME:
			MessageLoader_GetSpeciesName(monDataBlockA->species, HEAP_ID_SYSTEM, dest);
			break;
	}
	
	return result;
}


void Pokemon_SetValue(Pokemon* mon, PokemonDataParam param, const void* value) {
	if (mon->box.partyDecrypted == FALSE) {
		Pokemon_DecryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		Pokemon_DecryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
		u16 checksum = Pokemon_GetDataChecksum(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4);
		
		if (checksum != mon->box.checksum) {
			GF_ASSERT(checksum == mon->box.checksum);
			mon->box.checksumFailed = TRUE;
			Pokemon_EncryptData(mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
			return;
		}
	}
	
	Pokemon_SetDataInternal(mon, param, value);
	
	if (mon->box.partyDecrypted == FALSE) {
		Pokemon_EncryptData(&mon->party, sizeof(PartyPokemon), mon->box.personality);
		mon->box.checksum = Pokemon_GetDataChecksum(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4);
		Pokemon_EncryptData(&mon->box.dataBlocks, sizeof(PokemonDataBlock) * 4, mon->box.checksum);
	}
}


static void Pokemon_SetDataInternal(Pokemon* mon, PokemonDataParam param, const void* value) {
	u32* u32Value = value;
	u16* u16Value = value;
	u8*  u8Value  = value;
	
	switch (param) {
		case MON_DATA_STATUS:
			mon->party.status = *u32Value;
			break;
		
		case MON_DATA_LEVEL:
			mon->party.level = *u8Value;
			break;
		
		case MON_DATA_BALL_CAPSULE_ID:
			mon->party.ballCapsuleID = *u8Value;
			break;
		
		case MON_DATA_HP:
			mon->party.hp = *u16Value;
			break;
		
		case MON_DATA_MAX_HP:
			mon->party.maxHP = *u16Value;
			break;
		
		case MON_DATA_ATK:
			mon->party.attack = *u16Value;
			break;
		
		case MON_DATA_DEF:
			mon->party.defense = *u16Value;
			break;
		
		case MON_DATA_SPEED:
			mon->party.speed = *u16Value;
			break;
		
		case MON_DATA_SP_ATK:
			mon->party.spAtk = *u16Value;
			break;
		
		case MON_DATA_SP_DEF:
			mon->party.spDef = *u16Value;
			break;
		
		case MON_DATA_MAIL:
			Mail_Copy(value, &mon->party.mail);
			break;
		
		case MON_DATA_BALL_CAPSULE:
			BallCapsule_Copy(value, &mon->party.ballCapsule);
			break;
		
		default:
			BoxPokemon_SetDataInternal(&mon->box, param, value);
			break;
	}
}


void BoxPokemon_SetValue(BoxPokemon* boxMon, PokemonDataParam param, const void* value) {
	if (boxMon->boxDecrypted == FALSE) {
		Pokemon_DecryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
		u16 checksum = Pokemon_GetDataChecksum(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4);
		
		if (checksum != boxMon->checksum) {
			GF_ASSERT(checksum == boxMon->checksum);
			boxMon->checksumFailed = TRUE;
			Pokemon_EncryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
			return;
		}
	}
	
	BoxPokemon_SetDataInternal(boxMon, param, value);
	
	if (boxMon->boxDecrypted == FALSE) {
		boxMon->checksum = Pokemon_GetDataChecksum(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4);
		Pokemon_EncryptData(boxMon->dataBlocks, sizeof(PokemonDataBlock) * 4, boxMon->checksum);
	}
}


static void BoxPokemon_SetDataInternal(BoxPokemon* boxMon, PokemonDataParam param, const void* value) {
	u32* u32Value = value;
	u16* u16Value = value;
	u8*  u8Value  = value;
	
	u64 bit;
	
	PokemonDataBlockA* monDataBlockA = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_A);
	PokemonDataBlockB* monDataBlockB = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_B);
	PokemonDataBlockC* monDataBlockC = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_C);
	PokemonDataBlockD* monDataBlockD = BoxPokemon_GetDataBlock(boxMon, boxMon->personality, DATA_BLOCK_D);
	
	switch (param) {
		case MON_DATA_PERSONALITY:
			boxMon->personality = *u32Value;
			break;
		
		case MON_DATA_IS_PARTY_DECRYPTED:
			GF_ASSERT(FALSE);
			boxMon->partyDecrypted = *u8Value;
			break;
		
		case MON_DATA_IS_BOX_DECRYPTED:
			GF_ASSERT(FALSE);
			boxMon->boxDecrypted = *u8Value;
			break;
		
		case MON_DATA_CHECKSUM_FAILED:
			boxMon->checksumFailed = *u8Value;
			break;
		
		case MON_DATA_CHECKSUM:
			boxMon->checksum = *u16Value;
			break;
		
		case MON_DATA_SPECIES:
			monDataBlockA->species = *u16Value;
			break;
		
		case MON_DATA_HELD_ITEM:
			monDataBlockA->heldItem = *u16Value;
			break;
		
		case MON_DATA_OT_ID:
			monDataBlockA->otID = *u32Value;
			break;
		
		case MON_DATA_EXPERIENCE:
			monDataBlockA->exp = *u32Value;
			break;
		
		case MON_DATA_FRIENDSHIP:
			monDataBlockA->friendship = *u8Value;
			break;
		
		case MON_DATA_ABILITY:
			monDataBlockA->ability = *u8Value;
			break;
		
		case MON_DATA_MARKINGS:
			monDataBlockA->markings = *u8Value;
			break;
		
		case MON_DATA_LANGUAGE:
			monDataBlockA->originLanguage = *u8Value;
			break;
		
		case MON_DATA_HP_EV:
			monDataBlockA->hpEV = *u8Value;
			break;
		
		case MON_DATA_ATK_EV:
			monDataBlockA->atkEV = *u8Value;
			break;
		
		case MON_DATA_DEF_EV:
			monDataBlockA->defEV = *u8Value;
			break;
		
		case MON_DATA_SPEED_EV:
			monDataBlockA->speedEV = *u8Value;
			break;
		
		case MON_DATA_SPATK_EV:
			monDataBlockA->spAtkEV = *u8Value;
			break;
		
		case MON_DATA_SPDEF_EV:
			monDataBlockA->spDefEV = *u8Value;
			break;
		
		case MON_DATA_COOL:
			monDataBlockA->cool = *u8Value;
			break;
		
		case MON_DATA_BEAUTY:
			monDataBlockA->beauty = *u8Value;
			break;
		
		case MON_DATA_CUTE:
			monDataBlockA->cute = *u8Value;
			break;
		
		case MON_DATA_SMART:
			monDataBlockA->smart = *u8Value;
			break;
		
		case MON_DATA_TOUGH:
			monDataBlockA->tough = *u8Value;
			break;
		
		case MON_DATA_SHEEN:
			monDataBlockA->sheen = *u8Value;
			break;
		
		case MON_DATA_SINNOH_CHAMP_RIBBON:
		case MON_DATA_ABILITY_RIBBON:
		case MON_DATA_GREAT_ABILITY_RIBBON:
		case MON_DATA_DOUBLE_ABILITY_RIBBON:
		case MON_DATA_MULTI_ABILITY_RIBBON:
		case MON_DATA_PAIR_ABILITY_RIBBON:
		case MON_DATA_WORLD_ABILITY_RIBBON:
		case MON_DATA_ALERT_RIBBON:
		case MON_DATA_SHOCK_RIBBON:
		case MON_DATA_DOWNCAST_RIBBON:
		case MON_DATA_CARELESS_RIBBON:
		case MON_DATA_RELAX_RIBBON:
		case MON_DATA_SNOOZE_RIBBON:
		case MON_DATA_SMILE_RIBBON:
		case MON_DATA_GORGEOUS_RIBBON:
		case MON_DATA_ROYAL_RIBBON:
		case MON_DATA_GORGEOUS_ROYAL_RIBBON:
		case MON_DATA_FOOTPRINT_RIBBON:
		case MON_DATA_RECORD_RIBBON:
		case MON_DATA_HISTORY_RIBBON:
		case MON_DATA_LEGEND_RIBBON:
		case MON_DATA_RED_RIBBON:
		case MON_DATA_GREEN_RIBBON:
		case MON_DATA_BLUE_RIBBON:
		case MON_DATA_FESTIVAL_RIBBON:
		case MON_DATA_CARNIVAL_RIBBON:
		case MON_DATA_CLASSIC_RIBBON:
		case MON_DATA_PREMIER_RIBBON:
		case MON_DATA_UNUSED_RIBBON_53:
			bit = *u8Value;
			bit <<= (param - MON_DATA_SINNOH_CHAMP_RIBBON);
			if (*u8Value) {
				monDataBlockA->ribbonsDS1 |= bit;
			} else {
				monDataBlockA->ribbonsDS1 &= (bit ^ 0xFFFFFFFF);
			}
			break;
		
		case MON_DATA_MOVE1:
		case MON_DATA_MOVE2:
		case MON_DATA_MOVE3:
		case MON_DATA_MOVE4:
			monDataBlockB->moves[param - MON_DATA_MOVE1] = *u16Value;
			break;
		
		case MON_DATA_MOVE1_PP:
		case MON_DATA_MOVE2_PP:
		case MON_DATA_MOVE3_PP:
		case MON_DATA_MOVE4_PP:
			monDataBlockB->moveCurrentPPs[param - MON_DATA_MOVE1_PP] = *u8Value;
			break;
		
		case MON_DATA_MOVE1_PP_UPS:
		case MON_DATA_MOVE2_PP_UPS:
		case MON_DATA_MOVE3_PP_UPS:
		case MON_DATA_MOVE4_PP_UPS:
			monDataBlockB->movePPUps[param - MON_DATA_MOVE1_PP_UPS] = *u8Value;
			break;
		
		case MON_DATA_MOVE1_MAX_PP:
		case MON_DATA_MOVE2_MAX_PP:
		case MON_DATA_MOVE3_MAX_PP:
		case MON_DATA_MOVE4_MAX_PP:
			break;
		
		case MON_DATA_HP_IV:
			monDataBlockB->hpIV = *u8Value;
			break;
		
		case MON_DATA_ATK_IV:
			monDataBlockB->atkIV = *u8Value;
			break;
		
		case MON_DATA_DEF_IV:
			monDataBlockB->defIV = *u8Value;
			break;
		
		case MON_DATA_SPEED_IV:
			monDataBlockB->speedIV = *u8Value;
			break;
		
		case MON_DATA_SPATK_IV:
			monDataBlockB->spAtkIV = *u8Value;
			break;
		
		case MON_DATA_SPDEF_IV:
			monDataBlockB->spDefIV = *u8Value;
			break;
		
		case MON_DATA_IS_EGG:
			monDataBlockB->isEgg = *u8Value;
			break;
		
		case MON_DATA_HAS_NICKNAME:
			monDataBlockB->hasNickname = *u8Value;
			break;
		
		case MON_DATA_COOL_RIBBON:
		case MON_DATA_COOL_RIBBON_SUPER:
		case MON_DATA_COOL_RIBBON_HYPER:
		case MON_DATA_COOL_RIBBON_MASTER:
		case MON_DATA_BEAUTY_RIBBON:
		case MON_DATA_BEAUTY_RIBBON_SUPER:
		case MON_DATA_BEAUTY_RIBBON_HYPER:
		case MON_DATA_BEAUTY_RIBBON_MASTER:
		case MON_DATA_CUTE_RIBBON:
		case MON_DATA_CUTE_RIBBON_SUPER:
		case MON_DATA_CUTE_RIBBON_HYPER:
		case MON_DATA_CUTE_RIBBON_MASTER:
		case MON_DATA_SMART_RIBBON:
		case MON_DATA_SMART_RIBBON_SUPER:
		case MON_DATA_SMART_RIBBON_HYPER:
		case MON_DATA_SMART_RIBBON_MASTER:
		case MON_DATA_TOUGH_RIBBON:
		case MON_DATA_TOUGH_RIBBON_SUPER:
		case MON_DATA_TOUGH_RIBBON_HYPER:
		case MON_DATA_TOUGH_RIBBON_MASTER:
		case MON_DATA_CHAMPION_RIBBON:
		case MON_DATA_WINNING_RIBBON:
		case MON_DATA_VICTORY_RIBBON:
		case MON_DATA_ARTIST_RIBBON:
		case MON_DATA_EFFORT_RIBBON:
		case MON_DATA_MARINE_RIBBON:
		case MON_DATA_LAND_RIBBON:
		case MON_DATA_SKY_RIBBON:
		case MON_DATA_COUNTRY_RIBBON:
		case MON_DATA_NATIONAL_RIBBON:
		case MON_DATA_EARTH_RIBBON:
		case MON_DATA_WORLD_RIBBON:
			bit = *u8Value;
			bit <<= (param - MON_DATA_COOL_RIBBON);
			if (*u8Value) {
				monDataBlockB->ribbonsGBA |= bit;
			} else {
				monDataBlockB->ribbonsGBA &= (bit ^ 0xFFFFFFFF);
			}
			break;
		
		case MON_DATA_FATEFUL_ENCOUNTER:
			monDataBlockB->fatefulEncounter = *u8Value;
			break;
		
		case MON_DATA_GENDER:
			monDataBlockB->gender = *u8Value;
			break;
		
		case MON_DATA_FORM:
			monDataBlockB->form = *u8Value;
			break;
		
		case MON_DATA_UNUSED_113:
			monDataBlockB->unused1 = *u8Value;
			break;
		
		case MON_DATA_UNUSED_114:
			monDataBlockB->unused2 = *u16Value;
			break;
		
		case MON_DATA_NICKNAME_AND_FLAG:
			{
				charcode_t baseName[MON_NAME_LEN + 1];
				
				MessageLoader_GetSpeciesName(monDataBlockA->species, HEAP_ID_SYSTEM, baseName);
				monDataBlockB->hasNickname = CharCode_Compare(baseName, u16Value);
			}
			// Fallthrough
		
		case MON_DATA_NICKNAME:
			for (int i = 0; i < NELEMS(monDataBlockC->nickname); i++) {
				monDataBlockC->nickname[i] = u16Value[i];
			}
			break;
		
		case MON_DATA_NICKNAME_STRING_AND_FLAG:
			{
				charcode_t baseName[MON_NAME_LEN + 1];
				charcode_t nickname[MON_NAME_LEN + 1];
				
				MessageLoader_GetSpeciesName(monDataBlockA->species, HEAP_ID_SYSTEM, baseName);
				String_ToChars(value, nickname, NELEMS(nickname));
				
				monDataBlockB->hasNickname = CharCode_Compare(baseName, nickname);
			}
			// Fallthrough
		
		case MON_DATA_NICKNAME_STRING:
			String_ToChars(value, monDataBlockC->nickname, NELEMS(monDataBlockC->nickname));
			break;
		
		case MON_DATA_UNUSED_121:
			monDataBlockC->unused = *u8Value;
			break;
		
		case MON_DATA_MET_GAME:
			monDataBlockC->originGame = *u8Value;
			break;
		
		case MON_DATA_SUPER_COOL_RIBBON:
		case MON_DATA_SUPER_COOL_RIBBON_GREAT:
		case MON_DATA_SUPER_COOL_RIBBON_ULTRA:
		case MON_DATA_SUPER_COOL_RIBBON_MASTER:
		case MON_DATA_SUPER_BEAUTY_RIBBON:
		case MON_DATA_SUPER_BEAUTY_RIBBON_GREAT:
		case MON_DATA_SUPER_BEAUTY_RIBBON_ULTRA:
		case MON_DATA_SUPER_BEAUTY_RIBBON_MASTER:
		case MON_DATA_SUPER_CUTE_RIBBON:
		case MON_DATA_SUPER_CUTE_RIBBON_GREAT:
		case MON_DATA_SUPER_CUTE_RIBBON_ULTRA:
		case MON_DATA_SUPER_CUTE_RIBBON_MASTER:
		case MON_DATA_SUPER_SMART_RIBBON:
		case MON_DATA_SUPER_SMART_RIBBON_GREAT:
		case MON_DATA_SUPER_SMART_RIBBON_ULTRA:
		case MON_DATA_SUPER_SMART_RIBBON_MASTER:
		case MON_DATA_SUPER_TOUGH_RIBBON:
		case MON_DATA_SUPER_TOUGH_RIBBON_GREAT:
		case MON_DATA_SUPER_TOUGH_RIBBON_ULTRA:
		case MON_DATA_SUPER_TOUGH_RIBBON_MASTER:
		case MON_DATA_UNUSED_RIBBON_143:
			bit = *u8Value;
			bit <<= (param - MON_DATA_SUPER_COOL_RIBBON);
			if (*u8Value) {
				monDataBlockC->ribbonsDS2 |= bit;
			} else {
				monDataBlockC->ribbonsDS2 &= (bit ^ 0xFFFFFFFFFFFFFFFF);
			}
			break;
		
		case MON_DATA_OT_NAME:
			for (int i = 0; i < NELEMS(monDataBlockD->otName); i++) {
				monDataBlockD->otName[i] = u16Value[i];
			}
			break;
		
		case MON_DATA_OT_NAME_STRING:
			String_ToChars(value, monDataBlockD->otName, NELEMS(monDataBlockD->otName));
			break;
		
		case MON_DATA_EGG_YEAR:
			monDataBlockD->eggYear = *u8Value;
			break;
		
		case MON_DATA_EGG_MONTH:
			monDataBlockD->eggMonth = *u8Value;
			break;
		
		case MON_DATA_EGG_DAY:
			monDataBlockD->eggDay = *u8Value;
			break;
		
		case MON_DATA_MET_YEAR:
			monDataBlockD->metYear = *u8Value;
			break;
		
		case MON_DATA_MET_MONTH:
			monDataBlockD->metMonth = *u8Value;
			break;
		
		case MON_DATA_MET_DAY:
			monDataBlockD->metDay = *u8Value;
			break;
		
		case MON_DATA_EGG_LOCATION:
			// BUG: DP data write only
			monDataBlockD->EggLocation_DP = *u16Value;
			break;
		
		case MON_DATA_MET_LOCATION:
			// BUG: DP data write only
			monDataBlockD->MetLocation_DP = *u16Value;
			break;
		
		case MON_DATA_POKERUS:
			monDataBlockD->pokerus = *u8Value;
			break;
		
		case MON_DATA_POKEBALL:
			monDataBlockD->pokeball = *u8Value;
			break;
		
		case MON_DATA_MET_LEVEL:
			monDataBlockD->metLevel = *u8Value;
			break;
		
		case MON_DATA_OT_GENDER:
			monDataBlockD->otGender = *u8Value;
			break;
		
		case MON_DATA_MET_TERRAIN:
			monDataBlockD->metTerrain = *u8Value;
			break;
		
		case MON_DATA_UNUSED_159:
			monDataBlockD->unused = *u16Value;
			break;
		
		case MON_DATA_COMBINED_IVS:
			monDataBlockB->hpIV    = (*u32Value >>  0) & 0x1F;
			monDataBlockB->atkIV   = (*u32Value >>  5) & 0x1F;
			monDataBlockB->defIV   = (*u32Value >> 10) & 0x1F;
			monDataBlockB->speedIV = (*u32Value >> 15) & 0x1F;
			monDataBlockB->spAtkIV = (*u32Value >> 20) & 0x1F;
			monDataBlockB->spDefIV = (*u32Value >> 25) & 0x1F;
			break;
		
		case MON_DATA_NO_PRINT_GENDER:
		case MON_DATA_TYPE_1:
		case MON_DATA_TYPE_2:
			break;
		
		case MON_DATA_SPECIES_NAME:
			{
				String *string = MessageUtil_SpeciesName(monDataBlockA->species, HEAP_ID_SYSTEM);
				
				String_ToChars(string, monDataBlockC->nickname, NELEMS(monDataBlockC->nickname));
				String_Free(string);
				
				break;
			}
	}
}


SpeciesData* SpeciesData_FromMonSpecies(int monSpecies, HeapID heapID) {
	SpeciesData* speciesData = Heap_Alloc(heapID, sizeof(SpeciesData));
	SpeciesData_LoadSpecies(monSpecies, speciesData);
	return speciesData;
}


u32 SpeciesData_GetValue(SpeciesData* speciesData, SpeciesDataParam param) {
	GF_ASSERT(speciesData);
	
	u32 result;
	switch (param) {
		case SPECIES_DATA_BASE_HP:
			result = speciesData->baseStats.hp;
			break;
		
		case SPECIES_DATA_BASE_ATK:
			result = speciesData->baseStats.attack;
			break;
		
		case SPECIES_DATA_BASE_DEF:
			result = speciesData->baseStats.defense;
			break;
		
		case SPECIES_DATA_BASE_SPEED:
			result = speciesData->baseStats.speed;
			break;
		
		case SPECIES_DATA_BASE_SP_ATK:
			result = speciesData->baseStats.spAttack;
			break;
		
		case SPECIES_DATA_BASE_SP_DEF:
			result = speciesData->baseStats.spDefense;
			break;
		
		case SPECIES_DATA_TYPE_1:
			result = speciesData->types[0];
			break;
		
		case SPECIES_DATA_TYPE_2:
			result = speciesData->types[1];
			break;
		
		case SPECIES_DATA_CATCH_RATE:
			result = speciesData->catchRate;
			break;
		
		case SPECIES_DATA_BASE_EXP_REWARD:
			result = speciesData->baseExpReward;
			break;
		
		case SPECIES_DATA_EV_HP_YIELD:
			result = speciesData->evYields.hp;
			break;
		
		case SPECIES_DATA_EV_ATK_YIELD:
			result = speciesData->evYields.attack;
			break;
		
		case SPECIES_DATA_EV_DEF_YIELD:
			result = speciesData->evYields.defense;
			break;
		
		case SPECIES_DATA_EV_SPEED_YIELD:
			result = speciesData->evYields.speed;
			break;
		
		case SPECIES_DATA_EV_SP_ATK_YIELD:
			result = speciesData->evYields.spAttack;
			break;
		
		case SPECIES_DATA_EV_SP_DEF_YIELD:
			result = speciesData->evYields.spDefense;
			break;
		
		case SPECIES_DATA_HELD_ITEM_COMMON:
			result = speciesData->wildHeldItems.common;
			break;
		
		case SPECIES_DATA_HELD_ITEM_RARE:
			result = speciesData->wildHeldItems.rare;
			break;
		
		case SPECIES_DATA_GENDER_RATIO:
			result = speciesData->genderRatio;
			break;
		
		case SPECIES_DATA_HATCH_CYCLES:
			result = speciesData->hatchCycles;
			break;
		
		case SPECIES_DATA_BASE_FRIENDSHIP:
			result = speciesData->baseFriendship;
			break;
		
		case SPECIES_DATA_EXP_RATE:
			result = speciesData->expRate;
			break;
		
		case SPECIES_DATA_EGG_GROUP_1:
			result = speciesData->eggGroups[0];
			break;
		
		case SPECIES_DATA_EGG_GROUP_2:
			result = speciesData->eggGroups[1];
			break;
		
		case SPECIES_DATA_ABILITY_1:
			result = speciesData->abilities[0];
			break;
		
		case SPECIES_DATA_ABILITY_2:
			result = speciesData->abilities[1];
			break;
		
		case SPECIES_DATA_SAFARI_FLEE_RATE:
			result = speciesData->safariFleeRate;
			break;
		
		case SPECIES_DATA_BODY_COLOR:
			result = speciesData->bodyColor;
			break;
		
		case SPECIES_DATA_FLIP_SPRITE:
			result = speciesData->flipSprite;
			break;
		
		case SPECIES_DATA_TM_LEARNSET_MASK_1:
			result = speciesData->tmLearnsetMasks[0];
			break;
		
		case SPECIES_DATA_TM_LEARNSET_MASK_2:
			result = speciesData->tmLearnsetMasks[1];
			break;
		
		case SPECIES_DATA_TM_LEARNSET_MASK_3:
			result = speciesData->tmLearnsetMasks[2];
			break;
		
		case SPECIES_DATA_TM_LEARNSET_MASK_4:
			result = speciesData->tmLearnsetMasks[3];
			break;
	}
	
	return result;
}


void SpeciesData_Free(SpeciesData* speciesData) {
	GF_ASSERT(speciesData);
	Heap_Free(speciesData);
}


u32 SpeciesData_GetFormValue(int monSpecies, int monForm, SpeciesDataParam param) {
	monSpecies = Pokemon_GetFormNarcIndex(monSpecies, monForm);
	SpeciesData* speciesData = SpeciesData_FromMonSpecies(monSpecies, HEAP_ID_SYSTEM);
	
	u32 result = SpeciesData_GetValue(speciesData, param);
	
	SpeciesData_Free(speciesData);
	
	return result;
}


u32 SpeciesData_GetSpeciesValue(int monSpecies, SpeciesDataParam param) {
	SpeciesData* speciesData = SpeciesData_FromMonSpecies(monSpecies, HEAP_ID_SYSTEM);
	
	u32 result = SpeciesData_GetValue(speciesData, param);
	
	SpeciesData_Free(speciesData);
	
	return result;
}


u32 Pokemon_GetSpeciesBaseExpAt(int monSpecies, int monLevel) {
	return Pokemon_GetExpRateBaseExpAt(SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_EXP_RATE), monLevel);
}


static void Pokemon_LoadExperienceTableOf(ExpRate monExpRate, u32* monExpTable) {
	GF_ASSERT(monExpRate < EXP_RATE_MAX);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_POKETOOL__PERSONAL__GROWTBL;
	} else {
		narc = NARC_INDEX_PL_POKETOOL__PERSONAL__PL_GROWTBL;
	}
	NARC_ReadWholeMemberByIndexPair(monExpTable, narc, monExpRate);
}


static u32 Pokemon_GetExpRateBaseExpAt(ExpRate monExpRate, int monLevel) {
	GF_ASSERT(monExpRate < EXP_RATE_MAX);
	GF_ASSERT(monLevel <= MAX_POKEMON_LEVEL + 1);
	
	u32* expTable = Heap_Alloc(HEAP_ID_SYSTEM, (MAX_POKEMON_LEVEL + 1) * 4);
	Pokemon_LoadExperienceTableOf(monExpRate, expTable);
	
	u32 result = expTable[monLevel];
	Heap_Free(expTable);
	
	return result;
}


u32 Pokemon_GetLevel(Pokemon* mon) {
	return BoxPokemon_GetLevel(&mon->box);
}


u32 BoxPokemon_GetLevel(BoxPokemon* boxMon) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	int monSpecies = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	u32 monExp = BoxPokemon_GetValue(boxMon, MON_DATA_EXPERIENCE, NULL);
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	
	return Pokemon_GetSpeciesLevelAt(monSpecies, monExp);
}


u32 Pokemon_GetSpeciesLevelAt(u16 monSpecies, u32 monExp) {
	SpeciesData* speciesData = SpeciesData_FromMonSpecies(monSpecies, HEAP_ID_SYSTEM);
	
	u32 monLevel = SpeciesData_GetLevelAt(speciesData, monSpecies, monExp);
	
	SpeciesData_Free(speciesData);
	
	return monLevel;
}


u32 SpeciesData_GetLevelAt(SpeciesData* speciesData, u16 unused_monSpecies, u32 monExp) {
	#pragma unused(unused_monSpecies)
	
	static u32 monExpTable[MAX_POKEMON_LEVEL + 1];
	
	ExpRate monExpRate = SpeciesData_GetValue(speciesData, SPECIES_DATA_EXP_RATE);
	Pokemon_LoadExperienceTableOf(monExpRate, monExpTable);
	
	int i;
	for (i = 1; i < MAX_POKEMON_LEVEL + 1; i++) {
		if (monExpTable[i] > monExp) {
			break;
		}
	}
	
	return i - 1;
}


u8 Pokemon_GetNature(Pokemon* mon) {
	return BoxPokemon_GetNature(&mon->box);
}


u8 BoxPokemon_GetNature(BoxPokemon* boxMon) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	u32 monPersonality = BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL);
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	
	return Pokemon_GetNatureOf(monPersonality);
}


u8 Pokemon_GetNatureOf(u32 monPersonality) {
	return (u8)(monPersonality % NATURE_MAX);
}


static u16 Pokemon_GetNatureStatValue(u8 monNature, u16 monStatValue, u8 statType) {
	enum {
		STAT_HINDERED = -1,
		STAT_NEUTRAL  =  0,
		STAT_BOOSTED  =  1
	};

	static const s8 sNatureStatAffinities[][5] = {
		[NATURE_HARDY] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_LONELY] = {
			[STAT_ATTACK - 1]          = STAT_BOOSTED,
			[STAT_DEFENSE - 1]         = STAT_HINDERED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_BRAVE] = {
			[STAT_ATTACK - 1]          = STAT_BOOSTED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_HINDERED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_ADAMANT] = {
			[STAT_ATTACK - 1]          = STAT_BOOSTED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_HINDERED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_NAUGHTY] = {
			[STAT_ATTACK - 1]          = STAT_BOOSTED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_HINDERED
		},
		
		[NATURE_BOLD] = {
			[STAT_ATTACK - 1]          = STAT_HINDERED,
			[STAT_DEFENSE - 1]         = STAT_BOOSTED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_DOCILE] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_RELAXED] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_BOOSTED,
			[STAT_SPEED - 1]           = STAT_HINDERED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_IMPISH] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_BOOSTED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_HINDERED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_LAX] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_BOOSTED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_HINDERED
		},
		
		[NATURE_TIMID] = {
			[STAT_ATTACK - 1]          = STAT_HINDERED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_BOOSTED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_HASTY] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_HINDERED,
			[STAT_SPEED - 1]           = STAT_BOOSTED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_SERIOUS] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_JOLLY] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_BOOSTED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_HINDERED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_NAIVE] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_BOOSTED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_HINDERED
		},
		
		[NATURE_MODEST] = {
			[STAT_ATTACK - 1]          = STAT_HINDERED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_BOOSTED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_MILD] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_HINDERED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_BOOSTED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_QUIET] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_HINDERED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_BOOSTED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_BASHFUL] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		},
		
		[NATURE_RASH] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_BOOSTED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_HINDERED
		},
		
		[NATURE_CALM] = {
			[STAT_ATTACK - 1]          = STAT_HINDERED,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_BOOSTED
		},
		
		[NATURE_GENTLE] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_HINDERED,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_BOOSTED
		},
		
		[NATURE_SASSY] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_HINDERED,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_BOOSTED
		},
		
		[NATURE_CAREFUL] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_HINDERED,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_BOOSTED
		},
		
		[NATURE_QUIRKY] = {
			[STAT_ATTACK - 1]          = STAT_NEUTRAL,
			[STAT_DEFENSE - 1]         = STAT_NEUTRAL,
			[STAT_SPEED - 1]           = STAT_NEUTRAL,
			[STAT_SPECIAL_ATTACK - 1]  = STAT_NEUTRAL,
			[STAT_SPECIAL_DEFENSE - 1] = STAT_NEUTRAL
		}
	};
	
	enum {
		STAT_BOOSTED_MULTIPLIER = 110,
		STAT_HINDERED_MULTIPLIER = 90
	};
	
	if (statType < STAT_ATTACK || statType > STAT_SPECIAL_DEFENSE) {
		return monStatValue;
	}
	
	u16 result;
	switch (sNatureStatAffinities[monNature][statType - 1]) {
		case STAT_BOOSTED:
			result = monStatValue * STAT_BOOSTED_MULTIPLIER;
			result /= 100;
			break;
		
		case STAT_HINDERED:
			result = monStatValue * STAT_HINDERED_MULTIPLIER;
			result /= 100;
			break;
		
		default:
			result = monStatValue;
			break;
	}
	
	return result;
}


void Dummy_Unused_0205EA50(void);
void Dummy_Unused_0205EA50(void) {
	// For friendship modification in the base game. Unused.
	static const struct {
		s8 data[10][3];
	} sFriendshipChangeTable_Unused_0205EA50 = {
		{
			{  5,  3,  2  },
			{  5,  3,  2  },
			{  1,  1,  0  },
			{  3,  2,  1  },
			{  1,  1,  0  },
			{  1,  1,  1  },
			{ -1, -1, -1  },
			{ -5, -5, -10 },
			{ -5, -5, -10 },
			{  3,  2,  1  }
		}
	};
}


u8 Pokemon_GetGender(Pokemon* mon) {
	return BoxPokemon_GetGender(&mon->box);
}


u8 BoxPokemon_GetGender(BoxPokemon* boxMon) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	u16 monSpecies = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	u32 monPersonality = BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL);
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	
	return Pokemon_GetGenderOf(monSpecies, monPersonality);
}


u8 Pokemon_GetGenderOf(u16 monSpecies, u32 monPersonality) {
	SpeciesData* speciesData = SpeciesData_FromMonSpecies(monSpecies, HEAP_ID_SYSTEM);
	u8 monGender = SpeciesData_GetGenderOf(speciesData, monSpecies, monPersonality);
	
	SpeciesData_Free(speciesData);
	
	return monGender;
}


u8 SpeciesData_GetGenderOf(SpeciesData* speciesData, u16 unused_monSpecies, u32 monPersonality) {
	#pragma unused(unused_monSpecies)
	
	u8 monGender = SpeciesData_GetValue(speciesData, SPECIES_DATA_GENDER_RATIO);
	
	switch (monGender) {
		case GENDER_RATIO_MALE_ONLY:
			return GENDER_MALE;
		
		case GENDER_RATIO_FEMALE_ONLY:
			return GENDER_FEMALE;
		
		case GENDER_RATIO_NO_GENDER:
			return GENDER_NONE;
	}
	
	if (monGender > (monPersonality & 0xFF)) {
		return GENDER_FEMALE;
	} else {
		return GENDER_MALE;
	}
}


u8 Pokemon_IsShiny(Pokemon* mon) {
	return BoxPokemon_IsShiny(&mon->box);
}


static u8 BoxPokemon_IsShiny(BoxPokemon* boxMon) {
	u32 monOTID = BoxPokemon_GetValue(boxMon, MON_DATA_OT_ID, NULL);
	u32 monPersonality = BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL);
	
	return Pokemon_IsPersonalityShiny(monOTID, monPersonality);
}


u8 Pokemon_IsPersonalityShiny(u32 monOTID, u32 monPersonality) {
	return Pokemon_InlineIsPersonalityShiny(monOTID, monPersonality);
}


void Pokemon_BuildSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, Pokemon* mon, u8 face) {
	BoxPokemon_BuildSpriteTemplate(spriteTemplate, &mon->box, face);
}


void BoxPokemon_BuildSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, BoxPokemon *mon, u8 face) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(mon);
	
	u16 monSpeciesEgg = BoxPokemon_GetValue(mon, MON_DATA_SPECIES_OR_EGG, NULL);
	u8 monGender = BoxPokemon_GetGender(mon);
	u8 monShininess = BoxPokemon_IsShiny(mon);
	u32 monPersonality = BoxPokemon_GetValue(mon, MON_DATA_PERSONALITY, NULL);
	
	u8 monForm;
	if (monSpeciesEgg == SPECIES_EGG) {
		if (BoxPokemon_GetValue(mon, MON_DATA_SPECIES, NULL) == SPECIES_MANAPHY) {
			monForm = 1;
		} else {
			monForm = 0;
		}
	} else {
		monForm = BoxPokemon_GetValue(mon, MON_DATA_FORM, NULL);
	}
	
	BuildPokemonSpriteTemplate(spriteTemplate, monSpeciesEgg, monGender, face, monShininess, monForm, monPersonality);
	BoxPokemon_ExitDecryptionContext(mon, reencrypt);
}


void BuildPokemonSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, u16 species, u8 gender, u8 face, u8 shiny, u8 form, u32 personality) {
	spriteTemplate->spindaSpots = 0;
	spriteTemplate->dummy = 0;
	spriteTemplate->personality = 0;
	
	form = Pokemon_SanitizeFormId(species, form);
	
	switch (species) {
		case SPECIES_BURMY:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 72 + (face / 2) + (form * 2);
				spriteTemplate->palette = 146 + shiny + (form * 2);
			} else {
				spriteTemplate->character = 72 + (face / 2) + (form * 2);
				spriteTemplate->palette = 166 + shiny + (form * 2);
			}
			return;
		
		case SPECIES_WORMADAM:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 78 + (face / 2) + (form * 2);
				spriteTemplate->palette = 152 + shiny + (form * 2);
			} else {
				spriteTemplate->character = 78 + (face / 2) + (form * 2);
				spriteTemplate->palette = 172 + shiny + (form * 2);
			}
			return;
		
		case SPECIES_SHELLOS:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 84 + face + form;
				spriteTemplate->palette = 158 + shiny + (form * 2);
			} else {
				spriteTemplate->character = 84 + face + form;
				spriteTemplate->palette = 178 + shiny + (form * 2);
			}
			return;
		
		case SPECIES_GASTRODON:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 88 + face + form;
				spriteTemplate->palette = 162 + shiny + (form * 2);
			} else {
				spriteTemplate->character = 88 + face + form;
				spriteTemplate->palette = 182 + shiny + (form * 2);
			}
			return;
		
		case SPECIES_CHERRIM:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 92 + face + form;
				spriteTemplate->palette = 166 + (shiny * 2) + form;
			} else {
				spriteTemplate->character = 92 + face + form;
				spriteTemplate->palette = 186 + (shiny * 2) + form;
			}
			return;
		
		case SPECIES_ARCEUS:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 96 + (face / 2) + (form * 2);
				spriteTemplate->palette = 170 + shiny + (form * 2);
			} else {
				spriteTemplate->character = 96 + (face / 2) + (form * 2);
				spriteTemplate->palette = 190 + shiny + (form * 2);
			}
			return;
		
		case SPECIES_CASTFORM:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 64 + (face * 2) + form;
				spriteTemplate->palette = 138 + (shiny * 4) + form;
			} else {
				spriteTemplate->character = 64 + (face * 2) + form;
				spriteTemplate->palette = 158 + (shiny * 4) + form;
			}
			return;
		
		case SPECIES_DEOXYS:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 0 + (face / 2) + (form * 2);
				spriteTemplate->palette = 134 + shiny;
			} else {
				spriteTemplate->character = 0 + (face / 2) + (form * 2);
				spriteTemplate->palette = 154 + shiny;
			}
			return;
		
		case SPECIES_UNOWN:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 8 + (face / 2) + (form * 2);
				spriteTemplate->palette = 136 + shiny;
			} else {
				spriteTemplate->character = 8 + (face / 2) + (form * 2);
				spriteTemplate->palette = 156 + shiny;
			}
			return;
		
		case SPECIES_EGG:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 132 + form;
				spriteTemplate->palette = 206 + form;
			} else {
				spriteTemplate->character = 132 + form;
				spriteTemplate->palette = 226 + form;
			}
			return;
		
		case SPECIES_BAD_EGG:
			spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE;
			if (gIsDiamondPearl) {
				spriteTemplate->character = 132;
				spriteTemplate->palette = 206;
			} else {
				spriteTemplate->character = 132;
				spriteTemplate->palette = 226;
			}
			return;
	}
	
	// Handle Pt forms
	if (!gIsDiamondPearl) {
		if (species == SPECIES_SHAYMIN) {
			spriteTemplate->narcID = NARC_INDEX_PL_POKETOOL__POKEGRA__PL_OTHERPOKE;
			spriteTemplate->character = 134 + (face / 2) + (form * 2);
			spriteTemplate->palette = 228 + shiny + (form * 2);
			return;
		} else if (species == SPECIES_ROTOM) {
			spriteTemplate->narcID = NARC_INDEX_PL_POKETOOL__POKEGRA__PL_OTHERPOKE;
			spriteTemplate->character = 138 + (face / 2) + (form * 2);
			spriteTemplate->palette = 232 + shiny + (form * 2);
			return;
		} else if (species == SPECIES_GIRATINA) {
			spriteTemplate->narcID = NARC_INDEX_PL_POKETOOL__POKEGRA__PL_OTHERPOKE;
			spriteTemplate->character = 150 + (face / 2) + (form * 2);
			spriteTemplate->palette = 244 + shiny + (form * 2);
			return;
		}
	}
	
	// Default handling
	spriteTemplate->narcID = NARC_INDEX_DP_POKETOOL__POKEGRA__POKEGRA;
	spriteTemplate->character = species * 6 + face + (gender != GENDER_FEMALE ? 1 : 0);
	spriteTemplate->palette = species * 6 + 4 + shiny;
	
	if (species == SPECIES_SPINDA && face == FACE_FRONT) {
		spriteTemplate->spindaSpots = SPECIES_SPINDA;
		spriteTemplate->dummy = 0;
		spriteTemplate->personality = personality;
	}
}


u8 Pokemon_SanitizeFormId(u16 monSpecies, u8 monForm) {
	switch (monSpecies) {
		case SPECIES_BURMY:
			if (monForm > BURMY_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_WORMADAM:
			if (monForm > WORMADAM_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_SHELLOS:
			if (monForm > SHELLOS_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_GASTRODON:
			if (monForm > GASTRODON_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_CHERRIM:
			if (monForm > CHERRIM_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_ARCEUS:
			if (monForm > ARCEUS_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_CASTFORM:
			if (monForm > CASTFORM_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_DEOXYS:
			if (monForm > DEOXYS_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_UNOWN:
			if (monForm >= UNOWN_FORM_COUNT) {
				monForm = 0;
			}
			break;
		
		case SPECIES_EGG:
			if (monForm > EGG_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		// Handle Pt forms
		case SPECIES_SHAYMIN:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm > SHAYMIN_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_ROTOM:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm > ROTOM_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
		
		case SPECIES_GIRATINA:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm > GIRATINA_FORM_COUNT - 1) {
				monForm = 0;
			}
			break;
	}
	
	return monForm;
}


void Pokemon_LoadSpriteAnimationFrames(SpriteAnimFramePt* animFrames, u16 species) {
	if (gIsDiamondPearl) {
		SpriteAnimationTableDP animDp;
		NARC_ReadFromMemberByIndexPair(
			&animDp,
			NARC_INDEX_DP_POKETOOL__POKEANM__POKEANM,
			0,
			sizeof(SpriteAnimationTableDP) * species,
			sizeof(SpriteAnimationTableDP)
		);
		
		// Convert DP structure to Pt
		for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
			animFrames[i].spriteFrame = animDp.animMain[i].spriteFrame;
			animFrames[i].frameDelay  = animDp.animMain[i].frameDelay;
		}
	} else {
		SpriteAnimationTablePt animPt;
		NARC_ReadFromMemberByIndexPair(
			&animPt,
			NARC_INDEX_PL_POKETOOL__POKEANM__PL_POKEANM,
			0,
			sizeof(SpriteAnimationTablePt) * species,
			sizeof(SpriteAnimationTablePt)
		);
		
		MI_CpuCopy8(&animPt.animMain[0], animFrames, sizeof(SpriteAnimFramePt) * MAX_ANIMATION_FRAMES);
	}
}


void Pokemon_LoadSpriteAnimation(PokemonAnimManager* monAnimMan, PokemonSprite* monSprite, u16 species, int face, int nature, BOOL flipSprite, int index) {
	PokemonAnimTemplate anim;
	
	if (gIsDiamondPearl) {
		SpriteAnimationTableDP animDp;
		NARC_ReadFromMemberByIndexPair(
			&animDp,
			NARC_INDEX_DP_POKETOOL__POKEANM__POKEANM,
			0,
			sizeof(SpriteAnimationTableDP) * species,
			sizeof(SpriteAnimationTableDP)
		);
		
		if (face == FACE_FRONT) {
			anim.animation  = animDp.animFront.spriteFrame;
			anim.startDelay = animDp.animFront.frameDelay;
			anim.flipSprite = flipSprite;
		} else {
			int speed = PokemonAnim_GetSpeedFromNature(nature);
			GF_ASSERT(speed < POKEANIMSPEED_MAX);
			
			anim.animation  = animDp.animBack[speed].spriteFrame;
			anim.startDelay = animDp.animBack[speed].frameDelay;
			anim.flipSprite = flipSprite;
		}
	} else {
		SpriteAnimationTablePt animPt;
		NARC_ReadFromMemberByIndexPair(
			&animPt,
			NARC_INDEX_PL_POKETOOL__POKEANM__PL_POKEANM,
			0,
			sizeof(SpriteAnimationTablePt) * species,
			sizeof(SpriteAnimationTablePt)
		);
		
		if (face == FACE_FRONT) {
			anim.animation  = animPt.animFront.spriteFrame;
			anim.startDelay = animPt.animFront.frameDelay;
			anim.flipSprite = flipSprite;
		} else {
			int speed = PokemonAnim_GetSpeedFromNature(nature);
			GF_ASSERT(speed < POKEANIMSPEED_MAX);
			
			anim.animation  = animPt.animBack[speed].spriteFrame;
			anim.startDelay = animPt.animBack[speed].frameDelay;
			anim.flipSprite = flipSprite;
		}
	}
	
	PokemonAnimManager_InitAnim(monAnimMan, monSprite, &anim, index);
}


u32 Pokemon_GetStructSize(void) {
	return sizeof(Pokemon);
}


u32 BoxPokemon_GetStructSize(void) {
	return sizeof(BoxPokemon);
}


u8 Pokemon_GetForm(Pokemon* mon) {
	return BoxPokemon_GetForm(&mon->box);
}


u8 BoxPokemon_GetForm(BoxPokemon* boxMon) {
	return BoxPokemon_GetValue(boxMon, MON_DATA_FORM, NULL);
}


BoxPokemon* Pokemon_GetBoxPokemon(Pokemon* mon) {
	return &mon->box;
}


static void BoxPokemon_SetDefaultMoves(BoxPokemon* boxMon) {
	BOOL reencrypt;
	u16* monLevelUpMoves = Heap_Alloc(HEAP_ID_SYSTEM, sizeof(SpeciesLearnset));
	reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	u16 monSpecies = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	int monForm = BoxPokemon_GetValue(boxMon, MON_DATA_FORM, NULL);
	u8 monLevel = BoxPokemon_GetLevel(boxMon);
	
	Pokemon_LoadLevelUpMovesOf(monSpecies, monForm, monLevelUpMoves);
	
	for (int i = 0; monLevelUpMoves[i] != LEARNSET_SENTINEL_ENTRY; i++) {
		if ((monLevelUpMoves[i] & 0xFE00) <= monLevel << 9) {
			u16 monLevelUpMoveID = monLevelUpMoves[i] & 0x1FF;
			if (BoxPokemon_AddMove(boxMon, monLevelUpMoveID) == LEARNSET_ALL_SLOTS_FILLED) {
				BoxPokemon_ReplaceMove(boxMon, monLevelUpMoveID);
			}
		} else {
			break;
		}
	}
	
	Heap_Free(monLevelUpMoves);
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


static u16 BoxPokemon_AddMove(BoxPokemon* boxMon, u16 moveID) {
	u16 result = LEARNSET_ALL_SLOTS_FILLED;
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	for (int i = 0; i < LEARNED_MOVES_MAX; i++) {
		u16 slotMove;
		if ((slotMove = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1 + i, NULL)) == MOVE_NONE) {
			BoxPokemon_SetMoveSlot(boxMon, moveID, i);
			result = moveID;
			break;
		} else {
			if (slotMove == moveID) {
				result = LEARNSET_MOVE_ALREADY_KNOWN;
				break;
			}
		}
	}
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	return result;
}


static void BoxPokemon_ReplaceMove(BoxPokemon* boxMon, u16 moveID) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	u16 moveIDs[LEARNED_MOVES_MAX];
	u8 movePPs[LEARNED_MOVES_MAX];
	u8 movePPUps[LEARNED_MOVES_MAX];
	
	// Bubble move slots 2 through 4 upwards
	for (int i = 0; i < LEARNED_MOVES_MAX - 1; i++) {
		moveIDs[i]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE2 + i, NULL);
		movePPs[i]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE2_PP + i, NULL);
		movePPUps[i] = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE2_PP_UPS + i, NULL);
	}
	
	moveIDs[LEARNED_MOVES_MAX - 1]   = moveID;
	movePPs[LEARNED_MOVES_MAX - 1]   = MoveTable_LoadParam(moveID, MOVEATTRIBUTE_PP);
	movePPUps[LEARNED_MOVES_MAX - 1] = 0;
	
	for (int i = 0; i < LEARNED_MOVES_MAX; i++) {
		BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1 + i, &moveIDs[i]);
		BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP + i, &movePPs[i]);
		BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP_UPS + i, &movePPUps[i]);
	}
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


void Pokemon_ResetMoveSlot(Pokemon* mon, u16 moveID, u8 moveSlot) {
	Pokemon_SetMoveSlot(mon, moveID, moveSlot);
	
	u32 moveMaxPP, movePPUps = 0;
	Pokemon_SetValue(mon, MON_DATA_MOVE1_PP_UPS + moveSlot, &movePPUps);
	
	moveMaxPP = MoveTable_CalcMaxPP(moveID, 0);
	Pokemon_SetValue(mon, MON_DATA_MOVE1_PP + moveSlot, &moveMaxPP);
}


void Pokemon_SetMoveSlot(Pokemon* mon, u16 moveID, u8 moveSlot) {
	BoxPokemon_SetMoveSlot(&mon->box, moveID, moveSlot);
}


static void BoxPokemon_SetMoveSlot(BoxPokemon* boxMon, u16 moveID, u8 moveSlot) {
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1 + moveSlot, &moveID);
	
	u8 movePPUps = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_PP_UPS + moveSlot, NULL);
	u8 moveMaxPP = MoveTable_CalcMaxPP(moveID, movePPUps);
	
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP + moveSlot, &moveMaxPP);
}


void Pokemon_SwapMoveSlots(Pokemon* mon, int moveSlot1, int moveSlot2) {
	BoxPokemon_SwapMoveSlots(&mon->box, moveSlot1, moveSlot2);
}


void BoxPokemon_SwapMoveSlots(BoxPokemon *boxMon, int moveSlot1, int moveSlot2) {
	u16 moveIDs[2];
	u8 movePPs[2];
	u8 movePPUps[2];
	
	moveIDs[0]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1 + moveSlot1, NULL);
	movePPs[0]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_PP + moveSlot1, NULL);
	movePPUps[0] = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_PP_UPS + moveSlot1, NULL);
	moveIDs[1]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1 + moveSlot2, NULL);
	movePPs[1]   = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_PP + moveSlot2, NULL);
	movePPUps[1] = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_PP_UPS + moveSlot2, NULL);
	
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1 + moveSlot1, &moveIDs[1]);
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP + moveSlot1, &movePPs[1]);
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP_UPS + moveSlot1, &movePPUps[1]);
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1 + moveSlot2, &moveIDs[0]);
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP + moveSlot2, &movePPs[0]);
	BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP_UPS + moveSlot2, &movePPUps[0]);
}


void Pokemon_ClearMoveSlot(Pokemon* mon, u32 moveSlot) {
	u16 moveID;
	u8 movePP;
	u8 movePPUps;
	
	for (u32 i = moveSlot; i < 3; i++) {
		moveID    = Pokemon_GetValue(mon, MON_DATA_MOVE1 + i + 1, NULL);
		movePP    = Pokemon_GetValue(mon, MON_DATA_MOVE1_PP + i + 1, NULL);
		movePPUps = Pokemon_GetValue(mon, MON_DATA_MOVE1_PP_UPS + i + 1, NULL);
		
		Pokemon_SetValue(mon, MON_DATA_MOVE1 + i, &moveID);
		Pokemon_SetValue(mon, MON_DATA_MOVE1_PP + i, &movePP);
		Pokemon_SetValue(mon, MON_DATA_MOVE1_PP_UPS + i, &movePPUps);
	}
	
	moveID    = 0;
	movePP    = 0;
	movePPUps = 0;
	
	Pokemon_SetValue(mon, MON_DATA_MOVE4, &moveID);
	Pokemon_SetValue(mon, MON_DATA_MOVE4_PP, &movePP);
	Pokemon_SetValue(mon, MON_DATA_MOVE4_PP_UPS, &movePPUps);
}


void Pokemon_FromBoxPokemon(BoxPokemon* boxMon, Pokemon* mon) {
	u32 data = 0;
	
	mon->box = *boxMon;
	if (mon->box.boxDecrypted) {
		mon->box.partyDecrypted = TRUE;
	}
	
	Pokemon_SetValue(mon, MON_DATA_STATUS, &data);
	Pokemon_SetValue(mon, MON_DATA_HP, &data);
	Pokemon_SetValue(mon, MON_DATA_MAX_HP, &data);
	
	Mail* mail = Mail_New(HEAP_ID_SYSTEM);
	Pokemon_SetValue(mon, MON_DATA_MAIL, mail);
	Heap_Free(mail);
	
	Pokemon_SetValue(mon, MON_DATA_BALL_CAPSULE_ID, &data);
	
	BallCapsule capsule;
	MI_CpuClearFast(&capsule, sizeof(BallCapsule));
	Pokemon_SetValue(mon, MON_DATA_BALL_CAPSULE, &capsule);
	
	Pokemon_CalcLevelAndStats(mon);
}


u16 Pokemon_SinnohDexNumber(u16 species) {
	u16 result;
	NARC_ReadFromMemberByIndexPair(&result, NARC_INDEX_DP_POKETOOL__POKEZUKAN, 0, species * sizeof(u16), sizeof(u16));
	return result;
}


s8 Pokemon_GetFlavorAffinity(Pokemon* mon, Flavor flavor) {
	return BoxPokemon_GetFlavorAffinity(&mon->box, flavor);
}


static s8 BoxPokemon_GetFlavorAffinity(BoxPokemon* boxMon, Flavor flavor) {
	return Pokemon_GetFlavorAffinityOf(BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL), flavor);
}


s8 Pokemon_GetFlavorAffinityOf(u32 monPersonality, Flavor flavor) {
	u8 monNature = Pokemon_GetNatureOf(monPersonality);
	return sNatureFlavorAffinities[monNature][flavor];
}


BOOL Pokemon_IsInfectedWithPokerus(Pokemon* mon) {
	return BoxPokemon_IsInfectedWithPokerus(&mon->box);
}


static BOOL BoxPokemon_IsInfectedWithPokerus(BoxPokemon* boxMon) {
	return (BoxPokemon_GetValue(boxMon, MON_DATA_POKERUS, NULL) & 0xF) != 0;
}


BOOL Pokemon_HasCuredPokerus(Pokemon* mon) {
	return BoxPokemon_HasCuredPokerus(&mon->box);
}


static BOOL BoxPokemon_HasCuredPokerus(BoxPokemon* boxMon) {
	u8 monPokerus = BoxPokemon_GetValue(boxMon, MON_DATA_POKERUS, NULL);
	if (monPokerus & 0xF) {
		return FALSE;
	}
	
	return (monPokerus & 0xF0) != 0;
}


void BoxPokemon_SetArceusForm(BoxPokemon* boxMon) {
	int monSpecies  = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	int monAbility  = BoxPokemon_GetValue(boxMon, MON_DATA_ABILITY, NULL);
	int monHeldItem = BoxPokemon_GetValue(boxMon, MON_DATA_HELD_ITEM, NULL);
	
	if (monSpecies == SPECIES_ARCEUS && monAbility == ABILITY_MULTITYPE) {
		int monForm = Pokemon_GetArceusTypeOf(Item_LoadParam(monHeldItem, ITEM_PARAM_HOLD_EFFECT, HEAP_ID_SYSTEM));
		BoxPokemon_SetValue(boxMon, MON_DATA_FORM, &monForm);
	}
}


u8 Pokemon_GetArceusTypeOf(u16 itemHoldEffect) {
	u8 type;
	
	switch (itemHoldEffect) {
		case HOLD_EFFECT_ARCEUS_FIRE:
			type = TYPE_FIRE;
			break;
		
		case HOLD_EFFECT_ARCEUS_WATER:
			type = TYPE_WATER;
			break;
		
		case HOLD_EFFECT_ARCEUS_ELECTRIC:
			type = TYPE_ELECTRIC;
			break;
		
		case HOLD_EFFECT_ARCEUS_GRASS:
			type = TYPE_GRASS;
			break;
		
		case HOLD_EFFECT_ARCEUS_ICE:
			type = TYPE_ICE;
			break;
		
		case HOLD_EFFECT_ARCEUS_FIGHTING:
			type = TYPE_FIGHTING;
			break;
		
		case HOLD_EFFECT_ARCEUS_POISON:
			type = TYPE_POISON;
			break;
		
		case HOLD_EFFECT_ARCEUS_GROUND:
			type = TYPE_GROUND;
			break;
		
		case HOLD_EFFECT_ARCEUS_FLYING:
			type = TYPE_FLYING;
			break;
		
		case HOLD_EFFECT_ARCEUS_PSYCHIC:
			type = TYPE_PSYCHIC;
			break;
		
		case HOLD_EFFECT_ARCEUS_BUG:
			type = TYPE_BUG;
			break;
		
		case HOLD_EFFECT_ARCEUS_ROCK:
			type = TYPE_ROCK;
			break;
		
		case HOLD_EFFECT_ARCEUS_GHOST:
			type = TYPE_GHOST;
			break;
		
		case HOLD_EFFECT_ARCEUS_DRAGON:
			type = TYPE_DRAGON;
			break;
		
		case HOLD_EFFECT_ARCEUS_DARK:
			type = TYPE_DARK;
			break;
		
		case HOLD_EFFECT_ARCEUS_STEEL:
			type = TYPE_STEEL;
			break;
		
		default:
			type = TYPE_NORMAL;
			break;
	}

	return type;
}


BOOL Pokemon_SetRotomForm(Pokemon* mon, int form, int moveSlot) {
	static const u16 sRotomFormMoves[] = {
		MOVE_NONE,
		MOVE_OVERHEAT,
		MOVE_HYDRO_PUMP,
		MOVE_BLIZZARD,
		MOVE_AIR_SLASH,
		MOVE_LEAF_STORM
	};
	
	int monSpecies = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
	if (monSpecies != SPECIES_ROTOM) {
		return FALSE;
	}
	
	int currentMonForm = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
	
	int newFormMoveID = sRotomFormMoves[form];
	
	int i;
	for (i = 0; i < LEARNED_MOVES_MAX; i++) {
		int j;
		int moveID = Pokemon_GetValue(mon, MON_DATA_MOVE1 + i, NULL);
		
		for (j = 1; j < NELEMS(sRotomFormMoves); j++) {
			if (moveID != MOVE_NONE && moveID == sRotomFormMoves[j]) {
				if (newFormMoveID != MOVE_NONE) {
					Pokemon_ResetMoveSlot(mon, newFormMoveID, i);
					newFormMoveID = MOVE_NONE;
					break;
				} else {
					Pokemon_ClearMoveSlot(mon, i);
					i--;
					break;
				}
			}
		}
	}
	
	if (newFormMoveID != MOVE_NONE) {
		for (i = 0; i < LEARNED_MOVES_MAX; i++) {
			if (Pokemon_GetValue(mon, MON_DATA_MOVE1 + i, NULL) == MOVE_NONE) {
				Pokemon_ResetMoveSlot(mon, newFormMoveID, i);
				break;
			}
		}
		
		if (i == 4) {
			Pokemon_ResetMoveSlot(mon, newFormMoveID, moveSlot);
		}
	}
	
	if (Pokemon_GetValue(mon, MON_DATA_MOVE1, NULL) == MOVE_NONE) {
		Pokemon_ResetMoveSlot(mon, MOVE_THUNDER_SHOCK, 0);
	}
	
	Pokemon_SetValue(mon, MON_DATA_FORM, &form);
	Pokemon_CalcAbility(mon);
	Pokemon_CalcLevelAndStats(mon);
	
	return TRUE;
}


void Dummy_Unused_0205EA3C(void);
void Dummy_Unused_0205EA3C(void) {
	// For giving wild Pokemon held items in the base game. Unused.
	static const struct {
		u16 data[2][2];
	} sHeldItemChance_0205EA3C = {
		{
			{ 45, 95 },
			{ 20, 80 }
		}
	};
}


void Pokemon_LoadLevelUpMovesOf(int monSpecies, int monForm, u16* monLevelUpMoves) {
	monSpecies = Pokemon_GetFormNarcIndex(monSpecies, monForm);
	NARC_ReadWholeMemberByIndexPair(monLevelUpMoves, NARC_INDEX_DP_POKETOOL__PERSONAL__WOTBL, monSpecies);
}


void Pokemon_CalcAbility(Pokemon* mon) {
	BoxPokemon_CalcAbility(&mon->box);
}


static void BoxPokemon_CalcAbility(BoxPokemon* boxMon) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	int monSpecies     = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	u32 monPersonality = BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL);
	
	// BUG: should get the form and call SpeciesData_GetFormValue for proper form-specific abilities
	int monAbility1 = SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_ABILITY_1);
	int monAbility2 = SpeciesData_GetSpeciesValue(monSpecies, SPECIES_DATA_ABILITY_2);
	
	if (monAbility2 != ABILITY_NONE) {
		if (monPersonality & 1) {
			BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &monAbility2);
		} else {
			BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &monAbility1);
		}
	} else {
		BoxPokemon_SetValue(boxMon, MON_DATA_ABILITY, &monAbility1);
	}
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


static void SpeciesData_LoadSpecies(int monSpecies, SpeciesData* speciesData) {
	NARC_ReadWholeMemberByIndexPair(speciesData, NARC_INDEX_DP_POKETOOL__PERSONAL__PERSONAL, monSpecies);
}


static void SpeciesData_LoadForm(int monSpecies, int monForm, SpeciesData* speciesData) {
	monSpecies = Pokemon_GetFormNarcIndex(monSpecies, monForm);
	NARC_ReadWholeMemberByIndexPair(speciesData, NARC_INDEX_DP_POKETOOL__PERSONAL__PERSONAL, monSpecies);
}


static void Pokemon_EncryptData(void* data, u32 bytes, u32 seed) {
	u16* halfWords = (u16*)data;
	for (int i = 0; i < bytes / 2; i++) {
		halfWords[i] ^= LCRNG_NextFrom(&seed);
	}
}


static void Pokemon_DecryptData(void* data, u32 bytes, u32 seed) {
	Pokemon_EncryptData(data, bytes, seed);
}


static u16 LCRNG_NextFrom(u32* seed) {
	*seed = *seed * LCRNG_MULTIPLIER + LCRNG_INCREMENT;
	return *seed >> 16;
}


static u16 Pokemon_GetDataChecksum(void* data, u32 bytes) {
	int i;
	u16 checksum = 0;
	
	u16* halfWords = data;
	
	for (i = 0; i < bytes / 2; i++) {
		checksum += halfWords[i];
	}
	
	return checksum;
}


#define DATA_BLOCK_SHUFFLE_CASE(v1, v2, v3, v4)            \
    do {                                                   \
        PokemonDataBlock* dataBlocks = boxMon->dataBlocks; \
        switch (dataBlockID) {                             \
            case DATA_BLOCK_A:                             \
                result = &dataBlocks[v1];                  \
                break;                                     \
            case DATA_BLOCK_B:                             \
                result = &dataBlocks[v2];                  \
                break;                                     \
            case DATA_BLOCK_C:                             \
                result = &dataBlocks[v3];                  \
                break;                                     \
            case DATA_BLOCK_D:                             \
                result = &dataBlocks[v4];                  \
                break;                                     \
            }                                              \
    } while(FALSE)

static void* BoxPokemon_GetDataBlock(BoxPokemon* boxMon, u32 personality, PokemonDataBlockID dataBlockID) {
	personality = (personality & 0x3E000) >> 13;
	GF_ASSERT(personality <= 31);
	
	void* result;
	switch (personality) {
		case 0:
		case 24:
			DATA_BLOCK_SHUFFLE_CASE(0, 1, 2, 3);
			break;
		
		case 1:
		case 25:
			DATA_BLOCK_SHUFFLE_CASE(0, 1, 3, 2);
			break;
		
		case 2:
		case 26:
			DATA_BLOCK_SHUFFLE_CASE(0, 2, 1, 3);
			break;
		
		case 3:
		case 27:
			DATA_BLOCK_SHUFFLE_CASE(0, 3, 1, 2);
			break;
		
		case 4:
		case 28:
			DATA_BLOCK_SHUFFLE_CASE(0, 2, 3, 1);
			break;
		
		case 5:
		case 29:
			DATA_BLOCK_SHUFFLE_CASE(0, 3, 2, 1);
			break;
		
		case 6:
		case 30:
			DATA_BLOCK_SHUFFLE_CASE(1, 0, 2, 3);
			break;
		
		case 7:
		case 31:
			DATA_BLOCK_SHUFFLE_CASE(1, 0, 3, 2);
			break;
		
		case 8:
			DATA_BLOCK_SHUFFLE_CASE(2, 0, 1, 3);
			break;
		
		case 9:
			DATA_BLOCK_SHUFFLE_CASE(3, 0, 1, 2);
			break;
		
		case 10:
			DATA_BLOCK_SHUFFLE_CASE(2, 0, 3, 1);
			break;
		
		case 11:
			DATA_BLOCK_SHUFFLE_CASE(3, 0, 2, 1);
			break;
		
		case 12:
			DATA_BLOCK_SHUFFLE_CASE(1, 2, 0, 3);
			break;
		
		case 13:
			DATA_BLOCK_SHUFFLE_CASE(1, 3, 0, 2);
			break;
		
		case 14:
			DATA_BLOCK_SHUFFLE_CASE(2, 1, 0, 3);
			break;
		
		case 15:
			DATA_BLOCK_SHUFFLE_CASE(3, 1, 0, 2);
			break;
		
		case 16:
			DATA_BLOCK_SHUFFLE_CASE(2, 3, 0, 1);
			break;
		
		case 17:
			DATA_BLOCK_SHUFFLE_CASE(3, 2, 0, 1);
			break;
		
		case 18:
			DATA_BLOCK_SHUFFLE_CASE(1, 2, 3, 0);
			break;
		
		case 19:
			DATA_BLOCK_SHUFFLE_CASE(1, 3, 2, 0);
			break;
		
		case 20:
			DATA_BLOCK_SHUFFLE_CASE(2, 1, 3, 0);
			break;
		
		case 21:
			DATA_BLOCK_SHUFFLE_CASE(3, 1, 2, 0);
			break;
		
		case 22:
			DATA_BLOCK_SHUFFLE_CASE(2, 3, 1, 0);
			break;
		
		case 23:
			DATA_BLOCK_SHUFFLE_CASE(3, 2, 1, 0);
			break;
	}
	
	return result;
}


static int Pokemon_GetFormNarcIndex(int monSpecies, int monForm) {
	switch (monSpecies) {
		case SPECIES_DEOXYS:
			if (monForm && monForm <= DEOXYS_FORM_COUNT - 1) {
				monSpecies = (496 - 1) + monForm;
			}
			break;
		
		case SPECIES_WORMADAM:
			if (monForm && monForm <= WORMADAM_FORM_COUNT - 1) {
				monSpecies = (499 - 1) + monForm;
			}
			break;
		
		case SPECIES_GIRATINA:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm && monForm <= GIRATINA_FORM_COUNT - 1) {
				monSpecies = (501 - 1) + monForm;
			}
			break;
		
		case SPECIES_SHAYMIN:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm && monForm <= SHAYMIN_FORM_COUNT - 1) {
				monSpecies = (502 - 1) + monForm;
			}
			break;
		
		case SPECIES_ROTOM:
			if (gIsDiamondPearl) {
				break;
			}
			if (monForm && monForm <= ROTOM_FORM_COUNT - 1) {
				monSpecies = (503 - 1) + monForm;
			}
			break;
		
		default:
			break;
	}
	
	return monSpecies;
}


void Dummy_Unused_0205EA6E(void);
void Dummy_Unused_0205EA6E(void) {
	// For banning species from the Battle Frontier. Unused.
	static const struct {
		u16 data[18];
	} sBattleFrontierBanList_0205EA6E = {
		{
			SPECIES_MEWTWO,
			SPECIES_MEW,
			SPECIES_HO_OH,
			SPECIES_LUGIA,
			SPECIES_CELEBI,
			SPECIES_KYOGRE,
			SPECIES_GROUDON,
			SPECIES_RAYQUAZA,
			SPECIES_JIRACHI,
			SPECIES_DEOXYS,
			SPECIES_DIALGA,
			SPECIES_PALKIA,
			SPECIES_GIRATINA,
			SPECIES_PHIONE,
			SPECIES_MANAPHY,
			SPECIES_DARKRAI,
			SPECIES_SHAYMIN,
			SPECIES_ARCEUS
		}
	};
}


void BoxPokemon_RestorePP(BoxPokemon* boxMon) {
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	for (int i = 0; i < LEARNED_MOVES_MAX; i++) {
		if (BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1 + i, NULL)) {
			u8 moveMaxPP = BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1_MAX_PP + i, NULL);
			
			BoxPokemon_SetValue(boxMon, MON_DATA_MOVE1_PP + i, &moveMaxPP);
		}
	}
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


void PokemonSprite_LoadAnimFramesPt(SpriteAnimFramePt* frames, u16 species, u16 clientType) {
	int face = (clientType & 1) ? 0 : 1;
	
	SpeciesSpriteData data;
	NARC_ReadFromMemberByIndexPair(&data, NARC_INDEX_PL_POKETOOL__POKE_EDIT__PL_POKE_DATA, 0, species * sizeof(SpeciesSpriteData), sizeof(SpeciesSpriteData));
	MI_CpuCopy8(data.faceAnims[face].frames, frames, sizeof(SpriteAnimFramePt) * MAX_ANIMATION_FRAMES);
}


void PokemonSprite_LoadAnimPt(PokemonAnimManager* monAnimMan, PokemonSprite* sprite, u16 species, int face, int flipSprite, int frame) {
	int faceType = (face == FACE_FRONT) ? 0 : 1;
	
	SpeciesSpriteData spriteData;
	NARC_ReadFromMemberByIndexPair(
		&spriteData,
		NARC_INDEX_PL_POKETOOL__POKE_EDIT__PL_POKE_DATA, 
		0, 
		species * sizeof(SpeciesSpriteData),
		sizeof(SpeciesSpriteData)
	);
	
	PokemonAnimTemplate animTemplate;
	animTemplate.animation  = spriteData.faceAnims[faceType].animation;
	animTemplate.startDelay = spriteData.faceAnims[faceType].startDelay;
	animTemplate.flipSprite = flipSprite;
	
	PokemonAnimManager_InitAnim(monAnimMan, sprite, &animTemplate, frame);
}
