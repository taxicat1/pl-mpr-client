#include <nitro.h>

#include "savedata_misc.h"

#include "constants/charcode.h"
#include "game_version.h"
#include "berry_patches.h"
#include "persisted_map_features.h"


int MiscSaveBlock_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(MiscSaveBlockDP);
	} else {
		return sizeof(MiscSaveBlockPt);
	}
}


void MiscSaveBlock_Init(MiscSaveBlockCommon* miscSave) {
	MI_CpuClearFast(miscSave, MiscSaveBlock_GetSaveSize());
	
	BerryPatches_Clear(miscSave->berryPatches);
	PersistedMapFeatures_Init(&miscSave->persistedMapFeatures);
	
	MI_CpuFill16(miscSave->rivalName, CHAR_EOS, TRAINER_NAME_LEN + 1);
	MI_CpuFill16(miscSave->tabletName, CHAR_EOS, TABLET_NAME_LEN + 1);
}
