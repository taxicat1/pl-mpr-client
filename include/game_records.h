#ifndef MPR_GAME_RECORDS_H
#define MPR_GAME_RECORDS_H

#include <nitro/types.h>

#define NUM_U32_RECORDS_PT  71
#define NUM_U16_RECORDS_PT  77

#define NUM_U32_RECORDS_DP  44
#define NUM_U16_RECORDS_DP  77

typedef struct {
	u32  recordsU32[NUM_U32_RECORDS_DP];
	u16  recordsU16[NUM_U16_RECORDS_DP];
} GameRecordsDP;

typedef struct {
	u16  byteSum;
	u16  modifier;
} EncodingSeed;

typedef struct {
	u32           recordsU32[NUM_U32_RECORDS_PT];
	u16           recordsU16[NUM_U16_RECORDS_PT];
	u16           padding;
	EncodingSeed  seed;
} GameRecordsPt;

// No useful fields in common
typedef void GameRecordsCommon;

int GameRecords_GetSaveSize(void);
void GameRecords_Init(GameRecordsCommon* records);

#endif /* MPR_GAME_RECORDS_H */
