#include <nitro.h>
#include <string.h>

#include "applications/game_init/game_init.h"

#include "game_version.h"
#include "bg_window.h"
#include "font.h"
#include "gx_layers.h"
#include "heap.h"
#include "message.h"
#include "overlay_manager.h"
#include "render_window.h"
#include "savedata.h"
#include "screen_fade.h"
#include "string_gf.h"
#include "system.h"
#include "text.h"
#include "mpr_client/main.h"
#include "mpr_client/mpr_menu.h"

#include "fs/msgdata/save_corrupted.h"

#include "fs_overlay_hook.h" //[!] Overlays disabled

FS_EXTERN_OVERLAY(main_menu);

enum {
	APP_STATE_INIT,
	APP_STATE_RUN,
	APP_STATE_EXIT
};

typedef enum {
	STATE_CHECK_SAVE_FILE,
	STATE_SAVE_ERROR_START,
	STATE_SAVE_ERROR_FADE_IN,
	STATE_SAVE_ERROR_SHOW,
	STATE_SAVE_ERROR_FADE_OUT,
	STATE_EXIT
} GameInitState;

typedef enum {
	SAVE_ERROR_STATE_PRINT,
	SAVE_ERROR_STATE_PRINT_WAIT,
	SAVE_ERROR_STATE_IDLE
} SaveErrorState;

typedef struct {
	HeapID          heapID;
	GameInitState   state;
	int             displayMessageID;
	SaveErrorState  saveErrorState;
	int             printerID;
	String*         text;
	BgConfig*       bgConfig;
	MessageLoader*  messageLoader;
	Window          window;
	void*           unused1;
	SaveData*       saveData;
	void*           unused2;
} GameInitContext;

static BOOL GameInitApp_Init(ApplicationManager* appMan, int* state);
static BOOL GameInitApp_Main(ApplicationManager* appMan, int* state);
static BOOL GameInitApp_Exit(ApplicationManager* appMan, int* state);
static void GameInit_InitGraphics(GameInitContext* ctx);
static void GameInit_FreeGraphics(GameInitContext* ctx);
static void GameInit_InitWindow(GameInitContext* ctx);
static void GameInit_FreeWindow(GameInitContext* ctx);
static BOOL GameInit_Update(GameInitContext* ctx);
static BOOL GameInit_UpdateSaveErrorWindow(GameInitContext* ctx, int messageID, BOOL autoExit, int textRenderDelay);

static const WindowTemplate sWindowTemplate = {
	.bgLayer     = BG_LAYER_MAIN_0, 
	.tilemapLeft = 2, 
	.tilemapTop  = 19,
	.width       = 27,
	.height      = 4, 
	.palette     = 1, 
	.baseTile    = 365
};

const ApplicationManagerTemplate gGameInitApplication = {
	.init      = GameInitApp_Init,
	.main      = GameInitApp_Main,
	.exit      = GameInitApp_Exit,
	.overlayID = FS_OVERLAY_ID_NONE
};


static BOOL GameInitApp_Init(ApplicationManager* appMan, int* state) {
	#pragma unused(state)
	
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_88, 0x20000);
	
	GameInitContext* ctx = ApplicationManager_NewData(appMan, sizeof(GameInitContext), HEAP_ID_88);
	memset(ctx, 0, sizeof(GameInitContext));
	
	ctx->heapID = HEAP_ID_88;
	ctx->state = STATE_CHECK_SAVE_FILE;
	ctx->saveData = ((ApplicationArgs*)ApplicationManager_GetArgs(appMan))->saveData;
	
	return TRUE;
}


static BOOL GameInitApp_Main(ApplicationManager* appMan, int* state) {
	GameInitContext* ctx = ApplicationManager_GetData(appMan);
	BOOL ret = FALSE;
	
	switch (*state) {
		case APP_STATE_INIT:
			SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
			SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);
			
			SetVBlankCallback(NULL, NULL);
			SetHBlankCallback(NULL, NULL);
			
			GXLayers_DisableEngineALayers();
			GXLayers_DisableEngineBLayers();
			
			GX_SetVisiblePlane(0);
			GXS_SetVisiblePlane(0);
			
			SetAutorepeat(4, 8);
			
			GameInit_InitGraphics(ctx);
			GameInit_InitWindow(ctx);
			
			GXLayers_TurnBothDispOn();
			
			*state = APP_STATE_RUN;
			break;
		
		case APP_STATE_RUN:
			if (GameInit_Update(ctx) == TRUE) {
				*state = APP_STATE_EXIT;
			}
			break;
		
		case APP_STATE_EXIT:
			GameInit_FreeWindow(ctx);
			GameInit_FreeGraphics(ctx);
			
			SetVBlankCallback(NULL, NULL);
			
			ret = TRUE;
			break;
	}
	
	return ret;
}


//#define TEST_LAUNCH_PC_BOX
#ifdef TEST_LAUNCH_PC_BOX
#include "applications/pc_boxes/box_app_manager.h"
#endif
static BOOL GameInitApp_Exit(ApplicationManager* appMan, int* state) {
	GameInitContext* ctx = ApplicationManager_GetData(appMan);
	HeapID heapID = ctx->heapID;
	
	ApplicationManager_FreeData(appMan);
	Heap_Destroy(heapID);
	
#ifdef TEST_LAUNCH_PC_BOX
	static const ApplicationManagerTemplate sBoxAppTemplate = {
		.init      = BoxAppMan_Init,
		.main      = BoxAppMan_Main,
		.exit      = BoxAppMan_Exit,
		.overlayID = FS_OVERLAY_ID_NONE
	};
	
	EnqueueApplication(FS_OVERLAY_ID(main_menu), &sBoxAppTemplate);
#else
	EnqueueApplication(FS_OVERLAY_ID(main_menu), &gMPRMenuAppTemplate);
#endif
	return TRUE;
}


static void GameInit_InitGraphics(GameInitContext* ctx) {
	GXBanks banks = {
		GX_VRAM_BG_256_AB,
		GX_VRAM_BGEXTPLTT_NONE,
		GX_VRAM_SUB_BG_NONE,
		GX_VRAM_SUB_BGEXTPLTT_NONE,
		GX_VRAM_OBJ_NONE,
		GX_VRAM_OBJEXTPLTT_NONE,
		GX_VRAM_SUB_OBJ_NONE,
		GX_VRAM_SUB_OBJEXTPLTT_NONE,
		GX_VRAM_TEX_NONE,
		GX_VRAM_TEXPLTT_NONE
	};
	GXLayers_SetBanks(&banks);
	
	ctx->bgConfig = BgConfig_New(ctx->heapID);
	
	GraphicsModes modes = {
		GX_DISPMODE_GRAPHICS,
		GX_BGMODE_0,
		GX_BGMODE_0,
		GX_BG0_AS_2D
	};
	SetAllGraphicsModes(&modes);
	
	BgTemplate template = {
		.x          = 0,
		.y          = 0,
		.bufferSize = 0x800,
		.baseTile   = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode  = GX_BG_COLORMODE_16,
		.screenBase = GX_BG_SCRBASE_0x0000,
		.charBase   = GX_BG_CHARBASE_0x18000,
		.bgExtPltt  = GX_BG_EXTPLTT_01,
		.priority   = 1,
		.areaOver   = 0,
		.mosaic     = FALSE,
	};
	Bg_InitFromTemplate(ctx->bgConfig, BG_LAYER_MAIN_0, &template, 0);
	Bg_ClearTilemap(ctx->bgConfig, BG_LAYER_MAIN_0);
	
	LoadMessageBoxGraphics(ctx->bgConfig, BG_LAYER_MAIN_0, 512 - (18 + 12), 2, 0, ctx->heapID);
	LoadStandardWindowGraphics(ctx->bgConfig, BG_LAYER_MAIN_0, (512 - (18 + 12)) - 9, 3, 0, ctx->heapID);
	Font_LoadTextPalette(PAL_LOAD_MAIN_BG, PLTT_OFFSET(1), ctx->heapID);
	Bg_ClearTilesRange(BG_LAYER_MAIN_0, 32, 0, ctx->heapID);
	Bg_MaskPalette(BG_LAYER_MAIN_0, 0);
	Bg_MaskPalette(BG_LAYER_SUB_0, 0);
}


static void GameInit_FreeGraphics(GameInitContext* ctx) {
	Bg_ToggleLayer(BG_LAYER_MAIN_0, FALSE);
	Bg_ToggleLayer(BG_LAYER_MAIN_1, FALSE);
	Bg_ToggleLayer(BG_LAYER_MAIN_2, FALSE);
	Bg_ToggleLayer(BG_LAYER_MAIN_3, FALSE);
	Bg_ToggleLayer(BG_LAYER_SUB_0,  FALSE);
	Bg_ToggleLayer(BG_LAYER_SUB_1,  FALSE);
	Bg_ToggleLayer(BG_LAYER_SUB_2,  FALSE);
	Bg_ToggleLayer(BG_LAYER_SUB_3,  FALSE);
	
	Bg_FreeTilemapBuffer(ctx->bgConfig, BG_LAYER_MAIN_0);
	
	Heap_Free(ctx->bgConfig);
}


static void GameInit_InitWindow(GameInitContext* ctx) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	ctx->messageLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SAVE_CORRUPTED, ctx->heapID);
	Text_ResetAllPrinters();
	ctx->saveErrorState = SAVE_ERROR_STATE_PRINT;
	
	Window_AddFromTemplate(ctx->bgConfig, &ctx->window, &sWindowTemplate);
	Window_FillRectWithColor(&ctx->window, 15, 0, 0, 27 * 8, 4 * 8);
}


static void GameInit_FreeWindow(GameInitContext* ctx) {
	Window_Remove(&ctx->window);
	MessageLoader_Free(ctx->messageLoader);
}


static BOOL GameInit_Update(GameInitContext* ctx) {
	BOOL ret = FALSE;
	
	switch (ctx->state) {
		case STATE_CHECK_SAVE_FILE:
			switch (SaveData_GetLoadCheckStatus(ctx->saveData)) {
				case LOAD_RESULT_EMPTY:
				case LOAD_RESULT_OK:
					ctx->state = STATE_EXIT;
					break;
				
				case LOAD_RESULT_CORRUPT:
					ctx->state = STATE_EXIT;
					break;
				
				case LOAD_RESULT_ERROR:
					ctx->displayMessageID = SAVE_CORRUPTED_FileErased;
					ctx->state = STATE_SAVE_ERROR_START;
					break;
			}
			break;
		
		case STATE_SAVE_ERROR_START:
			Bg_MaskPalette(BG_LAYER_MAIN_0, GX_RGB(1, 1, 27));
			Bg_MaskPalette(BG_LAYER_SUB_0, GX_RGB(1, 1, 27));
			StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 6, 1, ctx->heapID);
			ctx->state = STATE_SAVE_ERROR_FADE_IN;
			break;
		
		case STATE_SAVE_ERROR_FADE_IN:
			if (IsScreenFadeDone() == TRUE) {
				ctx->state = STATE_SAVE_ERROR_SHOW;
			}
			break;
		
		case STATE_SAVE_ERROR_SHOW:
			if (GameInit_UpdateSaveErrorWindow(ctx, ctx->displayMessageID, FALSE, 4) == TRUE) {
				StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 6, 1, ctx->heapID);
				ctx->state = STATE_SAVE_ERROR_FADE_OUT;
			}
			break;
		
		case STATE_SAVE_ERROR_FADE_OUT:
			if (IsScreenFadeDone() == TRUE) {
				Bg_MaskPalette(BG_LAYER_MAIN_0, 0);
				Bg_MaskPalette(BG_LAYER_SUB_0, 0);
				ctx->state = STATE_EXIT;
			}
			break;
		
		case STATE_EXIT:
			ret = TRUE;
			break;
	}
	
	return ret;
}


static BOOL GameInit_UpdateSaveErrorWindow(GameInitContext* ctx, int messageID, BOOL autoExit, int textRenderDelay) {
	BOOL ret = FALSE;
	
	switch (ctx->saveErrorState) {
		case SAVE_ERROR_STATE_PRINT:
			Window_FillRectWithColor(&ctx->window, 15, 0, 0, 27 * 8, 4 * 8);
			Window_DrawMessageBoxWithScrollCursor(&ctx->window, FALSE, 512 - (18 + 12), 2);
			
			ctx->text = String_Init(0x400, ctx->heapID);
			MessageLoader_GetString(ctx->messageLoader, messageID, ctx->text);
			ctx->printerID = Text_AddPrinterWithParams(&ctx->window, FONT_MESSAGE, ctx->text, 0, 0, textRenderDelay, NULL);
			
			if (textRenderDelay == 0) {
				String_Free(ctx->text);
				ctx->saveErrorState++;
			}
			
			ctx->saveErrorState++;
			break;
		
		case SAVE_ERROR_STATE_PRINT_WAIT:
			if (!Text_IsPrinterActive(ctx->printerID)) {
				String_Free(ctx->text);
				ctx->saveErrorState++;
			}
			break;
		
		case SAVE_ERROR_STATE_IDLE:
			if (autoExit || JOY_NEW(PAD_BUTTON_A)) {
				ctx->saveErrorState = SAVE_ERROR_STATE_PRINT;
				ret = TRUE;
			}
			break;
	}
	
	return ret;
}
