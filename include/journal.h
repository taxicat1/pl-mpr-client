#ifndef MPR_JOURNAL_H
#define MPR_JOURNAL_H

#include <nitro/types.h>

#define MAX_JOURNAL_ENTRIES          10
#define MAX_JOURNAL_LOCATION_EVENTS  4
#define MAX_JOURNAL_ONLINE_EVENTS    2

typedef struct {
	u32  year  : 7;
	u32  month : 4;
	u32  week  : 3;
	u32  day   : 5;
	u32  mapID : 13;
} JournalEntryTitle;

typedef struct {
	u8   battleResult;
	u8   stringVariant : 2;
	u8   timeOfDay     : 4;
	u8   gender        : 2;
	u16  species;
} JournalEntryMon;

typedef struct {
	u16  standard  : 1;
	u16  trainerID : 15;
	u16  mapID;
} JournalEntryTrainer;

typedef struct {
	JournalEntryTitle    title;
	u32                  locationEvents[MAX_JOURNAL_LOCATION_EVENTS];
	JournalEntryMon      mon;
	JournalEntryTrainer  trainer;
	u8                   onlineEvents[MAX_JOURNAL_ONLINE_EVENTS][42];
} JournalEntry;

int Journal_GetSaveSize(void);
void Journal_Init(JournalEntry* journalEntry);

#endif /* MPR_JOURNAL_H */
