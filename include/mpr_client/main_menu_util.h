#ifndef MPR_MPR_CLIENT_MENU_UTIL_H
#define MPR_MPR_CLIENT_MENU_UTIL_H

#include "bg_window.h"
#include "font.h"
#include "mystery_gift.h"
#include "overlay_manager.h"
#include "palette.h"
#include "savedata.h"
#include "screen_fade.h"
#include "sprite.h"
#include "sprite_resource.h"
#include "sprite_util.h"
#include "string_template.h"
#include "text.h"

typedef struct {
	BOOL             scrollable;
	BOOL             drawFrame;
	BOOL             clearBeforePrinting;
	BOOL             textRightAligned;
	Window*          window;
	StringTemplate*  strTemplate;
	int              width;
	int              height;
	int              textXOffset;
	int              textYOffset;
	int              baseTile;
	BgLayer          bgLayer;
	int              textPltt;
	int              textBank;
	int              frameBaseTile;
	int              framePltt;
	Font             font;
	TextColor        textColor;
	u8               backgroundColorIdx;
	int              textEntryID;
	int              renderDelay;
} MainMenuWindow;

void MainMenuUtil_InitBGLayer(BgConfig* bgConfig, BgLayer bgLayer, u8 screenSize, u32 screenBase, u32 charBase);
void MainMenuUtil_SetFadeToWhite(BOOL enable);
void MainMenuUtil_StartScreenFadeToState(FadeType fadeType, int destState, int* state, int waitState);
void MainMenuUtil_CheckScreenFadeDone(int* state);
void MainMenuUtil_InitWindow(MainMenuWindow* mainMenuWindow, Window* window, int textPalette, int textBank, int baseTile, int framePalette);
void MainMenuWindow_SetDimensionsAndBasetile(MainMenuWindow* window, int width, int height, int baseTile);
int MainMenuUtil_ShowWindowAtPos(BgConfig* bgConfig, MainMenuWindow* window, int xPos, int yPos, int textEntryID);
void MainMenuUtil_UpdateSpritesForAnimStatus(void);

#endif /* MPR_MPR_CLIENT_MENU_UTIL_H */
