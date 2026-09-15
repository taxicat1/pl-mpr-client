#include <nitro.h>

#include "boot.h"

#include "mpr_client/main.h"

#define GAME_CODE_POKEMON_DIAMOND_JP  (('A' << 0) | ('D' << 8) | ('A' << 16) | ('J' << 24))
#define GAME_CODE_POKEMON_PEARL_JP    (('A' << 0) | ('P' << 8) | ('A' << 16) | ('J' << 24))
#define MAKER_CODE_NINTENDO           (('0' << 0) | ('1' << 8))


void AccessOuterRomFS(void) {
	static const char archiveName[] = "rom";
	
	if (!FS_IsAvailable()) {
		OS_Terminate();
	} else {
		const CARDRomHeader* outerHeader;
		
		CARDRomHeader* const checkHeader  = (CARDRomHeader*)HW_MAIN_MEM_SHARED; // 0x027FF000
		CARDRomHeader* const loadedHeader = (CARDRomHeader*)HW_ROM_HEADER_BUF;  // 0x027FFE00
		CARDRomHeader* const backupHeader = (CARDRomHeader*)HW_CARD_ROM_HEADER; // 0x027FFA80
		
		if (checkHeader->game_code == 0) {
			OS_Init();
			CARD_Init();
			MI_CpuCopy8(loadedHeader, checkHeader, HW_CARD_ROM_HEADER_SIZE);
			MI_CpuCopy8(loadedHeader, backupHeader, HW_CARD_ROM_HEADER_SIZE);
		}
		
		outerHeader = checkHeader;
		
		FSArchive* archive = FS_FindArchive(archiveName, 3);
		
		archive->fat      = outerHeader->fat.offset;
		archive->fat_size = outerHeader->fat.length;
		archive->fnt      = outerHeader->fnt.offset;
		archive->fnt_size = outerHeader->fnt.length;
		
		if ((outerHeader->game_code != GAME_CODE_POKEMON_DIAMOND_JP && outerHeader->game_code != GAME_CODE_POKEMON_PEARL_JP) ||
			outerHeader->maker_code != MAKER_CODE_NINTENDO) {
			OS_Terminate();
		}
	}
}


void RebootAndLoadROM(const char* filesystemPath) {
	FSFile file;
	FS_InitFile(&file);
	if (!FS_OpenFile(&file, filesystemPath)) {
		return;
	}
	
	u32 fileStartAddress = FS_GetFileImageTop(&file);
	*(u32*)HW_ROM_BASE_OFFSET_BUF = fileStartAddress;
	OS_ResetSystem(RESET_CLEAN);
}
