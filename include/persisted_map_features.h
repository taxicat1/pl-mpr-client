#ifndef MPR_PERSISTED_MAP_FEATURES_H
#define MPR_PERSISTED_MAP_FEATURES_H

#include <nitro/types.h>

#define PERSISTED_MAP_FEATURES_BUFFER_SIZE  32

typedef struct {
	int  id;
	u8   buffer[PERSISTED_MAP_FEATURES_BUFFER_SIZE];
} PersistedMapFeatures;

void PersistedMapFeatures_Init(PersistedMapFeatures* persistedMapFeatures);

#endif /* MPR_PERSISTED_MAP_FEATURES_H */
