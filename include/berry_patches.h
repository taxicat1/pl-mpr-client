#ifndef MPR_BERRY_PATCHES_H
#define MPR_BERRY_PATCHES_H

#define MAX_BERRY_PATCHES  128

typedef struct {
	u8   berryID;
	u8   growthStage;
	u16  stageMinutesRemaining;
	u16  moistureMinutesRemaining;
	u8   replantCount;
	u16  yield;
	u8   moistureRating;
	u8   yieldRating;
	u8   mulchType;
	u8   isGrowing;
} BerryPatch;

void BerryPatches_Clear(BerryPatch *patches);

#endif /* MPR_BERRY_PATCHES_H */
