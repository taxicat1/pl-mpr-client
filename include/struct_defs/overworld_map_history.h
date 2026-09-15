#ifndef MPR_OVERWORLD_MAP_HISTORY_H
#define MPR_OVERWORLD_MAP_HISTORY_H

#define OVERWORLD_MAP_HISTORY_LENGTH  6

typedef struct {
	u8  mapX;
	u8  mapZ;
	u8  faceDirection;
	u8  isSet;
} OverworldMapHistoryItem;

typedef struct {
	int  historyPointer;
	u8   currentMapX;
	u8   currentMapZ;
	
	OverworldMapHistoryItem items[OVERWORLD_MAP_HISTORY_LENGTH];
} OverworldMapHistory;

#endif /* MPR_OVERWORLD_MAP_HISTORY_H */
