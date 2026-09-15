#ifndef MPR_SPECIAL_ENCOUNTER_H
#define MPR_SPECIAL_ENCOUNTER_H

#include "savedata.h"

#define NUM_ROAMER_SLOTS_DP  3
#define NUM_ROAMER_SLOTS_PT  6
#define NUM_HONEY_TREES      21
#define NUM_RADAR_RECORDS    3

typedef struct {
	BOOL  unused;
	u16   slot1;
	u16   slot2;
} TrophyGardenMons;

typedef struct {
	int  minutesRemaining;
	u8   encounterSlot;
	u8   encounterTableIndex;
	u8   encounterGroup;
	u8   numShakes;
} HoneyTree;

typedef struct {
    u8         lastSlatheredTree;
    HoneyTree  honeyTrees[NUM_HONEY_TREES];
} PlayerHoneyTreeStates;

typedef struct {
	u16  species;
	u16  chainCount;
} ChainRecord;

typedef struct {
	ChainRecord records[NUM_RADAR_RECORDS];
} RadarChainRecords;

typedef struct {
	int  currentMapId;
	int  previousMapId;
} PlayerRecentRoutes;

typedef struct {
	int  currentMapId;
	u32  ivs;
	u32  personality;
	u16  species;
	u16  currentHP;
	u8   level;
	u8   status;
	u8   active;
} Roamer;

typedef struct {
	int                    marshDaily;
	int                    swarmDaily;
	TrophyGardenMons       trophyGarden;
	PlayerHoneyTreeStates  treeStates;
	RadarChainRecords      chainRecords;
	PlayerRecentRoutes     recentRoutes;
	Roamer                 roamers[NUM_ROAMER_SLOTS_DP];
	u8                     roamerRouteIndexes[NUM_ROAMER_SLOTS_DP];
	u8                     swarmEnabled;
	u8                     repelSteps;
	u8                     radarCharge;
	u8                     fluteFactor;
} SpecialEncounterDP;

typedef struct {
	int                    marshDaily;
	int                    swarmDaily;
	TrophyGardenMons       trophyGarden;
	PlayerHoneyTreeStates  treeStates;
	RadarChainRecords      chainRecords;
	PlayerRecentRoutes     recentRoutes;
	Roamer                 roamers[NUM_ROAMER_SLOTS_PT];
	u8                     roamerRouteIndexes[NUM_ROAMER_SLOTS_PT];
	u8                     swarmEnabled;
	u8                     repelSteps;
	u8                     radarCharge;
	u8                     fluteFactor;
} SpecialEncounterPt;

typedef struct {
	int                    marshDaily;
	int                    swarmDaily;
	TrophyGardenMons       trophyGarden;
	PlayerHoneyTreeStates  treeStates;
	RadarChainRecords      chainRecords;
	PlayerRecentRoutes     recentRoutes;
} SpecialEncounterCommon;

#define NUM_TROPHY_GARDEN_SPECIAL_MONS  16
#define TROPHY_GARDEN_SLOT_NONE         0xFFFF

int SpecialEncounter_GetSaveSize(void);
void SpecialEncounter_Init(SpecialEncounterCommon* speEnc);

#endif /* MPR_SPECIAL_ENCOUNTER_H */
