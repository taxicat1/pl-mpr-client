#include <nitro.h>

#include "wifi_history_save_data.h"


int WiFiHistory_GetSaveSize(void) {
	return sizeof(WiFiHistory);
}


void WiFiHistory_Init(WiFiHistory* wiFiHistory) {
	MI_CpuClear32(wiFiHistory, sizeof(WiFiHistory));
}
