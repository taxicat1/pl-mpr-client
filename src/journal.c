#include <nitro.h>
#include <string.h>

#include "journal.h"


int Journal_GetSaveSize(void) {
	return sizeof(JournalEntry) * MAX_JOURNAL_ENTRIES;
}


void Journal_Init(JournalEntry* journalEntry) {
	memset(journalEntry, 0, sizeof(JournalEntry) * MAX_JOURNAL_ENTRIES);
}
