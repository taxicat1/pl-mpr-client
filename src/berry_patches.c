#include <nitro.h>

#include "berry_patches.h"


void BerryPatches_Clear(BerryPatch*patches) {
	MI_CpuClear8(patches, sizeof(BerryPatch) * MAX_BERRY_PATCHES);
	for (int i = 0; i < MAX_BERRY_PATCHES; i++) {
		patches[i].growthStage = 0;
		patches[i].berryID = 0;
	}
}
