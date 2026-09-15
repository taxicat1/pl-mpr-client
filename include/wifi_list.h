#ifndef MPR_WIFI_LIST_H
#define MPR_WIFI_LIST_H

#include <nitro/types.h>

#include "constants/string.h"

/* Unimplemented: NitroDWC structures */
typedef struct {
	u8 dat[0x40];
} DWCUserData;

typedef struct {
	u8 dat[0xC];
} DWCFriendData;

#define MAX_FRIENDS  32

typedef struct {
	u16  unk_00[TRAINER_NAME_LEN + 1];
	u16  unk_10[TRAINER_NAME_LEN + 1];
	u32  unk_20;
	u16  unk_24;
	u16  unk_26;
	u16  unk_28;
	u16  year;
	u8   month;
	u8   day;
	u8   unk_2E;
	u8   unk_2F;
} UnkWiFiListSubStructDP;

typedef struct {
	u16  unk_00[TRAINER_NAME_LEN + 1];
	u16  unk_10[TRAINER_NAME_LEN + 1];
	u32  unk_20;
	u16  unk_24;
	u16  unk_26;
	u16  unk_28;
	u16  year;
	u8   month;
	u8   day;
	u8   unk_2E;
	u8   unk_2F;
	u16  unk_30;
	u16  unk_32;
	u16  unk_34;
	u16  unk_36;
} UnkWiFiListSubStructPt;

typedef struct {
	DWCUserData             userData;
	DWCFriendData           friendData[MAX_FRIENDS];
	UnkWiFiListSubStructDP  unk_1C0[MAX_FRIENDS];
} WiFiListDP;

typedef struct {
	DWCUserData             userData;
	DWCFriendData           friendData[MAX_FRIENDS];
	UnkWiFiListSubStructPt  unk_1C0[MAX_FRIENDS];
} WiFiListPt;

int WiFiList_GetSaveSize(void);
void WiFiList_Init(void* wiFiList);

#endif /* MPR_WIFI_LIST_H */
