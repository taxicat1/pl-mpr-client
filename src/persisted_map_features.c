#include <nitro.h>

#include "persisted_map_features.h"


void PersistedMapFeatures_Init(PersistedMapFeatures* persistedMapFeatures) {
	MI_CpuClear8(persistedMapFeatures, sizeof(PersistedMapFeatures));
}
