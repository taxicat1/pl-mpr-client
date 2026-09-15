#include <nitro.h>
#include <string.h>

#include "font_special_chars.h"

#include "game_version.h"
#include "bg_window.h"
#include "graphics.h"
#include "heap.h"
#include "constants/charcode.h"
#include "fs/graphic/font.naix"

#define BG_COLOR_F	    (0)
#define FG_COLOR_F	    (1 << 0)
#define SHADOW_COLOR_F  (1 << 1)

static const struct {
	u16  offset;
	u16  width;
} sNonNumericWidths[] = {
	[SPECIAL_CHAR_SLASH]                = { .offset = 10 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(1) },
	[SPECIAL_CHAR_LEVEL]                = { .offset = 11 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) },
	[SPECIAL_CHAR_NUMBER]               = { .offset = 13 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) },
	[SPECIAL_CHAR_ID]                   = { .offset = 15 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) },
	[SPECIAL_CHAR_LEVEL_FEMALE_UNUSED]  = { .offset = 17 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) },
	[SPECIAL_CHAR_LEVEL_MALE_UNUSED]    = { .offset = 19 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) },
	[SPECIAL_CHAR_LEVEL_UNUSED]         = { .offset = 21 * TILE_SIZE_4BPP, .width = TILES_TO_PIXELS(2) }
};

FontSpecialCharsContext* FontSpecialChars_Init(u32 fgColor, u32 shadowColor, u32 bgColor, HeapID heapID) {
	FontSpecialCharsContext* context = Heap_Alloc(heapID, sizeof(FontSpecialCharsContext));
	
	if (context) {
		u32 idx;
		u8* rawData;
		
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_GRAPHIC__FONT;
		} else {
			narc = NARC_INDEX_PL_GRAPHIC__PL_FONT;
		}
		context->ncgrBuffer = Graphics_GetCharData(narc, font_special_chars_NCGR_lz, TRUE, &context->charData, heapID);
		rawData = context->charData->pRawData;
		
		for (idx = 0; idx < context->charData->szByte; idx++) {
			// The initial value of each rawData byte is two 4bpp pixels.
			// Each nibble represents one of three color options:
			// 0b0000 (BG_COLOR_F): bgColor
			// 0b0001 (FG_COLOR_F): fgColor
			// 0b0010 (SHADOW_COLOR_F): shadowColor
			// The corresponding color parameters replace the bit fields.
			switch (rawData[idx]) {
				case (BG_COLOR_F << 4) | (BG_COLOR_F):
					rawData[idx] = (bgColor << 4) | (bgColor);
					break;
				
				case (BG_COLOR_F << 4) | (FG_COLOR_F):
					rawData[idx] = (bgColor << 4) | (fgColor);
					break;
				
				case (BG_COLOR_F << 4) | (SHADOW_COLOR_F):
					rawData[idx] = (bgColor << 4) | (shadowColor);
					break;
				
				case (FG_COLOR_F << 4) | (BG_COLOR_F):
					rawData[idx] = (fgColor << 4) | (bgColor);
					break;
				
				case (FG_COLOR_F << 4) | (FG_COLOR_F):
					rawData[idx] = (fgColor << 4) | (fgColor);
					break;
				
				case (FG_COLOR_F << 4) | (SHADOW_COLOR_F):
					rawData[idx] = (fgColor << 4) | (shadowColor);
					break;
				
				case (SHADOW_COLOR_F << 4) | (BG_COLOR_F):
					rawData[idx] = (shadowColor << 4) | (bgColor);
					break;
				
				case (SHADOW_COLOR_F << 4) | (FG_COLOR_F):
					rawData[idx] = (shadowColor << 4) | (fgColor);
					break;
				
				case (SHADOW_COLOR_F << 4) | (SHADOW_COLOR_F):
					rawData[idx] = (shadowColor << 4) | (shadowColor);
					break;
			}
		}
		
		context->bgColor = bgColor;
	}
	
	return context;
}


void FontSpecialChars_Free(FontSpecialCharsContext* context) {
	if (context) {
		if (context->ncgrBuffer) {
			Heap_Free(context->ncgrBuffer);
		}
		
		Heap_Free(context);
	}
}


void FontSpecialChars_DrawPartyScreenLevelText(FontSpecialCharsContext* context, NonNumericSpecialChar tableIdx, Window* window, u32 x, u32 y) {
	Window_BlitBitmapRect(
		window,
		(u8*)(context->charData->pRawData) + sNonNumericWidths[tableIdx].offset,
		0,
		0,
		sNonNumericWidths[tableIdx].width,
		TILE_HEIGHT_PIXELS,
		x,
		y,
		sNonNumericWidths[tableIdx].width,
		TILE_HEIGHT_PIXELS);
}


void FontSpecialChars_DrawPartyScreenHPText(FontSpecialCharsContext* context, s32 displayValue, u32 digits, PaddingMode paddingMode, Window* window, u32 x, u32 y) {
	int idx;
	CharCode_FromInt(context->charcodes, displayValue, paddingMode, digits);
	
	for (idx = 0; context->charcodes[idx] != CHAR_EOS; idx++) {
		if ((context->charcodes[idx] >= CHAR_WIDE_0) && (context->charcodes[idx] <= CHAR_WIDE_9)) {
			Window_BlitBitmapRect(
				window,
				(u8*)(context->charData->pRawData) + ((context->charcodes[idx] - CHAR_WIDE_0) * TILE_SIZE_4BPP),
				0,
				0,
				TILE_WIDTH_PIXELS,
				TILE_HEIGHT_PIXELS,
				x,
				y,
				TILE_WIDTH_PIXELS,
				TILE_HEIGHT_PIXELS);
		} else {
			Window_FillRectWithColor(window, context->bgColor, x, y, TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS);
		}
		
		x += TILE_WIDTH_PIXELS;
	}
}


void FontSpecialChars_DrawPartyScreenText(
	FontSpecialCharsContext* context,
	NonNumericSpecialChar tableIdx,
	s32 displayValue,
	u32 digits,
	PaddingMode paddingMode,
	Window* window,
	u32 x,
	u32 y
) {
	FontSpecialChars_DrawPartyScreenLevelText(context, tableIdx, window, x, y);
	FontSpecialChars_DrawPartyScreenHPText(context, displayValue, digits, paddingMode, window, x + 16, y);
}
