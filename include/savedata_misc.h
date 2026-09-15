#ifndef MPR_SAVEDATA_MISC_H
#define MPR_SAVEDATA_MISC_H

#include <nitro/types.h>

#include "constants/save_table.h"
#include "constants/string.h"
#include "berry_patches.h"
#include "easy_chat_sentence.h"
#include "persisted_map_features.h"
#include "savedata.h"
#include "string_gf.h"

typedef struct {
	u32  keys[EXTRA_SAVE_TABLE_ENTRY_MAX - 1];
	u32  oldKeys[EXTRA_SAVE_TABLE_ENTRY_MAX - 1];
	u8   keyFlags[EXTRA_SAVE_TABLE_ENTRY_MAX - 1];
	u8   padding[3];
} ExtraSaveKey;

typedef struct {
	BerryPatch            berryPatches[MAX_BERRY_PATCHES];
	PersistedMapFeatures  persistedMapFeatures;
	u16                   rivalName[TRAINER_NAME_LEN + 1];
	u16                   tabletName[TABLET_NAME_LEN + 1];
} MiscSaveBlockDP;

typedef struct {
	BerryPatch            berryPatches[MAX_BERRY_PATCHES];
	PersistedMapFeatures  persistedMapFeatures;
	u16                   rivalName[TRAINER_NAME_LEN + 1];
	u16                   tabletName[TABLET_NAME_LEN + 1];
	u16                   favoriteMon;
	u8                    favoriteMonForm : 7;
	u8                    favoriteMonIsEgg : 1;
	u8                    extraSaveInitalized : 1;
	u8                    vsRecorderColor : 4;
	u8                    padding : 3;
	u32                   unused;
	EasyChatSentence      introMsg;
	ExtraSaveKey          extraKey;
} MiscSaveBlockPt;

// Strict superset
typedef MiscSaveBlockDP MiscSaveBlockCommon;

int MiscSaveBlock_GetSaveSize(void);
void MiscSaveBlock_Init(MiscSaveBlockCommon* miscSave);

#endif /* MPR_SAVEDATA_MISC_H */
