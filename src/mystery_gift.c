#include <nitro.h>

#include "mystery_gift.h"

#include "game_version.h"


int MysteryGift_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(MysteryGiftDP);
	} else {
		return sizeof(MysteryGiftPt);
	}
}


void MysteryGift_Init(MysteryGiftCommon* mysteryGift) {
	/* Unimplemented */
	return;
}
