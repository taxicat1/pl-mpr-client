#include <nitro.h>
#include <string.h>

#include "trainer_info.h"

#include "game_version.h"
#include "constants/charcode.h"
#include "charcode_util.h"
#include "heap.h"
#include "string_gf.h"


int TrainerInfo_GetSize(void) {
	return sizeof(TrainerInfo);
}


void TrainerInfo_Init(TrainerInfo* info) {
	memset(info, 0, sizeof(TrainerInfo));
	info->language = GAME_LANGUAGE;
}


const charcode_t* TrainerInfo_GetName(const TrainerInfo* info) {
	return info->name;
}


u32 TrainerInfo_GetID(const TrainerInfo* info) {
	return info->id;
}


u32 TrainerInfo_GetGender(const TrainerInfo* info) {
	return info->gender;
}


void TrainerInfo_SetGameCode(TrainerInfo* info, u8 gameCode) {
	info->gameCode = gameCode;
}
