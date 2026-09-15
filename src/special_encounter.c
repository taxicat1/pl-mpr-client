#include <nitro.h>
#include <string.h>

#include "special_encounter.h"

#include "game_version.h"
#include "math_util.h"
#include "savedata.h"


int SpecialEncounter_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(SpecialEncounterDP);
	} else {
		return sizeof(SpecialEncounterPt);
	}
}


void SpecialEncounter_Init(SpecialEncounterCommon* specialEncounter) {
	memset(specialEncounter, 0, SpecialEncounter_GetSaveSize());
	
	specialEncounter->marshDaily = MTRNG_Next();
	specialEncounter->swarmDaily = MTRNG_Next();
	specialEncounter->trophyGarden.unused = 0;
	specialEncounter->trophyGarden.slot1 = TROPHY_GARDEN_SLOT_NONE;
	specialEncounter->trophyGarden.slot2 = TROPHY_GARDEN_SLOT_NONE;
	
	int i;
	PlayerHoneyTreeStates* honeyTreeDat = &specialEncounter->treeStates;
	
	honeyTreeDat->lastSlatheredTree = NUM_HONEY_TREES;
	
	for (i = 0; i < NUM_HONEY_TREES; i++) {
		honeyTreeDat->honeyTrees[i].minutesRemaining    = 0;
		honeyTreeDat->honeyTrees[i].encounterSlot       = 0;
		honeyTreeDat->honeyTrees[i].encounterTableIndex = 0;
		honeyTreeDat->honeyTrees[i].encounterGroup      = 0;
		honeyTreeDat->honeyTrees[i].numShakes           = 0;
	}
	
	// BUG: DP assumed
	SpecialEncounterDP* specialEncounterDP = (SpecialEncounterDP*)specialEncounter;
	specialEncounterDP->swarmEnabled = FALSE;
	specialEncounterDP->repelSteps   = 0;
	specialEncounterDP->radarCharge  = 0;
	specialEncounterDP->fluteFactor  = 0;
}
