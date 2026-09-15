#ifndef MPR_BATTLE_FRONTIER_SAVE_H
#define MPR_BATTLE_FRONTIER_SAVE_H

#include <nitro/types.h>

/* Unimplemented */
#define BATTLE_TOWER_DP_SAVE_SIZE     0xD00
#define BATTLE_FRONTIER_PT_SAVE_SIZE  0x1628

typedef struct {
	u8 dat[BATTLE_TOWER_DP_SAVE_SIZE];
} BattleFrontierSaveDP;

typedef struct {
	u8 dat[BATTLE_FRONTIER_PT_SAVE_SIZE];
} BattleFrontierSavePt;

// No useful fields in common
typedef void BattleFrontierSaveCommon;

int BattleFrontier_GetSaveSize(void);
void BattleFrontier_Init(BattleFrontierSaveCommon* frontier);

#endif /* MPR_BATTLE_FRONTIER_SAVE_H */
