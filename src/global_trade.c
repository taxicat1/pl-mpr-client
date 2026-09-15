#include <nitro.h>

#include "global_trade.h"

#include "game_version.h"


int GlobalTrade_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(GlobalTradeDP);
	} else {
		return sizeof(GlobalTradePt);
	}
}


void GlobalTrade_Init(GlobalTradeCommon* globalTrade) {
	MI_CpuClearFast(globalTrade, GlobalTrade_GetSaveSize());
}
