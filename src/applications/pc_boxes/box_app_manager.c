#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_app_manager.h"

#include "struct_defs/chatot_cry.h"

#include "applications/naming_screen/naming_screen_main.h"
#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_mon_bucket_sorting.h"
#include "applications/pc_boxes/box_menu.h"
#include "applications/pc_boxes/box_mpr.h"
#include "applications/pc_boxes/box_touch_dial.h"

#include "applications/pokemon_summary_screen/summary_screen_main.h"

#include "assert.h"
#include "bag.h"
#include "common.h"
#include "chatot_cry.h"
#include "dexmode_checker.h"
#include "game_options.h"
#include "game_overlay.h"
#include "game_version.h"
#include "heap.h"
#include "item.h"
#include "message.h"
#include "overlay_manager.h"
#include "party.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "save_player.h"
#include "save_table.h"
#include "savedata.h"
#include "sound_playback.h"
#include "string_gf.h"
#include "string_template.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "system.h"
#include "touch_screen.h"
#include "touch_screen_actions.h"
#include "special_ribbons.h"

#include "mpr_client/mpr_comm.h"
#include "mpr_client/mpr_text.h"

#include "constants/items.h"
#include "constants/species.h"
#include "constants/natures.h"
#include "constants/string.h"

#include "fs/msgdata/box_messages.h"

typedef enum {
	BOX_SELECTOR_START,
	BOX_SELECTOR_DISPLAY_MESSAGE_START,
	BOX_SELECTOR_DISPLAY_MESSAGE_DONE,
	BOX_SELECTOR_WAIT_FOR_USER
} BoxSelectorState;

typedef int (*CursorLocationInputHandler)(BoxApplicationManager*);
typedef void (*BoxApplicationAction)(BoxApplicationManager*, u32*);

static void BoxAppMan_RegisterCursorLocationInputHandler(BoxApplicationManager* boxAppMan, CursorLocationInputHandler cursorLocationInputHandler);
static void BoxAppMan_RegisterBoxApplicationAction(BoxApplicationManager* boxAppMan, BoxApplicationAction boxApplicationAction);
static void BoxAppMan_ClearBoxApplicationAction(BoxApplicationManager* boxAppMan);
static void BoxAppMan_ResetAction(BoxApplicationManager* boxAppMan);
static CursorLocationInputHandler BoxAppMan_GetCursorLocationInputHandler(BoxApplicationManager* boxAppMan);
static void BoxAppMan_FlagRecordBoxUseInJournal(BoxApplicationManager* boxAppMan);
static void BoxAppMan_LogInAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_ReturnToBoxFade1Action(BoxApplicationManager* boxAppMan, u32* state);
static int BoxAppMan_SortIndexToSortOrder(int index);
static BOOL BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(BoxApplicationManager* boxAppMan);
static int BoxAppMan_CursorInBoxInputHandler(BoxApplicationManager* boxAppMan);
static BOOL BoxAppMan_IsPreviewedMonHoldingMailOrHasBallCapsule(BoxApplicationManager* boxAppMan, int* destMessageID);
static int BoxAppMan_CursorInPartyInputHandler(BoxApplicationManager* boxAppMan);
static int BoxAppMan_CursorOnHeaderInputHandler(BoxApplicationManager* boxAppMan);
static int BoxAppMan_CursorOnCloseInputHandler(BoxApplicationManager* boxAppMan);
static int BoxAppMan_CursorOnPartyButtonInputHandler(BoxApplicationManager* boxAppMan);
static void BoxAppMan_InitSummary(BoxApplicationManager* boxAppMan);
static int BoxAppMan_LogOffScreenFade(BoxApplicationManager* boxAppMan);
static void BoxAppMan_CloseBoxAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_ContinueBoxOperationsAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_MonCursorMenuAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_MonItemMenuAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_BoxHeaderMenuAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_BoxJumpAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_RanchBoxJumpAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_WallpaperMenu(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_MarkAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_MultiSelectAction(BoxApplicationManager* boxAppMan, u32* state);
static BOOL BoxApp_IsBoxUnderSelectedMonsEmpty(const BoxApplication* boxApp);
static void BoxAppMan_PickUpMonAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_PlaceMonAction(BoxApplicationManager* boxAppMan, u32* state);
static BOOL BoxAppMan_CheckLastAliveMonReason(BoxApplicationManager* boxAppMan, u32* destMessageID);
static void BoxAppMan_ShiftMonAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_WithdrawMonAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_StoreMonAction(BoxApplicationManager* boxAppMan, u32* state);
static BOOL BoxAppMan_OnLastAliveMon(BoxApplicationManager* boxAppMan);
static BOOL BoxAppMan_CheckReleaseMonValid(BoxApplicationManager* boxAppMan, int* destBoxMessageID);
static void BoxAppMan_ReleaseMonAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_CheckShouldMonReturn(BoxApplicationManager* boxAppMan);
static void CheckLastMonWithReleaseBlockingMove(SysTask* task, void* releaseMonParam);
static BOOL BoxPokemon_HasMove(BoxPokemon* boxMon, u16 move);
static void BoxAppMan_RenameBoxAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_OpenSummaryAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxApp_SetCursorPosToSummaryMonPos(BoxApplication* boxApp, BoxApplicationManager* boxAppMan);
static void BoxAppMan_MonItemHeldAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_PutAwayItemAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_DisplayItemInfoAction(BoxApplicationManager* boxAppMan, u32* state);
static BOOL BoxApp_CanDepositMonToRanch(BoxApplication* boxApp, int* outMessageID);
static void BoxAppMan_RanchDepositAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_RanchWithdrawAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_RanchBatchDepositAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_RanchBatchWithdrawAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_DownloadRanchBoxesAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxSelectorPopup_Init(BoxApplicationManager* boxAppMan, u32 boxID, u32 boxMessageID);
static void BoxSelectorPopup_Reset(BoxApplicationManager* boxAppMan);
static BOOL BoxAppMan_TrySelectBoxFromPopup(BoxApplicationManager* boxAppMan);
static void BoxAppMan_ChangeToNewBoxAction(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_TouchScreenBoxJump(BoxApplicationManager* boxAppMan, u32* state);
static void BoxAppMan_MarkingsFilterAction(BoxApplicationManager* boxAppMan, u32* state);
static BOOL BoxAppMan_TryPressMarkingsButton(BoxApplicationManager* boxAppMan);
static void BoxApp_BoxTouchScreenMarkingsButtonHandler(u32 buttonIndex, TouchScreenButtonState buttonTouchState, void* context);
static void BoxAppMan_Load(BoxApplicationManager* boxAppMan, PokemonStorageSession* pokemonStorageSession);
static void BoxAppMan_Free(BoxApplicationManager* boxAppMan);
static void BoxAppMan_CommBufferRanchMons(BoxApplicationManager* boxAppMan, BoxMode boxMode);
static void BoxSettings_Init(BoxSettings* boxSettings, BoxMode boxMode);
static void BoxAppMan_InitCursor(BoxApplicationManager* boxAppMan);
static void BoxMonSelection_Init(BoxMonSelection* selection);
static void BoxMonSelection_Free(BoxMonSelection* selection);
static void PCBoxes_InitCustomization(PCBoxes* pcBoxes, BoxCustomization* customization, BoxMode mode);
static void Customization_Free(BoxCustomization* customization);
static void PCMonPreview_Init(PCMonPreview* preview);
static void PCMonPreview_Free(PCMonPreview* preview);
static void PCCompareHelper_Init(CompareModeHelper* compareHelper);
static void PCCompareHelper_Free(CompareModeHelper* compareHelper);
static void BoxTouchDialHelper_Init(BoxTouchDialHelper* touchDialHelper);
static void PCBoxes_LoadCustomization(const PCBoxes* pcBoxes, BoxCustomization* customization, BoxMode boxMode);
static void BoxApp_LoadWallpaper(BoxApplication* boxApp, PCBoxes* pcBoxes);
static BOOL BoxAppMan_TryMoveCursorFromUserInput(u32 heldKeys, BoxApplicationManager* boxAppMan);
static BOOL BoxApp_TryMoveCursor(BoxApplication* boxApp, int colChange, int rowChange);
static CursorMovementState BoxAppMan_TryMoveSelectionFromUserInput(u32 heldKeys, BoxApplicationManager* boxAppMan);
static CursorMovementState BoxApp_TryMoveSelection(BoxApplication* boxApp, int colChange, int rowChange);
static void BoxAppMan_MoveCursorToParty(BoxApplicationManager* boxAppMan);
static void BoxAppMan_ReturnCursorToBox(BoxApplicationManager* boxAppMan);
static void BoxAppMan_TryPreviewCursorMon(BoxApplicationManager* boxAppMan);
static BOOL BoxAppMan_TryPressTouchScreenButton(BoxApplicationManager* boxAppMan);
static void BoxAppMan_BoxTouchScreenButtonHandler(u32 buttonIndex, TouchScreenButtonState buttonTouchState, void* context);
static void BoxApp_SetBoxMessage(BoxApplication* boxApp, u32 boxMessageID);
static void BoxApp_SetCursorBoxLocation(BoxApplication* boxApp, u32 col, u32 row);
static void BoxAppMan_PickUpMon(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_PickUpMultiSelectedMons(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_ResetMultiSelectLocation(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_SetMultiSelectionEndLocation(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_PutDownCursorMon(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_PutDownSelectedMons(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static void BoxAppMan_SwapMonInCursor(BoxApplicationManager* boxAppMan, BoxApplication* boxApp);
static BOOL BoxAppMan_TryStoreCursorMonInBox(BoxApplicationManager* boxAppMan, u32 boxID);
static BOOL BoxAppMan_TryStoreSelectedMonInBox(BoxApplicationManager* boxAppMan, u32 boxID);
static void BoxAppMan_RemoveCursorMon(BoxApplicationManager* boxAppMan);
static void BoxAppMan_RemoveMonUnderCursor(BoxApplicationManager* boxAppMan);
static void BoxAppMan_AssertFail(BoxApplicationManager* boxAppMan);
static void BoxAppMan_RanchDepositMonUnderCursor(BoxApplicationManager* boxAppMan, u32 arg1);
static BOOL BoxAppMan_StoreRanchMonInBox(BoxApplicationManager* boxAppMan, u32 boxID, int arg2);
static void BoxAppMan_CommUpdateFocusedMon(BoxApplicationManager* boxAppMan);
static void BoxAppMan_CommUpdateMon(BoxApplicationManager* boxAppMan);
static void BoxApp_PreviewBoxMon(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan);
static void BoxApp_LoadBoxMonIntoPreview(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan);
static void BoxApp_LoadBoxMonIntoComparison(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan);
static void BoxApp_ToggleCompareMonSlot(BoxApplication* boxApp);
static void BoxApp_NextCompareMode(BoxApplication* boxApp);
static void BoxApp_SetCompareButtonPressed(BoxApplication* boxApp, BOOL pressed);
static void BoxApp_SetPreviewedBoxMon(BoxApplication* boxApp, BoxPokemon* boxMon);
static void BoxApp_UpdatePreviewMonMarkings(BoxApplication* boxApp);
static void BoxApp_GiveItemToSelectedMon(BoxApplication* boxApp, u16 item, BoxApplicationManager* boxAppMan);
static void BoxAppMan_LoadRightBoxCustomization(BoxApplicationManager* boxAppMan);
static void BoxAppMan_LoadLeftBoxCustomization(BoxApplicationManager* boxAppMan);
static void BoxApp_LoadCustomizationsFor(BoxApplication* boxApp, u32 boxID);
static void BoxApp_SetBoxSelectionBoxID(BoxApplication* boxApp, u32 boxID);
static void BoxApp_PickUpHeldItem(BoxApplication* boxApp, BoxApplicationManager* boxAppMan);
static void BoxApp_RemoveCursorItem(BoxApplication* boxApp);
static void BoxApp_GiveItemFromCursor(BoxApplication* boxApp, BoxApplicationManager* boxAppMan);
static void BoxApp_SwapMonAndCursorItems(BoxApplication* boxApp, BoxApplicationManager* boxAppMan);
static void BoxApp_SetTouchDialSelected(BoxApplication* boxApp, TouchDials touchDialID);
static void BoxApp_SetTouchDialOffset(BoxApplication* boxApp, u32 offset);
static void BoxApp_SetMarkingsButtonsScrollOffset(BoxApplication* boxApp, u32 offset);
static void BoxApp_SetTouchDialScrollDelta(BoxApplication* boxApp, int amount);
static void BoxApp_SetMonSpriteTransparencyMask(BoxApplication* boxApp, u32 mask);
static void BoxApp_ToggleCursorFastMode(BoxApplication* boxApp);
static u32 BoxApp_GetPreviewedMonValue(BoxApplication* boxApp, PokemonDataParam value, void* dest);
static u32 BoxApp_GetPreviewedOrSelectedMonValue(BoxApplication* boxApp, PokemonDataParam value, void* dest);
static BoxPokemon* BoxAppMan_GetBoxMonAt(BoxApplicationManager* boxAppMan, u32 index);
static void BoxAppMan_InitReleaseBlockingMoveCache(BoxApplicationManager* boxAppMan);
static void BoxAppMan_ReleaseBlockingMoveCacheTask(SysTask* task, void* param);
static BOOL BoxAppMan_IsReleaseBlockingMoveCacheTaskDone(BoxApplicationManager* boxAppMan);
static void BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(BoxApplicationManager* boxAppMan, int boxID);
static BOOL BoxAppMan_IsCursorMonDepositable(BoxApplicationManager* boxAppMan);
static int BoxAppMan_ReleaseBlockingMoveCacheUpdateAll(BoxApplicationManager* boxAppMan, int* outTotalMonsInBox);

static const TouchScreenHitTable sMainPcButtons[] = {
	[MAIN_PC_LEFT_BUTTON_ID]  = { TOUCHSCREEN_USE_CIRCLE, MAIN_PC_LEFT_BUTTON_X,  MAIN_PC_BUTTON_Y, MAIN_PC_BUTTON_RADIUS },
	[MAIN_PC_RIGHT_BUTTON_ID] = { TOUCHSCREEN_USE_CIRCLE, MAIN_PC_RIGHT_BUTTON_X, MAIN_PC_BUTTON_Y, MAIN_PC_BUTTON_RADIUS },
	{ TOUCHSCREEN_TABLE_TERMINATOR }
};

static const TouchScreenHitTable sComparePokemonButtons[] = {
	[MAIN_PC_LEFT_BUTTON_ID]   = { TOUCHSCREEN_USE_CIRCLE, MAIN_PC_LEFT_BUTTON_X,   MAIN_PC_BUTTON_Y,		MAIN_PC_BUTTON_RADIUS },
	[MAIN_PC_RIGHT_BUTTON_ID]  = { TOUCHSCREEN_USE_CIRCLE, MAIN_PC_RIGHT_BUTTON_X,  MAIN_PC_BUTTON_Y,		MAIN_PC_BUTTON_RADIUS },
	[COMPARE_MON_PC_BUTTON_ID] = { TOUCHSCREEN_USE_CIRCLE, COMPARE_MON_PC_BUTTON_X, COMPARE_MON_PC_BUTTON_Y, COMPARE_MON_PC_BUTTON_RADIUS },
	{ TOUCHSCREEN_TABLE_TERMINATOR }
};

static const TouchScreenHitTable sPokemonMarkingsButtons[] = {
	[PC_MARKINGS_BUTTON1_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON1_X, PC_MARKINGS_BUTTON1_Y, PC_MARKINGS_BUTTONS_RADIUS },
	[PC_MARKINGS_BUTTON2_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON2_X, PC_MARKINGS_BUTTON2_Y, PC_MARKINGS_BUTTONS_RADIUS },
	[PC_MARKINGS_BUTTON3_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON3_X, PC_MARKINGS_BUTTON3_Y, PC_MARKINGS_BUTTONS_RADIUS },
	[PC_MARKINGS_BUTTON4_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON4_X, PC_MARKINGS_BUTTON4_Y, PC_MARKINGS_BUTTONS_RADIUS },
	[PC_MARKINGS_BUTTON5_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON5_X, PC_MARKINGS_BUTTON5_Y, PC_MARKINGS_BUTTONS_RADIUS },
	[PC_MARKINGS_BUTTON6_ID] = { TOUCHSCREEN_USE_CIRCLE, PC_MARKINGS_BUTTON6_X, PC_MARKINGS_BUTTON6_Y, PC_MARKINGS_BUTTONS_RADIUS },
	{ TOUCHSCREEN_TABLE_TERMINATOR }
};

static const u16 sReleaseBlockingMoves[] = {
	MOVE_SURF,
	MOVE_ROCK_CLIMB,
	MOVE_WATERFALL
};


BOOL BoxAppMan_Init(ApplicationManager* appMan, int* state) {
	BoxApplicationManager* boxAppMan;
	
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_BOX_DATA, HEAP_SIZE_BOX_DATA);
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_BOX_GRAPHICS, HEAP_SIZE_BOX_GRAPHICS);
	
	boxAppMan = ApplicationManager_NewData(appMan, sizeof(BoxApplicationManager), HEAP_ID_BOX_DATA);
	if (boxAppMan != NULL) {
		BoxAppMan_Load(boxAppMan, ApplicationManager_GetArgs(appMan));
		BoxGraphics_Load(&boxAppMan->display, &boxAppMan->boxApp, boxAppMan);

		boxAppMan->cursorLocationHandlerState = 0;
		boxAppMan->cursorLocationInputHandler = BoxAppMan_GetCursorLocationInputHandler(boxAppMan);

		BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_LogInAction);
	}
	
	return TRUE;
}


BOOL BoxAppMan_Main(ApplicationManager* appMan, int* state) {
	BoxApplicationManager* boxAppMan = ApplicationManager_GetData(appMan);
	
	if (
		MPRComm_GetErrorFlag() &&
		boxAppMan->applicationManager == NULL &&
		(boxAppMan->display == NULL || BoxGraphics_CheckAllTasksDone(boxAppMan->display) != FALSE)
	) {
		GX_SetMasterBrightness(-16);
		GXS_SetMasterBrightness(-16);
		return TRUE;
	}
	
	if (boxAppMan->boxApplicationAction != NULL) {
		boxAppMan->boxApplicationAction(boxAppMan, &boxAppMan->boxApplicationActionState);
		if (boxAppMan->MPR_unk_1E0 == 0) {
			return FALSE;
		}
	}
	
	if (boxAppMan->cursorLocationInputHandler != NULL) {
		if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
			if (JOY_NEW(PAD_BUTTON_Y)) {
				if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == TRUE) {
					BoxApp_ToggleCursorFastMode(&boxAppMan->boxApp);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020302DC);
					return FALSE;
				}
			}
		}
		return boxAppMan->cursorLocationInputHandler(boxAppMan);
	}
	
	GF_ASSERT(FALSE);
	return TRUE;
}


BOOL BoxAppMan_Exit(ApplicationManager* appMan, int* state) {
	BoxApplicationManager* boxAppMan = ApplicationManager_GetData(appMan);
	
	BOOL freeGraphics;
	if (boxAppMan->display != NULL) {
		freeGraphics = TRUE;
	} else {
		freeGraphics = FALSE;
	}
	
	if (Party_HasSpecies(boxAppMan->party, SPECIES_CHATOT) == FALSE) {
		ChatotCry* chatotCry = SaveData_GetChatotCry(boxAppMan->saveData);
		ChatotCry_ResetStatus(chatotCry);
	}
	
	if (freeGraphics) {
		BoxGraphics_Free(boxAppMan->display);
	}
	
	BoxAppMan_Free(boxAppMan);
	Heap_Destroy(HEAP_ID_BOX_DATA);
	
	if (freeGraphics) {
		Heap_Destroy(HEAP_ID_BOX_GRAPHICS);
	}
	
	return TRUE;
}


static void BoxAppMan_RegisterCursorLocationInputHandler(BoxApplicationManager* boxAppMan, CursorLocationInputHandler cursorLocationInputHandler) {
	boxAppMan->cursorLocationInputHandler = cursorLocationInputHandler;
	boxAppMan->cursorLocationHandlerState = 0;
}


static void BoxAppMan_RegisterBoxApplicationAction(BoxApplicationManager* boxAppMan, BoxApplicationAction boxApplicationAction) {
	boxAppMan->boxApplicationAction = boxApplicationAction;
	boxAppMan->boxApplicationActionState = 0;
	boxAppMan->MPR_unk_1E0 = 0;
}


static void BoxAppMan_ClearBoxApplicationAction(BoxApplicationManager* boxAppMan) {
	boxAppMan->boxApplicationAction = NULL;
}


static void BoxAppMan_ResetAction(BoxApplicationManager* boxAppMan) {
	boxAppMan->boxApplicationAction = NULL;
	boxAppMan->MPR_unk_1E0 = 1;
}


static CursorLocationInputHandler BoxAppMan_GetCursorLocationInputHandler(BoxApplicationManager* boxAppMan) {
	switch (BoxApp_GetCursorLocation(&boxAppMan->boxApp)) {
		case CURSOR_IN_BOX:
		default:
			return BoxAppMan_CursorInBoxInputHandler;
		
		case CURSOR_IN_PARTY:
			return BoxAppMan_CursorInPartyInputHandler;
		
		case CURSOR_ON_BOX_HEADER:
			return BoxAppMan_CursorOnHeaderInputHandler;
		
		case CURSOR_ON_CLOSE_BUTTON:
			return BoxAppMan_CursorOnCloseInputHandler;
		
		case CURSOR_ON_PARTY_BUTTON:
			return BoxAppMan_CursorOnPartyButtonInputHandler;
	}
}


static void BoxAppMan_FlagRecordBoxUseInJournal(BoxApplicationManager* boxAppMan) {
	boxAppMan->pokemonStorageSession->recordBoxUseInJournal = TRUE;
}


void BoxAppMan_LogInAction(BoxApplicationManager* boxAppMan, u32* state) {
	switch (*state) {
		case 0:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW && MPRComm_IsConnected()) {
				if (MPRComm_0203C404()) {
					MPRComm_0203C3A4(1, 0, (BoxPokemon*)boxAppMan->mprFilter.unk_80);
					(*state)++;
				}
			} else if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT && MPRComm_IsConnected()) {
				u32 currBox = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				PCBoxes* boxes = BoxApp_GetPCBoxes(&boxAppMan->boxApp);
				BoxPokemon* boxMon = PCBoxes_GetBoxMonAt(boxes, currBox, 0);
				MPRComm_ShowPCBox(boxMon, currBox);
				BoxAppMan_InitReleaseBlockingMoveCache(boxAppMan);
				*state = 2;
			} else {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F60C);
				*state = 3;
			}
			break;
		
		case 1:
			if (MPRComm_0203C404()) {
				int ranchBoxID = 0;
				int ranchMonID = MPRComm_0203C33C();
				if (ranchMonID > 0) {
					ranchBoxID = ((ranchMonID + 29) / 30) - 1;
				}
				
				boxAppMan->boxApp.customization.maxRanchBoxID = ranchBoxID;
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F60C);
				MPRComm_0203C3A4(ranchBoxID, 0, (BoxPokemon*)boxAppMan->mprFilter.unk_88);
				*state = 3;
			}
			break;
		
		case 2:
			if (MPRComm_IsAwaitingShowPCBox()) {
				 BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F60C);
				*state = 3;
			}
			break;
		
		case 3:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202F60C)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack0);
				(*state)++;
			}
			break;
		
		case 4:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW && MPRComm_IsConnected() && !MPRComm_0203C404()) {
				break;
			}
			
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT && !BoxAppMan_IsReleaseBlockingMoveCacheTaskDone(boxAppMan)) {
				break;
			}
			
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack0)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_ReturnToBoxFade1Action(BoxApplicationManager* boxAppMan, u32* state) {
	switch (*state) {
		case 0:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F60C);
			(*state)++;
			break;
		
		case 1:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202F60C)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack1);
				(*state)++;
			}
			break;
		
		case 2:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack1)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static int BoxAppMan_SortIndexToSortOrder(int index) {
	return index + 1;
}


static BOOL BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(BoxApplicationManager* boxAppMan) {
	if (BoxAppMan_TryPressTouchScreenButton(boxAppMan) != 0) {
		if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) {
			BOOL applyNewSettings = FALSE;
			
			switch (boxAppMan->mprFilter.unk_00) {
				case 0:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 1;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 1:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 2;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 2:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 4;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 3:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 5;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 4:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 7;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 5:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 9;
							boxAppMan->mprFilter.activePage = 0;
							break;
						
						case 6:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							boxAppMan->mprFilter.unk_00 = 10;
							boxAppMan->mprFilter.activePage = 0;
							break;
					}
					
					boxAppMan->mprFilter.unk_4C = TRUE;
					TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
					boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
					break;
				
				case 1:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								int buttonIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (buttonIndex < 14) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.currSettings.unk_00 = buttonIndex;
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 2) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 2:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								u32 speciesButtonIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (speciesButtonIndex <= 9 && BoxMPR_0203A1EC(gSpeciesNameBuckets, 0x1ED, speciesButtonIndex)) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.unk_00 = 3;
									boxAppMan->mprFilter.activeBucket = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
									boxAppMan->mprFilter.activePage = 0;
									boxAppMan->mprFilter.unk_4C = TRUE;
									TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
									boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 1) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.species >= 0) {
								boxAppMan->mprFilter.currSettings.species = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 3:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								NarcID narc;
								if (gIsDiamondPearl) {
									narc = NARC_INDEX_DP_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
								} else {
									narc = NARC_INDEX_PL_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
								}
								int narcOutSize;
								u16* speciesInAlphabeticalOrder = LoadMemberFromNARC_OutFileSize(narc, 13, 0, HEAP_ID_BOX_GRAPHICS, 0, &narcOutSize);
								int speciesIndex = BoxMPR_AdvanceSpeciesIndex(
									gSpeciesNameBuckets[boxAppMan->mprFilter.activeBucket],
									boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage),
									speciesInAlphabeticalOrder);
								
								Heap_Free(speciesInAlphabeticalOrder);
								
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxSpeciesIndex;
								if (bucket == 9) {
									maxSpeciesIndex = NATIONAL_DEX_COUNT;
								} else {
									bucket++;
									maxSpeciesIndex = gSpeciesNameBuckets[bucket];
								}
								
								if (speciesIndex < maxSpeciesIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									
									// BUG: use after free
									boxAppMan->mprFilter.currSettings.species = speciesInAlphabeticalOrder[speciesIndex];
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							{
								NarcID narc;
								if (gIsDiamondPearl) {
									narc = NARC_INDEX_DP_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
								} else {
									narc = NARC_INDEX_PL_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
								}
								int narcOutSize;
								u16* speciesInAlphabeticalOrder = LoadMemberFromNARC_OutFileSize(narc, 13, 0, HEAP_ID_BOX_GRAPHICS, 0, &narcOutSize);
								int speciesIndex = BoxMPR_AdvanceSpeciesIndex(
									gSpeciesNameBuckets[boxAppMan->mprFilter.activeBucket],
									6 * (boxAppMan->mprFilter.activePage + 1),
									speciesInAlphabeticalOrder);
								
								Heap_Free(speciesInAlphabeticalOrder);
								
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxSpeciesIndex;
								if (bucket == 9) {
									maxSpeciesIndex = NATIONAL_DEX_COUNT;
								} else {
									bucket++;
									maxSpeciesIndex = gSpeciesNameBuckets[bucket];
								}
								
								if (speciesIndex < maxSpeciesIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.activePage++;
									boxAppMan->mprFilter.unk_4C = TRUE;
								}
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.species >= 0) {
								boxAppMan->mprFilter.currSettings.species = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 4:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								int typeIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (typeIndex < TYPE_MAX - 1) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.currSettings.type = gTypeNamesInAlphabeticalOrder[typeIndex];
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 2) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.type >= 0) {
								boxAppMan->mprFilter.currSettings.type = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 5:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								u32 moveButtonIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (moveButtonIndex <= 9 && BoxMPR_0203A1EC(gMoveNameBuckets, 0x1D2, moveButtonIndex) != 0) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.unk_00 = 6;
									boxAppMan->mprFilter.activeBucket = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
									boxAppMan->mprFilter.activePage = 0;
									boxAppMan->mprFilter.unk_4C = TRUE;
									TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
									boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 1) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.move >= 0) {
								boxAppMan->mprFilter.currSettings.move = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 6:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								int moveIndex = BoxMPR_AdvanceMoveIndex(
									gMoveNameBuckets[boxAppMan->mprFilter.activeBucket],
									boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage));
								
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxMoveIndex;
								if (bucket == 9) {
									maxMoveIndex = VALID_MOVE_COUNT - 1;
								} else {
									bucket++;
									maxMoveIndex = gMoveNameBuckets[bucket];
								}
								
								if (moveIndex < maxMoveIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.currSettings.move = gMoveNamesInAlphabeticalOrder[moveIndex];
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							{
								int moveIndex = BoxMPR_AdvanceMoveIndex(gMoveNameBuckets[boxAppMan->mprFilter.activeBucket], 6 * (boxAppMan->mprFilter.activePage + 1));
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxMoveIndex;
								if (bucket == 9) {
									maxMoveIndex = VALID_MOVE_COUNT - 1;
								} else {
									bucket++;
									maxMoveIndex = gMoveNameBuckets[bucket];
								}
								
								if (moveIndex < maxMoveIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.activePage++;
									boxAppMan->mprFilter.unk_4C = TRUE;
								}
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.move >= 0) {
								boxAppMan->mprFilter.currSettings.move = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 7:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								u32 abilityButtonIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (abilityButtonIndex <= 9 && BoxMPR_0203A1EC(gAbilityNameBuckets, ABILITY_COUNT, abilityButtonIndex)) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.unk_00 = 8;
									boxAppMan->mprFilter.activeBucket = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
									boxAppMan->mprFilter.activePage = 0;
									boxAppMan->mprFilter.unk_4C = TRUE;
									TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
									boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 1) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.ability >= 0) {
								boxAppMan->mprFilter.currSettings.ability = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 8:
					switch(boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								int abilityIndex = BoxMPR_AdvanceAbilityIndex(
									gAbilityNameBuckets[boxAppMan->mprFilter.activeBucket],
									boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage));
								
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxAbilityIndex;
								if (bucket == 9) {
									maxAbilityIndex = ABILITY_COUNT;
								} else {
									bucket++;
									maxAbilityIndex = gAbilityNameBuckets[bucket];
								}
								
								if (abilityIndex < maxAbilityIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.currSettings.ability = gAbilityNamesInAlphabeticalOrder[abilityIndex];
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							{
								int abilityIndex = BoxMPR_AdvanceAbilityIndex(gAbilityNameBuckets[boxAppMan->mprFilter.activeBucket], 6 * (boxAppMan->mprFilter.activePage + 1));
								int bucket = boxAppMan->mprFilter.activeBucket;
								int maxAbilityIndex;
								if (bucket == 9) {
									maxAbilityIndex = ABILITY_COUNT;
								} else {
									bucket++;
									maxAbilityIndex = gAbilityNameBuckets[bucket];
								}
								
								if (abilityIndex < maxAbilityIndex) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.activePage++;
									boxAppMan->mprFilter.unk_4C = TRUE;
								}
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.ability >= 0) {
								boxAppMan->mprFilter.currSettings.ability = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
							
					}
					break;
				
				case 9:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								int natureButtonIndex = boxAppMan->touchScreenButtonPressed + (6 * boxAppMan->mprFilter.activePage);
								if (natureButtonIndex < NATURE_MAX) {
									Sound_PlayEffect(SEQ_SE_DP_DECIDE);
									boxAppMan->mprFilter.currSettings.nature = gNatureNamesInAlphabeticalOrder[natureButtonIndex];
								}
							}
							break;
						
						case 6:
							if (boxAppMan->mprFilter.activePage > 0) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage--;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 7:
							if (boxAppMan->mprFilter.activePage < 4) {
								Sound_PlayEffect(SEQ_SE_DP_DECIDE);
								boxAppMan->mprFilter.activePage++;
								boxAppMan->mprFilter.unk_4C = TRUE;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.nature >= 0) {
								boxAppMan->mprFilter.currSettings.nature = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
				
				case 10:
					switch (boxAppMan->touchScreenButtonPressed) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.mark < 0) {
								boxAppMan->mprFilter.currSettings.mark = 0;
							}
							
							boxAppMan->mprFilter.currSettings.mark = (1 << boxAppMan->touchScreenButtonPressed);
							
							if (boxAppMan->mprFilter.currSettings.mark == 0) {
								boxAppMan->mprFilter.currSettings.mark = -1;
							}
							break;
						
						case 8:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							if (boxAppMan->mprFilter.currSettings.mark >= 0) {
								boxAppMan->mprFilter.currSettings.mark = -1;
							}
							break;
						
						case 9:
							Sound_PlayEffect(SEQ_SE_DP_DECIDE);
							TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
							boxAppMan->mainBoxAndCompareButtonsAction = NULL;
							applyNewSettings = TRUE;
							break;
					}
					break;
			}
			
			if (applyNewSettings) {
				if (memcmp(&boxAppMan->mprFilter.appliedSettings, &boxAppMan->mprFilter.currSettings, sizeof(MPRFilterSettings)) != 0) {
					int sortOrder = BoxAppMan_SortIndexToSortOrder(boxAppMan->mprFilter.currSettings.unk_00);
					BoxApp_LoadCustomizationsFor(&boxAppMan->boxApp, 0);
					PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, 0);
					BoxGraphics_02030AAC(boxAppMan->display, 0);
					
					if (sortOrder >= 0 && MPRComm_IsConnected()) {
						MPRComm_SetSortAndFilterParams(
							sortOrder,
							boxAppMan->mprFilter.currSettings.species,
							boxAppMan->mprFilter.currSettings.type,
							boxAppMan->mprFilter.currSettings.move,
							boxAppMan->mprFilter.currSettings.ability,
							boxAppMan->mprFilter.currSettings.nature,
							boxAppMan->mprFilter.currSettings.mark);
						
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_DownloadRanchBoxesAction);
					}
					
					boxAppMan->mprFilter.appliedSettings = boxAppMan->mprFilter.currSettings;
					
				} else {
					boxAppMan->mprFilter.unk_00 = 0;
					boxAppMan->mprFilter.unk_4C = TRUE;
					boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
				}
			}
		
		} else if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_COMPARE) {
			switch (boxAppMan->touchScreenButtonPressed) {
				case 0:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_TouchScreenBoxJump);
					break;
				
				case 1:
					if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_MOVE_ITEMS) {
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MarkingsFilterAction);
					} else {
						Sound_PlayEffect(SEQ_SE_DP_BOX03);
					}
					break;
			}
		} else {
			switch (boxAppMan->touchScreenButtonPressed) {
				case 0:
					if ((BoxApp_GetCompareMonSlot(&boxAppMan->boxApp) == 0) && (BoxApp_IsMonUnderCursor(&boxAppMan->boxApp) == TRUE)) {
						Sound_PlayEffect(SEQ_SE_DP_DECIDE);
						BoxApp_ToggleCompareMonSlot(&boxAppMan->boxApp);
						BoxApp_PreviewBoxMon(&boxAppMan->boxApp, (BoxPokemon*)boxAppMan->boxApp.cursor.mon, boxAppMan);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020303B0);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
						return TRUE;
					}
					
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					break;
				
				case 1:
					if ((BoxApp_GetCompareMonSlot(&boxAppMan->boxApp) == 1) && (BoxApp_IsMonUnderCursor(&boxAppMan->boxApp) == TRUE)) {
						Sound_PlayEffect(SEQ_SE_DP_DECIDE);
						BoxApp_ToggleCompareMonSlot(&boxAppMan->boxApp);
						BoxApp_PreviewBoxMon(&boxAppMan->boxApp, (BoxPokemon*)boxAppMan->boxApp.cursor.mon, boxAppMan);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020303B0);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
						return TRUE;
					}
					
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					break;
				
				case 2:
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxApp_NextCompareMode(&boxAppMan->boxApp);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020303CC);
					return TRUE;
			}
		}
	}
	
	return FALSE;
}


static int BoxAppMan_CursorInBoxInputHandler(BoxApplicationManager* boxAppMan) {
	enum {
		CURSOR_IN_BOX_WAIT_FOR_INPUT,
		CURSOR_IN_BOX_WAIT_FOR_MOVE_CURSOR,
		CURSOR_IN_BOX_WAIT_FOR_TOUCHSCREEN_DONE
	};
	
	switch (boxAppMan->cursorLocationHandlerState) {
		case CURSOR_IN_BOX_WAIT_FOR_INPUT:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
				if (JOY_NEW(PAD_BUTTON_A)) {
					if (BoxApp_IsMonAvailableToCursor(&boxAppMan->boxApp)) {
						if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_MOVE_ITEMS) {
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MonCursorMenuAction);
						} else {
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MonItemMenuAction);
						}
						break;
					}
				}
				
				if (JOY_NEW(PAD_BUTTON_B)) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ContinueBoxOperationsAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_L)) {
					BoxAppMan_LoadLeftBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_R)) {
					BoxAppMan_LoadRightBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (BoxAppMan_TryMoveCursorFromUserInput(gSystem.heldKeys, boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					}
					
					boxAppMan->cursorLocationHandlerState++;
					break;
				}
			}
			
			if (BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(boxAppMan)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_BOX_WAIT_FOR_TOUCHSCREEN_DONE;
			}
			
			break;
			
		case CURSOR_IN_BOX_WAIT_FOR_MOVE_CURSOR:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_GetCursorLocationInputHandler(boxAppMan));
			}
			break;
		
		case CURSOR_IN_BOX_WAIT_FOR_TOUCHSCREEN_DONE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_BOX_WAIT_FOR_INPUT;
			}
			break;
	}
	
	return 0;
}


static BOOL BoxAppMan_IsPreviewedMonHoldingMailOrHasBallCapsule(BoxApplicationManager* boxAppMan, int* destMessageID) {
	if (Item_IsMail(BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp))) {
		*destMessageID = BOX_MESSAGE_RemoveMail;
		return TRUE;
	}
	
	if (BoxApp_GetPreviewedMonValue(&boxAppMan->boxApp, MON_DATA_BALL_CAPSULE_ID, NULL)) {
		*destMessageID = BOX_MESSAGE_DetachBallCapsule;
		return TRUE;
	}
	
	return FALSE;
}


static int BoxAppMan_CursorInPartyInputHandler(BoxApplicationManager* boxAppMan) {
	enum {
		CURSOR_IN_PARTY_WAIT_FOR_INPUT,
		CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR,
		CURSOR_IN_PARTY_LEAVE_PARTY,
		CURSOR_IN_PARTY_RETURN_TO_BOX,
		CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR_TO_BOX,
		CURSOR_IN_PARTY_CONFIRM_MESSAGE,
		CURSOR_IN_PARTY_WAIT_FOR_TOUCHSCREEN_DONE
	};
	
	switch (boxAppMan->cursorLocationHandlerState) {
		case CURSOR_IN_PARTY_WAIT_FOR_INPUT:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
				if (JOY_NEW(PAD_BUTTON_A)) {
					if (BoxApp_GetCursorPartyPosition(&boxAppMan->boxApp) == MAX_PARTY_SIZE) {
						boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_LEAVE_PARTY;
						break;
					}
					
					if (BoxApp_IsMonAvailableToCursor(&boxAppMan->boxApp)) {
						if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_MOVE_ITEMS) {
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MonCursorMenuAction);
						} else {
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MonItemMenuAction);
						}
						break;
					}
					break;
				}
				
				if (
					JOY_NEW(PAD_BUTTON_B) || JOY_NEW(PAD_KEY_RIGHT) && 
					BoxApp_GetCursorPartyPosition(&boxAppMan->boxApp) & 1 || JOY_NEW(PAD_KEY_RIGHT) && 
					BoxApp_GetCursorPartyPosition(&boxAppMan->boxApp) == MAX_PARTY_SIZE
				) {
					boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_LEAVE_PARTY;
					break;
				}
				
				if (BoxAppMan_TryMoveCursorFromUserInput(gSystem.heldKeys, boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					}
					
					boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR;
					break;
				}
			}
			
			if (BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(boxAppMan)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_TOUCHSCREEN_DONE;
				break;
			}
			break;
		
		case CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_INPUT;
			}
			break;
		
		case CURSOR_IN_PARTY_LEAVE_PARTY:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_DEPOSIT) {
				BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ContinueBoxOperationsAction);
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_INPUT;
			} else {
				int messageID;
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) != PREVIEW_MON_UNDER_CURSOR && BoxAppMan_IsPreviewedMonHoldingMailOrHasBallCapsule(boxAppMan, &messageID)) {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_CONFIRM_MESSAGE;
				} else {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020300D0);
					boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_RETURN_TO_BOX;
				}
			}
			break;
		
		case CURSOR_IN_PARTY_RETURN_TO_BOX:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_020300D0)) {
				BoxAppMan_ReturnCursorToBox(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				}
				
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR_TO_BOX;
			}
			break;
		
		case CURSOR_IN_PARTY_WAIT_FOR_MOVE_CURSOR_TO_BOX:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_GetCursorLocationInputHandler(boxAppMan));
			}
			break;
		
		case CURSOR_IN_PARTY_CONFIRM_MESSAGE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_INPUT;
			}
			break;
		
		case CURSOR_IN_PARTY_WAIT_FOR_TOUCHSCREEN_DONE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_IN_PARTY_WAIT_FOR_INPUT;
			}
			break;
	}
	
	return 0;
}


static int BoxAppMan_CursorOnHeaderInputHandler(BoxApplicationManager* boxAppMan) {
	enum {
		CURSOR_ON_HEADER_WAIT_FOR_INPUT,
		CURSOR_ON_HEADER_WAIT_FOR_MOVE_CURSOR,
        CURSOR_ON_HEADER_WAIT_FOR_EXIT
	};
	
	switch (boxAppMan->cursorLocationHandlerState) {
		case CURSOR_ON_HEADER_WAIT_FOR_INPUT:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
				if (JOY_HELD(PAD_KEY_LEFT | PAD_BUTTON_L)) {
					BoxAppMan_LoadLeftBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (JOY_HELD(PAD_KEY_RIGHT | PAD_BUTTON_R)) {
					BoxAppMan_LoadRightBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_A)) {
                    (void)BoxApp_GetBoxMode(&boxAppMan->boxApp); /* Unused */
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_BoxHeaderMenuAction);
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_B)) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ContinueBoxOperationsAction);
					break;
				}
				
				if (BoxAppMan_TryMoveCursorFromUserInput(gSystem.heldKeys, boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					}
					
					boxAppMan->cursorLocationHandlerState = CURSOR_ON_HEADER_WAIT_FOR_MOVE_CURSOR;
					break;
				}
			}
			
			BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(boxAppMan);
			break;
		
		case CURSOR_ON_HEADER_WAIT_FOR_MOVE_CURSOR:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_GetCursorLocationInputHandler(boxAppMan));
			}
			break;
		
		case CURSOR_ON_HEADER_WAIT_FOR_EXIT:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				boxAppMan->cursorLocationHandlerState = 0;
			}
			break;
	}
	
	return 0;
}


static int BoxAppMan_CursorOnCloseInputHandler(BoxApplicationManager* boxAppMan) {
	enum {
		CURSOR_CLOSE_WAIT_FOR_INPUT,
		CURSOR_CLOSE_WAIT_FOR_BOX_CHANGE,
		CURSOR_CLOSE_WAIT_FOR_CURSOR_MOVE
	};
	
	switch (boxAppMan->cursorLocationHandlerState) {
		case CURSOR_CLOSE_WAIT_FOR_INPUT:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
				
				// Be double sure in case the above if statement didn't work
				if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW && boxAppMan->mprFilter.unk_00 != 0) {
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_A)) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_CloseBoxAction);
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_B)) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ContinueBoxOperationsAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_L)) {
					BoxAppMan_LoadLeftBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_R)) {
					BoxAppMan_LoadRightBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (BoxAppMan_TryMoveCursorFromUserInput(gSystem.heldKeys, boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					}
					
					boxAppMan->cursorLocationHandlerState = CURSOR_CLOSE_WAIT_FOR_CURSOR_MOVE;
					break;
				}
			}
			
			BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(boxAppMan);
			break;
		
		case CURSOR_CLOSE_WAIT_FOR_BOX_CHANGE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_CLOSE_WAIT_FOR_INPUT;
			}
			break;
		
		case CURSOR_CLOSE_WAIT_FOR_CURSOR_MOVE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_GetCursorLocationInputHandler(boxAppMan));
			}
			break;
	}
	
	return 0;
}


static int BoxAppMan_CursorOnPartyButtonInputHandler(BoxApplicationManager* boxAppMan) {
	enum {
		CURSOR_ON_PARTY_BUTTON_STATE_WAIT_FOR_INPUT,
		CURSOR_ON_PARTY_BUTTON_ANIMATE_PRESS_BUTTON,
		CURSOR_ON_PARTY_BUTTON_WAIT_FOR_OPEN_PARTY,
		CURSOR_ON_PARTY_BUTTON_WAIT_FOR_CHANGE_BOX,
		CURSOR_ON_PARTY_BUTTON_WAIT_FOR_MOVE_CURSOR,
		CURSOR_ON_PARTY_BUTTON_CONFIRM_MESSAGE
	};
	
	switch (boxAppMan->cursorLocationHandlerState) {
		case CURSOR_ON_PARTY_BUTTON_STATE_WAIT_FOR_INPUT:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW || boxAppMan->mprFilter.unk_00 == 0) {
				if (JOY_NEW(PAD_BUTTON_A)) {
					switch (BoxApp_GetBoxMode(&boxAppMan->boxApp)) {
						case PC_MODE_RANCH_DEPOSIT:
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RanchBatchDepositAction);
							break;
						
						case PC_MODE_RANCH_WITHDRAW:
							BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RanchBatchWithdrawAction);
							break;
						
						default:
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton);
							boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_ANIMATE_PRESS_BUTTON;
							break;
						
						case PC_MODE_WITHDRAW:
							Sound_PlayEffect(SEQ_SE_DP_BOX03);
							BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_PickOne);
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
							boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_CONFIRM_MESSAGE;
							break;
					}
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_B)) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ContinueBoxOperationsAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_L)) {
					BoxAppMan_LoadLeftBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (JOY_HELD(PAD_BUTTON_R)) {
					BoxAppMan_LoadRightBoxCustomization(boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ChangeToNewBoxAction);
					break;
				}
				
				if (BoxAppMan_TryMoveCursorFromUserInput(gSystem.heldKeys, boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					}
					boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_WAIT_FOR_MOVE_CURSOR;
					break;
				}
			}
			
			BoxAppMan_RegisterBoxTouchActionOrTryTouchCompareButton(boxAppMan);
			break;
		
		case CURSOR_ON_PARTY_BUTTON_ANIMATE_PRESS_BUTTON:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenPartyPopup);
				boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_WAIT_FOR_OPEN_PARTY;
			}
			break;
		
		case CURSOR_ON_PARTY_BUTTON_WAIT_FOR_OPEN_PARTY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_OpenPartyPopup)) {
				BoxAppMan_MoveCursorToParty(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				}
				boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_WAIT_FOR_MOVE_CURSOR;
			}
			break;
		
		case CURSOR_ON_PARTY_BUTTON_WAIT_FOR_CHANGE_BOX:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox)) {
				boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_STATE_WAIT_FOR_INPUT;
			}
			break;
		
		case CURSOR_ON_PARTY_BUTTON_WAIT_FOR_MOVE_CURSOR:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_GetCursorLocationInputHandler(boxAppMan));
			}
			break;
		
		case CURSOR_ON_PARTY_BUTTON_CONFIRM_MESSAGE:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				boxAppMan->cursorLocationHandlerState = CURSOR_ON_PARTY_BUTTON_STATE_WAIT_FOR_INPUT;
			}
			break;
	}
	
	return 0;
}


static void BoxAppMan_InitSummary(BoxApplicationManager* boxAppMan) {
	static const u8 summaryPages[] = {
		SUMMARY_PAGE_INFO,
		SUMMARY_PAGE_MEMO,
		SUMMARY_PAGE_SKILLS,
		SUMMARY_PAGE_CONDITION,
		SUMMARY_PAGE_BATTLE_MOVES,
		SUMMARY_PAGE_CONTEST_MOVES,
		SUMMARY_PAGE_RIBBONS,
		SUMMARY_PAGE_EXIT,
		SUMMARY_PAGE_MAX
	};
	
	if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
		boxAppMan->monSummary.monData = boxAppMan->boxApp.selection.boxMon;
		boxAppMan->monSummary.dataType = boxAppMan->boxApp.selection.cursorMonIsPartyMon ? 0 : 2;
		boxAppMan->monSummary.monMax = 1;
		boxAppMan->monSummary.monIndex = 0;
		boxAppMan->monSummary.mode = SUMMARY_MODE_NORMAL;
		boxAppMan->monSummary.move = MOVE_NONE;
		boxAppMan->monSummary.options = boxAppMan->options;
	
	} else if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) {
		boxAppMan->monSummary.monData = BoxAppMan_GetBoxMonAt(boxAppMan, 0);
		boxAppMan->monSummary.dataType = SUMMARY_DATA_BOX_MON;
		boxAppMan->monSummary.monMax = MAX_MONS_PER_BOX;
		boxAppMan->monSummary.monIndex = BoxApp_GetCursorBoxPosition(&boxAppMan->boxApp);
		boxAppMan->monSummary.mode = SUMMARY_MODE_NORMAL;
		boxAppMan->monSummary.move = MOVE_NONE;
	
	} else {
		boxAppMan->monSummary.monData = boxAppMan->party;
		boxAppMan->monSummary.dataType = SUMMARY_DATA_PARTY_MON;
		boxAppMan->monSummary.monMax = Party_GetCurrentCount(boxAppMan->party);
		boxAppMan->monSummary.monIndex = BoxApp_GetCursorPartyPosition(&boxAppMan->boxApp);
		boxAppMan->monSummary.mode = SUMMARY_MODE_NORMAL;
		boxAppMan->monSummary.move = MOVE_NONE;
	}
	
	boxAppMan->monSummary.chatotCry = NULL;
	boxAppMan->monSummary.dexMode = SaveData_GetDexMode(boxAppMan->saveData);
	boxAppMan->monSummary.showContest = PokemonSummaryScreen_ShouldShowContestData(boxAppMan->saveData);
	boxAppMan->monSummary.specialRibbons = SaveData_GetSpecialRibbons(boxAppMan->saveData);
	boxAppMan->monSummary.movesSwapped = FALSE;
	boxAppMan->monSummary.isRanchMon = (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW);
	boxAppMan->monSummary.mode = SUMMARY_MODE_LOCK_MOVES;
	boxAppMan->monSummary.boxIndex = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
	
	PokemonSummaryScreen_FlagVisiblePages(&boxAppMan->monSummary, summaryPages);
	PokemonSummaryScreen_SetPlayerProfile(&boxAppMan->monSummary, SaveData_GetTrainerInfo(boxAppMan->saveData));
}


static int BoxAppMan_LogOffScreenFade(BoxApplicationManager* boxAppMan) {
	switch (boxAppMan->cursorLocationHandlerState) {
		case 0:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlackLogOff);
			boxAppMan->cursorLocationHandlerState++;
			break;
		
		case 1:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				MPRComm_0203C1B4();
				return 1;
			}
	}
	
	return 0;
}


static void BoxAppMan_CloseBoxAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		CLOSE_BOX_START,
		CLOSE_BOX_SHOW_MENU,
		CLOSE_BOX_YES_NO,
		CLOSE_BOX_UNUSED,
		CLOSE_BOX_CANNOT_CLOSE,
		CLOSE_BOX_CONFIRM_MESSAGE,
		CLOSE_BOX_END
	};
	
	switch (*state) {
		case CLOSE_BOX_START:
			if ((BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_MOVE_ITEMS) && (BoxApp_GetCursorItem(&boxAppMan->boxApp) != 0)) {
				BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PutAwayItemAction);
				return;
			}
			
			if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) != PREVIEW_MON_UNDER_CURSOR) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_HoldingMon);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = CLOSE_BOX_CANNOT_CLOSE;
			} else {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton);
				*state = CLOSE_BOX_SHOW_MENU;
			}
			break;
		
		case CLOSE_BOX_SHOW_MENU:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				
				int messageID;
				switch (BoxApp_GetBoxMode(&boxAppMan->boxApp)) {
					case PC_MODE_RANCH_DEPOSIT:
						messageID = MPR_TEXT_FLAG | MPR_TEXT_PCDepositQuitConfirm;
						break;
					
					case PC_MODE_RANCH_WITHDRAW:
						messageID = MPR_TEXT_FLAG | MPR_TEXT_PCWithdrawQuitConfirm;
						break;
					
					default:
						messageID = BOX_MESSAGE_ConfirmExit;
						break;
				}
				
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
				BoxMenu_FillYesNo(&boxAppMan->boxApp, 1);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				
				*state = CLOSE_BOX_YES_NO;
			}
			break;
		
		case CLOSE_BOX_YES_NO:				   
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = CLOSE_BOX_END;
					break;
				
				case BOX_MENU_YES:
					BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_LogOffScreenFade);
					BoxAppMan_ClearBoxApplicationAction(boxAppMan);
					break;
			}
			break;
		
		case CLOSE_BOX_CANNOT_CLOSE:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = CLOSE_BOX_CONFIRM_MESSAGE;
			// Fall through
		
		case CLOSE_BOX_CONFIRM_MESSAGE:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = CLOSE_BOX_END;
			}
			break;
		
		case CLOSE_BOX_END:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_ContinueBoxOperationsAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		CONTINUE_OPERATIONS_START,
		CONTINUE_OPERATIONS_UNUSED,
		CONTINUE_OPERATIONS_WAIT_FOR_MESSAGE,
		CONTINUE_OPERATIONS_CONFIRM_MESSAGE,
		CONTINUE_OPERATIONS_END,
		CONTINUE_OPERATIONS_YES_NO
	};
	
	switch (*state) {
		case CONTINUE_OPERATIONS_START:
			if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) != PREVIEW_MON_UNDER_CURSOR) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_HoldingMon);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = CONTINUE_OPERATIONS_WAIT_FOR_MESSAGE;
				break;
			}
			
			if ((BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_MOVE_ITEMS) && (BoxApp_GetCursorItem(&boxAppMan->boxApp) != 0)) {
				BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PutAwayItemAction);
				break;
			}
			
			Sound_PlayEffect(SEQ_SE_DP_DECIDE);
			
			int messageID;
			switch (BoxApp_GetBoxMode(&boxAppMan->boxApp)) {
				case PC_MODE_RANCH_DEPOSIT:
					messageID = MPR_TEXT_FLAG | MPR_TEXT_PCDepositContinueConfirm;
					break;
				
				case PC_MODE_RANCH_WITHDRAW:
					messageID = MPR_TEXT_FLAG | MPR_TEXT_PCWithdrawContinueConfirm;
					break;
				
				default:
					messageID = BOX_MESSAGE_ConfirmContinue;
					break;
			}
			
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
			BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
			*state = CONTINUE_OPERATIONS_YES_NO;
			break;
		
		case CONTINUE_OPERATIONS_YES_NO:
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					BoxAppMan_RegisterCursorLocationInputHandler(boxAppMan, BoxAppMan_LogOffScreenFade);
					BoxAppMan_ClearBoxApplicationAction(boxAppMan);
					break;
				
				case BOX_MENU_YES:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = CONTINUE_OPERATIONS_END;
					break;
			}
			break;
		
		case CONTINUE_OPERATIONS_WAIT_FOR_MESSAGE:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = CONTINUE_OPERATIONS_CONFIRM_MESSAGE;
			// Fall through
		
		case CONTINUE_OPERATIONS_CONFIRM_MESSAGE:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = CONTINUE_OPERATIONS_END;
			}
			break;
		
		case CONTINUE_OPERATIONS_END:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_MonCursorMenuAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		MON_CURSOR_MENU_START,
		MON_CURSOR_MENU_SHOW_MENU,
		MON_CURSOR_MENU_NAVIGATE_MENU,
		MON_CURSOR_MENU_ITEM_SELECTED,
		MON_CURSOR_MENU_END_WHEN_READY,
		MON_CURSOR_MENU_END
	};
	
	switch (*state) {
		case MON_CURSOR_MENU_START:
			StringTemplate_SetNickname(boxAppMan->messageVariableBuffer, 0, BoxApp_GetPreviewedBoxMon(&boxAppMan->boxApp));
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonSelected);
			BoxMenu_FillTopLevelMenuItems(&boxAppMan->boxApp);
			if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
				boxAppMan->menuItem = BoxMenu_GetDefaultMenuItem(&boxAppMan->boxApp);
				*state = MON_CURSOR_MENU_ITEM_SELECTED;
				break;
			}
			
			Sound_PlayEffect(SEQ_SE_DP_DECIDE);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
			*state = MON_CURSOR_MENU_SHOW_MENU;
			break;
		
		case MON_CURSOR_MENU_SHOW_MENU:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = MON_CURSOR_MENU_NAVIGATE_MENU;
			// Fall through
		
		case MON_CURSOR_MENU_NAVIGATE_MENU:
			boxAppMan->menuItem = BoxMenu_GetMenuNavigation(&boxAppMan->boxApp);
			switch (boxAppMan->menuItem) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_CANCEL:
				case BOX_MENU_NAVIGATION_B:
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = MON_CURSOR_MENU_END_WHEN_READY;
					break;
				
				case BOX_MENU_NAVIGATION_NONE:
					break;
				
				default:
					*state = MON_CURSOR_MENU_ITEM_SELECTED;
					break;
			}
			break;
		
		case MON_CURSOR_MENU_ITEM_SELECTED:
			switch (boxAppMan->menuItem) {
				case BOX_MENU_MOVE:
					if ((BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) && (BoxApp_IsCursorFastMode(&boxAppMan->boxApp) == TRUE)) {
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MultiSelectAction);
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PickUpMonAction);
					}
					break;
				
				case BOX_MENU_PLACE:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PlaceMonAction);
					break;
				
				case BOX_MENU_SHIFT:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ShiftMonAction);
					break;
				
				case BOX_MENU_WITHDRAW:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_WithdrawMonAction);
					break;
				
				case BOX_MENU_STORE:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_StoreMonAction);
					break;
				
				case BOX_MENU_MARK:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MarkAction);
					break;
				
				case BOX_MENU_RELEASE:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ReleaseMonAction);
					break;
				
				case BOX_MENU_SUMMARY:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_OpenSummaryAction);
					break;
				
				case BOX_MENU_ITEM:
					if (BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp)) {
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MonItemHeldAction);
					}
					break;
				
				case BOX_MENU_SET_ON_LEFT:
				case BOX_MENU_SET_ON_RIGHT:
					BoxApp_ToggleCompareMonSlot(&boxAppMan->boxApp);
					BoxApp_PreviewBoxMon(&boxAppMan->boxApp, (BoxPokemon*)boxAppMan->boxApp.cursor.mon, boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030394);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = MON_CURSOR_MENU_END;
					break;
				
				case BOX_MENU_RANCH_DEPOSIT:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RanchDepositAction);
					break;
				
				case BOX_MENU_RANCH_WITHDRAW:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RanchWithdrawAction);
					break;
			}
			break;
		
		case MON_CURSOR_MENU_END_WHEN_READY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
		
		case MON_CURSOR_MENU_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_MonItemMenuAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		MON_ITEM_MENU_START,
		MON_ITEM_MENU_WAIT_FOR_MENU,
		MON_ITEM_MENU_NAVIGATE_MENU,
		MON_ITEM_MENU_SELECTED,
		MON_ITEM_MENU_END_WHEN_READY,
		MON_ITEM_MENU_WAIT_FOR_ANIMATIONS,
		MON_ITEM_MENU_END,
		MON_ITEM_MENU_CONFIRM_MESSAGE
	};
	
	u32 item;
	
	switch (*state) {
		case MON_ITEM_MENU_START:
			item = BoxApp_GetCursorItem(&boxAppMan->boxApp);
			if (item != ITEM_NONE) {
				StringTemplate_SetItemName(boxAppMan->messageVariableBuffer, 0, item);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_ItemSelected);
			} else {
				item = BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp);
				if (item != ITEM_NONE) {
					StringTemplate_SetItemName(boxAppMan->messageVariableBuffer, 0, item);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_ItemSelected);
				} else {
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_GiveToMon);
				}
			}
			
			BoxMenu_FillItemsMenu(&boxAppMan->boxApp);
			
			if (BoxApp_IsPreviewedMonEgg(&boxAppMan->boxApp)) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_EggsCantHoldItems);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = MON_ITEM_MENU_CONFIRM_MESSAGE;
			
			} else if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
				boxAppMan->menuItem = BoxMenu_GetDefaultMenuItem(&boxAppMan->boxApp);
				*state = MON_ITEM_MENU_SELECTED;
			
			} else {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = MON_ITEM_MENU_WAIT_FOR_MENU;
			}
			break;
		
		case MON_ITEM_MENU_WAIT_FOR_MENU:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = MON_ITEM_MENU_NAVIGATE_MENU;
			// Fall through
		
		case MON_ITEM_MENU_NAVIGATE_MENU:
			boxAppMan->menuItem = BoxMenu_GetMenuNavigation(&boxAppMan->boxApp);
			switch (boxAppMan->menuItem) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_ITEMS_CANCEL:
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = MON_ITEM_MENU_END_WHEN_READY;
					break;
				
				case BOX_MENU_NAVIGATION_NONE:
					break;
				
				default:
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = MON_ITEM_MENU_SELECTED;
					break;
			}
			break;
		
		case MON_ITEM_MENU_SELECTED:
			switch (boxAppMan->menuItem) {
				case BOX_MENU_GIVE:
					if (BoxApp_GetCursorItem(&boxAppMan->boxApp) != ITEM_NONE) {
						BoxApp_GiveItemFromCursor(&boxAppMan->boxApp, boxAppMan);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FC8C);
						Sound_PlayEffect(SEQ_SE_DP_DECIDE);
						*state = MON_ITEM_MENU_WAIT_FOR_ANIMATIONS;
					}
					break;
				
				case BOX_MENU_TAKE:
					if (Item_IsMail(BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp))) {
						Sound_PlayEffect(SEQ_SE_DP_BOX03);
						BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_CantTakeMail);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
						*state = MON_ITEM_MENU_CONFIRM_MESSAGE;
						break;
					}
					
					BoxApp_PickUpHeldItem(&boxAppMan->boxApp, boxAppMan);
					BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FC44);
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = MON_ITEM_MENU_WAIT_FOR_ANIMATIONS;
					break;
				
				case BOX_MENU_INFO:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_DisplayItemInfoAction);
					break;
				
				case BOX_MENU_SWITCH:
					if (Item_IsMail(BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp))) {
						Sound_PlayEffect(SEQ_SE_DP_BOX03);
						BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_CantTakeMail);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
						*state = MON_ITEM_MENU_CONFIRM_MESSAGE;
						break;
					}
					
					BoxApp_SwapMonAndCursorItems(&boxAppMan->boxApp, boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FCD4);
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = MON_ITEM_MENU_WAIT_FOR_ANIMATIONS;
					break;
				
				case BOX_MENU_BAG:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PutAwayItemAction);
					break;
			}
			break;
		
		case MON_ITEM_MENU_END_WHEN_READY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			return;
		
		case MON_ITEM_MENU_WAIT_FOR_ANIMATIONS:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FD1C);
				*state = MON_ITEM_MENU_END;
			}
			break;
		
		case MON_ITEM_MENU_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
		
		case MON_ITEM_MENU_CONFIRM_MESSAGE:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = MON_ITEM_MENU_END;
			}
			break;
	}
}


static void BoxAppMan_BoxHeaderMenuAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		BOX_HEADER_MENU_START,
		BOX_HEADER_MENU_SHOW_MENU,
		BOX_HEADER_MENU_NAVIGATE_MENU,
		BOX_HEADER_MENU_ITEM_SELECTED,
		BOX_HEADER_MENU_END_WHEN_READY,
		BOX_HEADER_MENU_END
	};
	
	int messageID;
	
	switch (*state) {
		case BOX_HEADER_MENU_START:
			messageID = BOX_MESSAGE_WhatDo;
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) {
				messageID = MPR_TEXT_FLAG | MPR_TEXT_WhatDoYouWant;
			}
			
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
			BoxMenu_FillHeaderMenu(&boxAppMan->boxApp);
			if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
				boxAppMan->menuItem = BoxMenu_GetDefaultMenuItem(&boxAppMan->boxApp);
				if (boxAppMan->menuItem == BOX_MENU_HEADER_CANCEL) {
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = BOX_HEADER_MENU_END_WHEN_READY;
				} else {
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					*state = BOX_HEADER_MENU_ITEM_SELECTED;
				}
			} else {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = BOX_HEADER_MENU_SHOW_MENU;
			}
			break;
		
		case BOX_HEADER_MENU_SHOW_MENU:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = BOX_HEADER_MENU_NAVIGATE_MENU;
			// Fall through
		
		case BOX_HEADER_MENU_NAVIGATE_MENU:
			boxAppMan->menuItem = BoxMenu_GetMenuNavigation(&boxAppMan->boxApp);
			switch (boxAppMan->menuItem) {
				case BOX_MENU_NAVIGATION_NONE:
					break;
				
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_HEADER_CANCEL:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = BOX_HEADER_MENU_END_WHEN_READY;
					break;
				
				default:
				case BOX_MENU_JUMP:
				case BOX_MENU_WALLPAPER:
				case BOX_MENU_NAME:
					*state = BOX_HEADER_MENU_ITEM_SELECTED;
					break;
			}
			break;
		
		case BOX_HEADER_MENU_ITEM_SELECTED:
			switch (boxAppMan->menuItem) {
				case BOX_MENU_JUMP:
					if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) {
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RanchBoxJumpAction);
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_BoxJumpAction);
					}
					break;
				
				case BOX_MENU_WALLPAPER:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					*state = BOX_HEADER_MENU_END;
					break;
				
				case BOX_MENU_NAME:
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_RenameBoxAction);
					break;
			}
			break;
		
		case BOX_HEADER_MENU_END_WHEN_READY:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
		
		case BOX_HEADER_MENU_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_WallpaperMenu);
			}
			break;
	}
}


static void BoxAppMan_BoxJumpAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		JUMP_START,
		JUMP_TO_BOX,
		JUMP_COMM_WAIT,
		JUMP_END
	};
	
	switch (*state) {
		case JUMP_START:
			BoxSelectorPopup_Init(boxAppMan, BoxApp_GetCurrentBox(&boxAppMan->boxApp), BOX_MESSAGE_JumpToBox);
			*state = JUMP_TO_BOX;
			return;
		
		case JUMP_TO_BOX:
			if (BoxAppMan_TrySelectBoxFromPopup(boxAppMan)) {
				int boxID = boxAppMan->boxSelector.boxID;
				if (boxID == -1 || boxID == BoxApp_GetCurrentBox(&boxAppMan->boxApp)) {
					*state = JUMP_END;
				} else {
					BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(boxAppMan, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
					BoxApp_LoadCustomizationsFor(&boxAppMan->boxApp, boxID);
					PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, boxID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
					
					if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT && MPRComm_IsConnected()) {
						BoxPokemon* pcBoxStart = PCBoxes_GetBoxMonAt(BoxApp_GetPCBoxes(&boxAppMan->boxApp), boxID, 0);
						MPRComm_ShowPCBox(pcBoxStart, boxID);
					}
					
					*state = JUMP_COMM_WAIT;
				}
				
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
				break;
			}
			break;
		
		case JUMP_COMM_WAIT:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) && (!MPRComm_IsConnected() || MPRComm_IsAwaitingShowPCBox())) {
				*state = JUMP_END;
			}
			break;
		
		case JUMP_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_RanchBoxJumpAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_JUMP_START,
		RANCH_JUMP_SHOW_MENU,
		RANCH_JUMP_NAVIGATE_MENU,
		RANCH_JUMP_JUMP_BOX,
		RANCH_JUMP_CANCEL,
		RANCH_JUMP_COMM_SEND,
		RANCH_JUMP_COMM_RECV,
		RANCH_JUMP_COMM_WAIT,
		RANCH_JUMP_END
	};
	
	switch (*state) {
		case RANCH_JUMP_START:
			{
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_JumpWhichBox);
				int pcBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				int ranchBoxID = BoxApp_GetMaxRanchBoxID(&boxAppMan->boxApp);
				
				BoxMenu_FillRanchHeaderMenu(&boxAppMan->boxApp, (ranchBoxID >= 10));
				
				if (pcBoxID < 10) {
					BoxMPR_020395E4(&boxAppMan->boxApp, BOX_MENU_RANCH_10_LEFT);
				}
				
				if (pcBoxID > ranchBoxID - 10) {
					BoxMPR_020395E4(&boxAppMan->boxApp, BOX_MENU_RANCH_10_RIGHT);
				}
				
				if (pcBoxID == 0) {
					BoxMPR_020395E4(&boxAppMan->boxApp, BOX_MENU_RANCH_FIRST);
				}
				
				if (pcBoxID == ranchBoxID) {
					BoxMPR_020395E4(&boxAppMan->boxApp, BOX_MENU_RANCH_LAST);
				}
				
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = RANCH_JUMP_SHOW_MENU;
			}
			break;
		
		case RANCH_JUMP_SHOW_MENU:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu)) {
				break;
			}
			*state = RANCH_JUMP_NAVIGATE_MENU;
			// Fall through
		
		case RANCH_JUMP_NAVIGATE_MENU:
			boxAppMan->menuItem = BoxMenu_GetMenuNavigation(&boxAppMan->boxApp);
			switch (boxAppMan->menuItem) {
				case BOX_MENU_NAVIGATION_NONE:
					break;
				
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_HEADER_CANCEL:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_JUMP_CANCEL;
					break;
				
				default:
				case BOX_MENU_JUMP:
				case BOX_MENU_WALLPAPER:
				case BOX_MENU_NAME:
					*state = RANCH_JUMP_JUMP_BOX;
					break;
			}
			break;
		
		case RANCH_JUMP_CANCEL:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
		
		case RANCH_JUMP_JUMP_BOX:
			{
				int boxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				int jumpDirection = 0;
				int originalBox = boxID;
				int maxRanchBoxID = boxAppMan->boxApp.customization.maxRanchBoxID;
				switch (boxAppMan->menuItem) {
					case BOX_MENU_RANCH_10_RIGHT:
						if (boxID < maxRanchBoxID) {
							boxID += 10;
							if (boxID > maxRanchBoxID) {
								boxID = maxRanchBoxID;
							}
							jumpDirection = 1;
						}
						break;
					
					case BOX_MENU_RANCH_10_LEFT:
						if (boxID > 0) {
							boxID -= 10;
							if (boxID < 0) {
								boxID = 0;
							}
							jumpDirection = -1;
						}
						break;
					
					case BOX_MENU_RANCH_FIRST:
						if (boxID > 0) {
							boxID = 0;
							jumpDirection = -1;
						}
						break;
					
					case BOX_MENU_RANCH_LAST:
						if (boxID < maxRanchBoxID) {
							boxID = maxRanchBoxID;
							jumpDirection = 1;
						}
						break;
				}
				
				if (jumpDirection != 0) {
					GF_ASSERT(boxID != originalBox);
					
					boxAppMan->boxApp.customization.ranchBoxJump = jumpDirection;
					BoxApp_LoadCustomizationsFor(&boxAppMan->boxApp, boxID);
					boxAppMan->menuItem = originalBox;
					MPRComm_0203C3A4(boxID, boxAppMan->menuItem, (BoxPokemon*)boxAppMan->mprFilter.unk_84);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					*state = RANCH_JUMP_COMM_SEND;
				} else {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_JUMP_END;
				}
			}
			break;
		
		case RANCH_JUMP_COMM_SEND:
			if (!MPRComm_IsConnected() || MPRComm_0203C404()) {
				int rightBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp) + 1;
				if (rightBoxID > boxAppMan->boxApp.customization.maxRanchBoxID) {
					rightBoxID = 0;
				}
				
				MPRComm_0203C3A4(rightBoxID, boxAppMan->menuItem, (BoxPokemon*)boxAppMan->mprFilter.unk_80);
				*state = RANCH_JUMP_COMM_RECV;
			}
			break;
		
		case RANCH_JUMP_COMM_RECV:
			if ((MPRComm_IsConnected() == 0) || (MPRComm_0203C404() != 0)) {
				int currBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				int leftBoxID = currBoxID - 1;
				if (leftBoxID < 0) {
					leftBoxID = boxAppMan->boxApp.customization.maxRanchBoxID;
				}
				
				MPRComm_0203C3A4(leftBoxID, currBoxID, (BoxPokemon*)boxAppMan->mprFilter.unk_88);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				boxAppMan->boxApp.MPR_unk_120 = boxAppMan->mprFilter.unk_84;
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
				*state = RANCH_JUMP_COMM_WAIT;
			}
			break;
		
		case RANCH_JUMP_COMM_WAIT:
			if ((!MPRComm_IsConnected() || MPRComm_0203C404()) && BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				*state = RANCH_JUMP_END;
			}
			break;
		
		case RANCH_JUMP_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_WallpaperMenu(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		WALLPAPER_MENU_START,
		WALLPAPER_MENU_PICK_THEME_INIT,
		WALLPAPER_MENU_PICK_THEME_WAIT_FOR_TASK,
		WALLPAPER_MENU_PICK_THEME_WAIT_FOR_USER,
		WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_TASK,
		WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_USER,
		WALLPAPER_MENU_TRANSITION_WALLPAPER,
		WALLPAPER_MENU_UNREACHABLE,
		WALLPAPER_MENU_END,
	};
	
	switch (*state) {
	case WALLPAPER_MENU_START:
		boxAppMan->menuItem = BOX_MENU_SCENERY_1;
		*state = WALLPAPER_MENU_PICK_THEME_INIT;
	case WALLPAPER_MENU_PICK_THEME_INIT:
		BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_PickTheme);
		BoxMenu_FillWallpaperMenu(&boxAppMan->boxApp, boxAppMan->menuItem);
		BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
		*state = WALLPAPER_MENU_PICK_THEME_WAIT_FOR_TASK;
		break;
	case WALLPAPER_MENU_PICK_THEME_WAIT_FOR_TASK:
		if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu) == FALSE) {
			break;
		}
		*state = WALLPAPER_MENU_PICK_THEME_WAIT_FOR_USER;
	case WALLPAPER_MENU_PICK_THEME_WAIT_FOR_USER:
		switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
		case BOX_MENU_NAVIGATION_NONE:
			break;
		case BOX_MENU_NAVIGATION_UP_DOWN:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
			break;
		case BOX_MENU_NAVIGATION_B:
		case BOX_MENU_HEADER_CANCEL:
		default:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
			*state = WALLPAPER_MENU_END;
			break;
		case BOX_MENU_SCENERY_1:
		case BOX_MENU_SCENERY_2:
		case BOX_MENU_SCENERY_3:
		case BOX_MENU_ETCETERA:
		case BOX_MENU_FRIENDS_1:
		case BOX_MENU_FRIENDS_2:
			boxAppMan->menuItem = BoxMenu_GetSelectedMenuItem(&boxAppMan->boxApp);
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_Wallpaper);
			BoxMenu_FillWallpaperSelectionMenu(&boxAppMan->boxApp, boxAppMan->menuItem);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
			*state = WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_TASK;
			break;
		}
		break;
	case WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_TASK:
		if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowMenu) == FALSE) {
			break;
		}
		*state = WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_USER;
	case WALLPAPER_MENU_PICK_WALLPAPER_WAIT_FOR_USER:
		switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
		case BOX_MENU_NAVIGATION_NONE:
			break;
		case BOX_MENU_NAVIGATION_UP_DOWN:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
			break;
		case BOX_MENU_NAVIGATION_B:
		case BOX_MENU_HEADER_CANCEL:
			*state = WALLPAPER_MENU_PICK_THEME_INIT;
			break;
		default:
			boxAppMan->menuItem = BoxMenu_GetSelectedMenuItem(&boxAppMan->boxApp);

			if (boxAppMan->menuItem >= BOX_MENU_FIRST_WALLPAPER && boxAppMan->menuItem <= BOX_MENU_LAST_WALLPAPER) {
				PCBoxes_SetWallpaper(boxAppMan->pcBoxes, USE_CURRENT_BOX, boxAppMan->menuItem - BOX_MENU_FIRST_WALLPAPER);
				BoxApp_LoadWallpaper(&boxAppMan->boxApp, boxAppMan->pcBoxes);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = WALLPAPER_MENU_TRANSITION_WALLPAPER;
			} else {
				GF_ASSERT(FALSE);
				*state = WALLPAPER_MENU_PICK_THEME_INIT;
			}
		}
		break;
	case WALLPAPER_MENU_TRANSITION_WALLPAPER:
		BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_TransitionWallpaper);
		*state = WALLPAPER_MENU_END;
		break;
	case WALLPAPER_MENU_UNREACHABLE:
		if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
			*state = WALLPAPER_MENU_PICK_THEME_INIT;
		}
		break;
	case WALLPAPER_MENU_END:
		if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
			BoxAppMan_ClearBoxApplicationAction(boxAppMan);
		}
		break;
	}
}


static void BoxAppMan_MarkAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		MARK_START,
		MARK_PROCESS_USER_INPUT,
		MARK_COMM_SYNC,
		MARK_END,
	};
	
	switch (*state) {
		case MARK_START:
			BoxMenu_FillMarkingsMenu(&boxAppMan->boxApp);
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MarkMon);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
			*state = MARK_PROCESS_USER_INPUT;
			return;
		
		case MARK_PROCESS_USER_INPUT:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				u32 menuNav = BoxMenu_GetMenuNavigation(&boxAppMan->boxApp);
				switch (menuNav) {
					case BOX_MENU_NAVIGATION_NONE:
						break;
					
					case BOX_MENU_NAVIGATION_UP_DOWN:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
						break;
					
					case BOX_MENU_NAVIGATION_B:
					case BOX_MENU_MARK_CANCEL:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						*state = MARK_END;
						break;
					
					case BOX_MENU_CONFIRM:
						BoxApp_UpdatePreviewMonMarkings(&boxAppMan->boxApp);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F94C);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_SetMonMarkingsTransparency);
						
						if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT && MPRComm_IsConnected()) {
							BoxAppMan_CommUpdateMon(boxAppMan);
							*state = MARK_COMM_SYNC;
							break;
						}
						
						*state = MARK_END;
						break;
					
					default:
						if (menuNav >= BOX_MENU_FIRST_MARKING && menuNav <= BOX_MENU_LAST_MARKING) {
							BoxMenu_ToggleMarking(&boxAppMan->boxApp, menuNav - BOX_MENU_FIRST_MARKING );
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FEA4);
						}
						break;
				}
			}
			break;
		
		case MARK_COMM_SYNC:
			if (MPRComm_IsConnected() && !MPRComm_IsMonUpdateComplete()) {
				break;
			}
			*state = MARK_END;
			// Fall through
		
		case MARK_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


void BoxAppMan_MultiSelectAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		MULTI_MOVE_START,
		MULTI_MOVE_DEFINE_SELECTION,
		MULTI_MOVE_SELECTED_MONS,
		MULTI_MOVE_WAIT_FOR_ANIMATIONS,
		MULTI_MOVE_END,
		MULTI_MOVE_PREVIEW_MON
	};
	
	switch (*state) {
		case MULTI_MOVE_START:
			if (JOY_HELD(PAD_BUTTON_A)) {
				BoxAppMan_ResetMultiSelectLocation(boxAppMan, &boxAppMan->boxApp);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_StartDrawMultiSelect);
				Sound_PlayEffect(SEQ_SE_DP_SELECT);
				*state = MULTI_MOVE_DEFINE_SELECTION;
			} else {
				BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PickUpMonAction);
			}
			break;
		
		case MULTI_MOVE_DEFINE_SELECTION:
			if (JOY_HELD(PAD_BUTTON_A)) {
				switch (BoxAppMan_TryMoveSelectionFromUserInput(gSystem.heldKeys, boxAppMan)) {
					case CURSOR_STOP:
						if (JOY_HELD(PAD_PLUS_KEY_MASK) == JOY_NEW(PAD_PLUS_KEY_MASK)) {
							Sound_PlayEffect(SEQ_SE_DP_BOX03);
						}
						break;
					
					case CURSOR_MOVE:
						BoxAppMan_SetMultiSelectionEndLocation(boxAppMan, &boxAppMan->boxApp);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ApplyMultiSelectMonShadingTask);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
						*state = MULTI_MOVE_WAIT_FOR_ANIMATIONS;
				}
			} else {
				if (BoxApp_IsMultiSelectSingleSelect(&boxAppMan->boxApp)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030320);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_PickUpMonAction);
				} else {
					BoxAppMan_PickUpMultiSelectedMons(boxAppMan, &boxAppMan->boxApp);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030358);
					Sound_PlayEffect(SEQ_SE_DP_BOX02);
					*state = MULTI_MOVE_SELECTED_MONS;
				}
			}
			break;
		
		case MULTI_MOVE_SELECTED_MONS:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				switch (BoxAppMan_TryMoveSelectionFromUserInput(gSystem.heldKeys, boxAppMan)) {
					case CURSOR_STOP:
						if (JOY_HELD(PAD_PLUS_KEY_MASK) == JOY_NEW(PAD_PLUS_KEY_MASK)) {
							Sound_PlayEffect(SEQ_SE_DP_BOX03);
						}
						break;
					
					case CURSOR_MOVE:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
						if (!(BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) & PREVIEW_MON_HELD)) {
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
						}
						break;
					
					case CURSOR_MOVE_TO_LEFT_BOX:
						BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(boxAppMan, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
						BoxAppMan_LoadLeftBoxCustomization(boxAppMan);
						PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
						break;
					
					case CURSOR_MOVE_TO_RIGHT_BOX:
						BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(boxAppMan, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
						BoxAppMan_LoadRightBoxCustomization(boxAppMan);
						PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
						break;
					
					case CURSOR_NO_MOVEMENT:
						if (JOY_NEW(PAD_BUTTON_A)) {
							if (BoxApp_IsBoxUnderSelectedMonsEmpty(&boxAppMan->boxApp)) {
								BoxAppMan_PutDownSelectedMons(boxAppMan, &boxAppMan->boxApp);
								BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlaceMonDownFromCursor);
								Sound_PlayEffect(SEQ_SE_DP_BOX01);
								*state = MULTI_MOVE_END;
							} else {
								Sound_PlayEffect(SEQ_SE_DP_BOX03);
							}
						} else if (JOY_NEW(PAD_BUTTON_B)) {
							Sound_PlayEffect(SEQ_SE_DP_BOX03);
						}
						break;
				}
			}
			break;
		
		case MULTI_MOVE_WAIT_FOR_ANIMATIONS:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				*state = MULTI_MOVE_DEFINE_SELECTION;
			}
			break;
		
		case MULTI_MOVE_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static BOOL BoxApp_IsBoxUnderSelectedMonsEmpty(const BoxApplication* boxApp) {
	BoxPokemon* boxMon;
	int i, posInBox;
	const BoxCursor* cursor = &boxApp->cursor;
	const BoxMonSelection* selection = &boxApp->selection;
	
	int selectionTopLeftPos = BoxApp_GetMultiSelectTopLeftPos(boxApp);
	int origSelectionTopLeftPos = selection->origSelectionTopLeftPos;
	
	for (i = 0; i < selection->selectedMonCount; i++) {
		posInBox = selectionTopLeftPos + (selection->selectedMonsOrigBoxPos[i] - origSelectionTopLeftPos);
		boxMon = PCBoxes_GetBoxMonAt(boxApp->pcBoxes, USE_CURRENT_BOX, posInBox);
		
		if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
			return FALSE;
		}
	}
	
	return TRUE;
}


static void BoxAppMan_PickUpMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		PICK_UP_MON_START,
		PICK_UP_PARTY_MON,
		PICK_UP_MON_CONFIRM_LAST_MON,
		PICK_UP_MON_DONE
	};
	
	switch (*state) {
		case PICK_UP_MON_START:
			if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_PARTY) {
				if (BoxAppMan_OnLastAliveMon(boxAppMan) == FALSE) {
					Sound_PlayEffect(SEQ_SE_DP_BOX02);
					BoxAppMan_PickUpMon(boxAppMan, &boxAppMan->boxApp);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PickUpMonIntoCursor);
					*state = PICK_UP_PARTY_MON;
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_LastMon);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = PICK_UP_MON_CONFIRM_LAST_MON;
				}
			} else {
				Sound_PlayEffect(SEQ_SE_DP_BOX02);
				BoxAppMan_PickUpMon(boxAppMan, &boxAppMan->boxApp);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PickUpMonIntoCursor);
				*state = PICK_UP_MON_DONE;
			}
			break;
		
		case PICK_UP_PARTY_MON:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlayAdjustPartyAnimation);
				*state = PICK_UP_MON_DONE;
			}
			break;
		
		case PICK_UP_MON_CONFIRM_LAST_MON:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = PICK_UP_MON_DONE;
			}
			break;
		
		case PICK_UP_MON_DONE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_PlaceMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		PLACE_MON_DOWN,
		PLACE_ADJUST_PARTY_ANIMATION,
		PLACE_MON_END
	};
	
	switch (*state) {
		case PLACE_MON_DOWN:
			BoxAppMan_PutDownCursorMon(boxAppMan, &boxAppMan->boxApp);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlaceMonDownFromCursor);
			Sound_PlayEffect(SEQ_SE_DP_BOX01);
			
			if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_PARTY) {
				u32 cursorPosition = BoxApp_GetCursorPartyPosition(&boxAppMan->boxApp);
				u32 partyCount = Party_GetCurrentCount(boxAppMan->party);
				
				if (cursorPosition != partyCount - 1) {
					*state = PLACE_ADJUST_PARTY_ANIMATION;
					BoxAppMan_TryPreviewCursorMon(boxAppMan);
					break;
				}
			}
			*state = PLACE_MON_END;
			break;
		
		case PLACE_ADJUST_PARTY_ANIMATION:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030190);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = PLACE_MON_END;
			}
			break;
		
		case PLACE_MON_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static BOOL BoxAppMan_CheckLastAliveMonReason(BoxApplicationManager* boxAppMan, u32* destMessageID) {
	if (BoxAppMan_OnLastAliveMon(boxAppMan)) {
		if (BoxApp_GetPreviewedMonValue(&boxAppMan->boxApp, MON_DATA_SANITY_IS_EGG, NULL)) {
			*destMessageID = BOX_MESSAGE_LastMon;
			return TRUE;
		}
		
		if (BoxApp_GetCursorMonIsPartyMon(&boxAppMan->boxApp)) {
			if (BoxApp_GetPreviewedMonValue(&boxAppMan->boxApp, MON_DATA_HP, NULL) == 0) {
				*destMessageID = BOX_MESSAGE_LastMon;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


static void BoxAppMan_ShiftMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		SHIFT_START,
		SHIFT_CONFIRM_MESSAGE,
		SHIFT_END
	};
	
	switch (*state) {
		case SHIFT_START:
			{
				u32 messageID;
				if (BoxAppMan_CheckLastAliveMonReason(boxAppMan, &messageID)) {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = SHIFT_CONFIRM_MESSAGE;
				} else {
					BoxAppMan_SwapMonInCursor(boxAppMan, &boxAppMan->boxApp);
					Sound_PlayEffect(SEQ_SE_DP_SELECT);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FA44);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					*state = SHIFT_END;
				}
			}
			break;
		
		case SHIFT_CONFIRM_MESSAGE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = SHIFT_END;
			}
			break;
		
		case SHIFT_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_WithdrawMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		WITHDRAW_START,
		WITHDRAW_WAIT_FOR_PICK_UP_MON,
		WITHDRAW_WAIT_FOR_PARTY_POPUP,
		WITHDRAW_WAIT_FOR_MOVE_CURSOR_TO_PARTY,
		WITHDRAW_WAIT_FOR_MOVE_PLACE_MON_DOWN,
		WITHDRAW_RETURN_TO_BOX,
		WITHDRAW_UNUSED,
		WITHDRAW_CONFIRM_PARTY_FULL,
		WITHDRAW_END
	};
	
	switch (*state) {
		case WITHDRAW_START:
			if (Party_GetCurrentCount(boxAppMan->party) != MAX_PARTY_SIZE) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenPartyPopup);
					*state = WITHDRAW_WAIT_FOR_PARTY_POPUP;
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX02);
					BoxAppMan_PickUpMon(boxAppMan, &boxAppMan->boxApp);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PickUpMonIntoCursor);
					*state = WITHDRAW_WAIT_FOR_PICK_UP_MON;
				}
			} else {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_PartyFull);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = WITHDRAW_CONFIRM_PARTY_FULL;
			}
			break;
		
		case WITHDRAW_WAIT_FOR_PICK_UP_MON:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PickUpMonIntoCursor)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenPartyPopup);
				*state = WITHDRAW_WAIT_FOR_PARTY_POPUP;
			}
			break;
		
		case WITHDRAW_WAIT_FOR_PARTY_POPUP:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_OpenPartyPopup)) {
				BoxAppMan_MoveCursorToParty(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				*state = WITHDRAW_WAIT_FOR_MOVE_CURSOR_TO_PARTY;
			}
			break;
		
		case WITHDRAW_WAIT_FOR_MOVE_CURSOR_TO_PARTY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_MoveCursor)) {
				BoxAppMan_PutDownCursorMon(boxAppMan, &boxAppMan->boxApp);
				BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlaceMonDownFromCursor);
				*state = WITHDRAW_WAIT_FOR_MOVE_PLACE_MON_DOWN;
			}
			break;
		
		case WITHDRAW_WAIT_FOR_MOVE_PLACE_MON_DOWN:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PlaceMonDownFromCursor)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_020300D0);
				*state = WITHDRAW_RETURN_TO_BOX;
			}
			break;
		
		case WITHDRAW_RETURN_TO_BOX:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_020300D0)) {
				BoxAppMan_ReturnCursorToBox(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = WITHDRAW_END;
			}
			break;
		
		case WITHDRAW_CONFIRM_PARTY_FULL:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = WITHDRAW_END;
			}
			break;
		
		case WITHDRAW_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_StoreMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		STORE_MON_CHECK_CAN_STORE_MON,
		STORE_MON_SELECT_BOX,
		STORE_MON_BOX_FULL,
		STORE_MON_CONFIRM_CANNOT_STORE_MON,
		STORE_MON_FROM_CURSOR,
		STORE_MON_FROM_PARTY,
		STORE_MON_END
	};
	
	switch (*state) {
		case STORE_MON_CHECK_CAN_STORE_MON:
			if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR && BoxAppMan_OnLastAliveMon(boxAppMan) == TRUE) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_LastMon);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = STORE_MON_CONFIRM_CANNOT_STORE_MON;
			} else {
				int boxMessageID;
				if (BoxAppMan_IsPreviewedMonHoldingMailOrHasBallCapsule(boxAppMan, &boxMessageID)) {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, boxMessageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = STORE_MON_CONFIRM_CANNOT_STORE_MON;
				} else {
					BoxSelectorPopup_Init(boxAppMan, boxAppMan->boxApp.selectedBoxID, BOX_MESSAGE_PickDepositBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = STORE_MON_SELECT_BOX;
				}
			}
			break;
		
		case STORE_MON_SELECT_BOX:
			if (BoxAppMan_TrySelectBoxFromPopup(boxAppMan) == FALSE) {
				break;
			}
			
			if (boxAppMan->boxSelector.boxID == -1) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
				*state = STORE_MON_END;
				break;
			}
			
			boxAppMan->boxApp.selectedBoxID = boxAppMan->boxSelector.boxID;
			
			if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
				if (BoxAppMan_TryStoreCursorMonInBox(boxAppMan, boxAppMan->boxSelector.boxID)) {
					BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					*state = STORE_MON_FROM_CURSOR;
					break;
				}
			} else {
				if (BoxAppMan_TryStoreSelectedMonInBox(boxAppMan, boxAppMan->boxSelector.boxID)) {
					BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					*state = STORE_MON_FROM_PARTY;
					break;
				}
			}
			
			Sound_PlayEffect(SEQ_SE_DP_BOX03);
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_BoxFull);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
			*state = STORE_MON_BOX_FULL;
			break;
		
		case STORE_MON_BOX_FULL:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxSelectorPopup_Reset(boxAppMan);
				*state = STORE_MON_SELECT_BOX;
			}
			break;
		
		case STORE_MON_CONFIRM_CANNOT_STORE_MON:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = STORE_MON_END;
			}
			break;
		
		case STORE_MON_FROM_CURSOR:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FF48)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FA80);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = STORE_MON_END;
			}
			break;
		
		case STORE_MON_FROM_PARTY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FF48)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FAB0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = STORE_MON_END;
			}
			break;
		
		case STORE_MON_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static BOOL BoxAppMan_OnLastAliveMon(BoxApplicationManager* boxAppMan) {
	Pokemon* mon;
	int count;
	BOOL reencrypt;
	
	int partyCount = Party_GetCurrentCount(boxAppMan->party);
	for (int i = 0, count = 0; i < partyCount; i++) {
		mon = Party_GetPokemonBySlotIndex(boxAppMan->party, i);
		reencrypt = Pokemon_EnterDecryptionContext(mon);
		
		if (Pokemon_GetValue(mon, MON_DATA_SANITY_IS_EGG, NULL) == FALSE && Pokemon_GetValue(mon, MON_DATA_HP, NULL)) {
			count++;
		}
		
		Pokemon_ExitDecryptionContext(mon, reencrypt);

		if (count >= 2) {
			return FALSE;
		}
	}
	
	if (BoxApp_GetPreviewedOrSelectedMonValue(&boxAppMan->boxApp, MON_DATA_SANITY_IS_EGG, NULL)) {
		return FALSE;
	}
	
	if (BoxApp_GetPreviewedOrSelectedMonValue(&boxAppMan->boxApp, MON_DATA_HP, NULL) == 0) {
		return FALSE;
	}
	
	return TRUE;
}


static BOOL BoxAppMan_CheckReleaseMonValid(BoxApplicationManager* boxAppMan, int* destBoxMessageID) {
	if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_DEPOSIT && BoxApp_GetPreviewedMonValue(&boxAppMan->boxApp, MON_DATA_SANITY_IS_EGG, NULL)) {
		*destBoxMessageID = BOX_MESSAGE_CantReleaseEgg;
		return FALSE;
	}
	
	(void)BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp); /* Unused */
	
	if (Item_IsMail(BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp))) {
		*destBoxMessageID = BOX_MESSAGE_RemoveMail;
		return FALSE;
	}
	
	if (BoxApp_GetPreviewedMonValue(&boxAppMan->boxApp, MON_DATA_BALL_CAPSULE_ID, NULL)) {
		*destBoxMessageID = BOX_MESSAGE_DetachBallCapsule;
		return FALSE;
	}
	
	if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
		if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_PARTY) {
			if (BoxAppMan_OnLastAliveMon(boxAppMan)) {
				*destBoxMessageID = BOX_MESSAGE_LastMon;
				return FALSE;
			}
		}
	}
	
	return TRUE;
}


static void BoxAppMan_ReleaseMonAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RELEASE_MON_START,
		RELEASE_MON_CONFIRM,
		RELEASE_MON_RELEASE_ANIMATION,
		RELEASE_MON_TRY_RELEASE,
		RELEASE_MON_CONFIRM_RETURNED,
		RELEASE_MON_CONFIRM_RELEASED,
		RELEASE_MON_CLOSE_MESSAGE_BOX,
		RELEASE_MON_PREPARE_END_RELEASE,
		RELEASE_MON_CANNOT_RELEASE,
		RELEASE_MON_END
	};
	
	switch (*state) {
		case RELEASE_MON_START:
			{
				int boxMessageID;
				
				if (BoxAppMan_CheckReleaseMonValid(boxAppMan, &boxMessageID)) {
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_ReleaseMon);
					BoxMenu_FillYesNo(&boxAppMan->boxApp, 1);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
					*state = RELEASE_MON_CONFIRM;
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, boxMessageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = RELEASE_MON_CANNOT_RELEASE;
				}
			}
			break;
		
		case RELEASE_MON_CONFIRM:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RELEASE_MON_END;
					break;
				
				case BOX_MENU_YES:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RELEASE_MON_RELEASE_ANIMATION;
					break;
			}
			break;
		
		case RELEASE_MON_RELEASE_ANIMATION:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				StringTemplate_SetNickname(boxAppMan->messageVariableBuffer, 0, BoxApp_GetPreviewedBoxMon(&boxAppMan->boxApp));
				BoxAppMan_CheckShouldMonReturn(boxAppMan);
				
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlayReleaseCursorMonAnimation);
					boxAppMan->releasedFrom = RELEASED_FROM_CURSOR;
				} else {
					if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlayReleaseBoxMonAnimation);
						boxAppMan->releasedFrom = RELEASED_FROM_BOX;
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlayReleasePartyMonAnimation);
						boxAppMan->releasedFrom = RELEASED_FROM_PARTY;
					}
				}
				*state = RELEASE_MON_TRY_RELEASE;
			}
			break;
		
		case RELEASE_MON_TRY_RELEASE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				if (BoxAppMan_CanReleaseMon(boxAppMan)) {
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
						BoxAppMan_RemoveCursorMon(boxAppMan);
					} else {
						BoxAppMan_RemoveMonUnderCursor(boxAppMan);
					}
					
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonReleased);
					*state = RELEASE_MON_CONFIRM_RELEASED;
				} else {
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonReturned);
					*state = RELEASE_MON_CONFIRM_RETURNED;
				}
				
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
			}
			break;
		
		case RELEASE_MON_CONFIRM_RELEASED:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_GoodbyeForever);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = RELEASE_MON_CLOSE_MESSAGE_BOX;
			}
			break;
		
		case RELEASE_MON_CONFIRM_RETURNED:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonWasWorried);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = RELEASE_MON_CLOSE_MESSAGE_BOX;
			}
			break;
		
		case RELEASE_MON_CLOSE_MESSAGE_BOX:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = RELEASE_MON_PREPARE_END_RELEASE;
			}
			break;
		
		case RELEASE_MON_PREPARE_END_RELEASE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				if (BoxAppMan_CanReleaseMon(boxAppMan)) {
					if (boxAppMan->releasedFrom == RELEASED_FROM_PARTY) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PlayAdjustPartyAnimation);
					}
					
					BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
				}
				
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RELEASE_MON_END;
			}
			break;
		
		case RELEASE_MON_CANNOT_RELEASE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = RELEASE_MON_END;
			}
			break;
		
		case RELEASE_MON_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_CheckShouldMonReturn(BoxApplicationManager* boxAppMan) {
	ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	int i, releaseBlockingMovesCount;
	
	releaseMon->boxMon = boxAppMan->boxApp.pcMonPreview.mon;
	
	for (i = 0, releaseBlockingMovesCount = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
		releaseMon->hasReleaseBlockingMove[i] = BoxPokemon_HasMove(releaseMon->boxMon, sReleaseBlockingMoves[i]);
		
		if (releaseMon->hasReleaseBlockingMove[i]) {
			releaseBlockingMovesCount++;
		}
	}
	
	if (releaseBlockingMovesCount == 0) {
		releaseMon->checkedCanReleaseMon = TRUE;
		releaseMon->canReleaseMon = TRUE;
	} else {
		releaseMon->checkedCanReleaseMon = FALSE;
		releaseMon->canReleaseMon = FALSE;
		releaseMon->boxID = 0;
		releaseMon->monPosInBox = 0;
		releaseMon->pcBoxes = boxAppMan->pcBoxes;
		releaseMon->party = boxAppMan->party;
		releaseMon->monHeldInCursor = BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) != PREVIEW_MON_UNDER_CURSOR;
		
		for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
			releaseMon->monsWithReleaseBlockingMoveCount[i] = 0;
		}
		
		SysTask_Start(CheckLastMonWithReleaseBlockingMove, releaseMon, 0);
	}
}


static void CheckLastMonWithReleaseBlockingMove(SysTask* task, void* releaseMonParam) {
	ReleaseMon* releaseMon = releaseMonParam;
	BoxPokemon* boxMon;
	int monIndex, i;
	
	if (releaseMon->boxID < MAX_PC_BOXES) {
		int v4 = releaseMon->monPosInBox + 15;
		if (v4 > MAX_MONS_PER_BOX) {
			v4 = MAX_MONS_PER_BOX;
		}
		
		for (monIndex = releaseMon->monPosInBox; monIndex < v4; monIndex++) {
			boxMon = PCBoxes_GetBoxMonAt(releaseMon->pcBoxes, releaseMon->boxID, monIndex);
			
			if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
				for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
					if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[i])) {
						releaseMon->monsWithReleaseBlockingMoveCount[i]++;
					}
				}
			}
		}
		
		if (v4 == MAX_MONS_PER_BOX) {
			releaseMon->monPosInBox = 0;
			releaseMon->boxID++;
		} else {
			releaseMon->monPosInBox = v4;
		}
	} else {
		int partyCount = Party_GetCurrentCount(releaseMon->party);
		for (monIndex = 0; monIndex < partyCount; monIndex++) {
			boxMon = (BoxPokemon*)Party_GetPokemonBySlotIndex(releaseMon->party, monIndex);

			for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
				if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[i])) {
					releaseMon->monsWithReleaseBlockingMoveCount[i]++;
				}
			}
		}
		
		if (releaseMon->monHeldInCursor) {
			for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
				if (BoxPokemon_HasMove(releaseMon->boxMon, sReleaseBlockingMoves[i])) {
					releaseMon->monsWithReleaseBlockingMoveCount[i]++;
				}
			}
		}
		
		releaseMon->canReleaseMon = TRUE;
		
		for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
			if (releaseMon->monsWithReleaseBlockingMoveCount[i] == 1 && releaseMon->hasReleaseBlockingMove[i] == TRUE) {
				releaseMon->canReleaseMon = FALSE;
				break;
			}
		}
		
		releaseMon->checkedCanReleaseMon = TRUE;
		SysTask_Done(task);
	}
}


static BOOL BoxPokemon_HasMove(BoxPokemon* boxMon, u16 move) {
	BOOL hasMove = FALSE;
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	if (BoxPokemon_GetValue(boxMon, MON_DATA_SANITY_IS_EGG, NULL) == FALSE) {
		for (int i = 0; i < LEARNED_MOVES_MAX; i++) {
			if (BoxPokemon_GetValue(boxMon, MON_DATA_MOVE1 + i, NULL) == move) {
				hasMove = TRUE;
				break;
			}
		}
	}
	
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	return hasMove;
}


BOOL BoxAppMan_HasCheckedCanReleaseMon(const BoxApplicationManager* boxAppMan) {
	return (&boxAppMan->releaseMon)->checkedCanReleaseMon;
}


BOOL BoxAppMan_CanReleaseMon(const BoxApplicationManager* boxAppMan) {
	const ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	
	if (releaseMon->checkedCanReleaseMon) {
		return releaseMon->canReleaseMon;
	}
	
	return FALSE;
}


static void BoxAppMan_RenameBoxAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RENAME_BOX_START,
		RENAME_BOX_LAUNCH_TEXT_INPUT_APP,
		RENAME_BOX_RETURN_TO_BOX
	};
	
	switch (*state) {
		case RENAME_BOX_START:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack2);
			(*state)++;
			break;
		
		case RENAME_BOX_LAUNCH_TEXT_INPUT_APP:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack2)) {
				BoxGraphics_Free(boxAppMan->display);
				boxAppMan->display = NULL;
				Heap_Destroy(HEAP_ID_BOX_GRAPHICS);
				PCBoxes_BufferBoxName(boxAppMan->pcBoxes, PCBoxes_GetCurrentBoxID(boxAppMan->pcBoxes), boxAppMan->namingScreenArgs->textInputStr);
				boxAppMan->applicationManager = ApplicationManager_New(&gNamingScreenAppTemplate, boxAppMan->namingScreenArgs, HEAP_ID_BOX_DATA);
				(*state)++;
			}
			break;
		
		case RENAME_BOX_RETURN_TO_BOX:
			if (ApplicationManager_Exec(boxAppMan->applicationManager)) {
				u32 boxID = PCBoxes_GetCurrentBoxID(boxAppMan->pcBoxes);
				
				ApplicationManager_Free(boxAppMan->applicationManager);
				boxAppMan->applicationManager = NULL;
				
				if (!MPRComm_GetErrorFlag()) {
					Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_BOX_GRAPHICS, 0x3C000);
					PCBoxes_RenameBox(boxAppMan->pcBoxes, boxID, boxAppMan->namingScreenArgs->textInputStr);
					
					PCBoxes_LoadCustomization(boxAppMan->pcBoxes, &boxAppMan->boxApp.customization, BoxApp_GetBoxMode(&boxAppMan->boxApp));
					BoxGraphics_Load(&boxAppMan->display, &boxAppMan->boxApp, boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ReturnToBoxFade1Action);
				}
			}
			break;
	}
}


static void BoxAppMan_OpenSummaryAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		SUMMARY_START,
		SUMMARY_LAUNCH,
		SUMMARY_RETURN_TO_BOX
	};
	
	switch (*state) {
		case SUMMARY_START:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack2);
			(*state)++;
			break;
		
		case SUMMARY_LAUNCH:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ScreenFadeBothToBlack2)) {
				BoxGraphics_Free(boxAppMan->display);
				boxAppMan->display = NULL;
				Heap_Destroy(HEAP_ID_BOX_GRAPHICS);
				BoxAppMan_InitSummary(boxAppMan);
				boxAppMan->applicationManager = ApplicationManager_New(&gPokemonSummaryScreenApp, &boxAppMan->monSummary, HEAP_ID_BOX_DATA);
				(*state)++;
			}
			break;
		
		case SUMMARY_RETURN_TO_BOX:
			if (ApplicationManager_Exec(boxAppMan->applicationManager)) {
				u32 boxID = PCBoxes_GetCurrentBoxID(boxAppMan->pcBoxes);
				
				ApplicationManager_Free(boxAppMan->applicationManager);
				boxAppMan->applicationManager = NULL;
				
				if (!MPRComm_GetErrorFlag()) {
					Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_BOX_GRAPHICS, 0x3C000);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
						BoxApp_SetCursorPosToSummaryMonPos(&boxAppMan->boxApp, boxAppMan);
					}
					
					BoxGraphics_Load(&boxAppMan->display, &boxAppMan->boxApp, boxAppMan);
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_ReturnToBoxFade1Action);
					
					switch (BoxApp_GetBoxMode(&boxAppMan->boxApp)) {
						case PC_MODE_RANCH_DEPOSIT:
						case PC_MODE_RANCH_WITHDRAW:
							if (boxAppMan->monSummary.movesSwapped) {
								BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
							}
							break;
						
						default:
							BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
					}
				}
			}
			break;
	}
}


static void BoxApp_SetCursorPosToSummaryMonPos(BoxApplication* boxApp, BoxApplicationManager* boxAppMan) {
	BoxCursor* cursor = &boxApp->cursor;
	
	switch (cursor->cursorLocation) {
		case CURSOR_IN_BOX:
			cursor->posInBox = boxAppMan->monSummary.monIndex;
			cursor->boxCol = cursor->posInBox % MAX_PC_COLS;
			cursor->boxRow = cursor->posInBox / MAX_PC_COLS;
			break;
		
		case CURSOR_IN_PARTY:
			cursor->posInParty = boxAppMan->monSummary.monIndex;
			break;
		
		default:
			return;
	}
	
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static u32 sHeldItem;
static u32 sPutAwayItem;

static void BoxAppMan_MonItemHeldAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum ItemHeldState {
		ITEM_HELD_START,
		ITEM_HELD_YES_NO,
		ITEM_HELD_ADD_TO_BAG,
		ITEM_HELD_DISPLAY_TOOK_ITEM_MESSAGE,
		ITEM_HELD_CONFIRM_MESSAGE,
		ITEM_HELD_END
	};
	
	switch (*state) {
		case ITEM_HELD_START:
			sHeldItem = BoxApp_GetPreviewedMonHeldItem(&boxAppMan->boxApp);
			if (Item_IsMail(sHeldItem)) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_CantTakeMail);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = ITEM_HELD_CONFIRM_MESSAGE;
			} else {
				StringTemplate_SetItemName(boxAppMan->messageVariableBuffer, 0, sHeldItem);
				BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_ConfirmTakeItem);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = ITEM_HELD_YES_NO;
			}
			break;
		
		case ITEM_HELD_YES_NO:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = ITEM_HELD_END;
					break;
				
				case BOX_MENU_YES:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					*state = ITEM_HELD_ADD_TO_BAG;
					break;
			}
			break;
		
		case ITEM_HELD_ADD_TO_BAG:
			if (Bag_TryAddItem(SaveData_GetBag(boxAppMan->saveData), sHeldItem, 1, HEAP_ID_BOX_DATA)) {
				BoxApp_GiveItemToSelectedMon(&boxAppMan->boxApp, ITEM_NONE, boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FD1C);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = ITEM_HELD_DISPLAY_TOOK_ITEM_MESSAGE;
			} else {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_BagFull);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = ITEM_HELD_CONFIRM_MESSAGE;
			}
			break;
		
		case ITEM_HELD_DISPLAY_TOOK_ITEM_MESSAGE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_TookItem);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = ITEM_HELD_CONFIRM_MESSAGE;
			}
			break;
		
		case ITEM_HELD_CONFIRM_MESSAGE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage)) {
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = ITEM_HELD_END;
				}
			}
			break;
		
		case ITEM_HELD_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_PutAwayItemAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		PUT_AWAY_ITEM_START,
		PUT_AWAY_ITEM_YES_NO,
		PUT_AWAY_ITEM_ADD_TO_BAG,
		PUT_AWAY_ITEM_ANIMATE_ITEM_AWAY,
		PUT_AWAY_ITEM_SHOW_STORED_MESSAGE,
		PUT_AWAY_ITEM_CONFIRM_MESSAGE,
		PUT_AWAY_ITEM_END
	};
	
	switch (*state) {
		case PUT_AWAY_ITEM_START:
			sPutAwayItem = BoxApp_GetCursorOrPreviewedItem(&boxAppMan->boxApp);
			
			if (Item_IsMail(sPutAwayItem)) {
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_CantTakeMail);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = PUT_AWAY_ITEM_CONFIRM_MESSAGE;
			} else {
				StringTemplate_SetItemName(boxAppMan->messageVariableBuffer, 0, sPutAwayItem);
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_PutAwayItem);
				BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = PUT_AWAY_ITEM_YES_NO;
			}
			break;
		
		case PUT_AWAY_ITEM_YES_NO:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display) == FALSE) {
				break;
			}
			
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = PUT_AWAY_ITEM_END;
					break;
				
				case BOX_MENU_YES:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = PUT_AWAY_ITEM_ADD_TO_BAG;
					break;
			}
			break;
		
		case PUT_AWAY_ITEM_ADD_TO_BAG:
			if (Bag_TryAddItem(SaveData_GetBag(boxAppMan->saveData), sPutAwayItem, 1, HEAP_ID_BOX_DATA)) {
				if (BoxApp_GetCursorItem(&boxAppMan->boxApp) != ITEM_NONE) {
					BoxApp_RemoveCursorItem(&boxAppMan->boxApp);
					*state = PUT_AWAY_ITEM_SHOW_STORED_MESSAGE;
				} else {
					BoxApp_GiveItemToSelectedMon(&boxAppMan->boxApp, ITEM_NONE, boxAppMan);
					*state = PUT_AWAY_ITEM_ANIMATE_ITEM_AWAY;
				}
				
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ItemShrinkToNothing);
			} else {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_BagFull);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = PUT_AWAY_ITEM_CONFIRM_MESSAGE;
			}
			break;
		
		case PUT_AWAY_ITEM_ANIMATE_ITEM_AWAY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ItemShrinkToNothing)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FD1C);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = PUT_AWAY_ITEM_SHOW_STORED_MESSAGE;
			}
			break;
		
		case PUT_AWAY_ITEM_SHOW_STORED_MESSAGE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_PlaceItemInBag);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = PUT_AWAY_ITEM_CONFIRM_MESSAGE;
			}
			break;
		
		case PUT_AWAY_ITEM_CONFIRM_MESSAGE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage)) {
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
					Sound_PlayEffect(SEQ_SE_DP_DECIDE);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = PUT_AWAY_ITEM_END;
				}
			}
			break;
		
		case PUT_AWAY_ITEM_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_DisplayItemInfoAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		ITEM_INFO_START,
		ITEM_INFO_CLOSE_MENU,
		ITEM_INFO_DISPLAY,
		ITEM_INFO_CONFIRM,
		ITEM_INFO_END
	};
	
	switch (*state) {
		case ITEM_INFO_START:
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
			(*state)++;
			break;
		
		case ITEM_INFO_CLOSE_MENU:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayItemInfo);
				(*state)++;
			}
			break;
		
		case ITEM_INFO_DISPLAY:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayItemInfo)) {
				(*state)++;
			}
			break;
		
		case ITEM_INFO_CONFIRM:
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B | PAD_PLUS_KEY_MASK)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseItemInfo);
				(*state)++;
			}
			break;
		
		case ITEM_INFO_END:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_CloseItemInfo)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static BOOL BoxApp_CanDepositMonToRanch(BoxApplication* boxApp, int* outMessageID) {
	if (BoxApp_GetPreviewedMonValue(boxApp, MON_DATA_CHECKSUM_FAILED, NULL)) {
		*outMessageID = MPR_TEXT_FLAG | MPR_TEXT_CannotDeposit;
		return FALSE;
	}
	
	return TRUE;
}


static void BoxAppMan_RanchDepositAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_DEPOSIT_START,
		RANCH_DEPOSIT_CONFIRM,
		RANCH_DEPOSIT_START_DEPOSIT,
		RANCH_DEPOSIT_END_DEPOSIT,
		RANCH_DEPOSIT_MON_WORRIED,
		RANCH_DEPOSIT_MENU_WAIT,
		RANCH_DEPOSIT_PREVIEW_UPDATE,
		RANCH_DEPOSIT_FAILED,
		RANCH_DEPOSIT_END
	};
	
	switch (*state) {
		case RANCH_DEPOSIT_START:
			{
				int messageID;
				if (BoxApp_CanDepositMonToRanch(&boxAppMan->boxApp, &messageID)) {
					if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
						*state = RANCH_DEPOSIT_START_DEPOSIT;
					} else {
						BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_DepositMonConfirm);
						BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
						*state = RANCH_DEPOSIT_CONFIRM;
					}
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = RANCH_DEPOSIT_FAILED;
				}
			}
			break;
		
		case RANCH_DEPOSIT_CONFIRM:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
				case BOX_MENU_NAVIGATION_UP_DOWN:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
					break;
				
				case BOX_MENU_NAVIGATION_B:
				case BOX_MENU_NO:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_DEPOSIT_END;
					break;
				
				case BOX_MENU_YES:
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
					*state = RANCH_DEPOSIT_START_DEPOSIT;
					break;
			}
			break;
		
		case RANCH_DEPOSIT_START_DEPOSIT:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FE58)) {
				StringTemplate_SetNickname(boxAppMan->messageVariableBuffer, 0, BoxApp_GetPreviewedBoxMon(&boxAppMan->boxApp));
				
				if (MPRComm_GetRanchDepositSpace() <= 0 && MPRComm_IsConnected()) {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_RanchFull);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = RANCH_DEPOSIT_FAILED;
				} else {
					boxAppMan->releaseMon.checkedCanReleaseMon = TRUE;
					boxAppMan->releaseMon.canReleaseMon = BoxAppMan_IsCursorMonDepositable(boxAppMan);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_MonDeposited);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					
					if (BoxAppMan_CanReleaseMon(boxAppMan)) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_52);
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_53);
					}
					*state = RANCH_DEPOSIT_END_DEPOSIT;
				}
			}
			break;
		
		case RANCH_DEPOSIT_END_DEPOSIT:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			if (BoxAppMan_CanReleaseMon(boxAppMan)) {
				if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
					BoxAppMan_AssertFail(boxAppMan);
				} else {
					BoxAppMan_RanchDepositMonUnderCursor(boxAppMan, 0);
				}
				
				if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_DEPOSIT_PREVIEW_UPDATE;
					break;
				}
				
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_MonHaveAGoodTime);
				boxAppMan->menuTimer = 75;
				*state = RANCH_DEPOSIT_MENU_WAIT;
			} else {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonReturned);
				*state = RANCH_DEPOSIT_MON_WORRIED;
			}
			
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
			break;
		
		case RANCH_DEPOSIT_MON_WORRIED:
			if ((BoxGraphics_CheckAllTasksDone(boxAppMan->display)) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, BOX_MESSAGE_MonWasWorried);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				boxAppMan->menuTimer = 75;
				*state = RANCH_DEPOSIT_MENU_WAIT;
			}
			break;
		
		case RANCH_DEPOSIT_MENU_WAIT:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->menuTimer--;
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B) || boxAppMan->menuTimer == 0) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_DEPOSIT_PREVIEW_UPDATE;
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_L | PAD_BUTTON_R | PAD_PLUS_KEY_MASK)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					BoxAppMan_ResetAction(boxAppMan);
					break;
				}
			}
			break;
		
		case RANCH_DEPOSIT_PREVIEW_UPDATE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_DEPOSIT_END;
			}
			break;
		
		case RANCH_DEPOSIT_FAILED:
			if ((BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage)) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = RANCH_DEPOSIT_END;
			}
			break;
		
		case RANCH_DEPOSIT_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_RanchWithdrawAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_WITHDRAW_START,
		RANCH_WITHDRAW_SELECT_BOX,
		RANCH_WITHDRAW_BOX_FULL,
		RANCH_WITHDRAW_TRY_WITHDRAW,
		RANCH_WITHDRAW_WITHDRAW_OK,
		RANCH_WITHDRAW_WELCOME_BACK,
		RANCH_WITHDRAW_DIALOG_WAIT,
		RANCH_WITHDRAW_FAILED_WAIT,
		RANCH_WITHDRAW_PREVIEW_UPDATE,
		RANCH_WITHDRAW_END
	};
	
	switch (*state) {
		case RANCH_WITHDRAW_START:
			boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
			if (PCBoxes_FirstEmptyBox(boxAppMan->pcBoxes) == MAX_PC_BOXES) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_AllDSBoxesFull);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = RANCH_WITHDRAW_FAILED_WAIT;
			} else {
				BoxSelectorPopup_Init(boxAppMan, PCBoxes_GetCurrentBoxID(boxAppMan->pcBoxes), MPR_TEXT_FLAG | MPR_TEXT_WithdrawWhichBox);
				*state = RANCH_WITHDRAW_SELECT_BOX;
			}
			break;
		
		case RANCH_WITHDRAW_SELECT_BOX:
			if (BoxAppMan_TrySelectBoxFromPopup(boxAppMan)) {
				int boxID = boxAppMan->boxSelector.boxID;
				if (boxID == -1) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					*state = RANCH_WITHDRAW_END;
					break;
				}
				
				if (PCBoxes_CountEmptySpaceInBox(boxAppMan->pcBoxes, boxID) > 0) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, boxAppMan->boxSelector.boxID);
					*state = RANCH_WITHDRAW_TRY_WITHDRAW;
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_BoxFull);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = RANCH_WITHDRAW_BOX_FULL;
				}
				break;
			}
			break;
		
		case RANCH_WITHDRAW_BOX_FULL:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxSelectorPopup_Reset(boxAppMan);
				*state = RANCH_WITHDRAW_SELECT_BOX;
			}
			break;
		
		case RANCH_WITHDRAW_TRY_WITHDRAW:
			if (!BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FE58)) {
				break;
			}
			
			StringTemplate_SetNickname(boxAppMan->messageVariableBuffer, 0, BoxApp_GetPreviewedBoxMon(&boxAppMan->boxApp));
			if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_IN_CURSOR) {
				boxAppMan->releasedFrom = RELEASED_FROM_CURSOR;
			} else {
				if (BoxAppMan_StoreRanchMonInBox(boxAppMan, MAX_PC_BOXES, 0)) {
					if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_54);
						boxAppMan->releasedFrom = RELEASED_FROM_BOX;
					} else {
						boxAppMan->releasedFrom = RELEASED_FROM_PARTY;
					}
				} else {
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_AllDSBoxesFull);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					*state = RANCH_WITHDRAW_FAILED_WAIT;
					break;
				}
			}
			
			BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_WithdrawnMon);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
			*state = RANCH_WITHDRAW_WITHDRAW_OK;
			break;
		
		case RANCH_WITHDRAW_WITHDRAW_OK:
			if (MPRComm_0203C444() && BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				GF_ASSERT(BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) != PREVIEW_MON_IN_CURSOR);
				
				if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
					if (boxAppMan->mprFilter.pendingRotomRevertMsg) {
						BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_RotomReverted);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
						boxAppMan->menuTimer = 150;
						boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
						*state = RANCH_WITHDRAW_DIALOG_WAIT;
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						*state = RANCH_WITHDRAW_PREVIEW_UPDATE;
					}
				} else {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_MonWelcomeBack);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					boxAppMan->menuTimer = 75;
					*state = RANCH_WITHDRAW_DIALOG_WAIT;
				}
			}
			break;
		
		case RANCH_WITHDRAW_WELCOME_BACK:
			if ((BoxGraphics_CheckAllTasksDone(boxAppMan->display) != 0) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_MonWelcomeBack);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				boxAppMan->menuTimer = 75;
				*state = RANCH_WITHDRAW_DIALOG_WAIT;
			}
			break;
		
		case RANCH_WITHDRAW_DIALOG_WAIT:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			boxAppMan->menuTimer--;
			if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B) || boxAppMan->menuTimer == 0) {
				if (boxAppMan->mprFilter.pendingRotomRevertMsg) {
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_RotomReverted);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					boxAppMan->menuTimer = 150;
					boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
					*state = RANCH_WITHDRAW_DIALOG_WAIT;
				} else {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_WITHDRAW_PREVIEW_UPDATE;
				}
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_L | PAD_BUTTON_R | PAD_PLUS_KEY_MASK) && !boxAppMan->mprFilter.pendingRotomRevertMsg) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				BoxAppMan_ResetAction(boxAppMan);
				break;
			}
			break;
		
		case RANCH_WITHDRAW_FAILED_WAIT:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_WITHDRAW_END;
				}
			}
			break;
		
		case RANCH_WITHDRAW_PREVIEW_UPDATE:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_WITHDRAW_END;
			}
			break;
		
		case RANCH_WITHDRAW_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_RanchBatchDepositAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_BATCH_DEPOSIT_START,
		RANCH_BATCH_DEPOSIT_VALIDATE,
		RANCH_BATCH_DEPOSIT_NAVIGATE_MENU,
		RANCH_BATCH_DEPOSIT_UNK_3,
		RANCH_BATCH_DEPOSIT_UNK_4,
		RANCH_BATCH_DEPOSIT_UNK_5,
		RANCH_BATCH_DEPOSIT_UNK_6,
		RANCH_BATCH_DEPOSIT_UNK_7,
		RANCH_BATCH_DEPOSIT_UNK_8,
		RANCH_BATCH_DEPOSIT_UNK_9,
		RANCH_BATCH_DEPOSIT_UNK_10,
		RANCH_BATCH_DEPOSIT_UNK_11,
		RANCH_BATCH_DEPOSIT_UNK_12,
		RANCH_BATCH_DEPOSIT_UNK_13,
		RANCH_BATCH_DEPOSIT_UNK_14,
		RANCH_BATCH_DEPOSIT_END
	};
	
	MPRBatchOp* mprBatch = &boxAppMan->mprBatch;
	
	switch (*state) {
		case RANCH_BATCH_DEPOSIT_START: 
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton);
			*state = RANCH_BATCH_DEPOSIT_VALIDATE;
			break;
		
		case RANCH_BATCH_DEPOSIT_VALIDATE:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			{
				int totalMons = 0;
				int depositableMons = BoxAppMan_ReleaseBlockingMoveCacheUpdateAll(boxAppMan, &totalMons);
				u32 depositSpace = MPRComm_GetRanchDepositSpace();
				int messageID;
				
				if (depositableMons == 0 || depositSpace == 0) {
					if (totalMons == 0) {
						messageID = MPR_TEXT_FLAG | MPR_TEXT_ItsEmpty;
					} else if (depositableMons == 0) {
						messageID = MPR_TEXT_FLAG | MPR_TEXT_CannotDeposit;
					} else if (depositSpace == 0) {
						messageID = MPR_TEXT_FLAG | MPR_TEXT_RanchFull;
					} else {
						GF_ASSERT(FALSE);
					}
					
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					*state = RANCH_BATCH_DEPOSIT_UNK_14;
					break;
				}
				
				mprBatch->unk_00_0 = 0;
				mprBatch->unk_00_5 = (depositableMons > depositSpace) ? depositSpace : depositableMons;
				mprBatch->unk_02_0 = totalMons;
				mprBatch->unk_02_5 = 0;
				mprBatch->unk_02_A = (totalMons > depositSpace) ? depositSpace : totalMons;
				
				if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
					*state = RANCH_BATCH_DEPOSIT_UNK_8;
					break;
				}
				
				if (depositableMons > depositSpace) {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_DepositAsManyPokemonConfirm;
				} else {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_DepositAllPokemonConfirm;
				}
				
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
				BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = RANCH_BATCH_DEPOSIT_NAVIGATE_MENU;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_NAVIGATE_MENU: 
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
					case BOX_MENU_NAVIGATION_UP_DOWN:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
						break;
					
					case BOX_MENU_NO:
					case BOX_MENU_NAVIGATION_B:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						*state = RANCH_BATCH_DEPOSIT_END;
						break;
					
					case BOX_MENU_YES:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
						*state = RANCH_BATCH_DEPOSIT_UNK_3;
						break;
				}
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_3: 
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FE58)) {
				*state = RANCH_BATCH_DEPOSIT_UNK_8;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_4: 
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PreviewMon)) {
				int reasonMessageID; /* Unused */
				if (!BoxApp_CanDepositMonToRanch(&boxAppMan->boxApp, &reasonMessageID)) {
					mprBatch->unk_00_0++;
					mprBatch->unk_00_A = 0;
					*state = RANCH_BATCH_DEPOSIT_UNK_5;
					break;
				}
				
				if (!BoxAppMan_IsCursorMonDepositable(boxAppMan)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_53);
					mprBatch->unk_00_0++;
					mprBatch->unk_00_A = 0;
					*state = RANCH_BATCH_DEPOSIT_UNK_5;
					break;
				}
				
				int v0 = mprBatch->unk_00_5;
				if (mprBatch->unk_02_5 + 1 != v0) {
					v0 = -1;
				}
				BoxAppMan_RanchDepositMonUnderCursor(boxAppMan, v0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_52);
				mprBatch->unk_02_5++;
				mprBatch->unk_00_A = 0;
				*state = RANCH_BATCH_DEPOSIT_UNK_6;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_5: 
			mprBatch->unk_00_A++;
			if (mprBatch->unk_00_A > 8) {
				*state = RANCH_BATCH_DEPOSIT_UNK_8;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_6: 
			mprBatch->unk_00_A++;
			if (mprBatch->unk_00_A > 8) {
				*state = RANCH_BATCH_DEPOSIT_UNK_7;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_7: 
			if (MPRComm_IsConnected() && !MPRComm_0203C2B8()) {
				break;
			}
			*state = RANCH_BATCH_DEPOSIT_UNK_8;
			// Fall through
		
		case RANCH_BATCH_DEPOSIT_UNK_8:
			{
				int v0 = mprBatch->unk_00_0;
				if (mprBatch->unk_02_5 == mprBatch->unk_02_A) {
					v0 = MAX_MONS_PER_BOX;
				}
				
				while (v0 < MAX_MONS_PER_BOX) {
					if (BoxPokemon_GetValue(PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, -1, v0), MON_DATA_SPECIES_EXISTS, NULL)) {
						break;
					}
					v0++;
				}
				
				mprBatch->unk_00_0 = v0;
				
				if (v0 >= MAX_MONS_PER_BOX) {
					boxAppMan->boxApp.cursor.cursorLocation = CURSOR_ON_PARTY_BUTTON;
					BoxAppMan_TryPreviewCursorMon(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
						*state = RANCH_BATCH_DEPOSIT_END;
					} else {
						*state = RANCH_BATCH_DEPOSIT_UNK_9;
					}
					break;
				}
				
				boxAppMan->boxApp.cursor.cursorLocation = CURSOR_IN_BOX;
				BoxApp_SetCursorBoxLocation(&boxAppMan->boxApp, v0 % 6, v0 / 6);
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_BATCH_DEPOSIT_UNK_4;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_9:
			{
				if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
					break;
				}
				
				StringTemplate_SetNumber(boxAppMan->messageVariableBuffer, 0, mprBatch->unk_02_5, 2, PADDING_MODE_NONE, CHARSET_MODE_JP);
				
				int messageID;
				if (mprBatch->unk_02_5 < mprBatch->unk_02_0) {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_DepositedNumMons;
				} else {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_DepositedAll;
				}
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				boxAppMan->menuTimer = 75;
				*state = RANCH_BATCH_DEPOSIT_UNK_12;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_12:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->menuTimer--;
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B) || boxAppMan->menuTimer == 0) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					*state = RANCH_BATCH_DEPOSIT_UNK_13;
				}
				
				if (JOY_NEW(PAD_BUTTON_L | PAD_BUTTON_R | PAD_PLUS_KEY_MASK)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxAppMan_ResetAction(boxAppMan);
				}
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_13:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_BATCH_DEPOSIT_END;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_UNK_14:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = RANCH_BATCH_DEPOSIT_END;
			}
			break;
		
		case RANCH_BATCH_DEPOSIT_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_RanchBatchWithdrawAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_BATCH_WITHDRAW_START,
		RANCH_BATCH_WITHDRAW_VALIDATE,
		RANCH_BATCH_WITHDRAW_SELECT_BOX,
		RANCH_BATCH_WITHDRAW_CONFIRM,
		RANCH_BATCH_WITHDRAW_NAVIGATE_MENU,
		RANCH_BATCH_WITHDRAW_UNK_5,
		RANCH_BATCH_WITHDRAW_UNK_6,
		RANCH_BATCH_WITHDRAW_UNK_7,
		RANCH_BATCH_WITHDRAW_UNK_8,
		RANCH_BATCH_WITHDRAW_UNK_9,
		RANCH_BATCH_WITHDRAW_UNK_10,
		RANCH_BATCH_WITHDRAW_UNK_11,
		RANCH_BATCH_WITHDRAW_UNK_12,
		RANCH_BATCH_WITHDRAW_UNK_13,
		RANCH_BATCH_WITHDRAW_UNK_14,
		RANCH_BATCH_WITHDRAW_UNK_15,
		RANCH_BATCH_WITHDRAW_UNK_16,
		RANCH_BATCH_WITHDRAW_END
	};
	
	MPRBatchOp* mprBatch = &boxAppMan->mprBatch;
	
	switch (*state) {
		case RANCH_BATCH_WITHDRAW_START:
			boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PressBoxButton);
			*state = RANCH_BATCH_WITHDRAW_VALIDATE;
			break;
		
		case RANCH_BATCH_WITHDRAW_VALIDATE:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			{
				int messageID;
				int ranchMons = 0;
				
				for (u32 i = 0; i < MAX_MONS_PER_BOX; i++) {
					if (BoxPokemon_GetValue(BoxAppMan_GetBoxMonAt(boxAppMan, i), MON_DATA_SPECIES_EXISTS, NULL)) {
						ranchMons++;
					}
				}
				
				int pcBoxSpace = PCBoxes_CountAllEmptySpace(boxAppMan->pcBoxes);
				if (ranchMons == 0 || pcBoxSpace == 0) {
					if (ranchMons == 0) {
						messageID = MPR_TEXT_FLAG | MPR_TEXT_ItsEmpty2;
					} else if (pcBoxSpace == 0) {
						messageID = MPR_TEXT_FLAG | MPR_TEXT_AllDSBoxesFull;
					} else {
						GF_ASSERT(FALSE);
					}
					
					BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
					*state = RANCH_BATCH_WITHDRAW_UNK_16;
					break;
				}
				
				mprBatch->unk_00_0 = 0;
				mprBatch->unk_00_5 = ranchMons;
				mprBatch->unk_02_5 = 0;
				BoxSelectorPopup_Init(boxAppMan, PCBoxes_GetCurrentBoxID(boxAppMan->pcBoxes), MPR_TEXT_FLAG | MPR_TEXT_WithdrawWhichBox);
				*state = RANCH_BATCH_WITHDRAW_SELECT_BOX;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_SELECT_BOX:
			if (BoxAppMan_TrySelectBoxFromPopup(boxAppMan) == 0) {
				break;
			}
			
			{
				int boxID = boxAppMan->boxSelector.boxID;
				if (boxID == -1) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					*state = RANCH_BATCH_WITHDRAW_END;
					break;
				}
				
				u32 ranchMons = mprBatch->unk_00_5;
				u32 boxSpace = PCBoxes_CountEmptySpaceInBox(boxAppMan->pcBoxes, boxID);
				int v1;
				if (ranchMons > boxSpace) {
					v1 = boxSpace;
				} else {
					v1 = ranchMons;
				}
				mprBatch->unk_02_A = v1;
				
				if (boxSpace >= ranchMons || (BoxApp_IsCursorFastMode(&boxAppMan->boxApp) && boxSpace > 0)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, boxAppMan->boxSelector.boxID);
					*state = RANCH_BATCH_WITHDRAW_UNK_10;
					break;
				}
				
				if (boxSpace > 0) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FF48);
					*state = RANCH_BATCH_WITHDRAW_CONFIRM;
					break;
				}
				
				Sound_PlayEffect(SEQ_SE_DP_BOX03);
				int messageID;
				if (boxSpace == 0) {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_BoxFull;
				} else {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_BoxNotEnoughSpace2;
				}
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				*state = RANCH_BATCH_WITHDRAW_UNK_6;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_CONFIRM:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_0202FF48)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_WithdrawAsManyPokemonConfirm);
				BoxMenu_FillYesNo(&boxAppMan->boxApp, 0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowMenu);
				*state = RANCH_BATCH_WITHDRAW_NAVIGATE_MENU;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_NAVIGATE_MENU:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				switch (BoxMenu_GetMenuNavigation(&boxAppMan->boxApp)) {
					case BOX_MENU_NAVIGATION_UP_DOWN:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMenuCursor);
						break;
					
					case BOX_MENU_NO:
					case BOX_MENU_NAVIGATION_B:
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
						BoxSelectorPopup_Init(boxAppMan, boxAppMan->boxSelector.boxID, MPR_TEXT_FLAG | MPR_TEXT_WithdrawWhichBox);
						*state = RANCH_BATCH_WITHDRAW_SELECT_BOX;
						break;
					
					case BOX_MENU_YES:
						PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, boxAppMan->boxSelector.boxID);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202FE58);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						*state = RANCH_BATCH_WITHDRAW_UNK_5;
						break;
				}
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_5:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				*state = RANCH_BATCH_WITHDRAW_UNK_10;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_6:
			if ((BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage)) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxSelectorPopup_Reset(boxAppMan);
				*state = RANCH_BATCH_WITHDRAW_SELECT_BOX;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_7:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PreviewMon)) {
				int v0 = mprBatch->unk_02_A;
				if (mprBatch->unk_02_5 + 1 != v0) {
					v0 = -1;
				}
				
				BoxAppMan_StoreRanchMonInBox(boxAppMan, MAX_PC_BOXES, v0);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_54);
				mprBatch->unk_02_5++;
				mprBatch->unk_00_A = 0;
				*state = RANCH_BATCH_WITHDRAW_UNK_8;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_8:
			mprBatch->unk_00_A++;
			if (mprBatch->unk_00_A > 8) {
				*state = RANCH_BATCH_WITHDRAW_UNK_9;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_9:
			if (MPRComm_IsConnected() && !MPRComm_0203C444()) {
				break;
			}
			*state = RANCH_BATCH_WITHDRAW_UNK_10;
			// Fall through
		
		case RANCH_BATCH_WITHDRAW_UNK_10:
			{
				int v0 = mprBatch->unk_00_0;
				if (mprBatch->unk_02_5 == mprBatch->unk_02_A) {
					v0 = MAX_MONS_PER_BOX;
				}
				
				while (v0 < MAX_MONS_PER_BOX) {
					if (BoxPokemon_GetValue(BoxAppMan_GetBoxMonAt(boxAppMan, v0), MON_DATA_SPECIES_EXISTS, NULL)) {
						break;
					}
					v0++;
				}
				
				mprBatch->unk_00_0 = v0;
				
				if (v0 >= MAX_MONS_PER_BOX) {
					boxAppMan->boxApp.cursor.cursorLocation = CURSOR_ON_PARTY_BUTTON;
					BoxAppMan_TryPreviewCursorMon(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					if (BoxApp_IsCursorFastMode(&boxAppMan->boxApp)) {
						if (boxAppMan->mprFilter.pendingRotomRevertMsg) {
							BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_RotomReverted);
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
							boxAppMan->menuTimer = 150;
							boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
							*state = RANCH_BATCH_WITHDRAW_UNK_14;
							break;
						} else {
							*state = RANCH_BATCH_WITHDRAW_END;
							break;
						}
					} else {
						*state = RANCH_BATCH_WITHDRAW_UNK_11;
						break;
					}
				}
				
				boxAppMan->boxApp.cursor.cursorLocation = CURSOR_IN_BOX;
				BoxApp_SetCursorBoxLocation(&boxAppMan->boxApp, v0 % 6, v0 / 6);
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_MoveCursor);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_BATCH_WITHDRAW_UNK_7;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_11:
			if (!BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				break;
			}
			
			{
				StringTemplate_SetNumber(boxAppMan->messageVariableBuffer, 0, mprBatch->unk_02_5, 2, PADDING_MODE_NONE, CHARSET_MODE_JP);
				
				int messageID;
				if (mprBatch->unk_02_5 < mprBatch->unk_00_5) {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_WithdrawnNumMons;
				} else {
					messageID = MPR_TEXT_FLAG | MPR_TEXT_WithdrawnAll;
				}
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, messageID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				boxAppMan->menuTimer = 75;
				*state = RANCH_BATCH_WITHDRAW_UNK_14;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_14:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->menuTimer--;
				if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B) || boxAppMan->menuTimer == 0) {
					if (boxAppMan->mprFilter.pendingRotomRevertMsg) {
						BoxApp_SetBoxMessage(&boxAppMan->boxApp, MPR_TEXT_FLAG | MPR_TEXT_RotomReverted);
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
						boxAppMan->menuTimer = 150;
						boxAppMan->mprFilter.pendingRotomRevertMsg = FALSE;
						*state = RANCH_BATCH_WITHDRAW_UNK_14;
					} else {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
						*state = RANCH_BATCH_WITHDRAW_END;
					}
					break;
				}
				
				if (JOY_NEW(PAD_BUTTON_L | PAD_BUTTON_R | PAD_PLUS_KEY_MASK) && !boxAppMan->mprFilter.pendingRotomRevertMsg) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
					BoxAppMan_ResetAction(boxAppMan);
				}
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_15:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_BATCH_WITHDRAW_END;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_UNK_16:
			if ((BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage)) && JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)) {
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_CloseMessageBox);
				*state = RANCH_BATCH_WITHDRAW_END;
			}
			break;
		
		case RANCH_BATCH_WITHDRAW_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_DownloadRanchBoxesAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		RANCH_BOX_DOWNLOAD_START,
		RANCH_BOX_DOWNLOAD_NEXT,
		RANCH_BOX_DOWNLOAD_PREV,
		RANCH_BOX_DOWNLOAD_COMM_SYNC,
		RANCH_BOX_DOWNLOAD_END
	};
	
	u32 currentBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
	
	switch (*state) {
		case RANCH_BOX_DOWNLOAD_START:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW) {
				*state = RANCH_BOX_DOWNLOAD_END;
				break;
			}
			
			boxAppMan->mprFilter.unk_48 = TRUE;
			MPRComm_0203C3A4(currentBoxID, currentBoxID, (BoxPokemon*)boxAppMan->mprFilter.unk_84);
			*state = RANCH_BOX_DOWNLOAD_NEXT;
			break;
		
		case RANCH_BOX_DOWNLOAD_NEXT:
			if (MPRComm_0203C404()) {
				int nextBoxID = currentBoxID + 1;
				if (nextBoxID > boxAppMan->boxApp.customization.maxRanchBoxID) {
					nextBoxID = 0;
				}
				MPRComm_0203C3A4(nextBoxID, currentBoxID, (BoxPokemon*)boxAppMan->mprFilter.unk_80);
				BoxAppMan_TryPreviewCursorMon(boxAppMan);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_MPR_Unk_55);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
				*state = RANCH_BOX_DOWNLOAD_PREV;
			}
			break;
		
		case RANCH_BOX_DOWNLOAD_PREV:
			if (MPRComm_0203C404()) {
				int prevBoxID = currentBoxID - 1;
				if (prevBoxID < 0) {
					prevBoxID = boxAppMan->boxApp.customization.maxRanchBoxID;
				}
				MPRComm_0203C3A4(prevBoxID, currentBoxID, (BoxPokemon*)boxAppMan->mprFilter.unk_88);
				*state = RANCH_BOX_DOWNLOAD_COMM_SYNC;
			}
			break;
		
		case RANCH_BOX_DOWNLOAD_COMM_SYNC:
			if (MPRComm_0203C404()) {
				*state = RANCH_BOX_DOWNLOAD_END;
			}
			break;
		
		case RANCH_BOX_DOWNLOAD_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				boxAppMan->mprFilter.unk_00 = 0;
				boxAppMan->mprFilter.unk_48 = FALSE;
				boxAppMan->mprFilter.unk_4C = TRUE;
				boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxSelectorPopup_Init(BoxApplicationManager* boxAppMan, u32 boxID, u32 boxMessageID) {
	boxAppMan->boxSelector.state = BOX_SELECTOR_START;
	boxAppMan->boxSelector.boxID = boxID;
	boxAppMan->boxSelector.boxMessageID = boxMessageID;
	boxAppMan->boxSelector.hasReset = FALSE;
}


static void BoxSelectorPopup_Reset(BoxApplicationManager* boxAppMan) {
	boxAppMan->boxSelector.state = BOX_SELECTOR_START;
	boxAppMan->boxSelector.hasReset = TRUE;
}


static BOOL BoxAppMan_TrySelectBoxFromPopup(BoxApplicationManager* boxAppMan) {
	BoxSelectorPopup* boxSelector = &boxAppMan->boxSelector;
	
	switch (boxSelector->state) {
		case BOX_SELECTOR_START:
			if (boxSelector->hasReset == TRUE) {
				boxSelector->state = BOX_SELECTOR_DISPLAY_MESSAGE_START;
				break;
			}
			
			if (boxSelector->boxID == -1) {
				boxSelector->boxID = 0;
			}
			
			BoxApp_SetBoxSelectionBoxID(&boxAppMan->boxApp, boxSelector->boxID);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ShowBoxSelectionPopup);
			boxSelector->state = BOX_SELECTOR_DISPLAY_MESSAGE_START;
			break;
		
		case BOX_SELECTOR_DISPLAY_MESSAGE_START:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ShowBoxSelectionPopup)) {
				BoxApp_SetBoxMessage(&boxAppMan->boxApp, boxSelector->boxMessageID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage);
				boxSelector->state = BOX_SELECTOR_DISPLAY_MESSAGE_DONE;
			}
			break;
		
		case BOX_SELECTOR_DISPLAY_MESSAGE_DONE:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_DisplayBoxMessage) == FALSE) {
				break;
			}
			
			boxSelector->state = BOX_SELECTOR_WAIT_FOR_USER;
			// Fall through
		
		case BOX_SELECTOR_WAIT_FOR_USER:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ScrollBoxSelectionPopup) == FALSE) {
				break;
			}
			
			if (JOY_NEW(PAD_KEY_LEFT | PAD_BUTTON_L)) {
				boxSelector->boxID--;
				
				if (boxSelector->boxID < 0) {
					boxSelector->boxID = MAX_PC_BOXES - 1;
				}
				
				BoxApp_SetBoxSelectionBoxID(&boxAppMan->boxApp, boxSelector->boxID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScrollBoxSelectionPopup);
				break;
			}
			
			if (JOY_NEW(PAD_KEY_RIGHT | PAD_BUTTON_R)) {
				if (++(boxSelector->boxID) >= MAX_PC_BOXES) {
					boxSelector->boxID = 0;
				}
				
				BoxApp_SetBoxSelectionBoxID(&boxAppMan->boxApp, boxSelector->boxID);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ScrollBoxSelectionPopup);
				break;
			}
			
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				return TRUE;
			}
			
			if (JOY_NEW(PAD_BUTTON_B)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				boxSelector->boxID = -1;
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


static void BoxAppMan_ChangeToNewBoxAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		MOVE_BOX_START,
		MOVE_BOX_WAIT_FOR_ANIMATION,
		MOVE_BOX_UPDATE_CACHE,
		MOVE_BOX_END
	};
	
	switch (*state) {
		case MOVE_BOX_START:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT) {
				int selectedBoxID = boxAppMan->menuItem;
				int currentBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(boxAppMan, selectedBoxID);
				if (MPRComm_IsConnected()) {
					BoxPokemon* pcBoxStart = PCBoxes_GetBoxMonAt(BoxApp_GetPCBoxes(&boxAppMan->boxApp), currentBoxID, 0);
					MPRComm_ShowPCBox(pcBoxStart, currentBoxID);
				}
			}
			
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) {
				int selectedBoxID = boxAppMan->menuItem;
				int currentBoxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
				int maxRanchBoxID = boxAppMan->boxApp.customization.maxRanchBoxID;
				int changeDirection = boxAppMan->boxApp.customization.ranchBoxJump;
				
				if (currentBoxID == selectedBoxID) {
					BoxAppMan_ClearBoxApplicationAction(boxAppMan);
					break;
				}
				
				GF_ASSERT(maxRanchBoxID != 0);
				
				if (changeDirection == 0) {
					if (currentBoxID == 0 && selectedBoxID == maxRanchBoxID) {
						changeDirection = 1;
					} else if (currentBoxID == maxRanchBoxID && selectedBoxID == 0) {
						changeDirection = -1;
					} else {
						changeDirection = currentBoxID - selectedBoxID;
					}
					
					boxAppMan->boxApp.customization.ranchBoxJump = changeDirection;
				}
				
				int targetBoxID = currentBoxID + changeDirection;
				if (targetBoxID > maxRanchBoxID) {
					targetBoxID = 0;
				} else if (targetBoxID < 0) {
					targetBoxID = maxRanchBoxID;
				}
				
				if (changeDirection != 1 && changeDirection != -1) {
					GF_ASSERT(FALSE);
					BoxAppMan_ClearBoxApplicationAction(boxAppMan);
					break;
				}
				
				boxAppMan->menuItem = changeDirection;
				void* box;
				if (changeDirection == 1) {
					box = boxAppMan->mprFilter.unk_88;
					boxAppMan->mprFilter.unk_7C = box;
					boxAppMan->mprFilter.unk_88 = NULL;
					boxAppMan->boxApp.MPR_unk_120 = boxAppMan->mprFilter.unk_80;
				} else {
					box = boxAppMan->mprFilter.unk_80;
					boxAppMan->mprFilter.unk_8C = box;
					boxAppMan->mprFilter.unk_80 = NULL;
					boxAppMan->boxApp.MPR_unk_120 = boxAppMan->mprFilter.unk_88;
				}
				
				if (MPRComm_IsConnected()) {
					MPRComm_0203C3A4(targetBoxID, currentBoxID, (BoxPokemon*)box);
				}
				
				*state = MOVE_BOX_WAIT_FOR_ANIMATION;
			} else {
				*state = MOVE_BOX_WAIT_FOR_ANIMATION;
			}
			break;
		
		case MOVE_BOX_WAIT_FOR_ANIMATION:
			if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_WITHDRAW) {
				PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
			}
			
			BoxAppMan_TryPreviewCursorMon(boxAppMan);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
			(*state)++;
			break;
		
		case MOVE_BOX_UPDATE_CACHE:
			if ((BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox) != 0) && ((BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_DEPOSIT) || (MPRComm_IsConnected() == 0) || (MPRComm_IsAwaitingShowPCBox() != 0))) {
				if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) {
					int changeDirection = boxAppMan->menuItem;
					
					if (!MPRComm_IsConnected() || MPRComm_0203C404()) {
						GF_ASSERT(changeDirection == 1 || changeDirection == -1);
						
						if (changeDirection == 1) {
							boxAppMan->mprFilter.unk_88 = boxAppMan->mprFilter.unk_84;
							boxAppMan->mprFilter.unk_84 = boxAppMan->mprFilter.unk_80;
							boxAppMan->mprFilter.unk_80 = boxAppMan->mprFilter.unk_7C;
							boxAppMan->mprFilter.unk_7C = NULL;
						} else {
							boxAppMan->mprFilter.unk_80 = boxAppMan->mprFilter.unk_84;
							boxAppMan->mprFilter.unk_84 = boxAppMan->mprFilter.unk_88;
							boxAppMan->mprFilter.unk_88 = boxAppMan->mprFilter.unk_8C;
							boxAppMan->mprFilter.unk_8C = NULL;
						}
						
						boxAppMan->boxApp.MPR_unk_11C = boxAppMan->mprFilter.unk_84;
						boxAppMan->boxApp.MPR_unk_120 = NULL;
					} else {
						break;
					}
				}
				
				boxAppMan->boxApp.customization.ranchBoxJump = 0;
				if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX && BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == PREVIEW_MON_UNDER_CURSOR) {
					BoxAppMan_TryPreviewCursorMon(boxAppMan);
					BoxAppMan_CommUpdateFocusedMon(boxAppMan);
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					(*state)++;
					break;
				}
				
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
		
		case MOVE_BOX_END:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PreviewMon)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_TouchScreenBoxJump(BoxApplicationManager* boxAppMan, u32* state) {
	enum {
		TOUCH_BOX_JUMP_START,
		TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP,
		TOUCH_BOX_JUMP_END_SCROLL,
		TOUCH_BOX_JUMP_WAIT_FOR_BOX_CHANGE,
		TOUCH_BOX_JUMP_WAIT_FOR_MON_PREVIEW,
		TOUCH_BOX_JUMP_END
	};
	
	switch (*state) {
		case TOUCH_BOX_JUMP_START:
			Sound_PlayEffect(SEQ_SE_DP_BUTTON9);
			BoxApp_SetTouchDialSelected(&boxAppMan->boxApp, TOUCH_DIALS_BOX_JUMP);
			BoxApp_SetTouchDialOffset(&boxAppMan->boxApp, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial);
			TouchDial_Init(&boxAppMan->touchDial, 0, 192, 56, 88);
			boxAppMan->touchDialBoxChanged = FALSE;
			boxAppMan->scrollEnded = TRUE;
			*state = TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP;
			break;
		
		case TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial) == 0) {
				break;
			}
			
			if (BoxAppMan_TryPressTouchScreenButton(boxAppMan) && boxAppMan->touchScreenButtonPressed == TRUE) {
				if (BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_MOVE_ITEMS) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_MarkingsFilterAction);
				} else {
					Sound_PlayEffect(SEQ_SE_DP_BOX03);
				}
				break;
			}
			
			if (JOY_NEW(PAD_PLUS_KEY_MASK | PAD_BUTTON_A | PAD_BUTTON_B)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				BoxApp_SetTouchDialSelected(&boxAppMan->boxApp, TOUCH_DIALS_NONE);
				BoxApp_SetTouchDialOffset(&boxAppMan->boxApp, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial);
				*state = TOUCH_BOX_JUMP_END;
				break;
			}
			
			if ((boxAppMan->touchDialBoxChanged != 0) && (boxAppMan->scrollEnded != 0)) {
				boxAppMan->boxJumpDelayCounter++;
				if (boxAppMan->boxJumpDelayCounter > 20) {
					u32 destinationBoxID = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
					
					BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(boxAppMan, BoxApp_GetCurrentBox(&boxAppMan->boxApp));
					BoxApp_LoadCustomizationsFor(&boxAppMan->boxApp, destinationBoxID);
					PCBoxes_SetCurrentBox(boxAppMan->pcBoxes, destinationBoxID);
					BoxAppMan_TryPreviewCursorMon(boxAppMan);
					
					if (BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == 1) {
						BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_0202F9B0);
					}
					
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox);
					
					if ((BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT) && MPRComm_IsConnected()) {
						BoxPokemon* pcBoxStart = PCBoxes_GetBoxMonAt(BoxApp_GetPCBoxes(&boxAppMan->boxApp), destinationBoxID, 0);
						MPRComm_ShowPCBox(pcBoxStart, destinationBoxID);
					}
					
					boxAppMan->touchDialBoxChanged = FALSE;
					boxAppMan->boxJumpDelayCounter = 0;
					*state = TOUCH_BOX_JUMP_WAIT_FOR_BOX_CHANGE;
					break;
				}
			}
			
			switch (TouchDial_HandleAction(&boxAppMan->touchDial)) {
				case TOUCH_DIAL_INITIAL_TOUCH:
					boxAppMan->unused1 = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
					boxAppMan->touchDialBoxChanged = FALSE;
					boxAppMan->touchDialScrollAmount = 0;
					boxAppMan->scrollEnded = FALSE;
					break;
				
				case TOUCH_DIAL_SCROLLING:
					{
						int scrollAmount = TouchDial_CalcScrollAmount(&boxAppMan->touchDial);
						if (scrollAmount != boxAppMan->touchDialScrollAmount) {
							int scrollDelta = scrollAmount - boxAppMan->touchDialScrollAmount;
							int newOffset = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp) + scrollDelta;
							
							if (newOffset < 0) {
								newOffset += MAX_PC_BOXES;
							} else if (newOffset >= MAX_PC_BOXES) {
								newOffset -= MAX_PC_BOXES;
							}
							
							boxAppMan->touchDialScrollAmount = scrollAmount;
							BoxApp_SetTouchDialOffset(&boxAppMan->boxApp, newOffset);
							BoxApp_SetTouchDialScrollDelta(&boxAppMan->boxApp, scrollDelta);
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030284);
							Sound_PlayEffect(SEQ_SE_DP_SELECT);
							*state = TOUCH_BOX_JUMP_END_SCROLL;
						}
					}
					break;
				
				case TOUCH_DIAL_END_SCROLL:
					{
						int currentBox = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
						int selectedBox = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
						
						boxAppMan->touchDialBoxChanged = currentBox != selectedBox;
						boxAppMan->boxJumpDelayCounter = 0;
						boxAppMan->scrollEnded = TRUE;
					}
					break;
			}
			break;
		
		case TOUCH_BOX_JUMP_END_SCROLL:
			if (boxAppMan->scrollEnded == FALSE) {
				if (TouchDial_HandleAction(&boxAppMan->touchDial) == TOUCH_DIAL_END_SCROLL) {
					int currentBox = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
					int selectedBox = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
					
					boxAppMan->touchDialBoxChanged = currentBox != selectedBox;
					boxAppMan->boxJumpDelayCounter = 0;
					boxAppMan->scrollEnded = TRUE;
				}
			}
			
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_02030284)) {
				*state = TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP;
			}
			break;
		
		case TOUCH_BOX_JUMP_WAIT_FOR_BOX_CHANGE:
			if (((BoxApp_GetBoxMode(&boxAppMan->boxApp) != PC_MODE_RANCH_DEPOSIT) || (!MPRComm_IsConnected()) || (MPRComm_IsAwaitingShowPCBox() != 0)) && (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_ChangeToNewBox) != 0)) {
				if ((BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) == 1) && BoxApp_IsMonUnderCursor(&boxAppMan->boxApp)) {
					BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_PreviewMon);
					*state = TOUCH_BOX_JUMP_WAIT_FOR_MON_PREVIEW;
				} else {
					*state = TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP;
				}
			}
			break;
		
		case TOUCH_BOX_JUMP_WAIT_FOR_MON_PREVIEW:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_PreviewMon)) {
				*state = TOUCH_BOX_JUMP_WAIT_FOR_INPUT_OR_JUMP;
			}
			break;
		
		case TOUCH_BOX_JUMP_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static void BoxAppMan_MarkingsFilterAction(BoxApplicationManager* boxAppMan, u32* state) {
	enum TouchMarkingsState {
		TOUCH_MARKINGS_START,
		TOUCH_MARKINGS_WAIT_FOR_USER_INPUT,
		TOUCH_MARKINGS_END_SCROLL,
		TOUCH_MARKINGS_END
	};
	
	switch (*state) {
		case TOUCH_MARKINGS_START:
			Sound_PlayEffect(SEQ_SE_DP_BUTTON9);
			BoxApp_SetTouchDialSelected(&boxAppMan->boxApp, TOUCH_DIALS_MARKINGS);
			BoxApp_SetMarkingsButtonsScrollOffset(&boxAppMan->boxApp, 0);
			BoxApp_SetTouchDialOffset(&boxAppMan->boxApp, 0);
			BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial);
			TouchDial_Init(&boxAppMan->touchDial, 255, 192, 56, 88);
			boxAppMan->touchDialBoxChanged = FALSE;
			boxAppMan->scrollEnded = TRUE;
			*state = TOUCH_MARKINGS_WAIT_FOR_USER_INPUT;
			break;
		
		case TOUCH_MARKINGS_WAIT_FOR_USER_INPUT:
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial) == FALSE) {
				break;
			}
			
			if (BoxAppMan_TryPressTouchScreenButton(boxAppMan)) {
				if (boxAppMan->touchScreenButtonPressed == MAIN_PC_LEFT_BUTTON_ID) {
					BoxAppMan_RegisterBoxApplicationAction(boxAppMan, BoxAppMan_TouchScreenBoxJump);
					break;
				}
			}
			
			if (JOY_NEW(PAD_PLUS_KEY_MASK | PAD_BUTTON_A | PAD_BUTTON_B)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				BoxApp_SetTouchDialSelected(&boxAppMan->boxApp, TOUCH_DIALS_NONE);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial);
				*state = TOUCH_MARKINGS_END;
			}
			
			if (BoxAppMan_TryPressMarkingsButton(boxAppMan)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_UpdateMarkingsTouchButton);
				BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_SetMonMarkingsTransparency);
				break;
			}
			
			switch (TouchDial_HandleAction(&boxAppMan->touchDial)) {
				case TOUCH_DIAL_INITIAL_TOUCH:
					boxAppMan->unused1 = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
					boxAppMan->touchDialBoxChanged = FALSE;
					boxAppMan->touchDialScrollAmount = 0;
					boxAppMan->scrollEnded = FALSE;
					break;
				
				case TOUCH_DIAL_SCROLLING:
					{
						int scrollAmount = TouchDial_CalcScrollAmount(&boxAppMan->touchDial);

						if (scrollAmount != boxAppMan->touchDialScrollAmount) {
							int scrollDelta = scrollAmount - boxAppMan->touchDialScrollAmount;
							int newOffset = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp) + scrollDelta;

							if (newOffset < 0) {
								newOffset += PC_MARKINGS_BUTTON_TYPES;
							} else if (newOffset >= PC_MARKINGS_BUTTON_TYPES) {
								newOffset -= PC_MARKINGS_BUTTON_TYPES;
							}

							boxAppMan->touchDialScrollAmount = scrollAmount;

							BoxApp_SetTouchDialOffset(&boxAppMan->boxApp, newOffset);
							BoxApp_SetMarkingsButtonsScrollOffset(&boxAppMan->boxApp, newOffset);
							BoxApp_SetTouchDialScrollDelta(&boxAppMan->boxApp, scrollDelta);
							BoxGraphics_TaskHandler(boxAppMan->display, FUNC_BoxGraphics_02030284);
							Sound_PlayEffect(SEQ_SE_DP_SELECT);
							*state = TOUCH_MARKINGS_END_SCROLL;
						}
					}
					break;
				
				case TOUCH_DIAL_END_SCROLL:
					{
						// Seems like GF just copy-pasted the code from the box jump touch dial into this section for the markings touch dial
						// Somehow surprisingly doesn't cause any bugs
						int currentBox = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
						int markingsScrollAmount = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
						
						boxAppMan->touchDialBoxChanged = currentBox != markingsScrollAmount;
						boxAppMan->boxJumpDelayCounter = 0;
						boxAppMan->scrollEnded = TRUE;
					}
					break;
			}
			break;
		
		case TOUCH_MARKINGS_END_SCROLL:
			if (boxAppMan->scrollEnded == FALSE) {
				if (TouchDial_HandleAction(&boxAppMan->touchDial) == TOUCH_DIAL_END_SCROLL) {
					// Also looks copy-pasted from the box scroll
					int currentBox = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
					int markingsScrollAmount = BoxApp_GetTouchDialOffset(&boxAppMan->boxApp);
					
					boxAppMan->touchDialBoxChanged = currentBox != markingsScrollAmount;
					boxAppMan->boxJumpDelayCounter = 0;
					boxAppMan->scrollEnded = TRUE;
				}
			}
			
			if (BoxGraphics_IsSysTaskDone(boxAppMan->display, FUNC_BoxGraphics_02030284)) {
				*state = TOUCH_MARKINGS_WAIT_FOR_USER_INPUT;
			}
			break;
		
		case TOUCH_MARKINGS_END:
			if (BoxGraphics_CheckAllTasksDone(boxAppMan->display)) {
				BoxAppMan_ClearBoxApplicationAction(boxAppMan);
			}
			break;
	}
}


static BOOL BoxAppMan_TryPressMarkingsButton(BoxApplicationManager* boxAppMan) {
	boxAppMan->touchScreenMarkingsButtonPressed = PC_MARKINGS_BUTTON_TYPES;
	
	TouchScreenActions_HandleAction(boxAppMan->markingsButtonsAction);
	
	if (boxAppMan->touchScreenMarkingsButtonPressed != PC_MARKINGS_BUTTON_TYPES) {
		return TRUE;
	}
	
	return FALSE;
}


static void BoxApp_BoxTouchScreenMarkingsButtonHandler(u32 buttonIndex, TouchScreenButtonState buttonTouchState, void* context) {
	BoxApplicationManager* boxAppMan = (BoxApplicationManager*)context;
	
	if (boxAppMan->touchScreenMarkingsButtonPressed == PC_MARKINGS_BUTTON_TYPES) {
		if (buttonTouchState == TOUCH_BUTTON_PRESSED) {
			u32 mask = BoxApp_GetMarkingsButtonsScrollOffset(&boxAppMan->boxApp);
			mask += buttonIndex;
			
			if (mask >= PC_MARKINGS_BUTTON_TYPES) {
				mask -= PC_MARKINGS_BUTTON_TYPES;
			}
			
			BoxApp_SetMonSpriteTransparencyMask(&boxAppMan->boxApp, mask);
			boxAppMan->touchScreenMarkingsButtonPressed = buttonIndex;
		}
	}
}


static void BoxAppMan_Load(BoxApplicationManager* boxAppMan, PokemonStorageSession* pokemonStorageSession) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	boxAppMan->pcBoxes = SaveData_GetPCBoxes(pokemonStorageSession->saveData);
	boxAppMan->saveData = pokemonStorageSession->saveData;
	boxAppMan->party = SaveData_GetParty(pokemonStorageSession->saveData);
	boxAppMan->options = SaveData_GetOptions(pokemonStorageSession->saveData);
	boxAppMan->pokemonStorageSession = pokemonStorageSession;
	pokemonStorageSession->recordBoxUseInJournal = FALSE;
	boxAppMan->releaseMon.checkedCanReleaseMon = TRUE;
	
	boxAppMan->boxMessagesLoader = MessageLoader_Init(MSG_LOADER_PRELOAD_ENTIRE_BANK, narc, TEXT_BANK_UNIFIED_BOX_MESSAGES, HEAP_ID_BOX_DATA);
	boxAppMan->speciesNameLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SPECIES_NAME, HEAP_ID_BOX_DATA);
	boxAppMan->natureNameLoader = MessageLoader_Init(MSG_LOADER_PRELOAD_ENTIRE_BANK, narc, TEXT_BANK_UNIFIED_NATURE_NAMES, HEAP_ID_BOX_DATA);
	boxAppMan->abilityNameLoader = MessageLoader_Init(MSG_LOADER_PRELOAD_ENTIRE_BANK, narc, TEXT_BANK_UNIFIED_ABILITY_NAMES, HEAP_ID_BOX_DATA);
	boxAppMan->messageVariableBuffer = StringTemplate_Default(HEAP_ID_BOX_DATA);
	boxAppMan->mon = Heap_Alloc(HEAP_ID_BOX_DATA, Pokemon_GetWorkSize());
	
	GF_ASSERT(boxAppMan->messageVariableBuffer != NULL);
	
	boxAppMan->namingScreenArgs = NamingScreenArgs_Init(HEAP_ID_BOX_DATA, NAMING_SCREEN_TYPE_BOX, 0, BOX_NAME_LEN, boxAppMan->options);
	if (pokemonStorageSession->boxMode == PC_MODE_RANCH_WITHDRAW) {
		boxAppMan->mprFilter.unk_00 = 0;
		boxAppMan->mprFilter.currSettings.unk_00 = 0;
		boxAppMan->mprFilter.currSettings.species = -1;
		boxAppMan->mprFilter.currSettings.type = -1;
		boxAppMan->mprFilter.currSettings.move = -1;
		boxAppMan->mprFilter.currSettings.ability = -1;
		boxAppMan->mprFilter.currSettings.nature = -1;
		boxAppMan->mprFilter.currSettings.mark = -1;
		
		boxAppMan->mprFilter.appliedSettings = boxAppMan->mprFilter.currSettings;
		
		boxAppMan->mprFilter.touchScreenButtonPressedPtr = &boxAppMan->touchScreenButtonPressed;
		boxAppMan->mprFilter.unk_48 = FALSE;
		boxAppMan->mprFilter.unk_4C = FALSE;
		boxAppMan->mainBoxAndCompareButtonsAction = BoxMPR_0203B260(&boxAppMan->mprFilter);
	
	} else if (pokemonStorageSession->boxMode != PC_MODE_COMPARE) {
		boxAppMan->mainBoxAndCompareButtonsAction = TouchScreenActions_RegisterHandler(
			sMainPcButtons,
			NELEMS(sMainPcButtons),
			BoxAppMan_BoxTouchScreenButtonHandler,
			boxAppMan,
			HEAP_ID_BOX_DATA);
	} else {
		boxAppMan->mainBoxAndCompareButtonsAction = TouchScreenActions_RegisterHandler(
			sComparePokemonButtons,
			NELEMS(sComparePokemonButtons),
			BoxAppMan_BoxTouchScreenButtonHandler,
			boxAppMan,
			HEAP_ID_BOX_DATA);
	}
	
	boxAppMan->markingsButtonsAction = TouchScreenActions_RegisterHandler(sPokemonMarkingsButtons, NELEMS(sPokemonMarkingsButtons), BoxApp_BoxTouchScreenMarkingsButtonHandler, boxAppMan, HEAP_ID_BOX_DATA);
	boxAppMan->boxApp.pcBoxes = boxAppMan->pcBoxes;
	boxAppMan->boxApp.party = boxAppMan->party;
	boxAppMan->boxApp.selectedBoxID = 0;
	boxAppMan->boxApp.cursorItem = ITEM_NONE;
	boxAppMan->applicationManager = NULL;
	
	BoxAppMan_CommBufferRanchMons(boxAppMan, pokemonStorageSession->boxMode);
	
	BoxSettings_Init(&boxAppMan->boxApp.boxSettings, pokemonStorageSession->boxMode);
	PCMonPreview_Init(&boxAppMan->boxApp.pcMonPreview);
	PCBoxes_InitCustomization(boxAppMan->pcBoxes, &boxAppMan->boxApp.customization, pokemonStorageSession->boxMode);
	PCCompareHelper_Init(&boxAppMan->boxApp.compareModeHelper);
	BoxAppMan_InitCursor(boxAppMan);
	BoxMonSelection_Init(&boxAppMan->boxApp.selection);
	BoxTouchDialHelper_Init(&boxAppMan->boxApp.touchDialHelper);
}


static void BoxAppMan_Free(BoxApplicationManager* boxAppMan) {
	TouchScreenActions_Free(boxAppMan->markingsButtonsAction);
	TouchScreenActions_Free(boxAppMan->mainBoxAndCompareButtonsAction);
	
	if (boxAppMan->mon) {
		Heap_Free(boxAppMan->mon);
	}
	
	StringTemplate_Free(boxAppMan->messageVariableBuffer);
	MessageLoader_Free(boxAppMan->boxMessagesLoader);
	MessageLoader_Free(boxAppMan->speciesNameLoader);
	MessageLoader_Free(boxAppMan->natureNameLoader);
	MessageLoader_Free(boxAppMan->abilityNameLoader);
	NamingScreenArgs_Free(boxAppMan->namingScreenArgs);
	
	PCMonPreview_Free(&boxAppMan->boxApp.pcMonPreview);
	BoxMonSelection_Free(&boxAppMan->boxApp.selection);
	Customization_Free(&boxAppMan->boxApp.customization);
	PCCompareHelper_Free(&boxAppMan->boxApp.compareModeHelper);
	
	Heap_Free(boxAppMan);
}


static void BoxAppMan_CommBufferRanchMons(BoxApplicationManager* boxAppMan, BoxMode boxMode) {
	if (boxMode == PC_MODE_RANCH_WITHDRAW) {
		u32 pokemonSize = BoxPokemon_GetStructSize();
		(void)BoxPokemon_GetStructSize(); // Unused
		
		boxAppMan->mprFilter.unk_80 = MPRComm_GetBoxBuffer(1);
		boxAppMan->mprFilter.unk_84 = MPRComm_GetBoxBuffer(0);
		boxAppMan->mprFilter.unk_88 = MPRComm_GetBoxBuffer(2);
		
		// First BoxPokemon is inited, then copied to the other 29
		BoxPokemon_Init((BoxPokemon*)boxAppMan->mprFilter.unk_80);
		for (int i = 1; i < MAX_MONS_PER_BOX; i++) {
			MI_CpuCopy32(boxAppMan->mprFilter.unk_80, &boxAppMan->mprFilter.unk_80[i * pokemonSize], pokemonSize);
		}
		
		// Box of 30 mons is copied to the other banks
		MI_CpuCopy32(boxAppMan->mprFilter.unk_80, boxAppMan->mprFilter.unk_88, 0x1000);
		if (!MPRComm_IsConnected()) {
			MI_CpuCopy32(boxAppMan->mprFilter.unk_80, boxAppMan->mprFilter.unk_84, 0x1000);
		}
	} else {
		boxAppMan->mprFilter.unk_84 = NULL;
		boxAppMan->mprFilter.unk_80 = NULL;
		boxAppMan->mprFilter.unk_88 = NULL;
	}
	
	boxAppMan->mprFilter.unk_7C = NULL;
	boxAppMan->mprFilter.unk_8C = NULL;
	boxAppMan->boxApp.MPR_unk_11C = boxAppMan->mprFilter.unk_84;
	boxAppMan->boxApp.MPR_unk_120 = NULL;
}


static void BoxSettings_Init(BoxSettings* boxSettings, BoxMode boxMode) {
	boxSettings->boxMode = boxMode;
	boxSettings->isCursorFastMode = FALSE;
	boxSettings->monSpriteTransparencyMask = 0;
}


static void BoxAppMan_InitCursor(BoxApplicationManager* boxAppMan) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	
	BoxApp_SetCursorBoxLocation(boxApp, 0, 0);
	
	cursor->posInParty = 0;
	cursor->prevCursorLocation = CURSOR_IN_PARTY;

	switch (boxApp->boxSettings.boxMode) {
		case PC_MODE_DEPOSIT:
			cursor->cursorLocation = CURSOR_IN_PARTY;
			break;
		
		case PC_MODE_COMPARE:
			cursor->cursorLocation = CURSOR_ON_BOX_HEADER;
			break;
		
		default:
			cursor->cursorLocation = CURSOR_IN_BOX;
			break;
	}
	
	cursor->previewMonSource = PREVIEW_MON_UNDER_CURSOR;
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static void BoxMonSelection_Init(BoxMonSelection* selection) {
	selection->boxMon = Heap_Alloc(HEAP_ID_BOX_DATA, MAX_MONS_PER_BOX * BoxPokemon_GetStructSize());
	selection->selectedMonCount = 0;
	selection->cursorMonIsPartyMon = FALSE;
}


static void BoxMonSelection_Free(BoxMonSelection* selection) {
	Heap_Free(selection->boxMon);
}


static void PCBoxes_InitCustomization(PCBoxes* pcBoxes, BoxCustomization* customization, BoxMode mode) {
	u8 maxBoxID;
	if (mode == PC_MODE_RANCH_WITHDRAW) {
		customization->boxID = 0;
		maxBoxID = 33;
	} else {
		customization->boxID = PCBoxes_GetCurrentBoxID(pcBoxes);
		maxBoxID = MAX_PC_BOXES - 1;
	}
	
	customization->maxRanchBoxID = maxBoxID;
	customization->ranchBoxJump = 0;
	customization->name = String_Init(PC_BOX_NAME_BUFFER_LEN, HEAP_ID_BOX_DATA);
	customization->ranchName = String_Init(PC_BOX_NAME_BUFFER_LEN, HEAP_ID_BOX_DATA);
	customization->ranchStringTemplate = StringTemplate_New(1, 4, HEAP_ID_BOX_DATA);
	PCBoxes_LoadCustomization(pcBoxes, customization, mode);
}


static void Customization_Free(BoxCustomization* customization) {
	StringTemplate_Free(customization->ranchStringTemplate);
	String_Free(customization->ranchName);
	String_Free(customization->name);
}


static void PCMonPreview_Init(PCMonPreview* preview) {
	preview->nickname     = String_Init(12, HEAP_ID_BOX_DATA);
	preview->speciesName  = String_Init(12, HEAP_ID_BOX_DATA);
	preview->heldItemName = String_Init(18, HEAP_ID_BOX_DATA);
	preview->nature       = String_Init(12, HEAP_ID_BOX_DATA);
	preview->ability      = String_Init(16, HEAP_ID_BOX_DATA);
}


static void PCMonPreview_Free(PCMonPreview* preview) {
	String_Free(preview->nickname);
	String_Free(preview->speciesName);
	String_Free(preview->heldItemName);
	String_Free(preview->nature);
	String_Free(preview->ability);
}


static void PCCompareHelper_Init(CompareModeHelper* compareHelper) {
	compareHelper->compareMonSlot = 0;
	compareHelper->compareMode = COMPARE_BATTLE_STATS;
	compareHelper->compareButtonAnimationPressed = FALSE;
	
	for (int i = 0; i < 2; i++) {
		compareHelper->compareSlotHasMon[i] = FALSE;
		compareHelper->compareMons[i].monName = String_Init(12, HEAP_ID_BOX_DATA);
		compareHelper->compareMons[i].nature = String_Init(12, HEAP_ID_BOX_DATA);
	}
}


static void PCCompareHelper_Free(CompareModeHelper* compareHelper) {
	for (int i = 0; i < 2; i++) {
		String_Free(compareHelper->compareMons[i].monName);
		String_Free(compareHelper->compareMons[i].nature);
	}
}


static void BoxTouchDialHelper_Init(BoxTouchDialHelper* touchDialHelper) {
	touchDialHelper->selectedTouchDial = TOUCH_DIALS_NONE;
	touchDialHelper->touchDialOffset = 0;
	touchDialHelper->markingsButtonsScrollOffset = 0;
	touchDialHelper->scrollDelta = 0;
}


void PCBoxes_LoadCustomization(const PCBoxes* pcBoxes, BoxCustomization* customization, BoxMode boxMode) {
	if (boxMode != PC_MODE_RANCH_WITHDRAW) {
		customization->wallpaper = PCBoxes_GetWallpaper(pcBoxes, customization->boxID);
		PCBoxes_BufferBoxName(pcBoxes, customization->boxID, customization->name);
	} else {
		MPRText_CopyChars(customization->ranchName, MPR_TEXT_RanchName);
		StringTemplate_SetNumber(customization->ranchStringTemplate, 0, customization->boxID + 1, 2, PADDING_MODE_ZEROES, CHARSET_MODE_JP);
		StringTemplate_Format(customization->ranchStringTemplate, customization->name, customization->ranchName);
	}
}


static void BoxApp_LoadWallpaper(BoxApplication* boxApp, PCBoxes* pcBoxes) {
	BoxCustomization* customization = &boxApp->customization;
	customization->wallpaper = PCBoxes_GetWallpaper(pcBoxes, customization->boxID);
}


static BOOL BoxAppMan_TryMoveCursorFromUserInput(u32 heldKeys, BoxApplicationManager* boxAppMan) {
	BOOL cursorMoved = FALSE;
	
	if (heldKeys & PAD_KEY_LEFT) {
		cursorMoved = BoxApp_TryMoveCursor(&boxAppMan->boxApp, -1, 0);
	} else if (heldKeys & PAD_KEY_RIGHT) {
		cursorMoved = BoxApp_TryMoveCursor(&boxAppMan->boxApp, 1, 0);
	} else if (heldKeys & PAD_KEY_UP) {
		cursorMoved = BoxApp_TryMoveCursor(&boxAppMan->boxApp, 0, -1);
	} else if (heldKeys & PAD_KEY_DOWN) {
		cursorMoved = BoxApp_TryMoveCursor(&boxAppMan->boxApp, 0, 1);
	}
	
	if (cursorMoved == TRUE) {
		BoxAppMan_TryPreviewCursorMon(boxAppMan);
		BoxAppMan_CommUpdateFocusedMon(boxAppMan);
	}
	
	return cursorMoved;
}


static BOOL BoxApp_TryMoveCursor(BoxApplication* boxApp, int colChange, int rowChange) {
	BoxCursor* cursor = &boxApp->cursor;
	
	switch (cursor->cursorLocation) {
		case CURSOR_IN_BOX:
			if (colChange != 0) {
				int newCol = cursor->boxCol += colChange;
				
				if (newCol < 0) {
					newCol = MAX_PC_COLS - 1;
				} else if (newCol >= MAX_PC_COLS) {
					newCol = 0;
				}
				
				BoxApp_SetCursorBoxLocation(boxApp, newCol, cursor->boxRow);
				
				return TRUE;
			} else if (rowChange != 0) {
				int newRow = cursor->boxRow + rowChange;
				
				if (newRow < 0) {
					newRow = 0;
					cursor->cursorLocation = CURSOR_ON_BOX_HEADER;
				} else if (newRow > 4) {
					newRow = 4;
					cursor->cursorLocation = cursor->boxCol == MAX_PC_COLS - 1 ? CURSOR_ON_CLOSE_BUTTON : CURSOR_ON_PARTY_BUTTON;
				}
				
				BoxApp_SetCursorBoxLocation(boxApp, cursor->boxCol, newRow);
				return TRUE;
			}
			break;
		
		case CURSOR_ON_BOX_HEADER:
			if (rowChange > 0) {
				cursor->cursorLocation = CURSOR_IN_BOX;
				BoxApp_SetCursorBoxLocation(boxApp, cursor->boxCol, 0);
				return TRUE;
			} else if (rowChange < 0) {
				cursor->cursorLocation = cursor->boxCol == MAX_PC_COLS - 1 ? CURSOR_ON_CLOSE_BUTTON : CURSOR_ON_PARTY_BUTTON;
				return TRUE;
			}
			break;
		
		case CURSOR_ON_CLOSE_BUTTON:
			if (rowChange < 0) {
				cursor->cursorLocation = CURSOR_IN_BOX;
				BoxApp_SetCursorBoxLocation(boxApp, cursor->boxCol, 4);
				return TRUE;
			} else if (rowChange > 0) {
				cursor->cursorLocation = CURSOR_ON_BOX_HEADER;
				return TRUE;
			} else if (colChange != 0) {
				cursor->cursorLocation = CURSOR_ON_PARTY_BUTTON;
				return TRUE;
			}
			break;
		
		case CURSOR_ON_PARTY_BUTTON:
			if (rowChange < 0) {
				cursor->cursorLocation = CURSOR_IN_BOX;
				BoxApp_SetCursorBoxLocation(boxApp, cursor->boxCol, 4);
				return TRUE;
			}
			
			if (rowChange > 0) {
				cursor->cursorLocation = CURSOR_ON_BOX_HEADER;
				return TRUE;
			}
			
			if (colChange != 0) {
				cursor->cursorLocation = CURSOR_ON_CLOSE_BUTTON;
				return TRUE;
			}
			break;
		
		case CURSOR_IN_PARTY:
			{
				u32 initialPosition = cursor->posInParty;
				
				if (rowChange < 0) {
					if (initialPosition == MAX_PARTY_SIZE) {
						cursor->posInParty = 5;
					} else {
						cursor->posInParty -= 2;

						if (cursor->posInParty < 0) {
							cursor->posInParty = MAX_PARTY_SIZE;
						}
					}
				} else if (rowChange > 0) {
					if (initialPosition == MAX_PARTY_SIZE) {
						cursor->posInParty = 0;
					} else {
						cursor->posInParty += 2;

						if (cursor->posInParty >= MAX_PARTY_SIZE) {
							cursor->posInParty = MAX_PARTY_SIZE;
						}
					}
				} else if (colChange < 0) {
					if (initialPosition & 1) {
						cursor->posInParty -= 1;
					}
				} else if (colChange > 0) {
					if (initialPosition != MAX_PARTY_SIZE) {
						if ((initialPosition & 1) == 0) {
							cursor->posInParty += 1;
						}
					}
				}
				
				if (initialPosition != cursor->posInParty) {
					return TRUE;
				}
			}
			break;
	}
	
	return FALSE;
}


static CursorMovementState BoxAppMan_TryMoveSelectionFromUserInput(u32 heldKeys, BoxApplicationManager* boxAppMan) {
	CursorMovementState cursorMovement = CURSOR_NO_MOVEMENT;
	
	if (heldKeys & PAD_KEY_LEFT) {
		cursorMovement = BoxApp_TryMoveSelection(&boxAppMan->boxApp, -1, 0);
	} else if (heldKeys & PAD_KEY_RIGHT) {
		cursorMovement = BoxApp_TryMoveSelection(&boxAppMan->boxApp, 1, 0);
	} else if (heldKeys & PAD_KEY_UP) {
		cursorMovement = BoxApp_TryMoveSelection(&boxAppMan->boxApp, 0, -1);
	} else if (heldKeys & PAD_KEY_DOWN) {
		cursorMovement = BoxApp_TryMoveSelection(&boxAppMan->boxApp, 0, 1);
	} else if (heldKeys & PAD_BUTTON_L) {
		cursorMovement = CURSOR_MOVE_TO_LEFT_BOX;
	} else if (heldKeys & PAD_BUTTON_R) {
		cursorMovement = CURSOR_MOVE_TO_RIGHT_BOX;
	}
	
	if (cursorMovement == CURSOR_MOVE) {
		BoxAppMan_TryPreviewCursorMon(boxAppMan);
	}
	
	return cursorMovement;
}


static CursorMovementState BoxApp_TryMoveSelection(BoxApplication* boxApp, int colChange, int rowChange) {
	BoxCursor* cursor = &boxApp->cursor;
	const BoxMonSelection* selection = &boxApp->selection;
	
	if (cursor->cursorLocation == CURSOR_IN_BOX) {
		if (colChange != 0) {
			int newCol = cursor->boxCol + colChange;
			
			if (BoxApp_GetPreviewMonSource(boxApp) & PREVIEW_MON_SELECTION) {
				int selectionLeftCol, selectionRightCol;
				
				selectionLeftCol = selectionRightCol = newCol;
				
				if (selection->selectionEndCol > selection->selectionStartCol) {
					selectionLeftCol -= selection->selectionEndCol - selection->selectionStartCol;
				} else {
					selectionRightCol += selection->selectionStartCol - selection->selectionEndCol;
				}
				
				if (selectionLeftCol < 0) {
					return CURSOR_MOVE_TO_LEFT_BOX;
				}
				
				if (selectionRightCol >= MAX_PC_COLS) {
					return CURSOR_MOVE_TO_RIGHT_BOX;
				}
			} else {
				if (newCol < 0 || newCol >= MAX_PC_COLS) {
					return CURSOR_STOP;
				}
			}
			
			BoxApp_SetCursorBoxLocation(boxApp, newCol, cursor->boxRow);
			return CURSOR_MOVE;
		}
		
		if (rowChange != 0) {
			int newRow = cursor->boxRow + rowChange;
			
			if (BoxApp_GetPreviewMonSource(boxApp) & PREVIEW_MON_SELECTION) {
				int selectionTopRow, selectionBottomRow;
				
				selectionTopRow = selectionBottomRow = newRow;
				
				if (selection->selectionEndRow > selection->selectionStartRow) {
					selectionTopRow -= selection->selectionEndRow - selection->selectionStartRow;
				} else {
					selectionBottomRow += selection->selectionStartRow - selection->selectionEndRow;
				}

				if (selectionTopRow < 0 || selectionBottomRow >= MAX_PC_ROWS) {
					return CURSOR_STOP;
				}
			} else {
				if (newRow < 0 || newRow >= MAX_PC_ROWS) {
					return CURSOR_STOP;
				}
			}
			
			BoxApp_SetCursorBoxLocation(boxApp, cursor->boxCol, newRow);
			return CURSOR_MOVE;
		}
	}
	
	GF_ASSERT(FALSE);
	return CURSOR_NO_MOVEMENT;
}


static void BoxAppMan_MoveCursorToParty(BoxApplicationManager* boxAppMan) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	
	if (BoxApp_GetPreviewMonSource(boxApp) == PREVIEW_MON_IN_CURSOR) {
		cursor->posInParty = Party_GetCurrentCount(boxAppMan->party);
		
		if (cursor->posInParty >= MAX_PARTY_SIZE) {
			cursor->posInParty = MAX_PARTY_SIZE - 1;
		}
	} else {
		cursor->posInParty = 0;
	}
	
	cursor->prevCursorLocation = cursor->cursorLocation;
	cursor->cursorLocation = CURSOR_IN_PARTY;
	
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static void BoxAppMan_ReturnCursorToBox(BoxApplicationManager* boxAppMan) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	cursor->cursorLocation = CURSOR_IN_BOX;
	
	if (cursor->prevCursorLocation != CURSOR_IN_BOX) {
		BoxApp_SetCursorBoxLocation(&boxAppMan->boxApp, 0, 0);
	}
	
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


void BoxAppMan_TryPreviewCursorMon(BoxApplicationManager* boxAppMan) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	
	cursor->isMonUnderCursor = FALSE;
	switch (cursor->cursorLocation) {
		case CURSOR_IN_BOX:
			cursor->mon = BoxAppMan_GetBoxMonAt(boxAppMan, cursor->posInBox);
			break;
		
		case CURSOR_IN_PARTY:
			if (cursor->posInParty < Party_GetCurrentCount(boxAppMan->party)) {
				cursor->mon = Party_GetPokemonBySlotIndex(boxAppMan->party, cursor->posInParty);
			} else {
				cursor->mon = NULL;
			}
			break;
		
		default:
		case CURSOR_ON_BOX_HEADER:
		case CURSOR_ON_CLOSE_BUTTON:
		case CURSOR_ON_PARTY_BUTTON:
			cursor->mon = NULL;
			break;
	}
	
	if (cursor->mon != NULL) {
		if (BoxPokemon_GetValue((BoxPokemon*)cursor->mon, MON_DATA_SPECIES_EXISTS, NULL)) {
			cursor->isMonUnderCursor = TRUE;
			if (!(BoxApp_GetPreviewMonSource(&boxAppMan->boxApp) & PREVIEW_MON_HELD)) {
				BoxApp_PreviewBoxMon(&boxAppMan->boxApp, (BoxPokemon*)cursor->mon, boxAppMan);
			}
		}
	}
}


static BOOL BoxAppMan_TryPressTouchScreenButton(BoxApplicationManager* boxAppMan) {
	boxAppMan->touchScreenButtonPressed = 0xFFFF;
	TouchScreenActions_HandleAction(boxAppMan->mainBoxAndCompareButtonsAction);
	
	return boxAppMan->touchScreenButtonPressed != 0xFFFF;
}


static void BoxAppMan_BoxTouchScreenButtonHandler(u32 buttonIndex, TouchScreenButtonState buttonTouchState, void* context) {
	BoxApplicationManager* boxAppMan = (BoxApplicationManager*)context;
	
	if (buttonTouchState == TOUCH_BUTTON_PRESSED && boxAppMan->touchScreenButtonPressed == 0xFFFF) {
		boxAppMan->touchScreenButtonPressed = buttonIndex;
	}
	
	if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_COMPARE) {
		if (buttonIndex == COMPARE_MON_PC_BUTTON_ID) {
			switch (buttonTouchState) {
				case TOUCH_BUTTON_PRESSED:
					BoxApp_SetCompareButtonPressed(&boxAppMan->boxApp, TRUE);
					break;
				
				case TOUCH_BUTTON_RELEASED:
				case TOUCH_BUTTON_HELD_OUT_OF_BOUNDS:
					BoxApp_SetCompareButtonPressed(&boxAppMan->boxApp, FALSE);
					break;
			}
		}
	}
}


static void BoxApp_SetBoxMessage(BoxApplication* boxApp, u32 boxMessageID) {
	boxApp->boxMessageID = boxMessageID;
}


static void BoxApp_SetCursorBoxLocation(BoxApplication* boxApp, u32 col, u32 row) {
	BoxCursor* cursor = &boxApp->cursor;
	
	cursor->boxCol = col;
	cursor->boxRow = row;
	cursor->posInBox = row * MAX_PC_COLS + col;
}


static void BoxAppMan_PickUpMon(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	BoxMonSelection* selection = &boxApp->selection;
	BoxCursor* cursor = &boxApp->cursor;
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
		MI_CpuCopy32(cursor->mon, selection->boxMon, BoxPokemon_GetStructSize());
		PCBoxes_InitBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox);
		selection->cursorMonIsPartyMon = FALSE;
	} else {
		MI_CpuCopy32(cursor->mon, selection->boxMon, Pokemon_GetStructSize());
		Party_RemovePokemonBySlotIndex(boxAppMan->party, cursor->posInParty);
		selection->cursorMonIsPartyMon = TRUE;
	}
	
	BoxApp_SetPreviewedBoxMon(boxApp, selection->boxMon);
	
	selection->selectionStartCol = selection->selectionEndCol = cursor->boxCol;
	selection->selectionStartRow = selection->selectionEndRow = cursor->boxRow;
	
	boxApp->cursor.previewMonSource = PREVIEW_MON_IN_CURSOR;
	boxApp->cursor.isMonUnderCursor = FALSE;
}


void BoxAppMan_PickUpMultiSelectedMons(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	u32 col, row, selectionLeftCol, selectionRightCol, selectionTopRow, selectionBottomRow, monPosInBox;
	BoxPokemon* boxMon;
	
	BoxMonSelection* selection = &boxApp->selection;
	selection->unused = 1;
	
	u32 cursorPosInBox = BoxApp_GetCursorBoxPosition(boxApp);
	u32 boxMonSize = BoxPokemon_GetStructSize();
	u32 processedMonCount = 0;
	void* cursorMonBuffer = selection->boxMon;
	
	BoxApp_GetMultiSelectBoundingBox(boxApp, &selectionLeftCol, &selectionRightCol, &selectionTopRow, &selectionBottomRow);
	
	for (row = selectionTopRow; row <= selectionBottomRow; row++) {
		monPosInBox = row * MAX_PC_COLS + selectionLeftCol;
		
		for (col = selectionLeftCol; col <= selectionRightCol; col++) {
			boxMon = PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, monPosInBox);
			
			if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
				MI_CpuCopy32(boxMon, cursorMonBuffer, boxMonSize);
				PCBoxes_InitBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, monPosInBox);
				cursorMonBuffer += boxMonSize;
				selection->selectedMonsOrigBoxPos[processedMonCount++] = monPosInBox;
				
				if (monPosInBox == cursorPosInBox) {
					selection->unused = 0;
				}
			}
			
			monPosInBox++;
		}
	}
	
	selection->selectedMonCount = processedMonCount;
	selection->origSelectionTopLeftPos = selectionTopRow * MAX_PC_COLS + selectionLeftCol;
	
	boxApp->cursor.previewMonSource = boxApp->cursor.isMonUnderCursor ? PREVIEW_MON_FROM_SELECTION : PREVIEW_MON_UNDER_SELECTION;
	boxApp->cursor.isMonUnderCursor = FALSE;
}


static void BoxAppMan_ResetMultiSelectLocation(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	BoxMonSelection* selection = &boxApp->selection;
	BoxCursor* cursor = &boxApp->cursor;
	
	selection->selectionStartCol = selection->selectionEndCol = cursor->boxCol;
	selection->selectionStartRow = selection->selectionEndRow = cursor->boxRow;
}


static void BoxAppMan_SetMultiSelectionEndLocation(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	BoxMonSelection* selection = &boxApp->selection;
	BoxCursor* cursor = &boxApp->cursor;
	
	selection->selectionEndCol = cursor->boxCol;
	selection->selectionEndRow = cursor->boxRow;
}


static void BoxAppMan_PutDownCursorMon(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	BoxMonSelection* selection = &boxApp->selection;
	BoxCursor* cursor = &boxApp->cursor;
	BoxPokemon* boxMon;
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
		PCBoxes_TryStoreBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox, selection->boxMon);
		boxMon = PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox);
	} else {
		if (selection->cursorMonIsPartyMon) {
			Party_AddPokemon(boxAppMan->party, (Pokemon*)selection->boxMon);
		} else {
			Pokemon_FromBoxPokemon(selection->boxMon, boxAppMan->mon);
			Party_AddPokemon(boxAppMan->party, boxAppMan->mon);
		}
		
		boxMon = (BoxPokemon*)Party_GetPokemonBySlotIndex(boxAppMan->party, Party_GetCurrentCount(boxAppMan->party) - 1);
	}
	
	BoxApp_SetPreviewedBoxMon(boxApp, boxMon);
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
	
	boxApp->cursor.previewMonSource = PREVIEW_MON_UNDER_CURSOR;
	boxApp->cursor.isMonUnderCursor = TRUE;
}


static void BoxAppMan_PutDownSelectedMons(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	int origSelectionTopLeftPos;
	BoxCursor* cursor = &boxApp->cursor;
	BoxMonSelection* selection = &boxApp->selection;
	
	int selectionTopLeftPos = BoxApp_GetMultiSelectTopLeftPos(boxApp);
	origSelectionTopLeftPos = selection->origSelectionTopLeftPos;
	int boxMonStructSize = BoxPokemon_GetStructSize();
	BoxPokemon* boxMon = selection->boxMon;
	
	int posInBox;
	for (int i = 0; i < selection->selectedMonCount; i++) {
		posInBox = selectionTopLeftPos + (selection->selectedMonsOrigBoxPos[i] - origSelectionTopLeftPos);
		PCBoxes_TryStoreBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, posInBox, boxMon);
		(u8*)boxMon += boxMonStructSize;
	}
	
	BoxApp_SetPreviewedBoxMon(boxApp, PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, BoxApp_GetCursorBoxPosition(boxApp)));
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
	
	selection->selectedMonCount = 0;
	boxApp->cursor.isMonUnderCursor = (boxApp->cursor.previewMonSource == PREVIEW_MON_FROM_SELECTION);
	boxApp->cursor.previewMonSource = PREVIEW_MON_UNDER_CURSOR;
}


static void BoxAppMan_SwapMonInCursor(BoxApplicationManager* boxAppMan, BoxApplication* boxApp) {
	BoxMonSelection* selection = &boxApp->selection;
	u32 monStructSize = Pokemon_GetStructSize();
	void* monBuffer = (u8*)selection->boxMon + monStructSize;
	BoxCursor* cursor = &boxApp->cursor;
	
	MI_CpuCopy32(selection->boxMon, monBuffer, monStructSize);
	MI_CpuCopy32(cursor->mon, selection->boxMon, monStructSize);
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
		PCBoxes_TryStoreBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox, monBuffer);
		selection->cursorMonIsPartyMon = FALSE;
	} else {
		if (selection->cursorMonIsPartyMon == FALSE) {
			Pokemon_FromBoxPokemon(monBuffer, boxAppMan->mon);
		} else {
			MI_CpuCopy32(monBuffer, boxAppMan->mon, monStructSize);
		}
		
		Party_AddPokemonBySlotIndex(boxAppMan->party, cursor->posInParty, boxAppMan->mon);
		selection->cursorMonIsPartyMon = TRUE;
	}
	
	BoxApp_PreviewBoxMon(boxApp, selection->boxMon, boxAppMan);
}


static BOOL BoxAppMan_TryStoreCursorMonInBox(BoxApplicationManager* boxAppMan, u32 boxID) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	BoxMonSelection* selection = &boxApp->selection;
	
	if (PCBoxes_TryStoreBoxMonInBox(boxAppMan->pcBoxes, boxID, selection->boxMon)) {
		boxApp->cursor.previewMonSource = PREVIEW_MON_UNDER_CURSOR;
		BoxAppMan_TryPreviewCursorMon(boxAppMan);
		return TRUE;
	}
	
	return FALSE;
}


static BOOL BoxAppMan_TryStoreSelectedMonInBox(BoxApplicationManager* boxAppMan, u32 boxID) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	
	if (PCBoxes_TryStoreBoxMonInBox(boxAppMan->pcBoxes, boxID, cursor->mon)) {
		Party_RemovePokemonBySlotIndex(boxAppMan->party, cursor->posInParty);
		BoxAppMan_TryPreviewCursorMon(boxAppMan);
		return TRUE;
	}
	
	return FALSE;
}


static void BoxAppMan_RemoveCursorMon(BoxApplicationManager* boxAppMan) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	BoxMonSelection* selection = &boxApp->selection;
	
	BoxPokemon_Init(selection->boxMon);
	cursor->previewMonSource = PREVIEW_MON_UNDER_CURSOR;
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static void BoxAppMan_RemoveMonUnderCursor(BoxApplicationManager* boxAppMan) {
	BoxApplication* boxApp = &boxAppMan->boxApp;
	BoxCursor* cursor = &boxApp->cursor;
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
		PCBoxes_InitBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox);
	} else {
		Party_RemovePokemonBySlotIndex(boxAppMan->party, cursor->posInParty);
	}
	
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static void BoxAppMan_AssertFail(BoxApplicationManager* boxAppMan) {
	GF_ASSERT(FALSE);
}


static void BoxAppMan_RanchDepositMonUnderCursor(BoxApplicationManager* boxAppMan, u32 arg1) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	
	if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) {
		MPRComm_DepositMon(cursor->posInBox + (MAX_MONS_PER_BOX * BoxApp_GetCurrentBox(&boxAppMan->boxApp)), arg1);
		PCBoxes_InitBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, cursor->posInBox);
		BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
	} else {
		GF_ASSERT(FALSE);
	}
	
	BoxAppMan_TryPreviewCursorMon(boxAppMan);
}


static BOOL BoxAppMan_StoreRanchMonInBox(BoxApplicationManager* boxAppMan, u32 boxID, int arg2) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	
	if (BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX) {
		Pokemon* tmpMon = Pokemon_New(HEAP_ID_BOX_DATA);
		BoxPokemon* tmpBoxMon = (BoxPokemon*)cursor->mon;
		Pokemon_FromBoxPokemon(tmpBoxMon, tmpMon);
		
		BOOL monIsEgg = BoxPokemon_GetValue((BoxPokemon*)cursor->mon, MON_DATA_SANITY_IS_EGG, NULL);
		
		if (
			!monIsEgg && 
			Pokemon_GetValue(tmpMon, MON_DATA_SPECIES, NULL) == SPECIES_ROTOM && 
			Pokemon_GetValue(tmpMon, MON_DATA_FORM, NULL) != 0
		) {
			Pokemon_SetRotomForm(tmpMon, 0, 0);
			tmpBoxMon = Pokemon_GetBoxPokemon(tmpMon);
		}
		
		BOOL storeSuccess;
		if (boxID >= MAX_PC_BOXES) {
			storeSuccess = PCBoxes_TryStoreBoxMon(boxAppMan->pcBoxes, tmpBoxMon);
		} else {
			storeSuccess = PCBoxes_TryStoreBoxMonInBox(boxAppMan->pcBoxes, boxID, tmpBoxMon);
		}
		
		if (storeSuccess) {
			int boxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
			
			if (tmpBoxMon != cursor->mon) {
				boxAppMan->mprFilter.pendingRotomRevertMsg = TRUE;
			}
			
			if (!monIsEgg) {
				Pokedex_Capture(SaveData_GetPokedex(boxAppMan->saveData), tmpMon);
			}
			
			if (MPRComm_IsConnected()) {
				MPRComm_0203C420(cursor->posInBox + (MAX_MONS_PER_BOX * boxID), arg2);
			}
			
			BoxPokemon_Init((BoxPokemon*)cursor->mon);
			BoxAppMan_TryPreviewCursorMon(boxAppMan);
			BoxAppMan_FlagRecordBoxUseInJournal(boxAppMan);
			Heap_Free(tmpMon);
			
			return TRUE;
		}
		
		Heap_Free(tmpMon);
	} else {
		GF_ASSERT(FALSE);
	}
	
	return FALSE;
}


static void BoxAppMan_CommUpdateFocusedMon(BoxApplicationManager* boxAppMan) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	
	if (
		BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX && 
		(BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT || BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_WITHDRAW) && 
		BoxPokemon_GetValue((BoxPokemon*)cursor->mon, MON_DATA_SPECIES_EXISTS, NULL)
	) {
		MPRComm_SetFocusedMonIndex(cursor->posInBox + (MAX_MONS_PER_BOX * BoxApp_GetCurrentBox(&boxAppMan->boxApp)));
	}
}


static void BoxAppMan_CommUpdateMon(BoxApplicationManager* boxAppMan) {
	BoxCursor* cursor = &boxAppMan->boxApp.cursor;
	
	GF_ASSERT(
		BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX &&
		BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT
	);
	
	MPRComm_UpdateMon(cursor->posInBox + (MAX_MONS_PER_BOX * BoxApp_GetCurrentBox(&boxAppMan->boxApp)), (BoxPokemon*)cursor->mon);
}


static void BoxApp_PreviewBoxMon(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan) {
	BoxApp_LoadBoxMonIntoPreview(boxApp, boxMon, boxAppMan);
	
	if (BoxApp_GetBoxMode(boxApp) == PC_MODE_COMPARE) {
		BoxApp_LoadBoxMonIntoComparison(boxApp, boxMon, boxAppMan);
	}
}


void BoxApp_LoadBoxMonIntoPreview(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	
	preview->mon = boxMon;
	preview->species = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	preview->heldItem = BoxPokemon_GetValue(boxMon, MON_DATA_HELD_ITEM, NULL);
	preview->dexNum = GetDexNumber(SaveData_GetDexMode(boxAppMan->saveData), preview->species);
	preview->isEgg = BoxPokemon_GetValue(boxMon, MON_DATA_SANITY_IS_EGG, NULL);
	SpeciesData* speciesData = SpeciesData_FromMonSpecies(preview->species, HEAP_ID_BOX_DATA);
	preview->level = SpeciesData_GetLevelAt(speciesData, preview->species, BoxPokemon_GetValue(boxMon, MON_DATA_EXPERIENCE, NULL));
	preview->markings = BoxPokemon_GetValue(boxMon, MON_DATA_MARKINGS, NULL);
	preview->type1 = BoxPokemon_GetValue(boxMon, MON_DATA_TYPE_1, NULL);
	preview->type2 = BoxPokemon_GetValue(boxMon, MON_DATA_TYPE_2, NULL);
	
	if ((preview->isEgg == FALSE) && BoxPokemon_GetValue(boxMon, MON_DATA_NO_PRINT_GENDER, NULL)) {
		preview->gender = SpeciesData_GetGenderOf(speciesData, preview->species, BoxPokemon_GetValue(boxMon, MON_DATA_PERSONALITY, NULL));
	} else {
		preview->gender = PREVIEW_GENDER_INVALID;
	}
	
	BoxPokemon_GetValue(boxMon, MON_DATA_NICKNAME_STRING, preview->nickname);
	
	if (preview->isEgg == FALSE) {
		MessageLoader_GetString(boxAppMan->speciesNameLoader, preview->species, preview->speciesName);
	} else {
		String_Copy(preview->speciesName, preview->nickname);
		String_Clear(preview->nickname);
	}
	
	if (preview->heldItem != ITEM_NONE) {
		Item_LoadName(preview->heldItemName, preview->heldItem, HEAP_ID_BOX_DATA);
	} else {
		MessageLoader_GetString(boxAppMan->boxMessagesLoader, BOX_MESSAGE_NoItem, preview->heldItemName);
	}
	
	u32 value = BoxPokemon_GetNature(boxMon);
	MessageLoader_GetString(boxAppMan->natureNameLoader, value, preview->nature);
	
	value = BoxPokemon_GetValue(boxMon, MON_DATA_ABILITY, NULL);
	MessageLoader_GetString(boxAppMan->abilityNameLoader, value, preview->ability);
	
	SpeciesData_Free(speciesData);
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


static void BoxApp_LoadBoxMonIntoComparison(BoxApplication* boxApp, BoxPokemon* boxMon, BoxApplicationManager* boxAppMan) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	PCCompareMon* compareMon = &boxApp->compareModeHelper.compareMons[boxApp->compareModeHelper.compareMonSlot];
	
	compareMon->mon = boxMon;
	compareMon->species = preview->species;
	compareMon->isEgg = preview->isEgg;
	compareMon->level = preview->level;
	
	if (compareMon->isEgg) {
		String_Copy(compareMon->monName, preview->speciesName);
	} else {
		String_Copy(compareMon->monName, preview->nickname);
	}
	
	String_Copy(compareMon->nature, preview->nature);
	Pokemon_FromBoxPokemon(boxMon, boxAppMan->mon);
	
	BOOL reencrypt = Pokemon_EnterDecryptionContext(boxAppMan->mon);
	
	compareMon->maxHP = Pokemon_GetValue(boxAppMan->mon, MON_DATA_MAX_HP, NULL);
	compareMon->attack = Pokemon_GetValue(boxAppMan->mon, MON_DATA_ATK, NULL);
	compareMon->defense = Pokemon_GetValue(boxAppMan->mon, MON_DATA_DEF, NULL);
	compareMon->spAttack = Pokemon_GetValue(boxAppMan->mon, MON_DATA_SP_ATK, NULL);
	compareMon->spDefense = Pokemon_GetValue(boxAppMan->mon, MON_DATA_SP_DEF, NULL);
	compareMon->speed = Pokemon_GetValue(boxAppMan->mon, MON_DATA_SPEED, NULL);
	compareMon->cool = Pokemon_GetValue(boxAppMan->mon, MON_DATA_COOL, NULL);
	compareMon->beauty = Pokemon_GetValue(boxAppMan->mon, MON_DATA_BEAUTY, NULL);
	compareMon->cute = Pokemon_GetValue(boxAppMan->mon, MON_DATA_CUTE, NULL);
	compareMon->smart = Pokemon_GetValue(boxAppMan->mon, MON_DATA_SMART, NULL);
	compareMon->tough = Pokemon_GetValue(boxAppMan->mon, MON_DATA_TOUGH, NULL);
	compareMon->moves[0] = Pokemon_GetValue(boxAppMan->mon, MON_DATA_MOVE1, NULL);
	compareMon->moves[1] = Pokemon_GetValue(boxAppMan->mon, MON_DATA_MOVE2, NULL);
	compareMon->moves[2] = Pokemon_GetValue(boxAppMan->mon, MON_DATA_MOVE3, NULL);
	compareMon->moves[3] = Pokemon_GetValue(boxAppMan->mon, MON_DATA_MOVE4, NULL);
	compareMon->form = Pokemon_GetValue(boxAppMan->mon, MON_DATA_FORM, NULL);
	
	Pokemon_ExitDecryptionContext(boxAppMan->mon, reencrypt);
	
	boxApp->compareModeHelper.compareSlotHasMon[boxApp->compareModeHelper.compareMonSlot] = TRUE;
}


static void BoxApp_ToggleCompareMonSlot(BoxApplication* boxApp) {
	boxApp->compareModeHelper.compareMonSlot ^= 1;
}


static void BoxApp_NextCompareMode(BoxApplication* boxApp) {
	boxApp->compareModeHelper.compareMode++;
	if (boxApp->compareModeHelper.compareMode >= NUM_COMPARE_MODES) {
		boxApp->compareModeHelper.compareMode = COMPARE_BATTLE_STATS;
	}
}


static void BoxApp_SetCompareButtonPressed(BoxApplication* boxApp, BOOL pressed) {
	boxApp->compareModeHelper.compareButtonAnimationPressed = pressed;
}


static void BoxApp_SetPreviewedBoxMon(BoxApplication* boxApp, BoxPokemon* boxMon) {
	boxApp->pcMonPreview.mon = boxMon;
}


static void BoxApp_UpdatePreviewMonMarkings(BoxApplication* boxApp) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	u8 markings = boxApp->boxMenu.markings;
	preview->markings = markings;
	
	BoxPokemon_SetValue(preview->mon, MON_DATA_MARKINGS, &markings);
}


void BoxApp_GiveItemToSelectedMon(BoxApplication* boxApp, u16 item, BoxApplicationManager* boxAppMan) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	preview->heldItem = item;
	
	if (preview->heldItem != ITEM_NONE) {
		Item_LoadName(preview->heldItemName, preview->heldItem, HEAP_ID_BOX_DATA);
	} else {
		MessageLoader_GetString(boxAppMan->boxMessagesLoader, BOX_MESSAGE_NoItem, preview->heldItemName);
	}
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX && BoxApp_GetPreviewMonSource(boxApp) == PREVIEW_MON_UNDER_CURSOR) {
		u32 posInBox = BoxApp_GetCursorBoxPosition(boxApp);
		PCBoxes_SetBoxMonData(boxAppMan->pcBoxes, USE_CURRENT_BOX, posInBox, MON_DATA_HELD_ITEM, &item);
	} else {
		BoxPokemon_SetValue(preview->mon, MON_DATA_HELD_ITEM, &item);
	}
	
	int species = BoxPokemon_GetValue(preview->mon, MON_DATA_SPECIES, NULL);
	if (species == SPECIES_ARCEUS) {
		BoxPokemon_SetArceusForm(preview->mon);
	}
}


static void BoxAppMan_LoadRightBoxCustomization(BoxApplicationManager* boxAppMan) {
	BoxCustomization* customization = &boxAppMan->boxApp.customization;
	BoxApp_GetBoxMode(&boxAppMan->boxApp); /* Unused */
	
	boxAppMan->menuItem = boxAppMan->boxApp.customization.boxID;
	
	if (customization->maxRanchBoxID != 0) {
		customization->ranchBoxJump = 1;
		customization->boxID++;
		if (customization->boxID > customization->maxRanchBoxID) {
			customization->boxID = 0;
		}
		
		PCBoxes_LoadCustomization(boxAppMan->boxApp.pcBoxes, customization, BoxApp_GetBoxMode(&boxAppMan->boxApp));
	}
}


static void BoxAppMan_LoadLeftBoxCustomization(BoxApplicationManager* boxAppMan) {
	BoxCustomization* customization = &boxAppMan->boxApp.customization;
	BoxMode boxMode = BoxApp_GetBoxMode(&boxAppMan->boxApp);
	
	boxAppMan->menuItem = boxAppMan->boxApp.customization.boxID;
	
	if (customization->maxRanchBoxID != 0) {
		customization->ranchBoxJump = -1;
		
		int boxID = customization->boxID;
		if (boxID != 0) {
			boxID--;
		} else {
			boxID = customization->maxRanchBoxID;
		}
		customization->boxID = boxID;
		
		PCBoxes_LoadCustomization(boxAppMan->boxApp.pcBoxes, customization, boxMode);
	}
}


static void BoxApp_LoadCustomizationsFor(BoxApplication* boxApp, u32 boxID) {
	BoxCustomization* customization = &boxApp->customization;
	
	customization->boxID = boxID;
	PCBoxes_LoadCustomization(boxApp->pcBoxes, customization, BoxApp_GetBoxMode(boxApp));
}


static void BoxApp_SetBoxSelectionBoxID(BoxApplication* boxApp, u32 boxID) {
	boxApp->boxSelectionBoxID = boxID;
}


static void BoxApp_PickUpHeldItem(BoxApplication* boxApp, BoxApplicationManager* boxAppMan) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	u16 itemNone = ITEM_NONE;
	
	boxApp->cursorItem = preview->heldItem;
	
	MessageLoader_GetString(boxAppMan->boxMessagesLoader, BOX_MESSAGE_NoItem, preview->heldItemName);
	BoxApp_GiveItemToSelectedMon(boxApp, itemNone, boxAppMan);
}


static void BoxApp_RemoveCursorItem(BoxApplication* boxApp) {
	boxApp->cursorItem = ITEM_NONE;
}


static void BoxApp_GiveItemFromCursor(BoxApplication* boxApp, BoxApplicationManager* boxAppMan) {
	u16 item = boxApp->cursorItem;
	boxApp->cursorItem = ITEM_NONE;
	
	BoxApp_GiveItemToSelectedMon(boxApp, item, boxAppMan);
}


static void BoxApp_SwapMonAndCursorItems(BoxApplication* boxApp, BoxApplicationManager* boxAppMan) {
	PCMonPreview* preview = &boxApp->pcMonPreview;
	u16 item = boxApp->cursorItem;
	boxApp->cursorItem = preview->heldItem;
	
	BoxApp_GiveItemToSelectedMon(boxApp, item, boxAppMan);
}


static void BoxApp_SetTouchDialSelected(BoxApplication* boxApp, TouchDials touchDialID) {
	boxApp->touchDialHelper.selectedTouchDial = touchDialID;
}


static void BoxApp_SetTouchDialOffset(BoxApplication* boxApp, u32 offset) {
	boxApp->touchDialHelper.touchDialOffset = offset;
}


static void BoxApp_SetMarkingsButtonsScrollOffset(BoxApplication* boxApp, u32 offset) {
	boxApp->touchDialHelper.markingsButtonsScrollOffset = offset;
}


static void BoxApp_SetTouchDialScrollDelta(BoxApplication* boxApp, int amount) {
	boxApp->touchDialHelper.scrollDelta = amount;
}


static void BoxApp_SetMonSpriteTransparencyMask(BoxApplication* boxApp, u32 mask) {
	if (mask == 0) {
		boxApp->boxSettings.monSpriteTransparencyMask = 0;
	} else {
		boxApp->boxSettings.monSpriteTransparencyMask ^= (1 << (mask - 1));
	}
}


static void BoxApp_ToggleCursorFastMode(BoxApplication* boxApp) {
	boxApp->boxSettings.isCursorFastMode ^= 1;
}


MessageLoader* BoxAppMan_GetBoxMessagesLoader(const BoxApplicationManager* boxAppMan) {
	return boxAppMan->boxMessagesLoader;
}


const StringTemplate* BoxAppMan_GetMessageVariableBuffer(const BoxApplicationManager* boxAppMan) {
	return boxAppMan->messageVariableBuffer;
}


int BoxAppMan_GetOptionsFrame(const BoxApplicationManager* boxAppMan) {
	return Options_GetFrame(boxAppMan->options);
}


MPRFilterMenu* BoxAppMan_GetMPRFilter(const BoxApplicationManager* boxAppMan) {
	return (MPRFilterMenu*)&boxAppMan->mprFilter;
}


BoxMode BoxApp_GetBoxMode(const BoxApplication* boxApp) {
	return boxApp->boxSettings.boxMode;
}


CursorLocation BoxApp_GetCursorLocation(const BoxApplication* boxApp) {
	return boxApp->cursor.cursorLocation;
}


u32 BoxApp_GetCursorBoxCol(const BoxApplication* boxApp) {
	return boxApp->cursor.boxCol;
}


u32 BoxApp_GetCursorBoxRow(const BoxApplication* boxApp) {
	return boxApp->cursor.boxRow;
}


u32 BoxApp_GetCursorBoxPosition(const BoxApplication* boxApp) {
	return boxApp->cursor.posInBox;
}


u32 BoxApp_GetCursorPartyPosition(const BoxApplication* boxApp) {
	return boxApp->cursor.posInParty;
}


BOOL BoxApp_IsMonUnderCursor(const BoxApplication* boxApp) {
	return boxApp->cursor.isMonUnderCursor;
}


u32 BoxApp_GetPreviewMonSource(const BoxApplication* boxApp) {
	return boxApp->cursor.previewMonSource;
}


BOOL BoxApp_GetCursorMonIsPartyMon(const BoxApplication* boxApp) {
	if (boxApp->cursor.previewMonSource) {
		return boxApp->selection.cursorMonIsPartyMon;
	}
	
	return FALSE;
}


BOOL BoxApp_IsMonAvailableToCursor(const BoxApplication* boxApp) {
	const BoxCursor* cursor = &boxApp->cursor;
	
	if (cursor->previewMonSource == PREVIEW_MON_IN_CURSOR || cursor->previewMonSource == PREVIEW_MON_FROM_SELECTION) {
		return TRUE;
	}
	
	if (boxApp->cursor.isMonUnderCursor) {
		return TRUE;
	}
	
	return FALSE;
}


u32 BoxApp_GetCurrentBox(const BoxApplication* boxApp) {
	return boxApp->customization.boxID;
}


u32 BoxApp_GetMaxRanchBoxID(const BoxApplication* boxApp) {
	return boxApp->customization.maxRanchBoxID;
}


const PCMonPreview* BoxApp_GetPCMonPreview(const BoxApplication* boxApp) {
	return &boxApp->pcMonPreview;
}


u32 BoxApp_GetPreviewedMonHeldItem(const BoxApplication* boxApp) {
	if (BoxApp_IsMonAvailableToCursor(boxApp)) {
		return boxApp->pcMonPreview.heldItem;
	}
	
	return ITEM_NONE;
}


const BoxCustomization* BoxApp_GetBoxCustomization(const BoxApplication* boxApp) {
	return &boxApp->customization;
}


PCBoxes* BoxApp_GetPCBoxes(const BoxApplication* boxApp) {
	return (PCBoxes*)boxApp->pcBoxes;
}


u32 BoxApp_GetBoxMessageID(const BoxApplication* boxApp) {
	return boxApp->boxMessageID;
}


BoxPokemon* BoxApp_GetPreviewedBoxMon(const BoxApplication* boxApp) {
	return boxApp->pcMonPreview.mon;
}


u32 BoxApp_GetPreviewedMonMarkings(const BoxApplication* boxApp) {
	return boxApp->pcMonPreview.markings;
}


u32 BoxApp_GetBoxSelectionBoxID(const BoxApplication* boxApp) {
	return boxApp->boxSelectionBoxID;
}


s32 BoxApp_GetTouchDialScrollDelta(const BoxApplication* boxAppMan) {
	return boxAppMan->touchDialHelper.scrollDelta;
}


u32 BoxApp_GetTouchDialOffset(const BoxApplication* boxApp) {
	return boxApp->touchDialHelper.touchDialOffset;
}


u32 BoxApp_GetMarkingsButtonsScrollOffset(const BoxApplication* boxApp) {
	return boxApp->touchDialHelper.markingsButtonsScrollOffset;
}


u32 BoxApp_GetMonSpriteTransparencyMask(const BoxApplication* boxApp) {
	if (BoxApp_GetBoxMode(boxApp) != PC_MODE_MOVE_ITEMS) {
		return boxApp->boxSettings.monSpriteTransparencyMask;
	} else {
		return 1;
	}
}


BOOL BoxApp_IsCursorFastMode(const BoxApplication* boxApp) {
	return boxApp->boxSettings.isCursorFastMode;
}


void BoxApp_GetMultiSelectBoundingBox(const BoxApplication* boxApp, u32* leftCol, u32* rightCol, u32* topRow, u32* bottomRow) {
	const BoxMonSelection* selection = &boxApp->selection;
	
	if (selection->selectionStartCol <= selection->selectionEndCol) {
		*leftCol = selection->selectionStartCol;
		*rightCol = selection->selectionEndCol;
	} else {
		*leftCol = selection->selectionEndCol;
		*rightCol = selection->selectionStartCol;
	}
	
	if (selection->selectionStartRow <= selection->selectionEndRow) {
		*topRow = selection->selectionStartRow;
		*bottomRow = selection->selectionEndRow;
	} else {
		*topRow = selection->selectionEndRow;
		*bottomRow = selection->selectionStartRow;
	}
}


BOOL BoxApp_IsMultiSelectSingleSelect(const BoxApplication* boxApp) {
	const BoxMonSelection* selection = &boxApp->selection;
	
	if (selection->selectionStartCol == selection->selectionEndCol && selection->selectionStartRow == selection->selectionEndRow) {
		return TRUE;
	}
	
	return FALSE;
}


u32 BoxApp_GetMultiSelectTopLeftPos(const BoxApplication* boxApp) {
	const BoxMonSelection* selection = &boxApp->selection;
	int col = boxApp->cursor.boxCol;
	int row = boxApp->cursor.boxRow;
	
	if (selection->selectionEndCol > selection->selectionStartCol) {
		col -= selection->selectionEndCol - selection->selectionStartCol;
	}
	
	if (selection->selectionEndRow > selection->selectionStartRow) {
		row -= selection->selectionEndRow - selection->selectionStartRow;
	}
	
	return row * MAX_PC_COLS + col;
}


u32 BoxApp_GetRelativeMonPosInMultiSelection(const BoxApplication* boxApp, u32 position) {
	const BoxMonSelection* selection = &boxApp->selection;
	return selection->selectedMonsOrigBoxPos[position] - selection->origSelectionTopLeftPos;
}


u32 BoxApp_GetCursorItem(const BoxApplication* boxApp) {
	return boxApp->cursorItem;
}


u32 BoxApp_GetCursorOrPreviewedItem(const BoxApplication* boxApp) {
	u32 cursorItem = BoxApp_GetCursorItem(boxApp);
	
	if (cursorItem != ITEM_NONE) {
		return cursorItem;
	}
	
	return BoxApp_GetPreviewedMonHeldItem(boxApp);
}


u32 BoxApp_GetCompareMonSlot(const BoxApplication* boxApp) {
	return boxApp->compareModeHelper.compareMonSlot;
}


u32 BoxApp_GetCompareMode(const BoxApplication* boxApp) {
	return boxApp->compareModeHelper.compareMode;
}


const PCCompareMon* BoxApp_GetCompareMonFrom(const BoxApplication* boxApp, int compareSlot) {
	return &boxApp->compareModeHelper.compareMons[compareSlot];
}


BOOL BoxApp_CompareSlotHasMon(const BoxApplication* boxApp, int compareMonSlot) {
	return boxApp->compareModeHelper.compareSlotHasMon[compareMonSlot];
}


BOOL BoxApp_IsCompareButtonPressed(const BoxApplication* boxApp) {
	return boxApp->compareModeHelper.compareButtonAnimationPressed;
}


BOOL BoxApp_IsPreviewedMonEgg(const BoxApplication* boxApp) {
	return boxApp->pcMonPreview.isEgg;
}


BoxPokemon* BoxApp_0202E96C(const BoxApplication* boxApp, u32 index) {
	if (boxApp->MPR_unk_11C != NULL) {
		int offset = index * BoxPokemon_GetStructSize();
		return (BoxPokemon*)(boxApp->MPR_unk_11C + offset);
	}
	
	return PCBoxes_GetBoxMonAt(boxApp->pcBoxes, USE_CURRENT_BOX, index);
}


BoxPokemon* BoxApp_0202E99C(const BoxApplication* boxApp, u32 index) {
	if (boxApp->MPR_unk_120 != NULL) {
		int offset = index * BoxPokemon_GetStructSize();
		return (BoxPokemon*)(boxApp->MPR_unk_120 + offset);
	}
	
	GF_ASSERT(FALSE);
	return NULL;
}


BOOL BoxApp_0202E9C8(const BoxApplication* boxApp, u32 unused) {
	#pragma unused(unused)
	
	const BoxCustomization* customization = &boxApp->customization;
	
	if (BoxApp_GetBoxMode(boxApp) != PC_MODE_RANCH_WITHDRAW) {
		return TRUE;
	}
	
	if (customization->maxRanchBoxID != 0) {
		return TRUE;
	}
	
	return FALSE;
}


static u32 BoxApp_GetPreviewedMonValue(BoxApplication* boxApp, PokemonDataParam value, void* dest) {
	if (BoxApp_GetPreviewMonSource(boxApp) == PREVIEW_MON_UNDER_CURSOR) {
		if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
			return BoxPokemon_GetValue(boxApp->pcMonPreview.mon, value, dest);
		}
	} else {
		BoxMonSelection* selection = &boxApp->selection;

		if (selection->cursorMonIsPartyMon == FALSE) {
			return BoxPokemon_GetValue(boxApp->pcMonPreview.mon, value, dest);
		}
	}
	
	return Pokemon_GetValue(boxApp->pcMonPreview.mon, value, dest);
}


static u32 BoxApp_GetPreviewedOrSelectedMonValue(BoxApplication* boxApp, PokemonDataParam value, void* dest) {
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_BOX) {
		return BoxPokemon_GetValue(boxApp->pcMonPreview.mon, value, dest);
	} else {
		return Pokemon_GetValue(boxApp->cursor.mon, value, dest);
	}
}


static BoxPokemon* BoxAppMan_GetBoxMonAt(BoxApplicationManager* boxAppMan, u32 index) {
	if (boxAppMan->mprFilter.unk_84 != NULL) {
		int offset = index * BoxPokemon_GetStructSize();
		return (BoxPokemon*)(boxAppMan->mprFilter.unk_84 + offset);
	}
	
	return PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, index);
}


static void BoxAppMan_InitReleaseBlockingMoveCache(BoxApplicationManager* boxAppMan) {
	ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	
	releaseMon->checkedCanReleaseMon = 0;
	releaseMon->canReleaseMon = 0;
	releaseMon->boxID = 0;
	releaseMon->monPosInBox = 0;
	releaseMon->pcBoxes = boxAppMan->pcBoxes;
	releaseMon->party = boxAppMan->party;
	
	for (int i = 0; i < MAX_PC_BOXES; i++) {
		releaseMon->pcReleaseBlockingMoveMap[i] = 0;
	}
	
	releaseMon->partyReleaseBlockingMoveMap = 0;
	
	SysTask_Start(BoxAppMan_ReleaseBlockingMoveCacheTask, releaseMon, 0);
}


static void BoxAppMan_ReleaseBlockingMoveCacheTask(SysTask* task, void* param) {
	ReleaseMon* releaseMon = (ReleaseMon*)param;
	BoxPokemon* boxMon;
	BOOL reencrypt;
	int p, m;
	
	if (releaseMon->boxID < MAX_PC_BOXES) {
		int monPosEnd = releaseMon->monPosInBox + MAX_MONS_PER_BOX;
		if (monPosEnd > MAX_MONS_PER_BOX) {
			monPosEnd = MAX_MONS_PER_BOX;
		}
		
		for (p = releaseMon->monPosInBox; p < monPosEnd; p++) {
			boxMon = PCBoxes_GetBoxMonAt(releaseMon->pcBoxes, releaseMon->boxID, p);
			reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
			
			if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
				for (m = 0; m < NUM_RELEASE_BLOCKING_MOVES; m++) {
					if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[m])) {
						releaseMon->pcReleaseBlockingMoveMap[releaseMon->boxID] |= (1 << m);
					}
				}
			}
			
			BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
		}
		
		if (monPosEnd == MAX_MONS_PER_BOX) {
			releaseMon->monPosInBox = 0;
			releaseMon->boxID++;
		} else {
			releaseMon->monPosInBox = monPosEnd;
		}
	} else {
		int partySize = Party_GetCurrentCount(releaseMon->party);
		for (p = 0; p < partySize; p++) {
			boxMon = &(Party_GetPokemonBySlotIndex(releaseMon->party, p))->box;
			reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
			
			for (m = 0; m < NUM_RELEASE_BLOCKING_MOVES; m++) {
				if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[m])) {
					releaseMon->partyReleaseBlockingMoveMap |= (1 << m);
				}
			}
			
			BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
		}
		
		releaseMon->checkedCanReleaseMon = TRUE;
		SysTask_Done(task);
	}
}


static BOOL BoxAppMan_IsReleaseBlockingMoveCacheTaskDone(BoxApplicationManager* boxAppMan) {
	return boxAppMan->releaseMon.checkedCanReleaseMon;
}


static void BoxAppMan_ReleaseBlockingMoveCacheBoxUpdate(BoxApplicationManager* boxAppMan, int boxID) {
	ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	
	if (BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT) {
		releaseMon->pcReleaseBlockingMoveMap[boxID] = 0;
		
		for (int p = 0; p < MAX_MONS_PER_BOX; p++) {
			BoxPokemon* boxMon = PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, boxID, p);
			BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
			
			if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
				for (int m = 0; m < NUM_RELEASE_BLOCKING_MOVES; m++) {
					if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[m])) {
						releaseMon->pcReleaseBlockingMoveMap[boxID] |= (1 << m);
					}
				}
			}
			
			BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
		}
	}
}


static BOOL BoxAppMan_IsCursorMonDepositable(BoxApplicationManager* boxAppMan) {
	BoxPokemon* targetMon;
	int j, i;
	int cursorPos;
	BOOL reencrypt;
	u8 monMoveBit, otherMonsMoveBit;
	
	ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	
	GF_ASSERT(BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT);
	GF_ASSERT(BoxApp_GetCursorLocation(&boxAppMan->boxApp) == CURSOR_IN_BOX);
	
	int boxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
	cursorPos = BoxApp_GetCursorBoxPosition(&boxAppMan->boxApp);
	targetMon = BoxApp_GetPreviewedBoxMon(&boxAppMan->boxApp);
	
	GF_ASSERT(targetMon == PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, boxID, cursorPos));
	
	monMoveBit = 0;
	reencrypt = BoxPokemon_EnterDecryptionContext(targetMon);
	BOOL exists = BoxPokemon_GetValue(targetMon, MON_DATA_SPECIES_EXISTS, NULL);
	BOOL isEgg = BoxPokemon_GetValue(targetMon, MON_DATA_SANITY_IS_EGG, NULL);
	BOOL checksumFailed = BoxPokemon_GetValue(targetMon, MON_DATA_CHECKSUM_FAILED, NULL);
	
	if (exists && !isEgg && !checksumFailed) {
		for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
			if (BoxPokemon_HasMove(targetMon, sReleaseBlockingMoves[i]) != 0) {
				monMoveBit |= (1 << i);
			}
		}
	}
	
	BoxPokemon_ExitDecryptionContext(targetMon, reencrypt);
	
	// If this mon has no release-blocking moves, OK to deposit
	if (monMoveBit == 0) {
		return TRUE;
	}
	
	if (checksumFailed) {
		return FALSE;
	}
	
	// Get release-blocking moves in the party and in the cached other PC boxes
	otherMonsMoveBit = releaseMon->partyReleaseBlockingMoveMap;
	for (i = 0; i < MAX_PC_BOXES; i++) {
		if (i != boxID) {
			otherMonsMoveBit |= releaseMon->pcReleaseBlockingMoveMap[i];
		}
	}
	
	// If the mon's release-blocking moves are covered by mons in the party or in other boxes, OK to deposit
	if ((monMoveBit & otherMonsMoveBit) == monMoveBit) {
		return TRUE;
	}
	
	// Finally check every other mon in the same box individually
	for (j = 0; j < MAX_MONS_PER_BOX; j++) {
		if (j == cursorPos) {
			continue;
		}
		
		BoxPokemon* boxMon = PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, boxID, j);
		reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
		
		for (i = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
			if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[i])) {
				otherMonsMoveBit |= (1 << i);
			}
		}
		
		BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	}
	
	// If those mons cover the release-blocking moves, OK to deposit
	if ((monMoveBit & otherMonsMoveBit) == monMoveBit) {
		return TRUE;
	}
	
	// Otherwise, last mon with those move(s) and fail
	return FALSE;
}


static int BoxAppMan_ReleaseBlockingMoveCacheUpdateAll(BoxApplicationManager* boxAppMan, int* outTotalMonsInBox) {
	ReleaseMon* releaseMon = &boxAppMan->releaseMon;
	int j, i;
	int depositableMonsInBox = 0;
	int totalMonsInBox = 0;
	BoxPokemon* boxMon;
	BOOL reencrypt;
	
	GF_ASSERT(BoxApp_GetBoxMode(&boxAppMan->boxApp) == PC_MODE_RANCH_DEPOSIT);
	
	int boxID = BoxApp_GetCurrentBox(&boxAppMan->boxApp);
	
	// Initialize with party and all other PC boxees
	
	u8 releaseBlockingMoveBit = releaseMon->partyReleaseBlockingMoveMap;
	for (i = 0; i < MAX_PC_BOXES; i++) {
		if (i == boxID) {
			continue;
		}
		
		releaseBlockingMoveBit |= releaseMon->pcReleaseBlockingMoveMap[i];
	}
	
	// Update using every valid mon in this box
	for (j = MAX_MONS_PER_BOX - 1; j >= 0; j--) {
		boxMon = PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, boxID, j);
		GF_ASSERT(boxMon == PCBoxes_GetBoxMonAt(boxAppMan->pcBoxes, USE_CURRENT_BOX, j));
		reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
		
		BOOL exists = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL);
		BOOL isEgg = BoxPokemon_GetValue(boxMon, MON_DATA_SANITY_IS_EGG, NULL);
		BOOL checksumFailed = BoxPokemon_GetValue(boxMon, MON_DATA_CHECKSUM_FAILED, NULL);
		
		if (exists || isEgg || checksumFailed) {
			totalMonsInBox++;
		}
		
		if (isEgg && !checksumFailed) {
			depositableMonsInBox++;
		}
		
		if (exists && !isEgg && !checksumFailed) {
			u8 monMoveBit;
			for (i = 0, monMoveBit = 0; i < NUM_RELEASE_BLOCKING_MOVES; i++) {
				if (BoxPokemon_HasMove(boxMon, sReleaseBlockingMoves[i])) {
					monMoveBit |= (1 << i);
				}
			}
			
			if ((releaseBlockingMoveBit & monMoveBit) == monMoveBit) {
				depositableMonsInBox++;
			} else {
				releaseBlockingMoveBit |= monMoveBit;
			}
		}
		
		BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
	}
	
	if (outTotalMonsInBox != NULL) {
		*outTotalMonsInBox = totalMonsInBox;
	}
	
	return depositableMonsInBox;
}
