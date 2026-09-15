#include <nitro.h>

#include "game_records.h"

#include "game_version.h"


int GameRecords_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(GameRecordsDP);
	} else {
		return sizeof(GameRecordsPt);
	}
}


void GameRecords_Init(GameRecordsCommon* records) {
	MI_CpuClear32(records, GameRecords_GetSaveSize());
}
