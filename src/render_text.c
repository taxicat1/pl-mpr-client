#include <nitro.h>
#include <string.h>

#include "render_text.h"
#include "constants/charcode.h"
#include "bg_window.h"
#include "charcode.h"
#include "font.h"
#include "sound_playback.h"
#include "system.h"
#include "text.h"
#include "assert.h"

#define SPEED_UP_ON_TOUCH_PRESS (gSystem.touchPressed && sRenderControlFlags.speedUpOnTouch)
#define SPEED_UP_ON_TOUCH_HOLD  (gSystem.touchHeld && sRenderControlFlags.speedUpOnTouch)

typedef struct {
	u8 canABSpeedUpPrint : 1;
	u8 : 1;
	u8 autoScroll : 1;
	u8 : 1;
	u8 speedUpOnTouch : 1;
	u8 speedUpAutoScroll : 1;
	u8 speedUpBattle : 1;
	u8 waitBattle : 1;
} RenderControlFlags;

static RenderControlFlags sRenderControlFlags;

typedef enum {
	RENDER_STATE_HANDLE_CHAR = 0,
	RENDER_STATE_WAIT,
	RENDER_STATE_CLEAR,
	RENDER_STATE_START_SCROLL,
	RENDER_STATE_SCROLL,
	RENDER_STATE_DUMMY,
	RENDER_STATE_PAUSE,
} RenderState;


RenderResult RenderText(TextPrinter* printer) {
	TextPrinterSubstruct* substruct = (TextPrinterSubstruct*)printer->substruct;
	charcode_t currChar;
	
	switch (printer->state) {
	case RENDER_STATE_HANDLE_CHAR:
		if ((JOY_HELD(PAD_BUTTON_A | PAD_BUTTON_B) && substruct->speedUp) || SPEED_UP_ON_TOUCH_HOLD) {
			printer->delayCounter = 0;
			
			if (printer->textSpeedBottom != 0) {
				sRenderControlFlags.speedUpBattle = TRUE;
			}
		}
		
		if (printer->delayCounter && printer->textSpeedBottom) {
			printer->delayCounter--;
			
			if (sRenderControlFlags.canABSpeedUpPrint
				&& (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B) || SPEED_UP_ON_TOUCH_PRESS)) {
				substruct->speedUp = TRUE;
				printer->delayCounter = 0;
			}
			
			return RENDER_UPDATE;
		}
		
		printer->delayCounter = printer->textSpeedBottom;
		currChar = *printer->printerTemplate.toPrint.raw;
		printer->printerTemplate.toPrint.raw++;
		
		switch (currChar) {
			case CHAR_EOS:
				return RENDER_FINISH;
			
			case CHAR_CR:
				printer->printerTemplate.currX = printer->printerTemplate.x;
				printer->printerTemplate.currY += Font_GetAttribute(printer->printerTemplate.fontID, 1) + printer->printerTemplate.lineSpacing;
				return RENDER_REPEAT;
			
			case CHAR_PLACEHOLDER_BEGIN:
				printer->printerTemplate.toPrint.raw++;
				return RENDER_REPEAT;
			
			case CHAR_FORMAT_ARG:
				printer->printerTemplate.toPrint.raw--;
				currChar = CharCode_FormatArgType(printer->printerTemplate.toPrint.raw);
				
				switch (currChar) {
					case CHAR_CONTROL_SET_COLOR: {
						u16 color = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						
						printer->printerTemplate.fgColor = color * 2 + 1;
						printer->printerTemplate.shadowColor = color * 2 + 2;
						
						Text_GenerateFontHalfRowLookupTable(printer->printerTemplate.fgColor, printer->printerTemplate.bgColor, printer->printerTemplate.shadowColor);
						break;
					}
					
					case CHAR_CONTROL_SCREEN_INDICATOR: {
						// 0 -> touch the bottom screen
						// 1 -> look at the bottom screen
						// 2 -> respond to the top screen
						// 3 -> look at the top screen
						u16 screen = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						Text_RenderScreenIndicator(printer, printer->printerTemplate.currX, printer->printerTemplate.currY, screen);
						
						if (printer->textSpeedTop != 0) {
							Window_CopyToVRAM(printer->printerTemplate.window);
						}
						break;
					}
					
					case CHAR_CONTROL_PAUSE: {
						printer->delayCounter = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						printer->printerTemplate.toPrint.raw = CharCode_SkipFormatArg(printer->printerTemplate.toPrint.raw);
						printer->state = RENDER_STATE_PAUSE;
						return RENDER_UPDATE;
						break;
					}
					
					case CHAR_CONTROL_CALLBACK:
						printer->callbackParam = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						printer->printerTemplate.toPrint.raw = CharCode_SkipFormatArg(printer->printerTemplate.toPrint.raw);
						return RENDER_UPDATE;
					
					case CHAR_CONTROL_CURSOR_X:
						printer->printerTemplate.currX = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						break;
					
					case CHAR_CONTROL_CURSOR_Y:
						printer->printerTemplate.currY = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						break;
					
					case CHAR_CONTROL_SET_SIZE: {
						u16 percentScale = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						
						switch (percentScale) {
							case 100:
								printer->printerTemplate.glyphTable = 0;
								printer->printerTemplate.dummy1A = 0;
								break;
							case 200:
								printer->printerTemplate.glyphTable = 0xFFFC;
								printer->printerTemplate.dummy1A = 0;
								break;
						}
						break;
					}
					
					case CHAR_CONTROL_MOVE: {
						u16 param = CharCode_FormatArgParam(printer->printerTemplate.toPrint.raw, 0);
						
						switch (param) {
							case 0xFE01: // wait to scroll
								printer->state = RENDER_STATE_CLEAR;
								TextPrinter_InitScrollArrowAnim(printer);
								printer->printerTemplate.toPrint.raw = CharCode_SkipFormatArg(printer->printerTemplate.toPrint.raw);
								return RENDER_UPDATE;
							case 0xFE00: // scroll
								printer->state = RENDER_STATE_START_SCROLL;
								TextPrinter_InitScrollArrowAnim(printer);
								printer->printerTemplate.toPrint.raw = CharCode_SkipFormatArg(printer->printerTemplate.toPrint.raw);
								return RENDER_UPDATE;
						}
						
						break;
					}
				}
				
				printer->printerTemplate.toPrint.raw = CharCode_SkipFormatArg(printer->printerTemplate.toPrint.raw);
				return RENDER_REPEAT;
			
			case CHAR_CONTROL_CLEAR:
				printer->state = RENDER_STATE_CLEAR;
				TextPrinter_InitScrollArrowAnim(printer);
				return RENDER_UPDATE;
			
			case CHAR_CONTROL_SCROLL:
				printer->state = RENDER_STATE_START_SCROLL;
				TextPrinter_InitScrollArrowAnim(printer);
				return RENDER_UPDATE;
		}
		
		const TextGlyph *glyph = Font_TryLoadGlyph(substruct->fontID, currChar);
		Window_CopyGlyph(printer->printerTemplate.window,
			glyph->gfx,
			glyph->width,
			glyph->height,
			printer->printerTemplate.currX,
			printer->printerTemplate.currY,
			printer->printerTemplate.glyphTable);
		
		printer->printerTemplate.currX += glyph->width + printer->printerTemplate.letterSpacing;
		return RENDER_PRINT;
	
	case RENDER_STATE_WAIT:
		if (TextPrinter_Wait(printer)) {
			TextPrinter_ClearScrollArrow(printer);
			printer->state = RENDER_STATE_HANDLE_CHAR;
		}
		
		return RENDER_UPDATE;
	
	case RENDER_STATE_CLEAR:
		if (TextPrinter_WaitWithScrollArrow(printer)) {
			TextPrinter_ClearScrollArrow(printer);
			Window_FillTilemap(printer->printerTemplate.window, printer->printerTemplate.bgColor);
			
			printer->printerTemplate.currX = printer->printerTemplate.x;
			printer->printerTemplate.currY = printer->printerTemplate.y;
			printer->state = RENDER_STATE_HANDLE_CHAR;
		}
		
		return RENDER_UPDATE;
	
	case RENDER_STATE_START_SCROLL:
		if (TextPrinter_WaitWithScrollArrow(printer)) {
			TextPrinter_ClearScrollArrow(printer);
			
			printer->scrollDistance = (Font_GetAttribute(printer->printerTemplate.fontID, 1) + printer->printerTemplate.lineSpacing);
			printer->printerTemplate.currX = printer->printerTemplate.x;
			printer->state = RENDER_STATE_SCROLL;
		}
		
		return RENDER_UPDATE;
	
	case RENDER_STATE_SCROLL:
		if (printer->scrollDistance) {
			// This cast here is ugly, but is necessary to match without declaring a separate variable just for 4.
			if ((int)printer->scrollDistance < 4) {
				Window_Scroll(printer->printerTemplate.window, 0, printer->scrollDistance, (printer->printerTemplate.bgColor << 4) | printer->printerTemplate.bgColor);
				printer->scrollDistance = 0;
			} else {
				Window_Scroll(printer->printerTemplate.window, 0, 4, (printer->printerTemplate.bgColor << 4) | printer->printerTemplate.bgColor);
				printer->scrollDistance -= 4;
			}
			
			Window_CopyToVRAM(printer->printerTemplate.window);
		} else {
			printer->state = RENDER_STATE_HANDLE_CHAR;
		}
		
		return RENDER_UPDATE;
	
	case RENDER_STATE_DUMMY:
		printer->state = RENDER_STATE_HANDLE_CHAR;
		return RENDER_UPDATE;
	
	case RENDER_STATE_PAUSE:
		if (printer->delayCounter) {
			printer->delayCounter--;
		} else {
			printer->state = RENDER_STATE_HANDLE_CHAR;
		}
		
		return RENDER_UPDATE;
	}
	
	return RENDER_FINISH;
}


static u16 sScrollArrowBaseTile = 0;

void TextPrinter_SetScrollArrowBaseTile(u16 tile) {
	sScrollArrowBaseTile = tile;
}


void TextPrinter_InitScrollArrowAnim(TextPrinter *printer) {
	TextPrinterSubstruct *substruct = (TextPrinterSubstruct *)printer->substruct;
	
	if (sRenderControlFlags.autoScroll) {
		substruct->autoScrollDelay = 0;
	} else {
		substruct->scrollArrowYPosIdx = 0;
		substruct->scrollArrowDelay = 0;
	}
}


static const u8 sScrollArrowTileOffsets[] = { 0, 1, 2, 1 };

void TextPrinter_DrawScrollArrow(TextPrinter *printer) {
	TextPrinterSubstruct *substruct = (TextPrinterSubstruct *)printer->substruct;
	
	if (sRenderControlFlags.autoScroll) {
		return;
	}
	
	if (substruct->scrollArrowDelay) {
		substruct->scrollArrowDelay--;
		return;
	}
	
	u8 bgID = Window_GetBgLayer(printer->printerTemplate.window);
	u8 x = Window_GetXPos(printer->printerTemplate.window);
	u8 y = Window_GetYPos(printer->printerTemplate.window);
	u8 width = Window_GetWidth(printer->printerTemplate.window);
	u16 baseTile = sScrollArrowBaseTile;
	
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 18 + (sScrollArrowTileOffsets[substruct->scrollArrowYPosIdx] * 4),
		x + width + 1,
		y + 2,
		1,
		1,
		16);
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 19 + (sScrollArrowTileOffsets[substruct->scrollArrowYPosIdx] * 4),
		x + width + 2,
		y + 2,
		1,
		1,
		16);
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 20 + (sScrollArrowTileOffsets[substruct->scrollArrowYPosIdx] * 4),
		x + width + 1,
		y + 3,
		1,
		1,
		16);
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 21 + (sScrollArrowTileOffsets[substruct->scrollArrowYPosIdx] * 4),
		x + width + 2,
		y + 3,
		1,
		1,
		16);
	
	Bg_CopyTilemapBufferToVRAM(printer->printerTemplate.window->bgConfig, bgID);
	substruct->scrollArrowDelay = 8;
	substruct->scrollArrowYPosIdx++;
}


void TextPrinter_ClearScrollArrow(TextPrinter *printer) {
	u8 bgID = Window_GetBgLayer(printer->printerTemplate.window);
	u8 x = Window_GetXPos(printer->printerTemplate.window);
	u8 y = Window_GetYPos(printer->printerTemplate.window);
	u8 width = Window_GetWidth(printer->printerTemplate.window);
	u16 baseTile = sScrollArrowBaseTile;
	
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 10,
		x + width + 1,
		y + 2,
		1,
		2,
		16);
	Bg_FillTilemapRect(printer->printerTemplate.window->bgConfig,
		bgID,
		baseTile + 11,
		x + width + 2,
		y + 2,
		1,
		2,
		16);
	Bg_CopyTilemapBufferToVRAM(printer->printerTemplate.window->bgConfig, bgID);
}


static BOOL TextPrinter_Continue(TextPrinter *printer) {
	if (JOY_NEW(PAD_BUTTON_A | PAD_BUTTON_B)
		|| (gSystem.touchPressed && sRenderControlFlags.speedUpOnTouch)) {
		Sound_PlayEffect(SEQ_SE_DP_SELECT);
		sRenderControlFlags.waitBattle = TRUE;
		return TRUE;
	}
	
	return FALSE;
}


BOOL TextPrinter_WaitAutoMode(TextPrinter *printer) {
	TextPrinterSubstruct *substruct = (TextPrinterSubstruct*)printer->substruct;
	if (substruct->autoScrollDelay == 100) {
		return TRUE;
	}
	
	substruct->autoScrollDelay++;
	if (sRenderControlFlags.speedUpAutoScroll) {
		return TextPrinter_Continue(printer);
	}
	
	return FALSE;
}


BOOL TextPrinter_WaitWithScrollArrow(TextPrinter *printer) {
	if (sRenderControlFlags.autoScroll) {
		return TextPrinter_WaitAutoMode(printer);
	}
	
	TextPrinter_DrawScrollArrow(printer);
	return TextPrinter_Continue(printer);
}


BOOL TextPrinter_Wait(TextPrinter *printer) {
	// Ugly casts here are necessary to match. GF moment.
	if (sRenderControlFlags.autoScroll) {
		return (u8)TextPrinter_WaitAutoMode(printer);
	}
	
	return (u8)TextPrinter_Continue(printer);
}
