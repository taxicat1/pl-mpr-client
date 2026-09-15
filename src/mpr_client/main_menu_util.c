#include <nitro.h>
#include <string.h>

#include "mpr_client/main_menu_util.h"

#include "game_version.h"
#include "message.h"
#include "message_util.h"
#include "pokemon_sprite.h"
#include "screen_fade.h"
#include "sprite_system.h"
#include "string_gf.h"
#include "render_window.h"
#include "constants/graphics.h"

typedef struct {
	SpriteList*                spriteList;
	G2dRenderer                renderer;
	SpriteResourceCollection*  resourceCollections[SPRITE_RESOURCE_MAX];
	SpriteResource*            resources[DS_SCREEN_MAX][SPRITE_RESOURCE_MAX];
	SpriteResourcesHeader      resourceHeaders[DS_SCREEN_MAX];
	
	int unused_220[6];
} MainMenuSpriteManager;

typedef struct {
	BgConfig*                  bgConfig;
	HeapID                     heapID;
	int                        screenFadeQueuedState;
	BOOL                       fadeToWhite;
	int                        unused_14;
	SaveData*                  saveData;
	void*                      unused_1C[6];
	MainMenuSpriteManager      spriteManager;
	Sprite*                    mysteryGiftSprite;
	int                        giftSpriteDelay;
	u8                         unused_40[0xC98]; // Remnants of sprite buffer, sprite template, calback function
} MainMenuUtilManager;

static int MainMenuWindow_PrintText(MainMenuWindow* window, int textEntryID);

static MainMenuUtilManager sMainMenuUtilManager;


void MainMenuUtil_InitBGLayer(BgConfig* bgConfig, BgLayer bgLayer, u8 screenSize, u32 screenBase, u32 charBase) {
	BgTemplate bgTemplate = {
		.x          = 0,
		.y          = 0,
		.bufferSize = 0x800,
		.baseTile   = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode  = GX_BG_COLORMODE_16,
		.screenBase = GX_BG_SCRBASE_0x0000,
		.charBase   = GX_BG_CHARBASE_0x00000,
		.bgExtPltt  = GX_BG_EXTPLTT_01,
		.priority   = 0,
		.areaOver   = 0,
		.mosaic     = FALSE
	};
	
	bgTemplate.screenSize = screenSize;
	
	switch (screenSize) {
		case BG_SCREEN_SIZE_256x256:
			bgTemplate.bufferSize = 0x800;
			break;
		
		case BG_SCREEN_SIZE_256x512:
			bgTemplate.bufferSize = 0x1000;
			break;
		
		case BG_SCREEN_SIZE_512x256:
			bgTemplate.bufferSize = 0x1000;
			break;
		
		case BG_SCREEN_SIZE_512x512:
			bgTemplate.bufferSize = 0x2000;
			break;
	}
	
	bgTemplate.screenBase = screenBase / 0x800;
	bgTemplate.charBase = charBase / 0x4000;
	
	Bg_InitFromTemplate(bgConfig, bgLayer, &bgTemplate, BG_TYPE_STATIC);
	Bg_ClearTilemap(bgConfig, bgLayer);
}


void MainMenuUtil_SetFadeToWhite(BOOL enable) {
	MainMenuUtilManager* utilMan = &sMainMenuUtilManager;
	utilMan->fadeToWhite = enable;
}


void MainMenuUtil_StartScreenFadeToState(FadeType fadeType, int destState, int* state, int waitState) {
	MainMenuUtilManager* utilMan = &sMainMenuUtilManager;
	
	if (utilMan->fadeToWhite == FALSE) {
		StartScreenFade(FADE_BOTH_SCREENS, fadeType, fadeType, COLOR_BLACK, 6, 1, utilMan->heapID);
	} else {
		StartScreenFade(FADE_BOTH_SCREENS, fadeType, fadeType, COLOR_WHITE, 6, 1, utilMan->heapID);
	}
	
	if (state) {
		*state = waitState;
	}
	
	utilMan->screenFadeQueuedState = destState;
}


void MainMenuUtil_CheckScreenFadeDone(int* state) {
	MainMenuUtilManager* utilMan = &sMainMenuUtilManager;
	
	if (IsScreenFadeDone()) {
		*state = utilMan->screenFadeQueuedState;
	}
}


void MainMenuUtil_InitWindow(MainMenuWindow* mainMenuWindow, Window* window, int textPalette, int textBank, int baseTile, int framePalette) {
	memset(mainMenuWindow, 0, sizeof(MainMenuWindow));
	
	mainMenuWindow->drawFrame           = TRUE;
	mainMenuWindow->clearBeforePrinting = TRUE;
	mainMenuWindow->textEntryID         = -1;
	mainMenuWindow->window              = window;
	mainMenuWindow->textPltt            = textPalette;
	mainMenuWindow->textBank            = textBank;
	mainMenuWindow->frameBaseTile       = baseTile;
	mainMenuWindow->framePltt           = framePalette;
	mainMenuWindow->bgLayer             = BG_LAYER_MAIN_0;
	mainMenuWindow->font                = FONT_SYSTEM;
	mainMenuWindow->textColor           = TEXT_COLOR(1, 2, 15);
	mainMenuWindow->backgroundColorIdx  = 15;
	mainMenuWindow->scrollable          = FALSE;
	mainMenuWindow->textXOffset         = mainMenuWindow->textYOffset = 0;
	mainMenuWindow->strTemplate         = NULL;
	mainMenuWindow->renderDelay         = 0xFF;
}


void MainMenuWindow_SetDimensionsAndBasetile(MainMenuWindow* window, int width, int height, int baseTile) {
	window->width    = width;
	window->height   = height;
	window->baseTile = baseTile;
}


static int MainMenuWindow_PrintText(MainMenuWindow* window, int textEntryID) {
	MainMenuUtilManager* utilMan = &sMainMenuUtilManager;
	
	int printerID;
	if (textEntryID != -1 && window->textEntryID != textEntryID) {
		window->textEntryID = textEntryID;
		
		if (window->clearBeforePrinting == TRUE) {
			Window_FillTilemap(window->window, window->backgroundColorIdx);
		}
		
		if (window->textEntryID != -1) {
			StringTemplate* strTemplate;
			NarcID narc;
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_MSGDATA__MSG;
			} else {
				narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
			}
			MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, window->textBank, utilMan->heapID);
			
			if (window->strTemplate == NULL) {
				strTemplate = StringTemplate_Default(utilMan->heapID);
			} else {
				strTemplate = window->strTemplate;
			}
			
			String* string = MessageUtil_ExpandedString(strTemplate, msgLoader, window->textEntryID, utilMan->heapID);
			
			if (window->textRightAligned == FALSE) {
				printerID = Text_AddPrinterWithParamsAndColor(
					window->window,
					window->font,
					string,
					window->textXOffset,
					window->textYOffset,
					window->renderDelay,
					window->textColor,
					NULL);
			} else {
				int textWidth = Font_CalcStringWidth(window->font, string, Font_GetAttribute(window->font, FONTATTR_LETTER_SPACING));
				int windowWidth = Window_GetWidth(window->window) * TILE_WIDTH_PIXELS - textWidth;
				
				printerID = Text_AddPrinterWithParamsAndColor(
					window->window, 
					window->font,
					string, windowWidth,
					window->textYOffset,
					window->renderDelay,
					window->textColor,
					NULL);
				
				window->textRightAligned = FALSE;
			}
			
			String_Free(string);
			
			if (window->strTemplate == NULL) {
				StringTemplate_Free(strTemplate);
			}
			
			MessageLoader_Free(msgLoader);
		}
	}
	
	window->renderDelay = TEXT_SPEED_NO_TRANSFER;
	return printerID;
}


int MainMenuUtil_ShowWindowAtPos(BgConfig* bgConfig, MainMenuWindow* window, int xPos, int yPos, int textEntryID) {
	int printerID;
	if (window->window->bgConfig == NULL) {
		Window_Add(bgConfig, window->window, window->bgLayer, xPos, yPos, window->width, window->height, window->textPltt, window->baseTile);
		printerID = MainMenuWindow_PrintText(window, textEntryID);
	} else {
		if (xPos != -1) {
			Window_SetXPos(window->window, xPos);
		}
		
		if (yPos != -1) {
			Window_SetYPos(window->window, yPos);
		}
		
		printerID = MainMenuWindow_PrintText(window, textEntryID);
	}
	
	if (window->drawFrame == TRUE) {
		if (window->scrollable == FALSE) {
			Window_DrawStandardFrame(window->window, FALSE, window->frameBaseTile, window->framePltt);
		} else {
			Window_DrawMessageBoxWithScrollCursor(window->window, FALSE, window->frameBaseTile, window->framePltt);
		}
	}
	
	return printerID;
}


void MainMenuUtil_UpdateSpritesForAnimStatus(void) {
	MainMenuUtilManager* utilMan = &sMainMenuUtilManager;
	
	if (utilMan->mysteryGiftSprite) {
		if (utilMan->giftSpriteDelay == 0) {
			if (Sprite_GetDrawFlag(utilMan->mysteryGiftSprite) == FALSE) {
				Sprite_SetDrawFlag(utilMan->mysteryGiftSprite, TRUE);
			}
			
			VecFx32* spritePos = (VecFx32*)Sprite_GetPosition(utilMan->mysteryGiftSprite);
			if (spritePos->y < 384 * FX32_ONE) {
				spritePos->y += FX32_ONE * 3;
			}
		} else {
			utilMan->giftSpriteDelay--;
		}
	}
	
	if (utilMan->spriteManager.spriteList != NULL) {
		SpriteList_Update(utilMan->spriteManager.spriteList);
	}
}


void Dummy_Unused_02062F54(void);
void Dummy_Unused_02062F54(void) {
	// Uncertain what this is. Unreferenced.
	static const struct {
		u8 data[16];
	} Unk_Unused_02062F54 = {
		{
			0x14, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		}
	};
}

void Dummy_Unused_02062F64(void);
void Dummy_Unused_02062F64(void) {
	// Remnants of a table lookup for Mystery Gift data. Unused.
	static const struct {
		u8 data[26];
	} sGiftTypeToPlttOffset_Unused_02062F64 = {
		{
			0x00, 0x00, 0x01, 0x01, 0x02, 0x01, 0x03, 0x02, 0x04, 0x03, 0x05, 0x02, 0x06, 0x02, 0x07, 0x00,
			0x08, 0x05, 0x09, 0x05, 0x0A, 0x05, 0x0B, 0x04, 0xFF, 0x00
		}
	};
}
