#include <nitro.h>

#include "mpr_client/mpr_save.h"

#include "mpr_client/mpr_client_common.h"
#include "mpr_client/mpr_menu.h"
#include "mpr_client/mpr_comm.h"
#include "mpr_client/mpr_text.h"

#include "bg_window.h"
#include "heap.h"
#include "font.h"
#include "savedata.h"
#include "sound_playback.h"
#include "string_gf.h"
#include "system.h"
#include "render_window.h"
#include "text.h"
#include "math_util.h"
#include "menu.h"
#include "constants/sdat.h"

typedef enum {
	OPTION_YES,
	OPTION_NO,
	OPTION_CANCEL,
	OPTION_NONE = -1
} YesNoMenuOption;

typedef enum {
	STATE_INIT,
	STATE_SAVE_AND_QUIT_CONFIRM,
	STATE_QUIT_CONFIRM,
	STATE_NOTIFY_WII_SAVE,
	STATE_SAVING_INIT,
	STATE_SAVING_WAIT,
	STATE_SAVE_OK,
	STATE_NOTIFY_WII_SHUTDOWN,
	STATE_TURNING_OFF_POWER,
	STATE_END
} MPRSaveAppState;

typedef struct {
	MPRSaveAppArgs*  args;
	BgConfig*        bgConfig;
	SaveData*        saveData; // Unused
	Window           textWindow;
	Window           yesNoWindow;
	int              frameCounter;
    void*            waitDial;
	YesNoMenuOption  focusedOption;
    MPRSaveAppState  state;
	String*          string;
} MPRSaveMenu;

typedef enum {
	THREAD_IDLE,
	THREAD_SAVE_MAIN,
	THREAD_SAVE_MAIN_WAIT,
	THREAD_SAVE_MAIN_OK,
	THREAD_SAVE_FINAL,
	THREAD_SAVE_FINAL_WAIT,
	THREAD_SAVE_FINAL_OK,
	THREAD_SAVE_FAILED
} SaveThreadState;

typedef struct {
    u32              timer;
    SaveData*        saveData;
    SaveThreadState  state;
} SaveThreadCtx;

#define SAVE_FILE_THREAD_STACK_SIZE  (0x400)

#define YES_NO_MENU_BASE_TILE  (0x99)
#define MENU_DIAL_BASE_TILE    (19)

static void PrintMPRString(MPRSaveMenu* menu, int messageID);
static void DrawWindowBorder(BgConfig* bgConfig, int bgLayer, int tileMapTop, int tileMapLeft, int width, int height);
static void ClearWindowTilemap(BgConfig* bgConfig, Window* window);
static void DrawYesNoCursorBox(BgConfig* bgConfig, Window* window, YesNoMenuOption focusedOption);
static void DrawYesNoWindow(BgConfig* bgConfig, Window* window, String* string, YesNoMenuOption focusedOption);
static YesNoMenuOption ReadYesNoInput(BgConfig* bgConfig, Window* window, YesNoMenuOption* focusedOption);
static void SaveThreadFunc(void* saveData);

static OSThread sSaveThread;
static u8 sSaveThreadStack[SAVE_FILE_THREAD_STACK_SIZE];
static SaveThreadCtx sSaveThreadCtx;

static const WindowTemplate sYesNoWindowTemplate = {
	.bgLayer     = BG_LAYER_MAIN_1, 
	.tilemapLeft = 23, 
	.tilemapTop  = 13,
	.width       = 8,
	.height      = 4, 
	.palette     = 0, 
	.baseTile    = YES_NO_MENU_BASE_TILE
};


static void PrintMPRString(MPRSaveMenu* menu, int messageID) {
	MPRText_CopyChars(menu->string, messageID);
	MPRMenuWindow_PrintString(menu->bgConfig, &menu->textWindow, menu->string);
}


static void DrawWindowBorder(BgConfig* bgConfig, int bgLayer, int tileMapTop, int tileMapLeft, int width, int height) {
	u32 h, w;
	
	w = width;
	h = height;
	
	u16* tileBuffer = Bg_GetTilemapBuffer(bgConfig, bgLayer);
	tileBuffer += (tileMapTop - 1) + ((tileMapLeft - 1) * 32);
	
	*(tileBuffer) = 0x2001;
	
	for (u32 i = 0; i < w; i++) {
		*(tileBuffer + i + 1) = 0x2002;
	}
	
	*(tileBuffer + w + 1) = 0x2003;
	
	tileBuffer += 32;
	
	for (u32 i = 0; i < h; i++) {
		*tileBuffer = 0x2004;
		*(tileBuffer + w + 1) = 0x2006;
		tileBuffer += 32;
	}
	
	*tileBuffer = 0x2007;
	
	for (u32 i = 0; i < w; i++) {
		*(tileBuffer + i + 1) = 0x2008;
	}
	
	*(tileBuffer + w + 1) = 0x2009;
}


static void ClearWindowTilemap(BgConfig* bgConfig, Window* window) {
	u16* tileBuffer = Bg_GetTilemapBuffer(bgConfig, window->bgLayer);
	tileBuffer += (window->tilemapLeft - 1) + ((window->tilemapTop - 1) * 32);
	
	u32 j, i;
	for (i = 0; i < window->height + 2; i++) {
		for (j = 0; j < window->width + 2; j++) {
			*(tileBuffer + j) = 0;
		}
		tileBuffer += 32;
	} 
}


static void DrawYesNoCursorBox(BgConfig* bgConfig, Window* window, YesNoMenuOption focusedOption) {
	Window_FillRectWithColor(window, 15, 0, 0, 10, 32);
	Window_DrawMenuCursor(window, 0, focusedOption * 16);
	Window_LoadTiles(window);
}


static const int sMPRYesNoMessageIDs[] = { MPR_TEXT_Yes, MPR_TEXT_No };

static void DrawYesNoWindow(BgConfig* bgConfig, Window* window, String* string, YesNoMenuOption focusedOption) {
	Window_FillTilemap(window, 15);
	
	for (int i = 0; i < 2; i++) {
		MPRText_CopyChars(string, sMPRYesNoMessageIDs[i]);
		Text_AddPrinterWithParamsAndColor(
			window,
			FONT_SYSTEM,
			string,
			10,
			i * 16,
			TEXT_SPEED_NO_TRANSFER,
			TEXT_COLOR(1, 2, 15),
			NULL);
	}
	
	DrawYesNoCursorBox(bgConfig, window, focusedOption);
	
	u16* tileBuffer = Bg_GetTilemapBuffer(bgConfig, BG_LAYER_MAIN_1);
	tileBuffer += 0x1B7; // ?
	
	int width = 8;
	int height = 4;
	
	u32 j, i;
	int tile = YES_NO_MENU_BASE_TILE;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(tileBuffer + j) = tile++;
		}
		tileBuffer += 32;
	}
	
	DrawWindowBorder(bgConfig, BG_LAYER_MAIN_1, 23, 13, width, height);
	Window_LoadTiles(window);
	Bg_CopyTilemapBufferToVRAM(bgConfig, BG_LAYER_MAIN_1);
}


static YesNoMenuOption ReadYesNoInput(BgConfig* bgConfig, Window* window, YesNoMenuOption* focusedOption) {
	if (JOY_NEW(PAD_BUTTON_A)) {
		Sound_PlayEffect(SEQ_SE_DP_DECIDE);
		return *focusedOption;
	
	} else if (JOY_NEW(PAD_BUTTON_B)) {
		Sound_PlayEffect(SEQ_SE_DP_DECIDE);
		return OPTION_CANCEL;
	
	} else if (JOY_NEW(PAD_KEY_UP) && *focusedOption == OPTION_NO) {
		Sound_PlayEffect(SEQ_SE_DP_SELECT);
		*focusedOption = OPTION_YES;
	
	} else if (JOY_NEW(PAD_KEY_DOWN) && *focusedOption == OPTION_YES) {
		Sound_PlayEffect(SEQ_SE_DP_SELECT);
		*focusedOption = OPTION_NO;
	
	} else {
		return OPTION_NONE;
	}
	
	Sound_PlayEffect(SEQ_SE_DP_SELECT);
	DrawYesNoCursorBox(bgConfig, window, *focusedOption);
	return OPTION_NONE;
}


BOOL MPRSaveApp_Init(ApplicationManager* appMan, int* state) {
	#pragma unused(state)
	
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_88, 0x4000);
	
	MPRSaveMenu* appData = ApplicationManager_NewData(appMan, sizeof(MPRSaveMenu), HEAP_ID_88);
	MI_CpuClear16(appData, sizeof(MPRSaveMenu));
	
	appData->args     = (MPRSaveAppArgs*)ApplicationManager_GetArgs(appMan);
	appData->bgConfig = appData->args->bgConfig;
	appData->saveData = appData->args->saveData;
	
	Window_AddFromTemplate(appData->bgConfig, &appData->yesNoWindow, &sYesNoWindowTemplate);
	Bg_ClearTilemap(appData->bgConfig, BG_LAYER_MAIN_1);
	
	appData->state = STATE_INIT;
	appData->string = String_Init(256, HEAP_ID_88);
	
	return TRUE;
}


static inline int ScrollArrowIdx(int frameCounter) {
	return (frameCounter / 4) % 4;
}


static inline void TryDestroyWaitDial(MPRSaveMenu* menu) {
	if (menu->waitDial != NULL) {
		DestroyWaitDial(menu->waitDial);
		menu->waitDial = NULL;
	}
}


BOOL MPRSaveApp_Main(ApplicationManager* appMan, int* state) {
	#pragma unused(state)
	
	MPRSaveMenu* menu = ApplicationManager_GetData(appMan);
	
	if (MPRComm_GetErrorFlag() && 
		menu->state != STATE_SAVE_OK && 
		menu->state != STATE_NOTIFY_WII_SHUTDOWN && 
		menu->state != STATE_TURNING_OFF_POWER && 
		menu->state != STATE_END
	) {
		menu->args->returnValue = SAVEAPP_COMM_ERROR;
		
		TryDestroyWaitDial(menu);
		
		Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_0);
		Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_1);
		
		return TRUE;
	}
	
	switch (menu->state) {
		case STATE_INIT:
			if (menu->args->shutDownAfterSave) {
				if (menu->args->dataChanged) {
					PrintMPRString(menu, MPR_TEXT_SaveAndQuitConfirm);
					menu->focusedOption = OPTION_YES;
					DrawYesNoWindow(menu->bgConfig, &menu->yesNoWindow, menu->string, menu->focusedOption);
					menu->state = STATE_SAVE_AND_QUIT_CONFIRM;
				} else {
					PrintMPRString(menu, MPR_TEXT_QuitConfirm);
					menu->focusedOption = OPTION_NO;
					DrawYesNoWindow(menu->bgConfig, &menu->yesNoWindow, menu->string, menu->focusedOption);
					menu->state = STATE_QUIT_CONFIRM;
				}
			} else {
				menu->state = STATE_NOTIFY_WII_SAVE;
			}
			break;
		
		case STATE_SAVE_AND_QUIT_CONFIRM:
			switch (ReadYesNoInput(menu->bgConfig, &menu->yesNoWindow, &menu->focusedOption)) {
				case OPTION_YES:
					ClearWindowTilemap(menu->bgConfig, &menu->yesNoWindow);
					menu->state = STATE_NOTIFY_WII_SAVE;
					break;
				
				case OPTION_NO:
				case OPTION_CANCEL:
					PrintMPRString(menu, MPR_TEXT_QuitWithoutSavingConfirm);
					menu->state = STATE_QUIT_CONFIRM;
					break;
			}
			break;
		
		case STATE_QUIT_CONFIRM:
			switch (ReadYesNoInput(menu->bgConfig, &menu->yesNoWindow, &menu->focusedOption)) {
				case OPTION_YES:
					ClearWindowTilemap(menu->bgConfig, &menu->yesNoWindow);
					Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_1);
					menu->state = STATE_NOTIFY_WII_SHUTDOWN;
					break;
				
				case OPTION_NO:
				case OPTION_CANCEL:
					Window_Remove(&menu->textWindow);
					Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_1);
					menu->args->returnValue = SAVEAPP_CANCELLED;
					return TRUE;
			}
			break;
		
		case STATE_NOTIFY_WII_SAVE:
			MPRComm_0203C1F0();
			menu->state = STATE_SAVING_INIT;
			menu->frameCounter = 0;
			menu->waitDial = NULL;
			break;
		
		case STATE_SAVING_INIT:
			if (!MPRComm_IsConnected()) {
				menu->state = STATE_SAVING_WAIT;
				sSaveThreadCtx.state = THREAD_SAVE_FAILED;
			} else if (MPRComm_ParentInSavingMode()) {
				TryDestroyWaitDial(menu);
				PrintMPRString(menu, MPR_TEXT_Saving);
				menu->waitDial = Window_AddWaitDial(&menu->textWindow, MENU_DIAL_BASE_TILE);
				
				sSaveThreadCtx.state = THREAD_SAVE_MAIN;
				OS_WakeupThreadDirect(&sSaveThread);
				
				menu->state = STATE_SAVING_WAIT;
			} else {
				menu->frameCounter++;
				if (menu->frameCounter == 60) {
					PrintMPRString(menu, MPR_TEXT_WaitingForWii);
					menu->waitDial = Window_AddWaitDial(&menu->textWindow, MENU_DIAL_BASE_TILE);
				}
			}
			break;
		
		case STATE_SAVING_WAIT:
			switch (sSaveThreadCtx.state) {
				case THREAD_IDLE:
				case THREAD_SAVE_MAIN:
					break;
				
				case THREAD_SAVE_MAIN_WAIT:
					OS_WakeupThreadDirect(&sSaveThread);
					break;
				
				case THREAD_SAVE_MAIN_OK:
					if (MPRComm_ParentInSavingEndMode()) {
						sSaveThreadCtx.state = THREAD_SAVE_FINAL;
						OS_WakeupThreadDirect(&sSaveThread);
					}
					break;
				
				case THREAD_SAVE_FINAL:
					break;
				
				case THREAD_SAVE_FINAL_WAIT:
					OS_WakeupThreadDirect(&sSaveThread);
					break;
				
				case THREAD_SAVE_FINAL_OK:
					sSaveThreadCtx.state = THREAD_IDLE;
					Sound_PlayEffect(SEQ_SE_DP_SAVE);
					TryDestroyWaitDial(menu);
					PrintMPRString(menu, MPR_TEXT_SaveOK);
					
					if (menu->args->shutDownAfterSave) {
						menu->frameCounter = 0;
						menu->state = STATE_SAVE_OK;
					} else {
						MPRComm_0203C1B4();
						menu->args->returnValue = SAVEAPP_SAVED;
						menu->state = STATE_END;
					}
					break;
				
				case THREAD_SAVE_FAILED:
					sSaveThreadCtx.state = THREAD_IDLE;
					TryDestroyWaitDial(menu);
					PrintMPRString(menu, MPR_TEXT_SaveFailed);
					menu->state = STATE_SAVE_OK;
					break;
			}
			break;
		
		case STATE_SAVE_OK:
			MPRMenuWindow_DrawScrollArrow(&menu->textWindow, ScrollArrowIdx(menu->frameCounter++));
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_1);
				menu->state = STATE_NOTIFY_WII_SHUTDOWN;
			}
			break;
		
		case STATE_NOTIFY_WII_SHUTDOWN:
			MPRComm_0203C204();
			menu->state = STATE_TURNING_OFF_POWER;
			break;
		
		case STATE_TURNING_OFF_POWER:
			if (MPRComm_0203C520() || !MPRComm_IsConnected()) {
				MPRMenu_ClearNetworkIcon();
				PrintMPRString(menu, MPR_TEXT_TurningOffPower);
				menu->args->returnValue = SAVEAPP_POWER_OFF;
				menu->frameCounter = 0;
				MPRComm_SetIdle();
				menu->state = STATE_END;
			}
			break;
		
		case STATE_END:
			MPRMenuWindow_DrawScrollArrow(&menu->textWindow, ScrollArrowIdx(menu->frameCounter++));
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				Window_Remove(&menu->textWindow);
				Bg_ClearTilemap(menu->bgConfig, BG_LAYER_MAIN_1);
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


BOOL MPRSaveApp_Exit(ApplicationManager* appMan, int* state) {
	MPRSaveMenu* appData = ApplicationManager_GetData(appMan);
	
	String_Free(appData->string);
	Window_Remove(&appData->yesNoWindow);
	Heap_Free(appData);
	Heap_Destroy(HEAP_ID_88);
	return TRUE;
}


static void SaveThreadFunc(void* saveData) {
    sSaveThreadCtx.saveData = (SaveData*)saveData;
    sSaveThreadCtx.state = THREAD_IDLE;
    SaveResult saveResult;
    
    while (TRUE) {
        switch (sSaveThreadCtx.state) {
            case THREAD_IDLE:
                break;
            
            case THREAD_SAVE_MAIN:
                SaveData_SaveStateInit(sSaveThreadCtx.saveData, 2);
                do {
                    saveResult = SaveData_SaveStateMain(sSaveThreadCtx.saveData);
                } while (saveResult == SAVE_RESULT_PROCEED);
                
                if (saveResult == SAVE_RESULT_PROCEED_FINAL) {
                    sSaveThreadCtx.timer = OS_GetVBlankCount() + (LCRNG_Next() % 600);
                    sSaveThreadCtx.state = THREAD_SAVE_MAIN_WAIT;
                } else {
                    sSaveThreadCtx.state = THREAD_SAVE_FAILED;
                }
                break;
            
            case THREAD_SAVE_MAIN_WAIT:
                if (sSaveThreadCtx.timer < OS_GetVBlankCount()) {
                    MPRComm_SetSavingFlag();
                    sSaveThreadCtx.state = THREAD_SAVE_MAIN_OK;
                }
                break;
            
            case THREAD_SAVE_MAIN_OK:
                break;
            
            case THREAD_SAVE_FINAL:
                do {
                    saveResult = SaveData_SaveStateMain(sSaveThreadCtx.saveData);
                } while (saveResult == SAVE_RESULT_PROCEED);
                
                sSaveThreadCtx.timer = OS_GetVBlankCount() + 20 + (LCRNG_Next() % 40);
                sSaveThreadCtx.state = THREAD_SAVE_FINAL_WAIT;
                break;
            
            case THREAD_SAVE_FINAL_WAIT:
                if (sSaveThreadCtx.timer <= OS_GetVBlankCount()) {
                    if (saveResult == SAVE_RESULT_OK) {
                        sSaveThreadCtx.state = THREAD_SAVE_FINAL_OK;
                    } else {
                        sSaveThreadCtx.state = THREAD_SAVE_FAILED;
                    }
                }
                break;
        }
        
        OS_SleepThread(NULL);
    }
}


void MPRSave_InitThread(SaveData* saveData) {
	OS_InitThread();
	OS_CreateThread(
		&sSaveThread,
		SaveThreadFunc,
		saveData,
		sSaveThreadStack + SAVE_FILE_THREAD_STACK_SIZE,
		SAVE_FILE_THREAD_STACK_SIZE,
		16);
	
	OS_WakeupThreadDirect(&sSaveThread);
}
