#include <nitro.h>

#include "wifi_questions.h"


int WiFiQuestion_GetSaveSize(void) {
	return sizeof(WiFiQuestions);
}


void WiFiQuestion_Init(WiFiQuestions* wiFiQuestions) {
	MI_CpuClear32(wiFiQuestions, WiFiQuestion_GetSaveSize());
	wiFiQuestions->unk_00 = 0xFFFFFFFF;
}
