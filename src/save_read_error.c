#include <nitro.h>
#include <string.h>

#include "save_read_error.h"

#include "constants/graphics.h"

#include "bg_window.h"
#include "brightness_controller.h"
#include "font.h"
#include "game_version.h"
#include "gx_gf.h"
#include "gx_layers.h"
#include "heap.h"
#include "message.h"
#include "render_window.h"
#include "screen_fade.h"
#include "string_gf.h"
#include "system.h"
#include "text.h"
#include "mpr_client/main.h"

#include "fs/msgdata/save_data_read_error.h"

static const GXBanks sGXBanks = {
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

static const GraphicsModes sGraphicsModes = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D
};

static const BgTemplate sBgTemplate = {
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
	.mosaic     = FALSE
};

static const WindowTemplate sWindowTemplate = {
	.bgLayer     = BG_LAYER_MAIN_0, 
	.tilemapLeft = 3, 
	.tilemapTop  = 3,
	.width       = 26,
	.height      = 18, 
	.palette     = 1, 
	.baseTile    = 35
};


void SaveReadError_ShowWindow(HeapID heapID) {
	BgConfig* bgConfig;
	Window window;
	MessageLoader* messageLoader;
	String* errorString;
	int errorMessageID = SAVE_DATA_READ_ERROR_CouldNotAccessDS;
	
	SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
	SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);
	SetVBlankCallback(NULL, NULL);
	SetHBlankCallback(NULL, NULL);
	GXLayers_DisableEngineALayers();
	GXLayers_DisableEngineBLayers();
	
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	
	SetAutorepeat(4, 8);
	gSystem.whichScreenIs3D = DS_SCREEN_MAIN;
	GXLayers_SwapDisplay();
	
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	GXLayers_SetBanks(&sGXBanks);
	
	bgConfig = BgConfig_New(heapID);
	
	SetAllGraphicsModes(&sGraphicsModes);
	Bg_InitFromTemplate(bgConfig, BG_LAYER_MAIN_0, &sBgTemplate, 0);
	Bg_ClearTilemap(bgConfig, BG_LAYER_MAIN_0);
	LoadStandardWindowGraphics(bgConfig, BG_LAYER_MAIN_0, 512 - 9, 2, 0, heapID);
	Font_LoadTextPalette(PAL_LOAD_MAIN_BG, PLTT_OFFSET(1), heapID);
	Bg_ClearTilesRange(BG_LAYER_MAIN_0, 32, 0, heapID);
	Bg_MaskPalette(BG_LAYER_MAIN_0, GX_RGB(1, 1, 27));
	Bg_MaskPalette(BG_LAYER_SUB_0, GX_RGB(1, 1, 27));
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	messageLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SAVE_DATA_READ_ERROR, heapID);
	errorString = String_Init(0x180, heapID);
	
	Text_ResetAllPrinters();
	Window_AddFromTemplate(bgConfig, &window, &sWindowTemplate);
	Window_FillRectWithColor(&window, 15, 0, 0, 26 * 8, 18 * 8);
	Window_DrawStandardFrame(&window, 0, 512 - 9, 2);
	MessageLoader_GetString(messageLoader, errorMessageID, errorString);
	Text_AddPrinterWithParams(&window, FONT_SYSTEM, errorString, 0, 0, TEXT_SPEED_INSTANT, NULL);
	String_Free(errorString);
	GXLayers_TurnBothDispOn();
	ResetScreenMasterBrightness(DS_SCREEN_MAIN);
	ResetScreenMasterBrightness(DS_SCREEN_SUB);
	BrightnessController_SetScreenBrightness(0, GX_BLEND_ALL, BRIGHTNESS_BOTH_SCREENS);
	
	/* Infinite loop */
	while (TRUE) {
		HandleConsoleFold();
		OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	}
}
