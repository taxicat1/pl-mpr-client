#ifndef MPR_PAL_PARK_TRANSFERS_H
#define MPR_PAL_PARK_TRANSFERS_H

#include "pokemon.h"

#define CATCHING_SHOW_MONS        6
#define MAX_GBA_TRANSFER_HISTORY  20

typedef struct {
	Pokemon  mons[CATCHING_SHOW_MONS];
	u32      gbaTrainerIds[MAX_GBA_TRANSFER_HISTORY];
	s64      timeStamps[MAX_GBA_TRANSFER_HISTORY];
	s64      rtcOffset;
	u8       macAddress[6];
} MigratedPokemon;

int MigratedPokemon_GetSaveSize(void);
void MigratedPokemon_Init(MigratedPokemon* transferData);

#endif /* MPR_PAL_PARK_TRANSFERS_H */
