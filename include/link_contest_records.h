#ifndef MPR_LINK_CONTEST_RECORDS_H
#define MPR_LINK_CONTEST_RECORDS_H

#include "savedata.h"

#define NUM_CONTEST_TYPES         5
#define NUM_CONTEST_PARTICIPANTS  4

typedef struct {
	u16 contestPlacement[NUM_CONTEST_TYPES][NUM_CONTEST_PARTICIPANTS];
} LinkContestRecords;

int LinkContestRecords_GetSaveSize(void);
void LinkContestRecords_Init(LinkContestRecords* linkContestRecords);

#endif /* MPR_LINK_CONTEST_RECORDS_H */
