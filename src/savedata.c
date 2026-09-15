#include <nitro.h>
#include <string.h>

#include "savedata.h"

#include "assert.h"
#include "game_version.h"
#include "save_read_error.h"
#include "save_table.h"
#include "system.h"
#include "heap.h"

static void SaveData_CheckInfoInit(SaveCheckInfo* checkInfo);
static u16 SaveData_CalculateFooterChecksum(const SaveData* saveData, void* startAddress, u32 size);
static u32 SaveData_SaveOffset(int sectorID, const SaveBlockInfo* blockInfo);
static SaveBlockFooter* SaveBlockFooter_Get(SaveData* saveData, u32 bodyAddress, int blockID);
static BOOL SaveBlockFooter_Validate(SaveData* saveData, u32 bodyAddress, int blockID);
static void SaveBlockFooter_CheckInfo(SaveCheckInfo* checkInfo, SaveData* saveData, u32 bodyAddress, int blockID);
static void SaveBlockFooter_Set(SaveData* saveData, u32 bodyAddress, int blockID);
static int SaveCheckInfo_CompareCounters(u32 counter1, u32 counter2);
static int SaveCheckInfo_CompareSectors(const SaveCheckInfo* checkInfo1, const SaveCheckInfo* checkInfo2, int* currentSector, int* staleSector);
static void SaveData_SetBlockCheckInfo(SaveData* saveData, const SaveCheckInfo* normalCheck, const SaveCheckInfo* boxCheck, int normalOffset, int boxOffset);
static int SaveData_LoadCheck(SaveData* saveData);
static BOOL SaveBlock_Load(int sectorID, const SaveBlockInfo* blockInfo, u8* blockOffset);
static BOOL SaveDataState_Load(SaveData* saveData);
static s32 SaveDataState_InitBlock(SaveData* saveData, int blockID, u8 sectorID, BOOL* outSuccess);
static s32 SaveDataState_InitFooter(SaveData* saveData, int blockID, u8 sectorID);
static s32 SaveDataState_InitFooter_Secondary(SaveData* saveData, int blockID, u8 sectorID);
static void SaveDataState_Init(SaveData* saveData, SaveDataState* state, int blockID);
static SaveResult SaveDataState_Main(SaveData* saveData, SaveDataState* state);
static void SaveDataState_End(SaveData* saveData, SaveDataState* state, int saveResult);
static void SavePageInfo_Init(SavePageInfo* pageInfo);
static void SaveBlockInfo_Init(SaveBlockInfo* blockInfo, const SavePageInfo* pageInfo);
static void SaveTable_Clear(SaveDataBody* body, const SavePageInfo* pageInfo);
static void CB_SaveComplete(void* unused);
static s32 SaveData_CardSave_Init(u32 address, void* data, u32 size, BOOL* outSuccess);
static BOOL SaveData_CardSave_Main(s32 lockID, BOOL* result);

static SaveData* sSaveDataPtr = NULL;
static BOOL sSaveComplete;


SaveData* SaveData_Init(void) {
	SaveData* saveData = Heap_Alloc(HEAP_ID_SAVE, sizeof(SaveData));
	
	MI_CpuClearFast(saveData, sizeof(SaveData));
	
	sSaveDataPtr = saveData;
	
	saveData->backupExists     = SaveData_GetCardBackupType();
	saveData->dataExists       = FALSE;
	saveData->isNewGameData    = TRUE;
	saveData->fullSaveRequired = TRUE;
	
	MATH_CRC16CCITTInitTable(&saveData->crcTable);
	
	SavePageInfo_Init(saveData->pageInfo);
	SaveBlockInfo_Init(saveData->blockInfo, saveData->pageInfo);
	
	MI_CpuClearFast(saveData->blockCounters, sizeof(saveData->blockCounters));
	
	saveData->loadCheckStatus = SaveData_LoadCheck(saveData);
	switch (saveData->loadCheckStatus) {
		case LOAD_RESULT_OK:
		case LOAD_RESULT_CORRUPT:
			SaveDataState_Load(saveData);
			saveData->dataExists = TRUE;
			saveData->isNewGameData = FALSE;
			saveData->fullSaveRequired = FALSE;
			break;
		
		case LOAD_RESULT_ERROR:
		case LOAD_RESULT_EMPTY:
			SaveData_Clear(saveData);
			break;
	}
	
	return saveData;
}


void* SaveData_GetSaveTable(SaveData* saveData, int saveTableID) {
	GF_ASSERT(saveTableID < SAVE_TABLE_ENTRY_MAX);
	return &(saveData->body.data[saveData->pageInfo[saveTableID].location]);
}


void SaveData_Clear(SaveData* saveData) {
	saveData->isNewGameData = TRUE;
	saveData->fullSaveRequired = TRUE;
	
	SaveTable_Clear(&saveData->body, saveData->pageInfo);
}


BOOL SaveData_BackupExists(const SaveData* saveData) {
	return saveData->backupExists;
}


u32 SaveData_GetLoadCheckStatus(const SaveData* saveData) {
	return saveData->loadCheckStatus;
}


void SaveData_SetFullSaveRequired(void) {
	sSaveDataPtr->fullSaveRequired = TRUE;
}


void SaveData_SaveStateInit(SaveData* saveData, int blockID) {
	SaveDataState_Init(saveData, &saveData->state, blockID);
}


SaveResult SaveData_SaveStateMain(SaveData* saveData) {
	SaveResult saveResult = SaveDataState_Main(saveData, &saveData->state);
	
	if (saveResult != SAVE_RESULT_PROCEED && saveResult != SAVE_RESULT_PROCEED_FINAL) {
		SaveDataState_End(saveData, &saveData->state, saveResult);
	}
	
	return saveResult;
}


static void SaveData_CheckInfoInit(SaveCheckInfo* checkInfo) {
	checkInfo->valid = FALSE;
	checkInfo->globalCounter = 0;
	checkInfo->blockCounter = 0;
}


static u16 SaveData_CalculateFooterChecksum(const SaveData* saveData, void* startAddress, u32 size) {
	return MATH_CalcCRC16CCITT(&saveData->crcTable, startAddress, size - sizeof(SaveBlockFooter));
}


static u32 SaveData_SaveOffset(int sectorID, const SaveBlockInfo* blockInfo) {
	u32 offset;
	if (sectorID == SECTOR_ID_PRIMARY) {
		offset = PRIMARY_SECTOR_START * SAVE_SECTOR_SIZE;
	} else {
		offset = BACKUP_SECTOR_START * SAVE_SECTOR_SIZE;
	}
	
	offset += blockInfo->offset;
	return offset;
}


static SaveBlockFooter* SaveBlockFooter_Get(SaveData* saveData, u32 bodyAddress, int blockID) {
	u32 footerAddress;
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	
	footerAddress = bodyAddress + blockInfo->offset;
	GF_ASSERT(blockInfo->size);
	
	footerAddress += blockInfo->size;
	footerAddress -= sizeof(SaveBlockFooter);
	
	return (SaveBlockFooter*)footerAddress;
}


static BOOL SaveBlockFooter_Validate(SaveData* saveData, u32 bodyAddress, int blockID) {
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	SaveBlockFooter* footer = SaveBlockFooter_Get(saveData, bodyAddress, blockID);
	u32 startAddress = bodyAddress + blockInfo->offset;
	
	if (footer->size != blockInfo->size) {
		return FALSE;
	}
	
	if (footer->signature != SECTOR_SIGNATURE) {
		return FALSE;
	}
	
	if (footer->saveBlockID != blockID) {
		return FALSE;
	}
	
	if (footer->checksum != SaveData_CalculateFooterChecksum(saveData, (void*)startAddress, blockInfo->size)) {
		return FALSE;
	}
	
	return TRUE;
}


static void SaveBlockFooter_CheckInfo(SaveCheckInfo* checkInfo, SaveData* saveData, u32 bodyAddress, int blockID) {
	SaveBlockFooter* footer = SaveBlockFooter_Get(saveData, bodyAddress, blockID);
	
	checkInfo->valid = SaveBlockFooter_Validate(saveData, bodyAddress, blockID);
	checkInfo->globalCounter = footer->saveCounter;
	checkInfo->blockCounter = footer->blockCounter;
}


static void SaveBlockFooter_Set(SaveData* saveData, u32 bodyAddress, int blockID) {
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	SaveBlockFooter* footer = SaveBlockFooter_Get(saveData, bodyAddress, blockID);
	u32 startAddress = bodyAddress + blockInfo->offset;
	
	footer->saveCounter  = saveData->globalCounter;
	footer->blockCounter = saveData->blockCounters[blockID];
	footer->size         = blockInfo->size;
	footer->signature    = SECTOR_SIGNATURE;
	footer->saveBlockID  = blockID;
	footer->checksum     = SaveData_CalculateFooterChecksum(saveData, (void*)startAddress, blockInfo->size);
}


static int SaveCheckInfo_CompareCounters(u32 counter1, u32 counter2) {
	if (counter1 == 0xFFFFFFFF && counter2 == 0) {
		return -1;
	} else if (counter1 == 0 && counter2 == 0xFFFFFFFF) {
		return 1;
	} else if (counter1 > counter2) {
		return 1;
	} else if (counter1 < counter2) {
		return -1;
	}
	
	return 0;
}


static int SaveCheckInfo_CompareSectors(const SaveCheckInfo* checkInfo1, const SaveCheckInfo* checkInfo2, int* currentSector, int* staleSector) {
	int globalDiff = SaveCheckInfo_CompareCounters(checkInfo1->globalCounter, checkInfo2->globalCounter);
	int blockDiff = SaveCheckInfo_CompareCounters(checkInfo1->blockCounter, checkInfo2->blockCounter);
	
	if (checkInfo1->valid && checkInfo2->valid) {
		if (globalDiff > 0) {
			GF_ASSERT(blockDiff > 0);
			*currentSector = SECTOR_ID_PRIMARY;
			*staleSector = SECTOR_ID_BACKUP;
		} else if (globalDiff < 0) {
			GF_ASSERT(blockDiff < 0);
			*currentSector = SECTOR_ID_BACKUP;
			*staleSector = SECTOR_ID_PRIMARY;
		} else if (blockDiff > 0) {
			*currentSector = SECTOR_ID_PRIMARY;
			*staleSector = SECTOR_ID_BACKUP;
		} else if (blockDiff < 0) {
			*currentSector = SECTOR_ID_BACKUP;
			*staleSector = SECTOR_ID_PRIMARY;
		} else {
			*currentSector = SECTOR_ID_PRIMARY;
			*staleSector = SECTOR_ID_BACKUP;
		}
		
		return SECTOR_RESULT_VALID;
	
	} else if (checkInfo1->valid && !checkInfo2->valid) {
		*currentSector = SECTOR_ID_PRIMARY;
		*staleSector = SECTOR_ID_ERROR;
		return SECTOR_RESULT_PARTIAL_VALID;
	} else if (!checkInfo1->valid && checkInfo2->valid) {
		*currentSector = SECTOR_ID_BACKUP;
		*staleSector = SECTOR_ID_ERROR;
		return SECTOR_RESULT_PARTIAL_VALID;
	} else {
		*currentSector = SECTOR_ID_ERROR;
		*staleSector = SECTOR_ID_ERROR;
		return SECTOR_RESULT_INVALID;
	}
}


static void SaveData_SetBlockCheckInfo(SaveData* saveData, const SaveCheckInfo* normalCheck, const SaveCheckInfo* boxCheck, int normalOffset, int boxOffset) {
	saveData->globalCounter = normalCheck[normalOffset].globalCounter;
	saveData->blockCounters[SAVE_BLOCK_ID_NORMAL] = normalCheck[normalOffset].blockCounter;
	saveData->blockCounters[SAVE_BLOCK_ID_BOXES] = boxCheck[boxOffset].blockCounter;
	saveData->blockOffsets[SAVE_BLOCK_ID_NORMAL] = normalOffset;
	saveData->blockOffsets[SAVE_BLOCK_ID_BOXES] = boxOffset;
}


static int SaveData_LoadCheck(SaveData* saveData) {
	u8* primaryBuffer = Heap_AllocAtEnd(HEAP_ID_APPLICATION, SAVE_SECTOR_SIZE * SAVE_PAGE_MAX);
	u8* backupBuffer = Heap_AllocAtEnd(HEAP_ID_APPLICATION, SAVE_SECTOR_SIZE * SAVE_PAGE_MAX);
	
	SaveCheckInfo normalInfo[SECTOR_ID_MAX];
	SaveCheckInfo boxInfo[SECTOR_ID_MAX];
	
	if (SaveData_CardLoad(PRIMARY_SECTOR_START * SAVE_SECTOR_SIZE, primaryBuffer, SAVE_SECTOR_SIZE * SAVE_PAGE_MAX)) {
		SaveBlockFooter_CheckInfo(&normalInfo[SECTOR_ID_PRIMARY], saveData, (u32)primaryBuffer, SAVE_BLOCK_ID_NORMAL);
		SaveBlockFooter_CheckInfo(&boxInfo[SECTOR_ID_PRIMARY], saveData, (u32)primaryBuffer, SAVE_BLOCK_ID_BOXES);
	} else {
		SaveData_CheckInfoInit(&normalInfo[SECTOR_ID_PRIMARY]);
		SaveData_CheckInfoInit(&boxInfo[SECTOR_ID_PRIMARY]);
	}
	
	if (SaveData_CardLoad(BACKUP_SECTOR_START * SAVE_SECTOR_SIZE, backupBuffer, SAVE_SECTOR_SIZE * SAVE_PAGE_MAX)) {
		SaveBlockFooter_CheckInfo(&normalInfo[SECTOR_ID_BACKUP], saveData, (u32)backupBuffer, SAVE_BLOCK_ID_NORMAL);
		SaveBlockFooter_CheckInfo(&boxInfo[SECTOR_ID_BACKUP], saveData, (u32)backupBuffer, SAVE_BLOCK_ID_BOXES);
	} else {
		SaveData_CheckInfoInit(&normalInfo[SECTOR_ID_BACKUP]);
		SaveData_CheckInfoInit(&boxInfo[SECTOR_ID_BACKUP]);
	}
	
	Heap_Free(primaryBuffer);
	Heap_Free(backupBuffer);
	
	int currNormalSector, currBoxSector, staleNormalSector, staleBoxSector;
	int normalResult = SaveCheckInfo_CompareSectors(&normalInfo[SECTOR_ID_PRIMARY], &normalInfo[SECTOR_ID_BACKUP], &currNormalSector, &staleNormalSector);
	int boxResult = SaveCheckInfo_CompareSectors(&boxInfo[SECTOR_ID_PRIMARY], &boxInfo[SECTOR_ID_BACKUP], &currBoxSector, &staleBoxSector);
	
	if (normalResult == SECTOR_RESULT_INVALID && boxResult == SECTOR_RESULT_INVALID) {
		return LOAD_RESULT_EMPTY;
	}
	
	if (normalResult == SECTOR_RESULT_INVALID || boxResult == SECTOR_RESULT_INVALID) {
		return LOAD_RESULT_ERROR;
	}
	
	if (normalResult == SECTOR_RESULT_VALID && boxResult == SECTOR_RESULT_VALID) {
		if (normalInfo[currNormalSector].globalCounter == boxInfo[currBoxSector].globalCounter) {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, currBoxSector);
			return LOAD_RESULT_OK;
		} else {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, staleNormalSector, currBoxSector);
			return LOAD_RESULT_CORRUPT;
		}
	}
	
	if (normalResult == SECTOR_RESULT_PARTIAL_VALID && boxResult == SECTOR_RESULT_VALID) {
		if (normalInfo[currNormalSector].globalCounter == boxInfo[currBoxSector].globalCounter) {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, currBoxSector);
			return LOAD_RESULT_CORRUPT;
		} else if (normalInfo[currNormalSector].globalCounter == boxInfo[staleBoxSector].globalCounter) {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, staleBoxSector);
			return LOAD_RESULT_CORRUPT;
		}
		
		return LOAD_RESULT_ERROR;
	}
	
	if (normalResult == SECTOR_RESULT_VALID && boxResult == SECTOR_RESULT_PARTIAL_VALID) {
		if (normalInfo[currNormalSector].globalCounter == boxInfo[currBoxSector].globalCounter) {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, currBoxSector);
			return LOAD_RESULT_OK;
		} else {
			SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, staleNormalSector, currBoxSector);
			return LOAD_RESULT_CORRUPT;
		}
	}
	
	if (normalResult == SECTOR_RESULT_PARTIAL_VALID
		&& boxResult == SECTOR_RESULT_PARTIAL_VALID
		&& currNormalSector == currBoxSector) {
		GF_ASSERT(normalInfo[currNormalSector].globalCounter == boxInfo[currBoxSector].globalCounter);
		SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, currBoxSector);
		return LOAD_RESULT_OK;
	} else {
		GF_ASSERT(normalInfo[currNormalSector].globalCounter == boxInfo[currBoxSector].globalCounter);
		SaveData_SetBlockCheckInfo(saveData, normalInfo, boxInfo, currNormalSector, currBoxSector);
		return LOAD_RESULT_CORRUPT;
	}
}


static BOOL SaveBlock_Load(int sectorID, const SaveBlockInfo* blockInfo, u8* blockOffset) {
	u32 saveOffset = SaveData_SaveOffset(sectorID, blockInfo);
	blockOffset += blockInfo->offset;
	
	return SaveData_CardLoad(saveOffset, blockOffset, blockInfo->size);
}


static BOOL SaveDataState_Load(SaveData* saveData) {
	int i;
	
	for (i = 0; i < SAVE_BLOCK_ID_MAX; i++) {
		if (SaveBlock_Load(saveData->blockOffsets[i], &saveData->blockInfo[i], saveData->body.data) == FALSE) {
			return FALSE;
		}
		
		if (SaveBlockFooter_Validate(saveData, (u32)saveData->body.data, i) == FALSE) {
			return FALSE;
		}
	}
	
	return TRUE;
}


static s32 SaveDataState_InitBlock(SaveData* saveData, int blockID, u8 sectorID, BOOL* outSuccess) {
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	
	SaveBlockFooter_Set(saveData, (u32)saveData->body.data, blockID);
	
	u32 saveOffset = SaveData_SaveOffset(sectorID, blockInfo);
	u8* bodyOffset = saveData->body.data + blockInfo->offset;
	
	return SaveData_CardSave_Init(saveOffset, bodyOffset, blockInfo->size - sizeof(SaveBlockFooter), outSuccess);
}


static s32 SaveDataState_InitFooter(SaveData* saveData, int blockID, u8 sectorID) {
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	
	u32 saveOffset = SaveData_SaveOffset(sectorID, blockInfo) + blockInfo->size - sizeof(SaveBlockFooter);
	u8* bodyOffset = saveData->body.data + blockInfo->offset + blockInfo->size - sizeof(SaveBlockFooter);
	
	BOOL success;
	return SaveData_CardSave_Init(saveOffset, bodyOffset, sizeof(SaveBlockFooter), &success);
}


static s32 SaveDataState_InitFooter_Secondary(SaveData* saveData, int blockID, u8 sectorID) {
	const SaveBlockInfo* blockInfo = &saveData->blockInfo[blockID];
	
	u32 saveOffset = SaveData_SaveOffset(sectorID, blockInfo) + blockInfo->size - sizeof(SaveBlockFooter) + SECONDARY_FOOTER_SIZE;
	u8* bodyOffset = saveData->body.data + blockInfo->offset + blockInfo->size - sizeof(SaveBlockFooter) + SECONDARY_FOOTER_SIZE;
	
	BOOL success;
	return SaveData_CardSave_Init(saveOffset, bodyOffset, SECONDARY_FOOTER_SIZE, &success);
}


static void SaveDataState_Init(SaveData* saveData, SaveDataState* state, int blockID) {
	for (int i = 0; i < SAVE_BLOCK_ID_MAX; i++) {
		state->blockCounterBackup[i] = saveData->blockCounters[i];
		saveData->blockCounters[i]++;
	}
	
	state->mainSequence = 0;
	state->fullSaveMode = FALSE;
	
	if (blockID == SAVE_BLOCK_ID_MAX) {
		if (saveData->fullSaveRequired) {
			state->fullSaveMode = TRUE;
			state->globalCounterBackup = saveData->globalCounter;
			saveData->globalCounter++;
			
			state->startBlock = SAVE_BLOCK_ID_NORMAL;
			state->currentBlock = SAVE_BLOCK_ID_NORMAL;
			state->endBlock = SAVE_BLOCK_ID_MAX;
		} else {
			state->startBlock = SAVE_BLOCK_ID_NORMAL;
			state->currentBlock = SAVE_BLOCK_ID_NORMAL;
			state->endBlock = SAVE_BLOCK_ID_NORMAL + 1;
		}
	} else {
		state->startBlock = blockID;
		state->currentBlock = blockID;
		state->endBlock = blockID + 1;
	}
	
	SleepLock(SLEEP_TYPE_SAVE_DATA);
}


static SaveResult SaveDataState_Main(SaveData* saveData, SaveDataState* state) {
	BOOL saveResult;
	
	switch (state->mainSequence) {
		case 0:
			state->lockID = SaveDataState_InitBlock(saveData, state->currentBlock, !saveData->blockOffsets[state->currentBlock], &saveResult);
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			state->mainSequence++;
			// Fallthrough
		
		case 1:
			if (SaveData_CardSave_Main(state->lockID, &saveResult) == FALSE) {
				break;
			}
			
			// Be double sure
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			
			state->mainSequence++;
			// Fallthrough
		
		case 2:
			state->lockID = SaveDataState_InitFooter_Secondary(saveData, state->currentBlock, !saveData->blockOffsets[state->currentBlock]);
			state->mainSequence++;
			// Fallthrough
		
		case 3:
			if (SaveData_CardSave_Main(state->lockID, &saveResult) == FALSE) {
				break;
			}
			
			// Be double sure
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			
			state->mainSequence++;
			
			if (state->currentBlock + 1 == state->endBlock) {
				return SAVE_RESULT_PROCEED_FINAL;
			}
			// Fallthrough
		
		case 4:
			state->lockID = SaveDataState_InitFooter(saveData, state->currentBlock, !saveData->blockOffsets[state->currentBlock]);
			state->mainSequence++;
			// Fallthrough
		
		case 5:
			if (SaveData_CardSave_Main(state->lockID, &saveResult) == FALSE) {
				break;
			}
			
			// Be double sure
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			if (!saveResult) {
				return SAVE_RESULT_CORRUPT;
			}
			
			state->currentBlock++;
			
			if (state->currentBlock == state->endBlock) {
				return SAVE_RESULT_OK;
			}
			
			state->mainSequence = 0;
			break;
	}
	
	return SAVE_RESULT_PROCEED;
}


static void SaveDataState_End(SaveData* saveData, SaveDataState* state, int saveResult) {
	int i;
	if (saveResult == SAVE_RESULT_CORRUPT) {
		if (state->fullSaveMode) {
			saveData->globalCounter = state->globalCounterBackup;
		}
		
		for (i = 0; i < SAVE_BLOCK_ID_MAX; i++) {
			saveData->blockCounters[i] = state->blockCounterBackup[i];
		}
	} else {
		for (i = state->startBlock; i < state->endBlock; i++) {
			saveData->blockOffsets[i] = !saveData->blockOffsets[i];
		}
		
		saveData->dataExists = TRUE;
		saveData->isNewGameData = FALSE;
		saveData->fullSaveRequired = FALSE;
	}
	
	SleepUnlock(SLEEP_TYPE_SAVE_DATA);
}


int SaveTableEntry_GetBodySize(int saveTableID) {
	const SaveTableEntry* saveTable = gSaveTable;
	
	GF_ASSERT(saveTableID < gSaveTableSize);
	if (gIsDiamondPearl) {
		GF_ASSERT(saveTableID != gSaveTableSize - 3);
		GF_ASSERT(saveTableID != gSaveTableSize - 2);
	}
	
	int size = saveTable[saveTableID].sizeFunc();
	
	size += 4 - (size % 4);
	
	if (!gIsDiamondPearl) {
		size += 4;
	}
	
	return size;
}


static void SavePageInfo_Init(SavePageInfo* pageInfo) {
	const SaveTableEntry* saveTable = gSaveTable;
	int i, totalSize = 0;
	
	GF_ASSERT(gSaveTableSize == SAVE_TABLE_ENTRY_MAX);
	
	for (i = 0; i < gSaveTableSize; i++) {
		if (gIsDiamondPearl) {
			if (i == gSaveTableSize - 3) {
				continue;
			}
			if (i == gSaveTableSize - 2) {
				continue;
			}
		}
		
		GF_ASSERT(saveTable[i].dataID == i);
		
		pageInfo[i].pageID   = saveTable[i].dataID;
		pageInfo[i].size     = SaveTableEntry_GetBodySize(i);
		pageInfo[i].location = totalSize;
		pageInfo[i].checksum = 0;
		pageInfo[i].blockID  = saveTable[i].blockID;
		
		totalSize += pageInfo[i].size;
		
		if ((i == gSaveTableSize - 1) || (saveTable[i].blockID != saveTable[i + 1].blockID) || (gIsDiamondPearl && (i == gSaveTableSize - 4))) {
			totalSize += sizeof(SaveBlockFooter);
		}
	}
	
	GF_ASSERT(totalSize <= SAVE_SECTOR_SIZE * SAVE_PAGE_MAX);
}


static void SaveBlockInfo_Init(SaveBlockInfo* blockInfo, const SavePageInfo* pageInfo) {
	int totalSectors = 0, blockOffset = 0, i, page = 0;
	
	for (i = 0; i < SAVE_BLOCK_ID_MAX; i++) {
		blockInfo[i].saveBlockID = i;
		blockInfo[i].size = 0;
		
		while (pageInfo[page].blockID == i && page < gSaveTableSize) {
			blockInfo[i].size += pageInfo[page].size;
			page++;
		}
		
		blockInfo[i].size += sizeof(SaveBlockFooter);
		blockInfo[i].sectorStartPos = totalSectors;
		blockInfo[i].offset = blockOffset;
		blockInfo[i].sectorsInUse = (blockInfo[i].size + SAVE_SECTOR_SIZE - 1) / SAVE_SECTOR_SIZE;
		
		totalSectors += blockInfo[i].sectorsInUse;
		blockOffset += blockInfo[i].size;
	}
	
	GF_ASSERT(totalSectors == blockInfo[SAVE_BLOCK_ID_MAX - 1].sectorStartPos + blockInfo[SAVE_BLOCK_ID_MAX - 1].sectorsInUse);
	GF_ASSERT(totalSectors <= SAVE_PAGE_MAX);
}


static void SaveTable_Clear(SaveDataBody* body, const SavePageInfo* pageInfo) {
	const SaveTableEntry* saveTable = gSaveTable;
	MI_CpuClearFast(body->data, sizeof(body->data));
	
	int i, size;
	void* page;
	u32 location;
	for (i = 0; i < gSaveTableSize; i++) {
		location = pageInfo[i].location;
		page = &body->data[location];
		size = pageInfo[i].size;
		MI_CpuClearFast(page, size);
		saveTable[i].initFunc(page);
	}
}


BOOL SaveData_GetCardBackupType(void) {
	s32 lockID = OS_GetLockID();
	GF_ASSERT(lockID != OS_LOCK_ID_ERROR);
	
	CARD_LockBackup(lockID);
	
	BOOL result;
	if (CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_4MBITS)) {
		result = CARD_BACKUP_TYPE_FLASH_4MBITS;
	} else if (CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_2MBITS)) {
		result = CARD_BACKUP_TYPE_FLASH_2MBITS;
	} else {
		result = CARD_BACKUP_TYPE_NOT_USE;
	}
	
	CARD_UnlockBackup(lockID);
	OS_ReleaseLockID(lockID);
	
	return result != CARD_BACKUP_TYPE_NOT_USE;
}


BOOL SaveData_CardLoad(u32 address, void* data, u32 size) {
	s32 lockID = OS_GetLockID();
	GF_ASSERT(lockID != OS_LOCK_ID_ERROR);
	
	CARD_LockBackup(lockID);
	CARD_ReadFlashAsync(address, data, size, NULL, NULL);
	
	BOOL result = CARD_WaitBackupAsync();
	
	CARD_UnlockBackup(lockID);
	OS_ReleaseLockID(lockID);
	
	if (!result) {
		Heap_Free(sSaveDataPtr);
		SaveReadError_ShowWindow(HEAP_ID_SAVE);
	}
	
	return result;
}


static void CB_SaveComplete(void* unused) {
	sSaveComplete = TRUE;
}


static s32 SaveData_CardSave_Init(u32 address, void* data, u32 size, BOOL* outSuccess) {
	s32 lockID = OS_GetLockID();
	GF_ASSERT(lockID != OS_LOCK_ID_ERROR);
	
	CARD_LockBackup(lockID);
	
	u32 buffer;
	*outSuccess = CARD_ReadFlash(0, &buffer, sizeof(buffer));
	
	sSaveComplete = FALSE;
	if (*outSuccess) {
		CARD_WriteAndVerifyFlashAsync(address, data, size, CB_SaveComplete, NULL);
	} else {
		CARD_UnlockBackup(lockID);
	}
	
	return lockID;
}


static BOOL SaveData_CardSave_Main(s32 lockID, BOOL* result) {
	if (sSaveComplete == TRUE) {
		CARD_UnlockBackup(lockID);
		OS_ReleaseLockID(lockID);
		
		switch (CARD_GetResultCode()) {
			case CARD_RESULT_SUCCESS:
				*result = TRUE;
				break;
			
			default:
				GF_ASSERT(FALSE);
				// Fallthrough
			
			case CARD_RESULT_TIMEOUT:
				*result = FALSE;
				break;
			
			case CARD_RESULT_NO_RESPONSE:
				*result = FALSE;
				break;
		}
		
		return TRUE;
	}
	
	return FALSE;
}
