#ifndef MPR_SYSTEM_DATA_H
#define MPR_SYSTEM_DATA_H

#include <nitro/rtc.h>

#include "savedata.h"

typedef struct {
    BOOL     canary;
    RTCDate  date;
    RTCTime  time;
    u32      day;
    s64      startTimestamp;
    s64      firstCompletionTimestamp;
    u32      penaltyInMinutes;
} GameTime;

typedef struct {
    s64       rtcOffset;
    u8        macAddress[6];
    u8        ownerBirthMonth;
    u8        ownerBirthDayOfMonth;
    GameTime  gameTime;
    u8        isMysteryGiftUnlocked;
    u8        padding_49[3];
    s32       dwcProfileId;
    u32       unused_50[3];
} SystemData;

int SystemData_GetSaveSize(void);
void SystemData_Init(SystemData* systemData);
void GameTime_Clear(GameTime* gameTime);

#endif /* MPR_SYSTEM_DATA_H */