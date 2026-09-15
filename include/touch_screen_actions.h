#ifndef MPR_TOUCH_SCREEN_HIT_TABLE_ACTION_H
#define MPR_TOUCH_SCREEN_HIT_TABLE_ACTION_H

#include "touch_screen.h"

typedef void (*TouchScreenHitTableAction)(u32, TouchScreenButtonState, void*);

typedef struct TouchScreenHitTableActionStateInternal TouchScreenHitTableActionStateInternal;

typedef struct {
	const TouchScreenHitTable* hitTables;
	u32                        hitTablesLength;
	TouchScreenHitTableAction  action;
	void*                      hitTableContext;
	
	TouchScreenHitTableActionStateInternal* actionStates;
} TouchScreenActions;

TouchScreenActions* TouchScreenActions_RegisterHandler(const TouchScreenHitTable* hitTable, u32 hitTableLength, TouchScreenHitTableAction action, void* context, u32 heapID);
void TouchScreenActions_Free(TouchScreenActions* buttonAction);
void TouchScreenActions_HandleAction(TouchScreenActions* buttonAction);

#endif /* MPR_TOUCH_SCREEN_HIT_TABLE_ACTION_H */
