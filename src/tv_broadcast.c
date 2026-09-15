#include <nitro.h>

#include "tv_broadcast.h"

#include "game_version.h"


int TVBroadcast_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(TVBroadcastDP);
	} else {
		return sizeof(TVBroadcastPt);
	}
}


void TVBroadcast_Init(TVBroadcastCommon* tvBroadcast) {
	MI_CpuClearFast(tvBroadcast, TVBroadcast_GetSaveSize());
}
