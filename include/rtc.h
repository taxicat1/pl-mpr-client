#ifndef MPR_RTC_H
#define MPR_RTC_H

#include <nitro/rtc.h>

void InitRTC(void);
void UpdateRTC(void);
void GetCurrentDateTime(RTCDate* date, RTCTime* time);

#endif /* MPR_RTC_H */
