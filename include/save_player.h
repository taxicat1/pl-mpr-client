#ifndef MPR_SAVE_PLAYER_H
#define MPR_SAVE_PLAYER_H

#include "game_options.h"
#include "play_time.h"
#include "savedata.h"
#include "trainer_info.h"

typedef struct {
    Options      options;
    u8           padding_02[2];
    TrainerInfo  info;
    u16          coins;
    PlayTime     playTime;
    u8           padding_2A[2];
} PlayerSave;

int Player_GetSaveSize(void);
void Player_Init(PlayerSave* player);
TrainerInfo* SaveData_GetTrainerInfo(SaveData* saveData);
Options* SaveData_GetOptions(SaveData* saveData);

#endif /* MPR_SAVE_PLAYER_H */
