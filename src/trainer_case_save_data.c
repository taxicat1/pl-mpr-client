#include <nitro.h>
#include <string.h>

#include "trainer_case_save_data.h"

#define BADGE_POLISH_THRESHOLD_NORMAL  140


int TrainerCaseSaveData_GetSaveSize(void) {
	return sizeof(TrainerCaseSaveData);
}


void TrainerCaseSaveData_Init(TrainerCaseSaveData* tcSaveData) {
	memset(tcSaveData, 0, sizeof(TrainerCaseSaveData));
	
	int badgeID;
	TrainerCaseSaveDataBadge* badges = TrainerCaseSaveData_GetTrainerCaseSaveDataBadges(tcSaveData);
	
	for (badgeID = 0; badgeID < MAX_BADGES; badgeID++) {
		TrainerCaseSaveDataBadge_SetPolish(badgeID, BADGE_POLISH_THRESHOLD_NORMAL, badges);
	}
}


TrainerCaseSaveDataBadge *TrainerCaseSaveData_GetTrainerCaseSaveDataBadges(TrainerCaseSaveData* tcSaveData) {
	return tcSaveData->badges;
}


void TrainerCaseSaveDataBadge_SetPolish(u8 badgeID, int polish, TrainerCaseSaveDataBadge badges[]) {
	badges[badgeID].polish = polish;
}
