#ifndef MPR_WIFI_HISTORY_H
#define MPR_WIFI_HISTORY_H

typedef struct {
	u32  unused_00;
	u8   hasInteractedOutsideOfJapan;
	u8   country;
	u8   region;
	u8   geonetCommunicationMap[4080];
} WiFiHistory;

int WiFiHistory_GetSaveSize(void);
void WiFiHistory_Init(WiFiHistory* wiFiHistory);

#endif /* MPR_WIFI_HISTORY_H */
