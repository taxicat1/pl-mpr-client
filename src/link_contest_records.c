#include <nitro.h>

#include "link_contest_records.h"


int LinkContestRecords_GetSaveSize(void) {
	return sizeof(LinkContestRecords);
}


void LinkContestRecords_Init(LinkContestRecords* linkContestRecords) {
	int placement, type;
	for (type = 0; type < NUM_CONTEST_TYPES; type++) {
		for (placement = 0; placement < NUM_CONTEST_PARTICIPANTS; placement++) {
			linkContestRecords->contestPlacement[type][placement] = 0;
		}
	}
}
