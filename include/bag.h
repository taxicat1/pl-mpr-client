#ifndef MPR_BAG_H
#define MPR_BAG_H

#include "constants/heap.h"

#include "savedata.h"

#define ITEM_POCKET_SIZE          165
#define KEY_ITEM_POCKET_SIZE      50
#define TMHM_POCKET_SIZE          100
#define MAIL_POCKET_SIZE          12
#define MEDICINE_POCKET_SIZE      40
#define BERRY_POCKET_SIZE         64
#define POKEBALL_POCKET_SIZE      15
#define BATTLE_ITEM_POCKET_SIZE   30
#define POCKET_MAX                8
#define ITEM_BATTLE_CATEGORY_MAX  4

typedef struct {
	u16  item;
	u16  quantity;
} BagItem;

typedef struct {
	BagItem  items[ITEM_POCKET_SIZE];
	BagItem  keyItems[KEY_ITEM_POCKET_SIZE];
	BagItem  tmHms[TMHM_POCKET_SIZE];
	BagItem  mail[MAIL_POCKET_SIZE];
	BagItem  medicine[MEDICINE_POCKET_SIZE];
	BagItem  berries[BERRY_POCKET_SIZE];
	BagItem  pokeballs[POKEBALL_POCKET_SIZE];
	BagItem  battleItems[BATTLE_ITEM_POCKET_SIZE];
	u32      registeredItem;
} Bag;

typedef struct {
	u8   scroll[POCKET_MAX];
	u8   index[POCKET_MAX];
	u16  pocket;
	u16  dummy12;
} FieldBagCursor;

typedef struct {
	u8   scroll[ITEM_BATTLE_CATEGORY_MAX + 1];
	u8   index[ITEM_BATTLE_CATEGORY_MAX + 1];
	u16  lastUsedItemID;
	u16  lastUsedCategory;
	u16  currentCategory;
} BattleBagCursor;

typedef struct {
	FieldBagCursor   field;
	BattleBagCursor  battle;
} BagCursor;

int Bag_GetSaveSize(void);
void Bag_Init(Bag* bag);
BOOL Bag_TryAddItem(Bag* bag, u16 item, u16 count, HeapID heapID);
void Pocket_Sort(BagItem* pocket, const u32 size);
Bag* SaveData_GetBag(SaveData* saveData);

#endif /* MPR_BAG_H */
