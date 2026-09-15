#ifndef MPR_TRAINER_INFO_H
#define MPR_TRAINER_INFO_H

#include "constants/heap.h"
#include "constants/string.h"

#include "string_gf.h"

typedef struct {
	charcode_t  name[TRAINER_NAME_LEN + 1];
	u32         id;
	u32         money;
	u8          gender;
	u8          language;
	u8          badgeMask;
	u8          appearance;
	u8          gameCode;
	u8          isMainStoryCleared : 1;
	u8          hasNationalDex     : 1;
	u8                             : 6;
} TrainerInfo;

int TrainerInfo_GetSize(void);
void TrainerInfo_Init(TrainerInfo* info);
const charcode_t* TrainerInfo_GetName(const TrainerInfo* info);
u32 TrainerInfo_GetID(const TrainerInfo* info);
u32 TrainerInfo_GetGender(const TrainerInfo* info);
void TrainerInfo_SetGameCode(TrainerInfo* info, u8 gameCode);

#endif /* MPR_TRAINER_INFO_H */
