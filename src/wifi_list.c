#include <nitro.h>

#include "wifi_list.h"

#include "game_version.h"
#include "constants/charcode.h"


int WiFiList_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(WiFiListDP);
	} else {
		return sizeof(WiFiListPt);
	}
}


void WiFiList_Init(void* wiFiList) {
	MI_CpuClearFast(wiFiList, WiFiList_GetSaveSize());
	
	// BUG: DP assumed for clearing fields. Substruct unk_1C0 is a different size in Pt
	WiFiListDP* wiFiListDP = (WiFiListDP*)wiFiList;
	for (int i = 0; i < MAX_FRIENDS; i++) {
		wiFiListDP->unk_1C0[i].unk_10[0] = CHAR_EOS;
		wiFiListDP->unk_1C0[i].unk_00[0] = CHAR_EOS;
		wiFiListDP->unk_1C0[i].unk_2E = 2;
	}
}
