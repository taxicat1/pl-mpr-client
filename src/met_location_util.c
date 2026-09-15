#include <nitro.h>
#include <string.h>

#include "met_location_util.h"

#include "common.h"
#include "assert.h"

static const u16 MetlocMagics[] = {
	0, 2000, 3000
};


MetLocationType MetLoc_GetBankType(u32 metLoc) {
	int i;
	for (i = 0; i < MET_LOCATION_TYPE_MAX - 1; i++) {
		if (metLoc < MetlocMagics[i + 1]) {
			return (MetLocationType)i;
		}
	}
	
	return (MetLocationType)i;
}


int MetLoc_GetBankEntry(u32 metLoc) {
	int bank = MetLoc_GetBankType(metLoc);
	return metLoc - MetlocMagics[bank];
}


u32 MetLoc_GetByTypeAndEntry(MetLocationType type, u32 entry) {
	GF_ASSERT(type < MET_LOCATION_TYPE_MAX);
	return MetlocMagics[type] + entry;
}
