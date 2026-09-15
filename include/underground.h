#ifndef MPR_UNDERGROUND_H
#define MPR_UNDERGROUND_H

#include "charcode.h"
#include "savedata.h"
#include "trainer_info.h"

#define MAX_BASE_BOULDERS          16
#define MAX_PLACED_GOODS           15
#define MAX_CAPTURED_FLAG_RECORDS  5
#define MAX_SPAWNED_TRAPS          64
#define MAX_MINING_SPOTS           250
#define MAX_PLACED_TRAPS           16
#define MAX_BURIED_SPHERES         100
#define MAX_GOODS_PC_SLOTS         200
#define MAX_TRAP_SLOTS             40
#define MAX_GOODS_BAG_SLOTS        40
#define MAX_TREASURE_SLOTS         40
#define MAX_SPHERE_SLOTS           40

typedef enum {
	X_LOW_BITS_INDEX = 0,
	HIGH_BITS_INDEX,
	Z_LOW_BITS_INDEX,
	CONDENSED_COORDS_BYTE_COUNT
} CondensedCoordsIndex;

typedef struct {
	u32  trainerScore      : 20;
	u32  peopleMet         : 20;
	u32  numGiftsGiven     : 20;
	u32  flagsStolen       : 20;
	u32  numSpheresDug     : 20;
	u32  numFossilsDug     : 20;
	u32  numNonFossilsDug  : 20;
	u32  numTrapHits       : 20;
	u32  numTrapsTriggered : 20;
	u32  numPlayersHelped  : 20;
	u32  numGiftsReceived  : 20;
	u32  timesFlagTaken    : 20;
	u32  flagsRecovered    : 20;
	u32  timesBaseMoved    : 20;
	u32  capturedFlagCount : 20;
} UndergroundRecord;

typedef struct {
	u8  x;
	u8  z;
	u8  goodID;
} PlacedGood;

typedef struct {
	u8  x;
	u8  z;
} SecretBaseBoulder;

typedef struct {
	PlacedGood         placedGoods[MAX_PLACED_GOODS];
	SecretBaseBoulder  boulders[MAX_BASE_BOULDERS];
	UndergroundRecord  undergroundRecord;
	u16                entranceX;
	u16                entranceZ;
	u8                 entranceDir;
	u8                 active;
} SecretBase;

typedef struct {
	SecretBase  secretBase;
	u32         randomSeed;
	s32         giftPenaltyMinutesLeft;
	u8          giftPenaltyState;
	u8          collectedOrbUnused;
	u32         registeredFlagOwnerIDs[MAX_CAPTURED_FLAG_RECORDS];
	charcode_t  registeredFlagOwnerNames[MAX_CAPTURED_FLAG_RECORDS][TRAINER_NAME_LEN + 1];
	u8          registeredFlagOwnerLanguages[MAX_CAPTURED_FLAG_RECORDS];
	u8          registeredFlagOwnerGameCodes[MAX_CAPTURED_FLAG_RECORDS];
	u8          registeredFlagOwnerIndex;
	u8          spawnedTrapIDs[MAX_SPAWNED_TRAPS];
	u8          spawnedTrapCoordinates[MAX_SPAWNED_TRAPS][CONDENSED_COORDS_BYTE_COUNT];
	u8          miningSpotCoordinates[MAX_MINING_SPOTS + 5][CONDENSED_COORDS_BYTE_COUNT];
	u8          placedTrapIDs[MAX_PLACED_TRAPS];
	u8          placedTrapCoordinates[MAX_PLACED_TRAPS][CONDENSED_COORDS_BYTE_COUNT];
	u8          placedTrapSpawnedIndices[MAX_PLACED_TRAPS];
	u8          buriedSphereTypes[MAX_BURIED_SPHERES];
	u8          buriedSphereGrowth[MAX_BURIED_SPHERES];
	u8          buriedSphereInitialSizes[MAX_BURIED_SPHERES];
	u8          buriedSphereCoordinates[MAX_BURIED_SPHERES][CONDENSED_COORDS_BYTE_COUNT];
	u8          padding[88];
	u32         minedPlates;
	u8          goodsPC[MAX_GOODS_PC_SLOTS];
	u8          traps[MAX_TRAP_SLOTS];
	u8          goodsBag[MAX_GOODS_BAG_SLOTS];
	u8          treasure[MAX_TREASURE_SLOTS];
	u8          sphereTypes[MAX_SPHERE_SLOTS];
	u8          sphereSizes[MAX_SPHERE_SLOTS];
	u8          placedGoodSlots[MAX_PLACED_GOODS];
	u8          stepCount;
	u8          shouldSpawnNewBuriedObjects : 4;
	u8          hasMined                    : 4;
} Underground;

int Underground_GetSaveSize(void);
void Underground_Init(Underground* underground);

#endif /* MPR_UNDERGROUND_H */
