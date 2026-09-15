#ifndef MPR_BATTLE_REGULATION_H
#define MPR_BATTLE_REGULATION_H

#include "savedata.h"
#include "string_gf.h"

#define BATTLE_REGULATION_NAME_LENGTH  12

typedef struct {
	u16  name[BATTLE_REGULATION_NAME_LENGTH];
	u16  maxTotalLevel;
	u8   teamSize;
	u8   maxLevel;
	s8   heightRestriction;
	s8   weightRestriction;
	u8   evolutionRestriction : 1;
	u8   specialSpeciesRestriction : 1;
	u8   speciesUniqueness : 1;
	u8   itemUniqueness : 1;
	u8   moveRestrictions : 1;
} BattleRegulation;

typedef struct {
	BattleRegulation regulations[1];
} BattleRegulationData;

int BattleRegulation_GetSaveSize(void);
void BattleRegulation_Init(BattleRegulationData* regulationData);

#endif /* MPR_BATTLE_REGULATION_H */
