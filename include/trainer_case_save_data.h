#ifndef MPR_TRAINER_CASE_SAVE_DATA_H
#define MPR_TRAINER_CASE_SAVE_DATA_H

#include "savedata.h"

#include "constants/graphics.h"
#include "constants/badges.h"

#define SIGNATURE_WIDTH   24
#define SIGNATURE_HEIGHT  8

typedef struct {
	int polish;
} TrainerCaseSaveDataBadge;

typedef struct {
	TrainerCaseSaveDataBadge  badges[MAX_BADGES];
	u8                        signature[SIGNATURE_WIDTH * SIGNATURE_HEIGHT * TILE_SIZE_1BPP];
} TrainerCaseSaveData;

int TrainerCaseSaveData_GetSaveSize(void);
void TrainerCaseSaveData_Init(TrainerCaseSaveData* tcSaveData);
TrainerCaseSaveDataBadge* TrainerCaseSaveData_GetTrainerCaseSaveDataBadges(TrainerCaseSaveData* tcSaveData);
void TrainerCaseSaveDataBadge_SetPolish(u8 badgeID, int polish, TrainerCaseSaveDataBadge badges[]);

#endif /* MPR_TRAINER_CASE_SAVE_DATA_H */
