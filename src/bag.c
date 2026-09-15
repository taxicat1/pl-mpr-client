#include <nitro.h>
#include <string.h>

#include "bag.h"

#include "item.h"
#include "constants/items.h"

#define BAG_SLOT_INVALID       0xFFFFFFFF
#define BAG_MAX_QUANTITY_ITEM  999
#define BAG_MAX_QUANTITY_TMHM  99

static BagItem* Pocket_FindSlotToAddItem(BagItem *pocket, u32 pocketSize, u16 item, u16 count, u16 max);
static BagItem* Bag_FindSlotToAddItem(Bag* bag, u16 item, u16 count, HeapID heapID);
static void BagItem_Swap(BagItem* a, BagItem* b);


int Bag_GetSaveSize(void) {
	return sizeof(Bag);
}


void Bag_Init(Bag* bag) {
	MI_CpuClear16(bag, sizeof(Bag));
}


static u32 Bag_GetPocketForItem(Bag* bag, u16 item, BagItem** outPocket, u32* outMax, HeapID heapID) {
	u32 pocket = Item_LoadParam(item, ITEM_PARAM_FIELD_POCKET, heapID);
	
	switch (pocket) {
		case POCKET_KEY_ITEMS:
			*outPocket = bag->keyItems;
			*outMax = KEY_ITEM_POCKET_SIZE;
			break;
		
		case POCKET_ITEMS:
			*outPocket = bag->items;
			*outMax = ITEM_POCKET_SIZE;
			break;
		
		case POCKET_BERRIES:
			*outPocket = bag->berries;
			*outMax = BERRY_POCKET_SIZE;
			break;
		
		case POCKET_MEDICINE:
			*outPocket = bag->medicine;
			*outMax = MEDICINE_POCKET_SIZE;
			break;
		
		case POCKET_BALLS:
			*outPocket = bag->pokeballs;
			*outMax = POKEBALL_POCKET_SIZE;
			break;
		
		case POCKET_BATTLE_ITEMS:
			*outPocket = bag->battleItems;
			*outMax = BATTLE_ITEM_POCKET_SIZE;
			break;
		
		case POCKET_MAIL:
			*outPocket = bag->mail;
			*outMax = MAIL_POCKET_SIZE;
			break;
		
		case POCKET_TMHMS:
			*outPocket = bag->tmHms;
			*outMax = TMHM_POCKET_SIZE;
			break;
	}
	
	return pocket;
}


static BagItem* Pocket_FindSlotToAddItem(BagItem* pocket, u32 pocketSize, u16 item, u16 count, u16 max) {
	u32 i, slot = BAG_SLOT_INVALID;
	for (i = 0; i < pocketSize; i++) {
		if (pocket[i].item == item) {
			if (pocket[i].quantity + count > max) {
				return NULL;
			}
			
			return &pocket[i];
		}
		
		if (slot == BAG_SLOT_INVALID && pocket[i].item == ITEM_NONE && pocket[i].quantity == 0) {
			slot = i;
		}
	}
	
	return (slot == BAG_SLOT_INVALID) ? NULL : &pocket[slot];
}


static BagItem* Bag_FindSlotToAddItem(Bag* bag, u16 item, u16 count, HeapID heapID) {
	BagItem* pocketItems;
	u32 pocketSize;
	u32 pocket = Bag_GetPocketForItem(bag, item, &pocketItems, &pocketSize, heapID);
	if (pocket == POCKET_TMHMS) {
		return Pocket_FindSlotToAddItem(pocketItems, pocketSize, item, count, BAG_MAX_QUANTITY_TMHM);
	}
	
	return Pocket_FindSlotToAddItem(pocketItems, pocketSize, item, count, BAG_MAX_QUANTITY_ITEM);
}


BOOL Bag_TryAddItem(Bag* bag, u16 item, u16 count, HeapID heapID) {
	BagItem* slot = Bag_FindSlotToAddItem(bag, item, count, heapID);
	if (slot == NULL) {
		return FALSE;
	}
	
	slot->item = item;
	slot->quantity += count;
	
	u32 pocketSize;
	u32 pocket = Bag_GetPocketForItem(bag, item, &slot, &pocketSize, heapID);
	if (pocket == POCKET_BERRIES || pocket == POCKET_TMHMS) {
		Pocket_Sort(slot, pocketSize);
	}

	return TRUE;
}


static void BagItem_Swap(BagItem* a, BagItem* b) {
	BagItem temp = *a;
	*a = *b;
	*b = temp;
}


void Pocket_Sort(BagItem* pocket, const u32 size) {
	for (u32 i = 0; i < size - 1; i++) {
		for (u32 j = i + 1; j < size; j++) {
			if (pocket[i].quantity == 0 || (pocket[j].quantity != 0 && pocket[i].item > pocket[j].item)) {
				BagItem_Swap(&pocket[i], &pocket[j]);
			}
		}
	}
}


Bag* SaveData_GetBag(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_BAG);
}
