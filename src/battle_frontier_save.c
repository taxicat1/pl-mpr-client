#include <nitro.h>

#include "battle_frontier_save.h"

#include "game_version.h"


int BattleFrontier_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(BattleFrontierSaveDP);
	} else {
		return sizeof(BattleFrontierSavePt);
	}
}


void BattleFrontier_Init(BattleFrontierSaveCommon* frontier) {
	/* Unimplemented */
	return;
}
