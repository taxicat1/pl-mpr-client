#include <nitro.h>
#include <string.h>

#include "battle_regulation.h"


int BattleRegulation_GetSaveSize(void) {
	return sizeof(BattleRegulationData);
}


void BattleRegulation_Init(BattleRegulationData* regulation) {
	memset(regulation, 0, sizeof(BattleRegulationData));
}
