#include <nitro.h>

#include "rankings.h"

#include "charcode_util.h"

static void RankingEntry_Clear(RankingEntry* entry);


static void RankingEntry_Clear(RankingEntry* entry) {
	entry->seed = 0;
	entry->recordValue = 0;
	CharCode_FillWithEOS(entry->playerName, TRAINER_NAME_LEN + 1);
}


int Rankings_GetSaveSize(void) {
	return sizeof(Rankings);
}


void Rankings_Init(Rankings* rankings) {
	MI_CpuClear8(rankings, sizeof(Rankings));
	
	for (int listID = 0; listID < RECORDS_WITH_RANKINGS_COUNT * 2; listID++) {
		for (int entryID = 0; entryID < MAX_RANKINGS_ENTRIES; entryID++) {
			RankingEntry_Clear(&(rankings->lists[listID].entries[entryID]));
		}
	}
}
