#ifndef MPR_MYSTERY_GIFT_H
#define MPR_MYSTERY_GIFT_H

/* Unimplemented */
#define MYSTERY_GIFT_SIZE_DP  0x1354
#define MYSTERY_GIFT_SIZE_PT  0x1328

typedef struct {
	u8 dat[MYSTERY_GIFT_SIZE_DP];
} MysteryGiftDP;

typedef struct {
	u8 dat[MYSTERY_GIFT_SIZE_PT];
} MysteryGiftPt;

typedef void MysteryGiftCommon;

int MysteryGift_GetSaveSize(void);
void MysteryGift_Init(MysteryGiftCommon* mysteryGift);

#endif /* MPR_MYSTERY_GIFT_H */
