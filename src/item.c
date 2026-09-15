#include <nitro.h>

#include "item.h"

#include "common.h"
#include "assert.h"
#include "game_version.h"
#include "heap.h"
#include "narc.h"
#include "message.h"
#include "constants/moves.h"

#include "item_tables.dat" // sBerryItemIDs, sItemArchiveIDs, sMailItemIDs, sTMHMMoves

static s32 ItemPartyParam_Get(ItemPartyParam* partyParam, ItemDataParam attributeID);

void Dummy_Unused_0205EEAC(void);
void Dummy_Unused_0205EEAC(void) {
	sBerryItemIDs[0];
}


u16 Item_GetFileID(u16 item, ItemFileType type) {
	switch (type) {
		case ITEM_FILE_TYPE_DATA:
			if (item == ITEM_NONE || item == ITEM_RETURN_ID) {
				break;
			}
			
			return sItemArchiveIDs[item].dataID;
		
		case ITEM_FILE_TYPE_ICON:
			if (item == ITEM_NONE) {
				return none_NCGR;
			}
			
			if (item == ITEM_RETURN_ID) {
				return unused_701_NCGR;
			}
			
			return sItemArchiveIDs[item].iconID;
		
		case ITEM_FILE_TYPE_PALETTE:
			if (item == ITEM_NONE) {
				return none_NCLR;
			}
			
			if (item == ITEM_RETURN_ID) {
				return unused_702_NCLR;
			}
			
			return sItemArchiveIDs[item].paletteID;
		
		case ITEM_FILE_TYPE_GBA:
			if (item == ITEM_NONE || item == ITEM_RETURN_ID) {
				break;
			}
			
			return sItemArchiveIDs[item].gen3ID;
	}
	
	return 0;
}


void* Item_Load(u16 item, ItemFileType type, HeapID heapID) {
	// BUG: sItemArchiveIDs has DP-specific indices. Pt-exclusive items cannot have their graphics loaded.
	
    if (item > (ITEM_MAX - 1)) {
        item = ITEM_NONE;
    }
	
	NarcID narc;
	
    switch (type) {
		case ITEM_FILE_TYPE_DATA:
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_DATA;
			} else {
				narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__PL_ITEM_DATA;
			}
			
			return NARC_AllocAndReadWholeMemberByIndexPair(narc, sItemArchiveIDs[item].dataID, heapID);
		
		case ITEM_FILE_TYPE_ICON:
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_ICON;
			} else {
				narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__ITEM_ICON;
			}
			
			return NARC_AllocAndReadWholeMemberByIndexPair(narc, sItemArchiveIDs[item].iconID, heapID);
		
		case ITEM_FILE_TYPE_PALETTE:
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_ICON;
			} else {
				narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__ITEM_ICON;
			}
			
			return NARC_AllocAndReadWholeMemberByIndexPair(narc, sItemArchiveIDs[item].paletteID, heapID);
		
		default:
			return NULL;
    }
}


void Item_LoadName(String* dst, u16 item, HeapID heapID) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
    MessageLoader* msgData = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_ITEM_NAMES, heapID);
    MessageLoader_GetString(msgData, item, dst);
    MessageLoader_Free(msgData);
}


s32 Item_LoadParam(u16 item, ItemDataParam param, HeapID heapID) {
	ItemData* itemData = Item_Load(item, 0, heapID);
	s32 val = Item_Get(itemData, param);
	Heap_FreeExplicit(heapID, itemData);
	return val;
}


s32 Item_Get(ItemData* itemData, ItemDataParam param) {
	switch (param) {
		case ITEM_PARAM_PRICE:
			return itemData->price;
		
		case ITEM_PARAM_HOLD_EFFECT:
			return itemData->holdEffect;
		
		case ITEM_PARAM_EFFECT_PARAM:
			return itemData->effectParam;
		
		case ITEM_PARAM_PREVENT_TOSS:
			return itemData->preventToss;
		
		case ITEM_PARAM_CAN_REGISTER:
			return itemData->canRegister;
		
		case ITEM_PARAM_FIELD_POCKET:
			return itemData->fieldPocket;
		
		case ITEM_PARAM_FIELD_USE_FUNC:
			return itemData->fieldUseFunc;
		
		case ITEM_PARAM_BATTLE_USE_FUNC:
			return itemData->battleUseFunc;
		
		case ITEM_PARAM_PLUCK_EFFECT:
			return itemData->pluckEffect;
		
		case ITEM_PARAM_FLING_EFFECT:
			return itemData->flingEffect;
		
		case ITEM_PARAM_FLING_POWER:
			return itemData->flingPower;
		
		case ITEM_PARAM_NATURAL_GIFT_POWER:
			return itemData->naturalGiftPower;
		
		case ITEM_PARAM_NATURAL_GIFT_TYPE:
			return itemData->naturalGiftType;
		
		case ITEM_PARAM_BATTLE_POCKET:
			return itemData->battlePocket;
		
		case ITEM_PARAM_PARTY_USE:
			return itemData->partyUse;
		
		default:
			switch (itemData->partyUse) {
				case FALSE:
					return itemData->dummy;
				
				case TRUE:
					return ItemPartyParam_Get(&itemData->partyUseParam, param);
			}
	}

	return 0;
}


static s32 ItemPartyParam_Get(ItemPartyParam* partyParam, ItemDataParam attributeID) {
	switch (attributeID) {
		case ITEM_PARAM_HEAL_SLEEP:
			return partyParam->healSleep;
		
		case ITEM_PARAM_HEAL_POISON:
			return partyParam->healPoison;
		
		case ITEM_PARAM_HEAL_BURN:
			return partyParam->healBurn;
		
		case ITEM_PARAM_HEAL_FREEZE:
			return partyParam->healFreeze;
		
		case ITEM_PARAM_HEAL_PARALYSIS:
			return partyParam->healParalysis;
		
		case ITEM_PARAM_HEAL_CONFUSION:
			return partyParam->healConfusion;
		
		case ITEM_PARAM_HEAL_ATTRACT:
			return partyParam->healAttract;
		
		case ITEM_PARAM_GUARD_SPEC:
			return partyParam->guardSpec;
		
		case ITEM_PARAM_REVIVE:
			return partyParam->revive;
		
		case ITEM_PARAM_REVIVE_ALL:
			return partyParam->reviveAll;
		
		case ITEM_PARAM_LEVEL_UP:
			return partyParam->levelUp;
		
		case ITEM_PARAM_EVOLVE:
			return partyParam->evolve;
		
		case ITEM_PARAM_ATK_STAGES:
			return partyParam->atkStages;
		
		case ITEM_PARAM_DEF_STAGES:
			return partyParam->defStages;
		
		case ITEM_PARAM_SPATK_STAGES:
			return partyParam->spatkStages;
		
		case ITEM_PARAM_SPDEF_STAGES:
			return partyParam->spdefStages;
		
		case ITEM_PARAM_SPEED_STAGES:
			return partyParam->speedStages;
		
		case ITEM_PARAM_ACC_STAGES:
			return partyParam->accStages;
		
		case ITEM_PARAM_CRIT_STAGES:
			return partyParam->critStages;
		
		case ITEM_PARAM_PP_UP:
			return partyParam->ppUp;
		
		case ITEM_PARAM_PP_MAX:
			return partyParam->ppMax;
		
		case ITEM_PARAM_PP_RESTORE:
			return partyParam->ppRestore;
		
		case ITEM_PARAM_PP_RESTORE_ALL:
			return partyParam->ppRestoreAll;
		
		case ITEM_PARAM_HP_RESTORE:
			return partyParam->hpRestore;
		
		case ITEM_PARAM_GIVE_HP_EVS:
			return partyParam->giveHPEVs;
		
		case ITEM_PARAM_GIVE_ATK_EVS:
			return partyParam->giveAtkEVs;
		
		case ITEM_PARAM_GIVE_DEF_EVS:
			return partyParam->giveDefEVs;
		
		case ITEM_PARAM_GIVE_SPEED_EVS:
			return partyParam->giveSpeedEVs;
		
		case ITEM_PARAM_GIVE_SPATK_EVS:
			return partyParam->giveSpAtkEVs;
		
		case ITEM_PARAM_GIVE_SPDEF_EVS:
			return partyParam->giveSpDefEVs;
		
		case ITEM_PARAM_GIVE_FRIENDSHIP_LOW:
			return partyParam->giveFriendshipLow;
		
		case ITEM_PARAM_GIVE_FRIENDSHIP_MED:
			return partyParam->giveFriendshipMed;
		
		case ITEM_PARAM_GIVE_FRIENDSHIP_HIGH:
			return partyParam->giveFriendshipHigh;
		
		case ITEM_PARAM_HP_EVS:
			return partyParam->hpEVs;
		
		case ITEM_PARAM_ATK_EVS:
			return partyParam->atkEVs;
		
		case ITEM_PARAM_DEF_EVS:
			return partyParam->defEVs;
		
		case ITEM_PARAM_SPEED_EVS:
			return partyParam->speedEVs;
		
		case ITEM_PARAM_SPATK_EVS:
			return partyParam->spatkEVs;
		
		case ITEM_PARAM_SPDEF_EVS:
			return partyParam->spdefEVs;
		
		case ITEM_PARAM_HP_RESTORED:
			return partyParam->hpRestored;
		
		case ITEM_PARAM_PP_RESTORED:
			return partyParam->ppRestored;
		
		case ITEM_PARAM_FRIENDSHIP_LOW:
			return partyParam->friendshipLow;
		
		case ITEM_PARAM_FRIENDSHIP_MED:
			return partyParam->friendshipMed;
		
		case ITEM_PARAM_FRIENDSHIP_HIGH:
			return partyParam->friendshipHigh;
		
		default:
			return 0;
	}
}


u8 Item_IsHMMove(u16 move) {
	for (u8 i = 0; i < NUM_HMS; i++) {
		if (sTMHMMoves[NUM_TMS + i] == move) {
			return TRUE;
		}
	}
	
	return FALSE;
}


u8 Item_IsMail(u16 item) {
	for (u32 i = 0; i < NUM_MAIL_TYPES; i++) {
		if (sMailItemIDs[i] == item) {
			return TRUE;
		}
	}
	
	return FALSE;
}
