#include <nitro.h>

#include "system_data.h"

#include "rtc.h"


int SystemData_GetSaveSize(void) {
	return sizeof(SystemData);
}


void SystemData_Init(SystemData* systemData) {
	MI_CpuClearFast(systemData, sizeof(SystemData));
	GameTime_Clear(&systemData->gameTime);
}


void GameTime_Clear(GameTime* gameTime) {
	gameTime->canary = TRUE;
	GetCurrentDateTime(&gameTime->date, &gameTime->time);
	gameTime->day = RTC_ConvertDateToDay(&gameTime->date);
	gameTime->startTimestamp = RTC_ConvertDateTimeToSecond(&gameTime->date, &gameTime->time);
	gameTime->firstCompletionTimestamp = 0;
	gameTime->penaltyInMinutes = 0;
}
