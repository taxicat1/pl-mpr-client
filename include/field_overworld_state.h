#ifndef MPR_FIELD_OVERWORLD_STATE_H
#define MPR_FIELD_OVERWORLD_STATE_H

#include "struct_defs/player_data.h"
#include "struct_defs/location.h"
#include "struct_defs/map_object.h"
#include "struct_defs/overworld_map_history.h"
#include "savedata.h"

typedef struct {
	Location             player;
	Location             entrance;
	Location             previous;
	Location             special;
	Location             exit;
	u16                  bgm;
	u16                  weather;
	u16                  blackOutWarpId;
	u8                   cameraType;
	OverworldMapHistory  mapHistory;
	PlayerData           playerData;
	u16                  poisonSteps;
	u16                  safariSteps;
	u16                  safariBalls;
	u8                   padding_9A[6];
} FieldOverworldState;

typedef struct {
	MapObjectSave unk_00[64];
} FieldOverworldSave;

int FieldOverworldState_GetSaveSize(void);
int FieldOverworldSave_GetSaveSize(void);
void FieldOverworldSave_Init(FieldOverworldSave* fieldSave);
void FieldOverworldState_Init(FieldOverworldState* fieldState);

#endif /* MPR_FIELD_OVERWORLD_STATE_H */