#ifndef MPR_SAVEDATA_H
#define MPR_SAVEDATA_H

#include <nitro/math.h>

#include "constants/heap.h"
#include "constants/save_table.h"
#include "constants/savedata.h"

typedef struct {
	u32  saveCounter;
	u32  blockCounter;
	u32  size;
	u32  signature;
	u8   saveBlockID;
	u16  checksum;
} SaveBlockFooter;

typedef struct {
	u8   saveBlockID;
	u8   sectorStartPos;
	u8   sectorsInUse;
	u32  offset;
	u32  size;
} SaveBlockInfo;

typedef struct {
	int  pageID;
	u32  size;
	u32  location;
	u16  checksum;
	u16  blockID;
} SavePageInfo;

typedef struct {
	u8 data[SAVE_SECTOR_SIZE * SAVE_PAGE_MAX];
} SaveDataBody;

typedef struct {
	BOOL  fullSaveMode;
	int   startBlock;
	int   currentBlock;
	int   endBlock;
	s32   lockID;
	int   mainSequence;
	u32   globalCounterBackup;
	u32   blockCounterBackup[SAVE_BLOCK_ID_MAX];
} SaveDataState;

typedef struct {
	BOOL  backupExists;
	BOOL  dataExists;
	BOOL  isNewGameData;
	BOOL  fullSaveRequired;
	u32   loadCheckStatus;
	
	MATHCRC16Table crcTable;
	
	SaveDataBody body;
	
	u32  globalCounter;
	u32  blockCounters[SAVE_BLOCK_ID_MAX];
	u8   blockOffsets[SAVE_BLOCK_ID_MAX];
	
	SavePageInfo   pageInfo[SAVE_TABLE_ENTRY_MAX];
	SaveBlockInfo  blockInfo[SAVE_BLOCK_ID_MAX];
	SaveDataState  state;
	
	int  sectorSwitch;
	u32  sectorCounter;
} SaveData;

typedef struct {
	BOOL  valid;
	u32   globalCounter;
	u32   blockCounter;
} SaveCheckInfo;

typedef struct {
	u32  signature;
	u32  saveCounter;
	u32  size;
	u16  id;
	u16  checksum;
} SaveCheckFooter;

SaveData* SaveData_Init(void);
void* SaveData_GetSaveTable(SaveData* saveData, int saveTableID);
void SaveData_Clear(SaveData* saveData);
BOOL SaveData_BackupExists(const SaveData* saveData);
u32 SaveData_GetLoadCheckStatus(const SaveData* saveData);
void SaveData_SetFullSaveRequired(void);
void SaveData_SaveStateInit(SaveData* saveData, int blockID);
SaveResult SaveData_SaveStateMain(SaveData* saveData);
int SaveTableEntry_GetBodySize(int saveTableID);
BOOL SaveData_GetCardBackupType(void);
BOOL SaveData_CardLoad(u32 address, void* data, u32 size);

#endif /* MPR_SAVEDATA_H */
