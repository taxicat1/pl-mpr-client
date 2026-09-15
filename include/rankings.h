#ifndef MPR_RANKINGS_H
#define MPR_RANKINGS_H

#include "constants/heap.h"
#include "constants/string.h"
#include "savedata.h"
#include "string_gf.h"

#define MAX_RANKINGS_ENTRIES         6
#define RECORDS_WITH_RANKINGS_COUNT  13

typedef struct {
    u32  seed;
    u32  recordValue;
    u16  playerName[TRAINER_NAME_LEN + 1];
} RankingEntry;

typedef struct {
    RankingEntry entries[MAX_RANKINGS_ENTRIES];
} RankingList;

typedef struct {
    RankingList lists[RECORDS_WITH_RANKINGS_COUNT * 2];
} Rankings;

int Rankings_GetSaveSize(void);
void Rankings_Init(Rankings* rankings);

#endif /* MPR_RANKINGS_H */
