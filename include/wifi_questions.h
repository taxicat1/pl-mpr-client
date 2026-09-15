#ifndef MPR_WIFI_QUESTIONS_H
#define MPR_WIFI_QUESTIONS_H

#include <nitro/types.h>

typedef struct {
	u32  unk_00;
	u32  unk_04;
} WiFiQuestions;

int WiFiQuestion_GetSaveSize(void);
void WiFiQuestion_Init(WiFiQuestions* wiFiQuestions);

#endif /* MPR_WIFI_QUESTIONS_H */