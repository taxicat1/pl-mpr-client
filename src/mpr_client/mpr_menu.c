#include <nitro.h>
#include <string.h>

#include "mpr_client/mpr_menu.h"

#include "mpr_client/mpr_client_common.h"
#include "mpr_client/mpr_save.h"
#include "mpr_client/main_menu_util.h"
#include "mpr_client/mpr_text.h"
#include "mpr_client/mpr_comm.h"
#include "mpr_client/main.h"

#include "assert.h"
#include "font.h"
#include "game_version.h"
#include "gx_layers.h"
#include "render_window.h"
#include "vram_transfer.h"
#include "sound.h"
#include "sound_playback.h"
#include "system.h"
#include "sys_task.h"

#include "applications/pc_boxes/box_app_manager.h"

#include "constants/charcode.h"
#include "constants/sdat.h"
#include "constants/text_banks.h"

#include "fs/local.h"
#include "fs/graphic/poketch.naix"
#include "fs/graphic/zukan.naix"

#define NETWORK_ICON_PALETTE_FILE  ("data/wm.NCLR")
#define NETWORK_ICON_GRAPHIC_FILE  ("data/wm.NCGR")

#define NUM_OPTIONS  (4)

#define UNFOCUSED_OPTION_BG_COLOR  GX_RGB(26, 26, 26)

typedef enum {
	SUB_APP_DEPOSIT,
	SUB_APP_WITHDRAW,
	SUB_APP_SAVE,
	SUB_APP_QUIT
} SubApplicationID;

typedef struct {
	BgConfig*              bgConfig;
	u32                    optionWindowBaseTile;
	int                    focusedOption;
	u32                    activeSubApplication;
	Window                 optionWindows[NUM_OPTIONS];
	int                    focusedBorderCycleIndex;
	Window                 window;
	Window                 subWindow;
	u8                     drawnConnectedMessage;
	int                    inputTimer;
	int                    frameCounter;
	WaitDial*              waitDial;
	u8                     unk_84; // TODO what does this do?
	u8                     unk_85; // TODO what does this do?
	u8                     pad[2];
	ApplicationManager*    subApplication;
	PokemonStorageSession  boxAppArgs;
	MPRSaveAppArgs         saveAppArgs;
	u8                     dataChanged;
	String*                string;
} MPRMenuApp;

typedef struct {
	u32         unused;
	u32         strength;
	u32         unused2;
	u16         x;
	u16         y;
	u8          screenID;
	u8          createdOnSubScreen;
	u8          pad[2];
	SysTask*    task;
	GXOamAttr*  oam;
} NetworkIcon;

static void PrintMPRString(MPRMenuApp* mprMenu, int messageID);
static void PrintFullScreenStringWindow(
	BgConfig* bgConfig,
	Window* window,
	String* string,
	int messageID,
	int baseTile,
	int x,
	int y,
	int width,
	int height,
	BOOL isHeaderText);
static void InitBgs(MPRMenuApp* mprMenu);
static void LoadWarningGraphics(MPRMenuApp* mprMenu);
static void LoadWindowGraphics(MPRMenuApp* mprMenu);
static NetworkIcon* CreateNetworkIcon(u32 unused, HeapID heapID, u16 x, u16 y, u32 unused2, u32 unused3);
static inline NNS_G2D_VRAM_TYPE GetNetworkIconVramType(NetworkIcon* icon);
static inline void UpdateNetworkIconOAM(NetworkIcon* icon);
static void UpdateNetworkIconFunc(SysTask* task, void* param);
static inline void ClearOAM(GXOamAttr* oam);
static void FreeNetworkIcon(NetworkIcon* icon);
static int RenderOptionWindows(MPRMenuApp* mprMenu);
static void RenderOptionsFrames(MPRMenuApp* mprMenu, int focusedOption);
static void FocusNextOption(MPRMenuApp* mprMenu, int direction);
static void FreeApplicationResources(MPRMenuApp* mprMenu);
static void StepFocusColorCycle(MPRMenuApp* mprMenu);
static void WarningBackgroundScrollFunc(void* param);
static BOOL MPRMenuApp_Init(ApplicationManager* appMan, int* state);
static inline void TryDestroyWaitDial(MPRMenuApp* mprMenu);
static BOOL WiiCommOk(MPRMenuApp* mprMenu, int* state);
static void PrintConnectedToRanch(BgConfig* bgConfig, Window* window);
static void TryPrintConnectedToRanch(MPRMenuApp* mprMenu);
static BOOL MPRMenuApp_Main(ApplicationManager* appMan, int* state);
static void InitSubApplication(MPRMenuApp* mprMenu);
static BOOL MPRMenuApp_Exit(ApplicationManager* appMan, int* state);

const ApplicationManagerTemplate gMPRMenuAppTemplate = {
	.init      = MPRMenuApp_Init,
	.main      = MPRMenuApp_Main,
	.exit      = MPRMenuApp_Exit,
	.overlayID = FS_OVERLAY_ID_NONE
};

static u32 sNetworkIconColorParam;
static BOOL sScrollWarningBackground;
static NetworkIcon* sNetworkIcon;


#define BASE_TILE_MAIN_MENU  (49)

void MPRMenuWindow_PrintString(BgConfig* bgConfig, Window* window, String* string) {
	Bg_ToggleLayer(BG_LAYER_MAIN_1, FALSE);
	
	if (window->bgConfig != NULL) {
		Window_Remove(window);
	}
	
	LoadMessageBoxGraphics(bgConfig, BG_LAYER_MAIN_1, 19, 6, 0, HEAP_ID_MAIN_MENU);
	
	MainMenuWindow menuWindow;
	MainMenuUtil_InitWindow(&menuWindow, window, 5, TEXT_BANK_UNIFIED_MAIN_MENU_ALERTS, 19, 6);
	MainMenuWindow_SetDimensionsAndBasetile(&menuWindow, 26, TEXT_LINES_TILES(2), BASE_TILE_MAIN_MENU);
	menuWindow.bgLayer = BG_LAYER_MAIN_1;
	MainMenuUtil_ShowWindowAtPos(bgConfig, &menuWindow, 3, 19, menuWindow.textEntryID);
	Window_FillTilemap(menuWindow.window, menuWindow.backgroundColorIdx);
	
	Text_AddPrinterWithParamsAndColor(
		menuWindow.window,
		FONT_MESSAGE,
		string,
		menuWindow.textXOffset,
		menuWindow.textYOffset,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(1, 2, 0),
		NULL);
	
	Window_DrawMessageBoxWithScrollCursor(menuWindow.window, FALSE, menuWindow.frameBaseTile, menuWindow.framePltt);
	
	Bg_ToggleLayer(BG_LAYER_MAIN_1, TRUE);
}


static void PrintMPRString(MPRMenuApp* mprMenu, int messageID) {
	MPRText_CopyChars(mprMenu->string, messageID);
	MPRMenuWindow_PrintString(mprMenu->bgConfig, &mprMenu->window, mprMenu->string);
}


static const u8 sScrollArrowTileOffsets[] = { 0, 1, 2, 1 };

void MPRMenuWindow_DrawScrollArrow(Window* window, int idx) {
	u8 bgID  = Window_GetBgLayer(window);
	u8 xPos  = Window_GetXPos(window);
	u8 yPos  = Window_GetYPos(window);
	u8 width = Window_GetWidth(window);
	
	if (idx < 0) {
		Bg_FillTilemapRect(window->bgConfig, bgID, 29, xPos + width + 1, yPos + 2, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, 30, xPos + width + 2, yPos + 2, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, 29, xPos + width + 1, yPos + 3, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, 30, xPos + width + 2, yPos + 3, 1, 1, 16);
	} else {
		Bg_FillTilemapRect(window->bgConfig, bgID, (sScrollArrowTileOffsets[idx] * 4) + 37, xPos + width + 1, yPos + 2, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, (sScrollArrowTileOffsets[idx] * 4) + 38, xPos + width + 2, yPos + 2, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, (sScrollArrowTileOffsets[idx] * 4) + 39, xPos + width + 1, yPos + 3, 1, 1, 16);
		Bg_FillTilemapRect(window->bgConfig, bgID, (sScrollArrowTileOffsets[idx] * 4) + 40, xPos + width + 2, yPos + 3, 1, 1, 16);
	}
	
	Bg_CopyTilemapBufferToVRAM(window->bgConfig, bgID);
}


static void PrintFullScreenStringWindow(
	BgConfig* bgConfig,
	Window* window,
	String* string,
	int messageID,
	int baseTile,
	int x,
	int y,
	int width,
	int height,
	BOOL isHeaderText
) {
	MPRText_CopyChars(string, messageID);
	
	if (window->bgConfig != NULL) {
		Window_Remove(window);
	}
	
	LoadStandardWindowGraphics(bgConfig, BG_LAYER_MAIN_0, 1, 2, 0, HEAP_ID_MAIN_MENU);
	*HW_BG_A_PLTT_COLOR(2, 1) = UNFOCUSED_OPTION_BG_COLOR;
	
	MainMenuWindow menuWindow;
	MainMenuUtil_InitWindow(&menuWindow, window, 0, TEXT_BANK_UNIFIED_MAIN_MENU_ALERTS, 1, 2);
	MainMenuWindow_SetDimensionsAndBasetile(&menuWindow, width, height, baseTile);
	menuWindow.bgLayer = BG_LAYER_MAIN_0;
	MainMenuUtil_ShowWindowAtPos(bgConfig, &menuWindow, x, y, menuWindow.textEntryID);
	Window_FillTilemap(menuWindow.window, menuWindow.backgroundColorIdx);
	
	int xOffset = menuWindow.textXOffset;
	if (isHeaderText) {
		// Center
		int stringWidth = Font_CalcStringWidth(menuWindow.font, string, Font_GetAttribute(menuWindow.font, FONTATTR_LETTER_SPACING));
		int windowWidth = Window_GetWidth(menuWindow.window) * 8;
		xOffset += (windowWidth - stringWidth) / 2;
	}
	
	int yOffset = menuWindow.textYOffset;
	
	TextColor textColor;
	if (isHeaderText) {
		textColor = TEXT_COLOR(3, 4, 15);
	} else {
		textColor = menuWindow.textColor;
	}
	
	Text_AddPrinterWithParamsAndColor(
		menuWindow.window,
		menuWindow.font,
		string,
		xOffset,
		yOffset,
		TEXT_SPEED_NO_TRANSFER,
		textColor,
		NULL);
	
	Window_DrawStandardFrame(menuWindow.window, FALSE, menuWindow.frameBaseTile, menuWindow.framePltt);
}


void InitBgs(MPRMenuApp* mprMenu) {
	GF_ASSERT(mprMenu->bgConfig == NULL);
	
	GXBanks vramBanks = {
		GX_VRAM_BG_128_A,
		GX_VRAM_BGEXTPLTT_NONE,
		GX_VRAM_SUB_BG_128_C,
		GX_VRAM_SUB_BGEXTPLTT_NONE,
		GX_VRAM_OBJ_64_E,
		GX_VRAM_OBJEXTPLTT_NONE,
		GX_VRAM_SUB_OBJ_16_I,
		GX_VRAM_SUB_OBJEXTPLTT_NONE,
		GX_VRAM_TEX_0_B,
		GX_VRAM_TEXPLTT_01_FG
	};
	
	GraphicsModes graphicsModes = {
		GX_DISPMODE_GRAPHICS,
		GX_BGMODE_0,
		GX_BGMODE_0,
		GX_BG0_AS_2D
	};
	
	GXLayers_SetBanks(&vramBanks);
	
	mprMenu->bgConfig = BgConfig_New(HEAP_ID_MAIN_MENU);
	mprMenu->saveAppArgs.bgConfig = mprMenu->bgConfig;
	
	SetAllGraphicsModes(&graphicsModes);
	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_64K);
	
	MainMenuUtil_InitBGLayer(mprMenu->bgConfig, BG_LAYER_MAIN_0, BG_SCREEN_SIZE_256x512, 0x7000, 0);
	G2_SetBG0Priority(2);
	Bg_ClearTilesRange(BG_LAYER_MAIN_0, 32, 0, HEAP_ID_MAIN_MENU);
	
	MainMenuUtil_InitBGLayer(mprMenu->bgConfig, BG_LAYER_MAIN_1, BG_SCREEN_SIZE_256x256, 0x5800, 0x8000);
	G2_SetBG1Priority(1);
	Bg_ClearTilesRange(BG_LAYER_MAIN_1, 32, 0, HEAP_ID_MAIN_MENU);
	
	MainMenuUtil_InitBGLayer(mprMenu->bgConfig, BG_LAYER_MAIN_2, BG_SCREEN_SIZE_256x512, 0x6000, 0x10000);
	G2_SetBG2Priority(0);
	Bg_ClearTilesRange(BG_LAYER_MAIN_2, 32, 0, HEAP_ID_MAIN_MENU);
	
	Text_ResetAllPrinters();
	
	Font_LoadTextPalette(PAL_LOAD_MAIN_BG, PLTT_OFFSET(1), HEAP_ID_MAIN_MENU);
	Font_LoadTextPalette(PAL_LOAD_MAIN_BG, PLTT_OFFSET(0), HEAP_ID_MAIN_MENU);
	
	*HW_BG_A_PLTT_COLOR(1, 15) = UNFOCUSED_OPTION_BG_COLOR;
	
	Font_LoadScreenIndicatorsPalette(PAL_LOAD_MAIN_BG, PLTT_OFFSET(5), HEAP_ID_MAIN_MENU);
	
	*HW_BG_A_PLTT_COLOR(1, 17) = UNFOCUSED_OPTION_BG_COLOR;
	
	Utility_Clear2DMainOAM(HEAP_ID_MAIN_MENU);
	GXLayers_EngineAToggleLayers(GX_PLANEMASK_OBJ, TRUE);
	MainMenuUtil_InitBGLayer(mprMenu->bgConfig, BG_LAYER_SUB_0, BG_SCREEN_SIZE_256x256, 0xF000, 0);
	
	G2S_SetBG0Priority(2);
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__POKETCH;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__POKETCH;
	}
	Graphics_LoadTilesToBgLayer(narc, unavailable_bg_tiles_NCGR_lz, mprMenu->bgConfig, BG_LAYER_SUB_0, 0, 0, TRUE, HEAP_ID_MAIN_MENU);
	Graphics_LoadTilemapToBgLayer(narc, unavailable_NSCR_lz, mprMenu->bgConfig, BG_LAYER_SUB_0, 0, 0, TRUE, HEAP_ID_MAIN_MENU);
	Graphics_LoadPalette(narc, unavailable_bg_tiles_NCLR, PAL_LOAD_SUB_BG, 0, 0, HEAP_ID_MAIN_MENU);
	G2S_BlendNone();
	MainMenuUtil_InitBGLayer(mprMenu->bgConfig, BG_LAYER_SUB_1, BG_SCREEN_SIZE_256x256, 0xE000, 0x10000);
	
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__ZUKAN;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__ZUKAN;
	}
	Graphics_LoadPaletteWithSrcOffset(narc, digits_cell_NCER_lz, PAL_LOAD_SUB_BG, 96, 32, 32, HEAP_ID_MAIN_MENU);
}


static void LoadWarningGraphics(MPRMenuApp* mprMenu) {
	GX_LoadBGPltt(LOCAL_FILE(warning_bg_ncl) + 32, 0xE0, 0x100);
	Bg_LoadTiles(mprMenu->bgConfig, 1, LOCAL_FILE(warning_bg_ncg) + 36, 0x800, 0);
	
	int j, i;
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 32; j++) {
			// 16 x 4 tiles hardcoded
			// Correctly:
			//int width  = *(u32*)(LOCAL_FILE(warning_bg_ncg) + 24);
			//int height = *(u32*)(LOCAL_FILE(warning_bg_ncg) + 28);
			
			u32 offset = ((i % 4) * 16) + ((j + ((i / 4) * 8)) % 16);
			Bg_FillTilemapRect(mprMenu->bgConfig, 1, offset, j, i, 1, 1, LOCAL_FILE(warning_bg_ncg)[2100 + offset] + 7);
		}
	}
	
	Bg_CopyTilemapBufferToVRAM(mprMenu->bgConfig, 1);
}


static void LoadWindowGraphics(MPRMenuApp* mprMenu) {
	Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_0);
	
	Bg_ScheduleScroll(mprMenu->bgConfig, BG_LAYER_MAIN_1, BG_OFFSET_UPDATE_SET_X, 0);
	Bg_ScheduleScroll(mprMenu->bgConfig, BG_LAYER_MAIN_1, BG_OFFSET_UPDATE_SET_Y, 0);
	
	Bg_ClearTilesRange(BG_LAYER_MAIN_1, 32, 0, HEAP_ID_MAIN_MENU);
	Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_1);
	
	LoadStandardWindowGraphics(mprMenu->bgConfig, BG_LAYER_MAIN_0, 1, 2, STANDARD_WINDOW_SYSTEM, HEAP_ID_MAIN_MENU);
	LoadStandardWindowGraphics(mprMenu->bgConfig, BG_LAYER_MAIN_1, 1, 2, STANDARD_WINDOW_SYSTEM, HEAP_ID_MAIN_MENU);
	
	*HW_BG_A_PLTT_COLOR(2, 1) = UNFOCUSED_OPTION_BG_COLOR;
	
	LoadStandardWindowGraphics(mprMenu->bgConfig, BG_LAYER_MAIN_0, 10, 3, STANDARD_WINDOW_FIELD, HEAP_ID_MAIN_MENU);
}


static NetworkIcon* CreateNetworkIcon(u32 unused, HeapID heapID, u16 x, u16 y, u32 unused2, u32 unused3) {
	MPRMenu_LoadNetworkIconPalette(NNS_G2D_VRAM_TYPE_2DMAIN, 448, heapID);
	MPRMenu_LoadNetworkIconGraphic(NNS_G2D_VRAM_TYPE_2DMAIN, heapID);
	
	NetworkIcon* icon = Heap_AllocAtEnd(heapID, sizeof(NetworkIcon));
	
	icon->task = SysTask_ExecuteAfterVBlank(UpdateNetworkIconFunc, icon, 5);
	icon->x = x;
	icon->y = y;
	icon->unused = 0;
	icon->strength = 0;
	icon->unused2 = 0;
	icon->createdOnSubScreen = FALSE;
	icon->screenID = 1;
	icon->oam = (GXOamAttr*)HW_OAM;
	
	return icon;
}


static inline NNS_G2D_VRAM_TYPE GetNetworkIconVramType(NetworkIcon* icon) {
	switch (icon->screenID) {
		case 1:
			if (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB) {
				return NNS_G2D_VRAM_TYPE_2DMAIN;
			} else {
				return NNS_G2D_VRAM_TYPE_2DSUB;
			}
		
		case 2:
			if (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB) {
				return NNS_G2D_VRAM_TYPE_2DSUB;
			} else {
				return NNS_G2D_VRAM_TYPE_2DMAIN;
			}
		
		default:
			return NNS_G2D_VRAM_TYPE_2DMAIN;
	}
}


static inline void UpdateNetworkIconOAM(NetworkIcon* icon) {
	NNS_G2D_VRAM_TYPE vramType = GetNetworkIconVramType(icon);
	int strength = WM_LINK_LEVEL_3 - WM_GetLinkLevel();
	icon->strength = strength;
	
	GXOBJVRamModeChar ramMode;
	GXVRamOBJ gxBank;
	if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
		ramMode = GX_GetOBJVRamModeChar();
		gxBank = GX_GetBankForOBJ();
	} else {
		ramMode = GXS_GetOBJVRamModeChar();
		gxBank = GX_GetBankForSubOBJ();
	}
	
	int oamOffset;
	switch (ramMode) {
		case GX_OBJVRAMMODE_CHAR_1D_32K:
			if (gxBank == GX_VRAM_OBJ_16_G || gxBank == GX_VRAM_OBJ_16_F) {
				oamOffset = 512 - 16 + (4 * strength);
			} else {
				oamOffset = 1024 - 16 + (4 * strength);
			}
			break;
		
		case GX_OBJVRAMMODE_CHAR_1D_128K:
			if (gxBank == GX_VRAM_OBJ_80_EF || gxBank == GX_VRAM_OBJ_80_EG) {
				oamOffset = 640 - 4 + strength;
			} else {
				oamOffset = 1024 - 4 + strength;
			}
			break;
		
		default:
			oamOffset = 1024 - 8 + (2 * strength);
			break;
	}
	
	GXOamAttr* oam;
	if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
		oam = (GXOamAttr*)HW_OAM;
	} else {
		oam = (GXOamAttr*)HW_OAM_END;
	}
	
	G2_SetOBJAttr(
		oam,
		icon->x,
		icon->y,
		0,
		GX_OAM_MODE_NORMAL,
		FALSE,
		GX_OAM_EFFECT_NONE,
		GX_OAM_SHAPE_16x16,
		GX_OAM_COLORMODE_16,
		oamOffset,
		sNetworkIconColorParam,
		0);
	
	if (oam != icon->oam) {
		G2_SetOBJAttr(
			icon->oam,
			0,
			0,
			0,
			GX_OAM_MODE_NORMAL,
			FALSE,
			GX_OAM_EFFECT_NODISPLAY,
			GX_OAM_SHAPE_16x16,
			GX_OAM_COLORMODE_16,
			0,
			0,
			0);
		
		icon->oam = oam;
	}
}


void UpdateNetworkIconFunc(SysTask* task, void* param) {
	#pragma unused(task)
	UpdateNetworkIconOAM((NetworkIcon*)param);
}


static inline void ClearOAM(GXOamAttr* oam) {
	G2_SetOBJAttr(oam, 0, 0, 0, GX_OAM_MODE_NORMAL, 0, GX_OAM_EFFECT_NODISPLAY, GX_OAM_SHAPE_16x16, GX_OAM_COLORMODE_16, 0, 0, 0);
}


static void FreeNetworkIcon(NetworkIcon* icon) {
	SysTask_Done(icon->task);
	ClearOAM((GXOamAttr*)HW_OAM);
	
	if (icon->createdOnSubScreen) {
		ClearOAM((GXOamAttr*)HW_OAM_END);
	}
	
	Heap_Free(icon);
}


void MPRMenu_LoadNetworkIconPalette(NNS_G2D_VRAM_TYPE vramType, u32 offset, HeapID heapID) {
	void* wmBuffer = Heap_AllocAtEnd(heapID, 600);
	if (wmBuffer != NULL) {
		ReadFileToBuffer(NETWORK_ICON_PALETTE_FILE, &wmBuffer);
		DC_FlushRange(wmBuffer, 600);
		
		NNSG2dPaletteData* paletteData;
		NNS_G2dGetUnpackedPaletteData(wmBuffer, &paletteData);
		
		if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
			GX_LoadOBJPltt(paletteData->pRawData, offset, PALETTE_SIZE_BYTES);
		} else {
			GXS_LoadOBJPltt(paletteData->pRawData, offset, PALETTE_SIZE_BYTES);
		}
		
		sNetworkIconColorParam = offset / 32;
		
		Heap_Free(wmBuffer);
	}
}


void MPRMenu_LoadNetworkIconGraphic(NNS_G2D_VRAM_TYPE vramType, HeapID heapID) {
	void* ncgrFile = Heap_AllocAtEnd(heapID, 600);
	if (ncgrFile) {
		ReadFileToBuffer(NETWORK_ICON_GRAPHIC_FILE, &ncgrFile);
		DC_FlushRange(ncgrFile, 600);
		
		NNSG2dCharacterData* charData;
		NNS_G2dGetUnpackedCharacterData(ncgrFile, &charData);
		
		GXOBJVRamModeChar ramMode;
		GXVRamOBJ gxBank;
		if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
			ramMode = GX_GetOBJVRamModeChar();
			gxBank = GX_GetBankForOBJ();
		} else {
			ramMode = GXS_GetOBJVRamModeChar();
			gxBank = GX_GetBankForSubOBJ();
		}
		
		int offset;
		switch (ramMode) {
			case GX_OBJVRAMMODE_CHAR_1D_32K:
				if (gxBank == GX_VRAM_OBJ_16_G || gxBank == GX_VRAM_OBJ_16_F) {
					offset = PLTT_OFFSET(512 - 16);
				} else {
					offset = PLTT_OFFSET(1024 - 16);
				}
				break;
			
			case GX_OBJVRAMMODE_CHAR_1D_128K:
				if (gxBank == GX_VRAM_OBJ_80_EF || gxBank == GX_VRAM_OBJ_80_EG) {
					offset = PLTT_OFFSET(2560 - 16);
				} else {
					offset = PLTT_OFFSET(4096 - 16);
				}
				break;
			
			default:
				offset = PLTT_OFFSET(2048 - 16);
				break;
		}
		
		if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
			GX_LoadOBJ(charData->pRawData, offset, PLTT_OFFSET(4 * 4));
		} else {
			GXS_LoadOBJ(charData->pRawData, offset, PLTT_OFFSET(4 * 4));
		}
		
		Heap_Free(ncgrFile);
	}
}


void MPRMenu_SetNetworkIconDefault(void) {
	// Top right corner of screen
	MPRMenu_SetNetworkIcon(256 - 16, 0, 0);
}


void MPRMenu_SetNetworkIcon(u16 x, u16 y, u32 unused) {
	if (sNetworkIcon != NULL) {
		MPRMenu_ClearNetworkIcon();
	}
	
	sNetworkIcon = CreateNetworkIcon(0, HEAP_ID_MAIN_MENU, x, y, 0, 0);
}


void MPRMenu_ClearNetworkIcon(void) {
	if (sNetworkIcon != NULL) {
		FreeNetworkIcon(sNetworkIcon);
		sNetworkIcon = NULL;
	}
}


static int sMPRMenuOptionsMessageIDs[] = {
	 MPR_TEXT_DepositToRanch,
	 MPR_TEXT_WithdrawFromRanch,
	 MPR_TEXT_Save,
	 MPR_TEXT_Quit
};

int RenderOptionWindows(MPRMenuApp* mprMenu) {
	mprMenu->optionWindowBaseTile = 49;
	
	u32 i, offset = 1;
	for (i = 0; i < NUM_OPTIONS; i++) {
		MPRText_CopyChars(mprMenu->string, sMPRMenuOptionsMessageIDs[i]);
		
		MainMenuWindow menuWindow;
		MainMenuUtil_InitWindow(&menuWindow, &mprMenu->optionWindows[i], 1, TEXT_BANK_UNIFIED_MAIN_MENU_OPTIONS, 1, 2);
		MainMenuWindow_SetDimensionsAndBasetile(&menuWindow, 26, 2, mprMenu->optionWindowBaseTile);
		MainMenuUtil_ShowWindowAtPos(mprMenu->bgConfig, &menuWindow, 3, offset, menuWindow.textEntryID);
		Window_FillTilemap(menuWindow.window, menuWindow.backgroundColorIdx);
		
		Text_AddPrinterWithParamsAndColor(
			menuWindow.window,
			menuWindow.font,
			mprMenu->string,
			menuWindow.textXOffset,
			menuWindow.textYOffset,
			TEXT_SPEED_NO_TRANSFER,
			menuWindow.textColor,
			NULL);
		
		Window_DrawStandardFrame(menuWindow.window, FALSE, menuWindow.frameBaseTile, menuWindow.framePltt);
		
		offset += 4;
		mprMenu->optionWindowBaseTile += 52; // ?
	}
	
	// Return value unused
	return 0;
}


#define UNFOCUSED_OPTION_FRAME_BASE_TILE  (1)
#define FOCUSED_OPTION_FRAME_BASE_TILE    (10)

static void RenderOptionsFrames(MPRMenuApp* mprMenu, int focusedOption) {
	for (u32 i = 0; i < NUM_OPTIONS; i++) {
		if (i == focusedOption) {
			Window_DrawStandardFrame(&mprMenu->optionWindows[i], TRUE, FOCUSED_OPTION_FRAME_BASE_TILE, 3);
			
			Bg_ChangeTilemapRectPalette(
				mprMenu->bgConfig,
				BG_LAYER_MAIN_0,
				Window_GetXPos(&mprMenu->optionWindows[i]),
				Window_GetYPos(&mprMenu->optionWindows[i]),
				Window_GetWidth(&mprMenu->optionWindows[i]),
				Window_GetHeight(&mprMenu->optionWindows[i]),
				0);
		} else {
			Window_DrawStandardFrame(&mprMenu->optionWindows[i], TRUE, UNFOCUSED_OPTION_FRAME_BASE_TILE, 2);
			
			Bg_ChangeTilemapRectPalette(
				mprMenu->bgConfig,
				BG_LAYER_MAIN_0,
				Window_GetXPos(&mprMenu->optionWindows[i]),
				Window_GetYPos(&mprMenu->optionWindows[i]),
				Window_GetWidth(&mprMenu->optionWindows[i]),
				Window_GetHeight(&mprMenu->optionWindows[i]),
				1);
		}
	}
	
	Bg_CopyTilemapBufferToVRAM(mprMenu->bgConfig, BG_LAYER_MAIN_0);
}


static void FocusNextOption(MPRMenuApp* mprMenu, int direction) {
	int prevOption = mprMenu->focusedOption;
	int newOption = prevOption + direction;
	
	if (newOption == -1) {
		newOption = 0;
	}
	
	if (newOption == NUM_OPTIONS) {
		newOption = NUM_OPTIONS - 1;
	}
	
	if (newOption != prevOption) {
		Sound_PlayEffect(SEQ_SE_DP_SELECT);
	}
	
	mprMenu->focusedOption = newOption;
}


static void FreeApplicationResources(MPRMenuApp* mprMenu) {
	GF_ASSERT(mprMenu->bgConfig != NULL);
	
	for (u32 i = 0; i < NUM_OPTIONS; i++) {
		if (mprMenu->optionWindows[i].bgConfig != NULL) {
			Window_ClearAndCopyToVRAM(&mprMenu->optionWindows[i]);
			Window_Remove(&mprMenu->optionWindows[i]);
		}
	}
	
	if (mprMenu->window.bgConfig != NULL) {
		Window_ClearAndCopyToVRAM(&mprMenu->window);
		Window_Remove(&mprMenu->window);
	}
	
	if (mprMenu->subWindow.bgConfig != NULL) {
		Window_ClearAndCopyToVRAM(&mprMenu->subWindow);
		Window_Remove(&mprMenu->subWindow);
	}
	
	Bg_FreeTilemapBuffer(mprMenu->bgConfig, BG_LAYER_MAIN_0);
	Bg_FreeTilemapBuffer(mprMenu->bgConfig, BG_LAYER_MAIN_1);
	Bg_FreeTilemapBuffer(mprMenu->bgConfig, BG_LAYER_MAIN_2);
	Bg_FreeTilemapBuffer(mprMenu->bgConfig, BG_LAYER_SUB_0);
	Bg_FreeTilemapBuffer(mprMenu->bgConfig, BG_LAYER_SUB_1);
	
	Heap_Free(mprMenu->bgConfig);
	mprMenu->bgConfig = NULL;
	mprMenu->drawnConnectedMessage = FALSE;
	
	SetVBlankCallback(NULL, NULL);
}


#define COLORS_LIST_END  (0)

static GXRgb sFocusedOptionBorderColors[] = {
	GX_RGB( 1, 28, 20),
	GX_RGB( 3, 28, 20),
	GX_RGB( 5, 28, 20),
	GX_RGB( 7, 28, 20),
	GX_RGB( 9, 28, 20),
	GX_RGB(11, 28, 20),
	GX_RGB(13, 28, 20),
	GX_RGB(15, 28, 20),
	GX_RGB(17, 28, 20),
	GX_RGB(19, 28, 20),
	GX_RGB(21, 28, 20),
	GX_RGB(23, 28, 20),
	GX_RGB(25, 28, 20),
	GX_RGB(27, 28, 20),
	GX_RGB(29, 28, 20),
	GX_RGB(31, 28, 20),
	GX_RGB(29, 28, 20),
	GX_RGB(27, 28, 20),
	GX_RGB(25, 28, 20),
	GX_RGB(23, 28, 20),
	GX_RGB(21, 28, 20),
	GX_RGB(19, 28, 20),
	GX_RGB(17, 28, 20),
	GX_RGB(15, 28, 20),
	GX_RGB(13, 28, 20),
	GX_RGB(11, 28, 20),
	GX_RGB( 9, 28, 20),
	GX_RGB( 7, 28, 20),
	GX_RGB( 5, 28, 20),
	GX_RGB( 3, 28, 20),
	COLORS_LIST_END
};

static void StepFocusColorCycle(MPRMenuApp* mprMenu) {
	if (sFocusedOptionBorderColors[mprMenu->focusedBorderCycleIndex] == COLORS_LIST_END) {
		mprMenu->focusedBorderCycleIndex = 0;
	}
	
	*HW_BG_A_PLTT_COLOR(3, 6) = sFocusedOptionBorderColors[mprMenu->focusedBorderCycleIndex++];
}


static void WarningBackgroundScrollFunc(void* param) {
	BgConfig* bgConfig = (BgConfig*)param;
	
	if (sScrollWarningBackground) {
		Bg_ScheduleScroll(bgConfig, BG_LAYER_MAIN_1, BG_OFFSET_UPDATE_ADD_X, 1);
		Bg_ScheduleScroll(bgConfig, BG_LAYER_MAIN_1, BG_OFFSET_UPDATE_ADD_Y, 1);
	}
	
	VramTransfer_Process();
	RenderOam_Transfer();
	Bg_RunScheduledUpdates(bgConfig);
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


static BOOL MPRMenuApp_Init(ApplicationManager* appMan, int* state) {
	ApplicationArgs* args = (ApplicationArgs*)ApplicationManager_GetArgs(appMan);
	
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_MAIN_MENU, HEAP_SIZE_MAIN_MENU);
	
	MPRMenuApp* mprMenu = (MPRMenuApp*)ApplicationManager_NewData(appMan, sizeof(MPRMenuApp), HEAP_ID_MAIN_MENU);
	memset(mprMenu, 0, sizeof(MPRMenuApp));
	
	mprMenu->bgConfig = NULL;
	mprMenu->boxAppArgs.saveData  = args->saveData;
	mprMenu->saveAppArgs.saveData = args->saveData;
	Window_Init(&mprMenu->window);
	
	SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
	SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);
	
	Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_DEFAULT);
	Sound_SetScene(SOUND_SCENE_NONE);
	Sound_LoadSoundEffectsForScene(SOUND_SCENE_FIELD);
	
	// NOTE: in localized versions, 256 characters is not enough to display the warning message
	mprMenu->string = String_Init(256, HEAP_ID_MAIN_MENU);
	mprMenu->dataChanged = FALSE;
	
	Font_InitManager(FONT_SUBSCREEN, HEAP_ID_MAIN_MENU);
	
	InitBgs(mprMenu);
	
	return TRUE;
}


static inline void TryDestroyWaitDial(MPRMenuApp* mprMenu) {
	if (mprMenu->waitDial != NULL) {
		WaitDial_Destroy(mprMenu->waitDial);
		mprMenu->waitDial = NULL;
	}
}


static BOOL WiiCommOk(MPRMenuApp* mprMenu, int* state) {
	if (MPRComm_GetErrorFlag()) {
		TryDestroyWaitDial(mprMenu);
		
		GX_SetMasterBrightness(0);
		GXS_SetMasterBrightness(0);
		
		MPRComm_Reset();
		MPRMenu_SetNetworkIconDefault();
		
		*state = 7;
		return FALSE;
	}
	
	return TRUE;
}


static void PrintConnectedToRanch(BgConfig* bgConfig, Window* window) {
	int width;
	String* strConnected = String_Init(64, HEAP_ID_MAIN_MENU);
	String* strRanchName = String_Init(64, HEAP_ID_MAIN_MENU);
	
	MPRText_CopyChars(strConnected, MPR_TEXT_ConnectedToRanch);
	String_CopyChars(strRanchName, MPRComm_GetRanchName());
	
	width = Font_CalcStringWidth(FONT_SUBSCREEN, strConnected, Font_GetAttribute(FONT_SUBSCREEN, FONTATTR_LETTER_SPACING));
	int widthRanchName = Font_CalcStringWidth(FONT_SUBSCREEN, strRanchName, Font_GetAttribute(FONT_SUBSCREEN, FONTATTR_LETTER_SPACING));
	
	if (width < widthRanchName) {
		width = widthRanchName;
	}
	
	Window_Add(bgConfig, window, BG_LAYER_SUB_1, 1, 19, 30, 4, 1, 1);
	Window_FillTilemap(window, 0);
	
	Text_AddPrinterWithParamsAndColor(
		window,
		FONT_SUBSCREEN,
		strConnected,
		256 - 16 - width,
		0,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(3, 2, 1),
		NULL);
	
	Text_AddPrinterWithParamsAndColor(
		window,
		FONT_SUBSCREEN,
		strRanchName,
		256 - 16 - width,
		16,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(3, 2, 1),
		NULL);
	
	Window_CopyToVRAM(window);
	
	String_Free(strRanchName);
	String_Free(strConnected);
}


static void TryPrintConnectedToRanch(MPRMenuApp* mprMenu) {
	if (!mprMenu->drawnConnectedMessage) {
		charcode_t* ranchName = MPRComm_GetRanchName();
		if (ranchName[0] != CHAR_EOS) {
			PrintConnectedToRanch(mprMenu->bgConfig, &mprMenu->subWindow);
			mprMenu->drawnConnectedMessage = TRUE;
		}
	}
}


static inline int ScrollArrowIdx(int frameCounter) {
	return (frameCounter / 4) % 4;
}


static BOOL MPRMenuApp_Main(ApplicationManager* appMan, int* state) {
	MPRMenuApp* mprMenu = (MPRMenuApp*)ApplicationManager_GetData(appMan);
	
	if (mprMenu->subApplication == NULL) {
		StepFocusColorCycle(mprMenu);
	}
	
	switch (*state) {
		case 0: // Error checking
			if (gIsDiamondPearl || gIsPlatinum) {
				if (SaveData_GetLoadCheckStatus(mprMenu->boxAppArgs.saveData) == LOAD_RESULT_CORRUPT) {
					PrintMPRString(mprMenu, MPR_TEXT_SaveCorrupt);
					
					mprMenu->frameCounter = 0;
					MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 1, state, 25);
					*HW_BG_A_PLTT_COLOR(0, 0) = GX_RGB(12, 12, 31);
				
				} else {
					G2_SetBG0Priority(0);
					LoadWarningGraphics(mprMenu);
					*state = 2;
				}
			} else {
				PrintMPRString(mprMenu, MPR_TEXT_DPPtOnly);
				
				mprMenu->frameCounter = 0;
				MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 8, state, 25);
				*HW_BG_A_PLTT_COLOR(0, 0) = GX_RGB(12, 12, 31);
			}
			break;
		
		case 1: // Showing message if the player has no save file
			MPRMenuWindow_DrawScrollArrow(&mprMenu->window, ScrollArrowIdx(mprMenu->frameCounter++));
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				
				PrintMPRString(mprMenu, MPR_TEXT_RestartAndSave);
				MPRComm_0203C204();
				
				mprMenu->frameCounter = 0;
				
				*state = 8;
			}
			break;
		
		case 2: // Showing warning menu
			MPRMenu_SetNetworkIconDefault();
			
			sScrollWarningBackground = TRUE;
			SetVBlankCallback(WarningBackgroundScrollFunc, (void*)mprMenu->bgConfig);
			
			PrintFullScreenStringWindow(mprMenu->bgConfig, &mprMenu->window, mprMenu->string, MPR_TEXT_WarningSplashHeader, 49, 11, 1, 10, 2, TRUE);
			PrintFullScreenStringWindow(mprMenu->bgConfig, &mprMenu->window, mprMenu->string, MPR_TEXT_WarningSplashBody, 69, 3, 5, 26, 18, FALSE);
			
			mprMenu->inputTimer = 0;
			mprMenu->frameCounter = 0;
			
			MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 3, state, 25);
			*HW_BG_A_PLTT_COLOR(0, 0) = GX_RGB(12, 12, 31);
			
			break;
		
		case 3: // Warning menu idle waiting for A press
			if (mprMenu->inputTimer % 60 == 0) {
				// Show text
				MPRText_CopyChars(mprMenu->string, MPR_TEXT_PressA);
				
				int stringWidth = Font_CalcStringWidth(FONT_SYSTEM, mprMenu->string, Font_GetAttribute(FONT_SYSTEM, FONTATTR_LETTER_SPACING));
				int windowWidth = Window_GetWidth(&mprMenu->window);
				int windowHeight = Window_GetHeight(&mprMenu->window);
				
				Text_AddPrinterWithParamsAndColor(
					&mprMenu->window,
					FONT_SYSTEM,
					mprMenu->string,
					(windowWidth * 8) - stringWidth,
					(windowHeight * 8) - 16,
					TEXT_SPEED_NO_TRANSFER,
					TEXT_COLOR(1, 2, 15),
					NULL);
				
				Window_CopyToVRAM(&mprMenu->window);
			
			} else if (mprMenu->inputTimer % 60 == 30) {
				// Hide text
				int windowHeight = Window_GetHeight(&mprMenu->window);
				int windowWidth = Window_GetWidth(&mprMenu->window);
				
				Window_FillRectWithColor(
					&mprMenu->window,
					TEXT_COLOR(0, 0, 15),
					0,
					(windowHeight * 8) - 16,
					(windowWidth * 8),
					16);
				
				Window_CopyToVRAM(&mprMenu->window);
			}
			
			mprMenu->inputTimer++;
			if (mprMenu->inputTimer >= 1800 && !MPRComm_IsConnected()) {
				MPRComm_Reset();
			}
			
			TryPrintConnectedToRanch(mprMenu);
			
			if (JOY_NEW(PAD_BUTTON_A)) {
				mprMenu->frameCounter = 60;
			}
			
			if ((mprMenu->frameCounter > 0 && mprMenu->inputTimer >= mprMenu->frameCounter) || MPRComm_GetErrorFlag()) {
				sScrollWarningBackground = FALSE;
			    MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_OUT, 4, state, 25);
			}
			
			break;
		
		case 4:
			G2_SetBG0Priority(2);
			LoadWindowGraphics(mprMenu);
			Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_1);
			
			if (MPRComm_GetErrorFlag()) {
				MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 7, state, 25);
			} else if (MPRComm_IsConnected()) {
				mprMenu->unk_84 = TRUE;
				mprMenu->unk_85 = FALSE;
				*state = 12;
			} else {
				MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 5, state, 25);
			}
			break;
		
		case 5:
			if (MPRComm_IsConnected()) {
				mprMenu->unk_84 = FALSE;
				mprMenu->unk_85 = FALSE;
				*state = 12;
			} else {
				Bg_ToggleLayer(BG_LAYER_MAIN_1, FALSE);
				PrintMPRString(mprMenu, MPR_TEXT_Connecting);
				mprMenu->frameCounter = 0;
				mprMenu->waitDial = WaitDial_Create(&mprMenu->window, 19);
				Bg_ToggleLayer(BG_LAYER_MAIN_1, TRUE);
				*state = 6;
			}
			break;
		
		case 6:
			mprMenu->inputTimer++;
			mprMenu->frameCounter++;
			
			bool connected = MPRComm_IsConnected();
			if (connected) {
				mprMenu->unk_84 = FALSE;
				mprMenu->unk_85 = TRUE;
				*state = 12;
			} else {
				bool shouldReset = (mprMenu->inputTimer >= 1800);
				
				// If we are not connected but have a ranch name for some reason, reset
				charcode_t* ranchName = MPRComm_GetRanchName();
				if (ranchName[0] != CHAR_EOS) {
					shouldReset = TRUE;
				}
				
				if (shouldReset) {
					MPRComm_Reset();
				}
				
				if (shouldReset && mprMenu->frameCounter >= 90) {
					WaitDial_Destroy(mprMenu->waitDial);
					PrintMPRString(mprMenu, MPR_TEXT_FailedToConnect);
					*state = 8;
				}
			}
			break;
		
		case 7: // Error disconnect
			PrintMPRString(mprMenu, MPR_TEXT_Disconnected);
			mprMenu->frameCounter = 0;
			*state = 8;
			break;
		
		case 8: // Error disconnect idle awaiting A press
			MPRMenuWindow_DrawScrollArrow(&mprMenu->window, ScrollArrowIdx(mprMenu->frameCounter++));
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				if (MPRComm_IsConnected()) {
					MPRComm_0203C204();
				}
				
				*state = 9;
			}
			break;
		
		case 9: // Awaiting disconnect before turning off power
			if (!MPRComm_IsConnected() || MPRComm_0203C520()) {
				MPRMenu_ClearNetworkIcon();
				PrintMPRString(mprMenu, MPR_TEXT_TurningOffPower);
				mprMenu->frameCounter = 0;
				MPRComm_Reset();
				*state = 10;
			}
			break;
		
		case 10: // Turning off power idle awaiting A press
			MPRMenuWindow_DrawScrollArrow(&mprMenu->window, ScrollArrowIdx(mprMenu->frameCounter++));
			
			if (JOY_NEW(PAD_BUTTON_A)) {
				Sound_PlayEffect(SEQ_SE_DP_DECIDE);
				MPRMenuWindow_DrawScrollArrow(&mprMenu->window, -1);
				*state = 24;
			}
			break;
		
		case 11:
			GF_ASSERT(mprMenu->activeSubApplication == SUB_APP_DEPOSIT || mprMenu->activeSubApplication == SUB_APP_WITHDRAW);
			
			if (mprMenu->activeSubApplication != SUB_APP_WITHDRAW) {
				Font_InitManager(FONT_SUBSCREEN, HEAP_ID_MAIN_MENU);
			}
			
			InitBgs(mprMenu);
			*HW_BG_A_PLTT_COLOR(0, 0) = GX_RGB(12, 12, 31);
			LoadWindowGraphics(mprMenu);
			
			mprMenu->unk_84 = TRUE;
			mprMenu->unk_85 = FALSE;
			
			*state = 12;
			break;
		
		case 12:
			if (WiiCommOk(mprMenu, state)) {
				SetVBlankCallback(WarningBackgroundScrollFunc, (void*)mprMenu->bgConfig);
				Bg_ToggleLayer(BG_LAYER_MAIN_0, FALSE);
				Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_0);
				RenderOptionWindows(mprMenu);
				RenderOptionsFrames(mprMenu, mprMenu->focusedOption);
				Bg_ToggleLayer(BG_LAYER_MAIN_0, TRUE);
				TryPrintConnectedToRanch(mprMenu);
				
				if (!mprMenu->unk_84) {
					*state = 13;
				} else {
					MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 13, state, 25);
				}
				
				*HW_BG_A_PLTT_COLOR(0, 0) = GX_RGB(12, 12, 31);
			}
			break;
		
		case 13:
			if (WiiCommOk(mprMenu, state)) {
				TryPrintConnectedToRanch(mprMenu);
				
				if (MPRComm_0203C218()) {
					PrintMPRString(mprMenu, MPR_TEXT_WaitingForWii);
					
					if (!mprMenu->unk_85) {
						mprMenu->waitDial = WaitDial_Create(&mprMenu->window, 19);
					}
					
					*state = 14;
				} else {
					*state = 16;
				}
			}
			break;
		
		case 14:
			if (WiiCommOk(mprMenu, state)) {
				TryPrintConnectedToRanch(mprMenu);
				
				bool synced = !MPRComm_0203C218();
				if (synced) {
					TryDestroyWaitDial(mprMenu);
					*state = 15;
				}
			}
			break;
		
		case 15:
			if (WiiCommOk(mprMenu, state)) {
				Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_1);
				*state = 16;
			}
			break;
		
		case 16: // Menu idle awaiting selection
			if (WiiCommOk(mprMenu, state)) {
				if (JOY_NEW(PAD_BUTTON_A)) {
					mprMenu->activeSubApplication = mprMenu->focusedOption;
					Sound_PlayEffect(SEQ_SE_DP_SELECT);
					MPRComm_ClientEnterSubMenu();
					mprMenu->frameCounter = 0;
					
					*state = 17;
				} else {
					if (JOY_NEW(PAD_KEY_UP)) {
						FocusNextOption(mprMenu, -1);
					}
					
					if (JOY_NEW(PAD_KEY_DOWN)) {
						FocusNextOption(mprMenu, 1);
					}
					
					RenderOptionsFrames(mprMenu, mprMenu->focusedOption);
				}
			}
			break;
		
		case 17: // Sync with Wii prior to launching sub application
			mprMenu->frameCounter++;
			if (mprMenu->frameCounter == 30) {
				PrintMPRString(mprMenu, MPR_TEXT_WaitingForWii);
				
				GF_ASSERT(mprMenu->waitDial == NULL);
				mprMenu->waitDial = WaitDial_Create(&mprMenu->window, 19);
			}
			
			if (MPRComm_ParentSyncSubMenu()) {
				if (mprMenu->activeSubApplication - 2 <= 1) {
					if (mprMenu->frameCounter >= 30) {
						Sound_PlayEffect(SEQ_SE_DP_SELECT);
					}
					
					TryDestroyWaitDial(mprMenu);
				}
				
				if (mprMenu->activeSubApplication == SUB_APP_DEPOSIT) {
					MPRComm_0203C1C8();
					*state = 18;
				} else if (mprMenu->activeSubApplication == SUB_APP_WITHDRAW) {
					MPRComm_0203C1DC();
					*state = 18;
				} else if (mprMenu->activeSubApplication == SUB_APP_SAVE) {
					MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_OUT, 21, state, 25);
				} else if (mprMenu->activeSubApplication == SUB_APP_QUIT) {
					InitSubApplication(mprMenu);
					*state = 23;
				} else {
					GF_ASSERT(FALSE);
				}
			}
			break;
		
		case 18: // Sync with Wii prior to opening PC box
			if (WiiCommOk(mprMenu, state)) {
				bool syncedWithWii = FALSE;
				
				if (mprMenu->activeSubApplication == SUB_APP_DEPOSIT) {
					syncedWithWii = MPRComm_ParentInDepositMode();
				}
				
				if (mprMenu->activeSubApplication == SUB_APP_WITHDRAW) {
					syncedWithWii = MPRComm_ParentInWithdrawMode();
				}
				
				if (syncedWithWii) {
					if (mprMenu->activeSubApplication == SUB_APP_WITHDRAW && MPRComm_IsConnected()) {
						MPRComm_0203C3A4(0, 0, MPRComm_GetBoxBuffer(0));
					}
					
					*state = 19;
				} else {
					mprMenu->frameCounter++;
					if (mprMenu->frameCounter == 30) {
						PrintMPRString(mprMenu, MPR_TEXT_WaitingForWii);
						GF_ASSERT(mprMenu->waitDial == NULL);
						mprMenu->waitDial = WaitDial_Create(&mprMenu->window, 19);
					}
				}
			}
			break;
		
		case 19: // Transition to PC box
			TryDestroyWaitDial(mprMenu);
			MainMenuUtil_SetFadeToWhite(FALSE);
			MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_OUT, 20, state, 25);
			break;
		
		case 20: // PC box init
			if (WiiCommOk(mprMenu, state)) {
				Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_1);
				
				GF_ASSERT(mprMenu->activeSubApplication == SUB_APP_DEPOSIT || mprMenu->activeSubApplication == SUB_APP_WITHDRAW);
				FreeApplicationResources(mprMenu);
				
				if (mprMenu->activeSubApplication != SUB_APP_WITHDRAW) {
					Font_Free(FONT_SUBSCREEN);
				}
				
				InitSubApplication(mprMenu);
				
				*state = 23;
			}
			break;
		
		case 21: // Transition to saving screen
			Bg_ClearTilemap(mprMenu->bgConfig, BG_LAYER_MAIN_0);
			PrintFullScreenStringWindow(mprMenu->bgConfig, &mprMenu->window, mprMenu->string, MPR_TEXT_WarningSplashHeader, 49, 11, 1, 10, 2, TRUE);
			PrintFullScreenStringWindow(mprMenu->bgConfig, &mprMenu->window, mprMenu->string, MPR_TEXT_WarningSaveBody, 69, 3, 5, 26, 12, FALSE);
			
			mprMenu->inputTimer = 0;
			
			MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_IN, 22, state, 25);
			break;
		
		case 22: // Saving screen init
			mprMenu->inputTimer++;
			if (mprMenu->inputTimer >= 60) {
				InitSubApplication(mprMenu);
				*state = 23;
			}
			break;
		
		case 23: // Tick sub application
			if (ApplicationManager_Exec(mprMenu->subApplication)) {
				ApplicationManager_Free(mprMenu->subApplication);
				mprMenu->subApplication = NULL;
				MPRComm_ClientLeaveSubMenu();
				
				if (mprMenu->activeSubApplication <= SUB_APP_WITHDRAW) {
					if (mprMenu->boxAppArgs.recordBoxUseInJournal) {
						mprMenu->dataChanged = TRUE;
					}
					*state = 11;
				
				} else if (mprMenu->activeSubApplication - 2 <= 1) {
					switch (mprMenu->saveAppArgs.returnValue) {
						case SAVEAPP_CANCELLED:
							*state = 16;
							break;
						
						case SAVEAPP_SAVED:
							mprMenu->dataChanged = FALSE;
							mprMenu->unk_84 = TRUE;
							MainMenuUtil_StartScreenFadeToState(FADE_TYPE_BRIGHTNESS_OUT, 12, state, 25);
							break;
						
						case SAVEAPP_COMM_ERROR:
							*state = 7;
							break;
						
						case SAVEAPP_POWER_OFF:
							*state = 24;
							break;
					}
				} else {
					GF_ASSERT(FALSE);
				}
			}
			break;
		
		case 24: // Power off after disconnect
			if (!MPRComm_IsConnected()) {
				PM_ForceToPowerOff();
			}
			break;
		
		case 25: // Screen fade transition wait
			MainMenuUtil_CheckScreenFadeDone(state);
			break;
		
		case 26: // End (not reachable)
			return TRUE;
			break;
	}
	
	MainMenuUtil_UpdateSpritesForAnimStatus();
	return FALSE;
}


static void InitSubApplication(MPRMenuApp* mprMenu) {
	static const ApplicationManagerTemplate sBoxAppTemplate = {
		.init      = BoxAppMan_Init,
		.main      = BoxAppMan_Main,
		.exit      = BoxAppMan_Exit,
		.overlayID = FS_OVERLAY_ID_NONE
	};
	
	static const ApplicationManagerTemplate sMPRSaveAppTemplate = {
		.init      = MPRSaveApp_Init,
		.main      = MPRSaveApp_Main,
		.exit      = MPRSaveApp_Exit,
		.overlayID = FS_OVERLAY_ID_NONE
	};
	
	switch (mprMenu->activeSubApplication) {
		case SUB_APP_DEPOSIT:
			mprMenu->boxAppArgs.boxMode = PC_MODE_RANCH_DEPOSIT;
			mprMenu->subApplication = ApplicationManager_New(&sBoxAppTemplate, &mprMenu->boxAppArgs, HEAP_ID_MAIN_MENU);
			break;
		
		case SUB_APP_WITHDRAW:
			mprMenu->boxAppArgs.boxMode = PC_MODE_RANCH_WITHDRAW;
			mprMenu->subApplication = ApplicationManager_New(&sBoxAppTemplate, &mprMenu->boxAppArgs, HEAP_ID_MAIN_MENU);
			break;
		
		case SUB_APP_SAVE:
			mprMenu->saveAppArgs.shutDownAfterSave = FALSE;
			mprMenu->saveAppArgs.dataChanged = mprMenu->dataChanged;
			mprMenu->subApplication = ApplicationManager_New(&sMPRSaveAppTemplate, &mprMenu->saveAppArgs, HEAP_ID_MAIN_MENU);
			break;
		
		case SUB_APP_QUIT:
			mprMenu->saveAppArgs.shutDownAfterSave = TRUE;
			mprMenu->saveAppArgs.dataChanged = mprMenu->dataChanged;
			mprMenu->subApplication = ApplicationManager_New(&sMPRSaveAppTemplate, &mprMenu->saveAppArgs, HEAP_ID_MAIN_MENU);
			break;
	}
}


static BOOL MPRMenuApp_Exit(ApplicationManager* appMan, int* state) {
	#pragma unused(state)
	
	MPRMenuApp* mprMenu = (MPRMenuApp*)ApplicationManager_GetData(appMan);
	
	String_Free(mprMenu->string);
	FreeApplicationResources(mprMenu);
	Font_Free(FONT_SUBSCREEN);
	ApplicationManager_FreeData(appMan);
	Heap_Destroy(HEAP_ID_MAIN_MENU);
	
	return TRUE;
}
