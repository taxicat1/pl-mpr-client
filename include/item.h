#ifndef MPR_ITEM_DATA_H
#define MPR_ITEM_DATA_H

#include "constants/item_data_params.h"
#include "constants/heap.h"

#include "string_gf.h"

typedef struct {
	u8  healSleep     : 1;
	u8  healPoison    : 1;
	u8  healBurn      : 1;
	u8  healFreeze    : 1;
	u8  healParalysis : 1;
	u8  healConfusion : 1;
	u8  healAttract   : 1;
	u8  guardSpec     : 1;
	
	u8  revive    : 1;
	u8  reviveAll : 1;
	u8  levelUp   : 1;
	u8  evolve    : 1;
	u8  atkStages : 4;
	
	u8  defStages   : 4;
	u8  spatkStages : 4;
	
	u8  spdefStages : 4;
	u8  speedStages : 4;
	
	u8  accStages  : 4;
	u8  critStages : 2;
	u8  ppUp       : 1;
	u8  ppMax      : 1;
	
	u8  ppRestore    : 1;
	u8  ppRestoreAll : 1;
	u8  hpRestore    : 1;
	u8  giveHPEVs    : 1;
	u8  giveAtkEVs   : 1;
	u8  giveDefEVs   : 1;
	u8  giveSpeedEVs : 1;
	u8  giveSpAtkEVs : 1;
	
	u8  giveSpDefEVs       : 1;
	u8  giveFriendshipLow  : 1;
	u8  giveFriendshipMed  : 1;
	u8  giveFriendshipHigh : 1;
	
	s8  hpEVs;
	s8  atkEVs;
	s8  defEVs;
	s8  speedEVs;
	s8  spatkEVs;
	s8  spdefEVs;
	u8  hpRestored;
	u8  ppRestored;
	s8  friendshipLow;
	s8  friendshipMed;
	s8  friendshipHigh;
	u8  padding[2];
} ItemPartyParam;

typedef struct {
	u16  price;
	u8   holdEffect;
	u8   effectParam;
	u8   pluckEffect;
	u8   flingEffect;
	u8   flingPower;
	u8   naturalGiftPower;
	u16  naturalGiftType : 5;
	u16  preventToss     : 1;
	u16  canRegister     : 1;
	u16  fieldPocket     : 4;
	u16  battlePocket    : 5;
	u8   fieldUseFunc;
	u8   battleUseFunc;
	u8   partyUse;
	u8   padding_0D;
	union {
		u8              dummy;
		ItemPartyParam  partyUseParam;
	};
} ATTRIBUTE_ALIGN(4) ItemData;

typedef enum {
	ITEM_FILE_TYPE_DATA,
	ITEM_FILE_TYPE_ICON,
	ITEM_FILE_TYPE_PALETTE,
	ITEM_FILE_TYPE_GBA
} ItemFileType;

/**
 * @brief Get an ID of the specified type for an item.
 *
 * Each type points to a different value for the item's entry in sItemArchiveIDs.
 *
 * @param item
 * @param type      The type of ID to get.
 * @return The item's ID of the specified type.
 */
u16 Item_GetFileID(u16 item, ItemFileType type);

/**
 * @brief Load item data of the specified type.
 *
 * @param item      The item to load.
 * @param type      The type of data to load. Determines which NARC is loaded.
 * @param heapID    ID of the heap to own the allocated memory.
 * @return The loaded data structure.
 */
void* Item_Load(u16 item, ItemFileType type, HeapID heapID);

/**
 * @brief Load the name of the item into a string buffer.
 *
 * @param dst       The destination string buffer.
 * @param item      The item whose name we should load.
 * @param heapID    ID of the heap to own memory needed for the text archive.
 */
void Item_LoadName(String* dst, u16 item, HeapID heapID);

/**
 * @brief Load a parameter for a given item from the data archive.
 *
 * @param item      The item to load.
 * @param param     The param to load from the item.
 * @param heapID    The heap on which to load the item data.
 * @return Parameter value from the loaded item.
 */
s32 Item_LoadParam(u16 item, ItemDataParam param, HeapID heapID);

/**
 * @brief Get a param value from a loaded item.
 *
 * @param itemData  The loaded item.
 * @param param     The param value to get.
 * @return Parameter value from the loaded item.
 */
s32 Item_Get(ItemData* itemData, ItemDataParam param);

/**
 * @brief Check if a given move is an HM move.
 *
 * @param move      The move to check.
 * @return TRUE if the move is assignable via an HM. FALSE otherwise.
 */
u8 Item_IsHMMove(u16 move);

/**
 * @brief Check if a given item is mail.
 *
 * @param item
 * @return TRUE if the item is mail, FALSE otherwise.
 */
u8 Item_IsMail(u16 item);

#endif /* MPR_ITEM_DATA_H */
