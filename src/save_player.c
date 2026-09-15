#include <nitro.h>

#include "save_player.h"

#include "coins.h"
#include "game_options.h"
#include "play_time.h"
#include "savedata.h"
#include "trainer_info.h"

int Player_GetSaveSize(void) {
	return sizeof(PlayerSave);
}


void Player_Init(PlayerSave* player) {
	MI_CpuClearFast(player, sizeof(PlayerSave));
	
	Options_Init(&player->options);
	TrainerInfo_Init(&player->info);
	Coins_Init(&player->coins);
	PlayTime_Init(&player->playTime);
}


TrainerInfo* SaveData_GetTrainerInfo(SaveData* saveData) {
	PlayerSave* state = SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_PLAYER);
	return &state->info;
}


Options* SaveData_GetOptions(SaveData* saveData) {
	PlayerSave* state = SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_PLAYER);
	return &state->options;
}
