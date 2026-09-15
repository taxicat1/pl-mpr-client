#include <nitro.h>
#include <string.h>

#include "rtc.h"

#include "assert.h"

typedef struct {
	BOOL       valid;
	BOOL       readInProgress;
	int        framesSinceRead;
	RTCResult  status;
	RTCDate    date;
	RTCTime    time;
	RTCDate    tempDate;
	RTCTime    tempTime;
} RTCState;

static RTCState sRTCState;

static void GetTimeCallback(RTCResult result, void *data);
static void StartRTCRead(RTCState *param0);


void InitRTC(void) {
	RTC_Init();
	memset(&sRTCState, 0, sizeof(sRTCState));
	sRTCState.valid = 0;
	sRTCState.readInProgress = 0;
	sRTCState.framesSinceRead = 0;
	StartRTCRead(&sRTCState);
}


void UpdateRTC(void) {
	if (sRTCState.readInProgress) {
		return;
	}
	
	sRTCState.framesSinceRead++;
	if (sRTCState.framesSinceRead > 10) {
		sRTCState.framesSinceRead = 0;
		StartRTCRead(&sRTCState);
	}
}


static void GetTimeCallback(RTCResult result, void* data) {
	RTCState* state = data;
	
	state->status = result;
	
	GF_ASSERT(result == RTC_RESULT_SUCCESS);
	
	state->valid = 1;
	state->date = state->tempDate;
	state->time = state->tempTime;
	state->readInProgress = 0;
}


static void StartRTCRead(RTCState* rtc) {
	rtc->readInProgress = 1;
	rtc->status = RTC_GetDateTimeAsync(&rtc->tempDate, &rtc->tempTime, GetTimeCallback, rtc);
	GF_ASSERT(rtc->status == RTC_RESULT_SUCCESS);
}


void GetCurrentDateTime(RTCDate* date, RTCTime* time) {
	GF_ASSERT(sRTCState.valid == 1);
	
	*date = sRTCState.date;
	*time = sRTCState.time;
}
