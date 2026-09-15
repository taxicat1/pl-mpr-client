#include <nitro.h>
#include <string.h>

#include "pc_boxes.h"

#include "assert.h"
#include "game_version.h"
#include "message.h"
#include "pokemon.h"
#include "savedata.h"
#include "string_gf.h"
#include "constants/species.h"
#include "fs/msgdata/pokemon_storage_system.h"

static void PCBoxes_InitInternal(PCBoxes* pcBoxes);


void PCBoxes_Init(PCBoxes* pcBoxes) {
	PCBoxes_InitInternal(pcBoxes);
	SaveData_SetFullSaveRequired();
}


u32 PCBoxes_GetSaveSize(void) {
	return sizeof(PCBoxes);
}


static void PCBoxes_InitInternal(PCBoxes* pcBoxes) {
	u32 boxID, i;
	for (boxID = 0; boxID < MAX_PC_BOXES; boxID++) {
		for (i = 0; i < MAX_MONS_PER_BOX; i++) {
			BoxPokemon_Init(&pcBoxes->boxMons[boxID][i]);
		}
	}
	
	for (boxID = 0, i = 0; boxID < MAX_PC_BOXES; boxID++) {
		pcBoxes->wallpapers[boxID] = i++;

		if (i >= MAX_DEFAULT_WALLPAPERS) {
			i = 0;
		}
	}
	
	pcBoxes->unlockedWallpapers = 0;
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	MessageLoader* messageLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_POKEMON_STORAGE_SYSTEM, HEAP_ID_SYSTEM);
	if (messageLoader) {
		for (boxID = 0; boxID < MAX_PC_BOXES; boxID++) {
			MessageLoader_Get(messageLoader, POKEMON_STORAGE_SYSTEM_Text_Box1 + boxID, pcBoxes->names[boxID]);
		}
		
		MessageLoader_Free(messageLoader);
	}
	
	pcBoxes->currentBoxID = 0;
}


BOOL PCBoxes_TryStoreBoxMon(PCBoxes* pcBoxes, BoxPokemon* boxMon) {
	u32 boxID = pcBoxes->currentBoxID;
	do {
		BoxPokemon_RestorePP(boxMon);
		
		if (PCBoxes_TryStoreBoxMonInBox(pcBoxes, boxID, boxMon)) {
			SaveData_SetFullSaveRequired();
			return TRUE;
		}
		
		if (++boxID >= MAX_PC_BOXES) {
			boxID = 0;
		}
	} while (boxID != pcBoxes->currentBoxID);
	
	return FALSE;
}


BOOL PCBoxes_TryStoreBoxMonInBox(PCBoxes* pcBoxes, u32 boxID, BoxPokemon* boxMon) {
	u32 monPosInBox;
	
	BoxPokemon_RestorePP(boxMon);
	
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	for (monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
		if (BoxPokemon_GetValue(&pcBoxes->boxMons[boxID][monPosInBox], MON_DATA_SPECIES, NULL) == SPECIES_NONE) {
			pcBoxes->boxMons[boxID][monPosInBox] = *boxMon;
			SaveData_SetFullSaveRequired();
			return TRUE;
		}
	}
	
	return FALSE;
}


BOOL PCBoxes_TryStoreBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 monPosInBox, BoxPokemon* boxMon) {
	BoxPokemon_RestorePP(boxMon);
	
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (boxID < MAX_PC_BOXES && monPosInBox < MAX_MONS_PER_BOX) {
		pcBoxes->boxMons[boxID][monPosInBox] = *boxMon;
		SaveData_SetFullSaveRequired();
		return TRUE;
	} else {
		GF_ASSERT(FALSE);
	}
	
	return FALSE;
}


void PCBoxes_InitBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 monPosInBox) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (monPosInBox < MAX_MONS_PER_BOX && boxID < MAX_PC_BOXES) {
		BoxPokemon_Init(&pcBoxes->boxMons[boxID][monPosInBox]);
		SaveData_SetFullSaveRequired();
	} else {
		GF_ASSERT(FALSE);
	}
}


u32 PCBoxes_GetCurrentBoxID(const PCBoxes* pcBoxes) {
	return pcBoxes->currentBoxID;
}


u32 PCBoxes_FirstEmptyBox(PCBoxes* pcBoxes) {
	int boxID = pcBoxes->currentBoxID;
	
	while (TRUE) {
		for (int monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
			if (BoxPokemon_GetValue(&pcBoxes->boxMons[boxID][monPosInBox], MON_DATA_SPECIES_EXISTS, NULL) == 0) {
				return boxID;
			}
		}
		
		if (++boxID >= MAX_PC_BOXES) {
			boxID = 0;
		}
		
		if (boxID == pcBoxes->currentBoxID) {
			break;
		}
	}
	
	return MAX_PC_BOXES;
}


u32 PCBoxes_CountAllEmptySpace(PCBoxes* pcBoxes) {
	int boxID, monPosInBox;
	u32 count = 0;
	
	for (boxID = 0; boxID < MAX_PC_BOXES; boxID++) {
		for (monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
			if (BoxPokemon_GetValue(&pcBoxes->boxMons[boxID][monPosInBox], MON_DATA_SPECIES_EXISTS, NULL) == 0) {
				count++;
			}
		}
	}
	
	return count;
}


u32 PCBoxes_CountEmptySpaceInBox(PCBoxes* pcBoxes, u32 boxID) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	GF_ASSERT(boxID < MAX_PC_BOXES);
	
	u32 count = 0;
	for (int monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
		if (BoxPokemon_GetValue(&pcBoxes->boxMons[boxID][monPosInBox], MON_DATA_SPECIES_EXISTS, NULL) == 0) {
			count++;
		}
	}
	
	return count;
}


void PCBoxes_SetCurrentBox(PCBoxes* pcBoxes, u32 boxID) {
	if (boxID < MAX_PC_BOXES) {
		pcBoxes->currentBoxID = boxID;
		SaveData_SetFullSaveRequired();
	} else {
		GF_ASSERT(FALSE);
	}
}


u32 PCBoxes_GetWallpaper(const PCBoxes* pcBoxes, u32 boxID) {
	if (boxID < MAX_PC_BOXES) {
		return pcBoxes->wallpapers[boxID];
	} else {
		GF_ASSERT(FALSE);
		return 0;
	}
}


void PCBoxes_SetWallpaper(PCBoxes* pcBoxes, u32 boxID, u32 wallpaper) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (boxID < MAX_PC_BOXES && wallpaper < MAX_WALLPAPERS) {
		if (!gIsDiamondPearl) {
			if (wallpaper >= MAX_DEFAULT_WALLPAPERS) {
				wallpaper += MAX_UNLOCKABLE_WALLPAPERS;
			}
		}
		
		pcBoxes->wallpapers[boxID] = wallpaper;
		SaveData_SetFullSaveRequired();
	} else {
		GF_ASSERT(FALSE);
	}
}


void PCBoxes_BufferBoxName(const PCBoxes* pcBoxes, u32 boxID, String* dest) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (boxID < MAX_PC_BOXES) {
		String_CopyChars(dest, pcBoxes->names[boxID]);
	} else {
		GF_ASSERT(FALSE);
	}
}


void PCBoxes_RenameBox(PCBoxes* pcBoxes, u32 boxID, const String* newName) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (boxID < MAX_PC_BOXES) {
		String_ToChars(newName, pcBoxes->names[boxID], 20);
		SaveData_SetFullSaveRequired();
	}
}


u32 PCBoxes_CountMonsInBox(PCBoxes* pcBoxes, u32 boxID) {
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	if (boxID < MAX_PC_BOXES) {
		u32 count = 0;
		
		for (int monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
			if (BoxPokemon_GetValue(&pcBoxes->boxMons[boxID][monPosInBox], MON_DATA_SPECIES_EXISTS, NULL)) {
				count++;
			}
		}
		
		return count;
	} else {
		GF_ASSERT(FALSE);
	}
	
	return 0;
}


void PCBoxes_SetBoxMonData(PCBoxes* pcBoxes, u32 boxID, u32 slot, PokemonDataParam pokemonData, void* value) {
	GF_ASSERT(boxID < MAX_PC_BOXES || boxID == USE_CURRENT_BOX);
	GF_ASSERT(slot < MAX_MONS_PER_BOX);
	
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	BoxPokemon_SetValue((&pcBoxes->boxMons[boxID][slot]), pokemonData, value);
	SaveData_SetFullSaveRequired();
}


BoxPokemon* PCBoxes_GetBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 slot) {
	GF_ASSERT(boxID < MAX_PC_BOXES || boxID == USE_CURRENT_BOX);
	GF_ASSERT(slot < MAX_MONS_PER_BOX);
	
	if (boxID == USE_CURRENT_BOX) {
		boxID = pcBoxes->currentBoxID;
	}
	
	return &pcBoxes->boxMons[boxID][slot];
}


BOOL PCBoxes_CheckHasUnlockedWallpaper(const PCBoxes* pcBoxes, u32 wallpaper) {
	GF_ASSERT(wallpaper < MAX_UNLOCKABLE_WALLPAPERS);
	return (pcBoxes->unlockedWallpapers & (1 << wallpaper)) != 0;
}


u32 PCBoxes_CountUnlockedWallpapers(const PCBoxes* pcBoxes) {
	u32 i, count;
	for (i = 0, count = 0; i < MAX_UNLOCKABLE_WALLPAPERS; i++) {
		if (PCBoxes_CheckHasUnlockedWallpaper(pcBoxes, i)) {
			count++;
		}
	}
	
	return count;
}
