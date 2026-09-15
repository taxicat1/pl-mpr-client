#ifndef MPR_FONT_SPECIAL_CHARS_H
#define MPR_FONT_SPECIAL_CHARS_H

#include <nnsys/g2d.h>

#include "bg_window.h"
#include "charcode_util.h"

typedef enum {
	SPECIAL_CHAR_SLASH = 0,
	SPECIAL_CHAR_LEVEL,
	SPECIAL_CHAR_NUMBER,
	SPECIAL_CHAR_ID,
	SPECIAL_CHAR_LEVEL_FEMALE_UNUSED,
	SPECIAL_CHAR_LEVEL_MALE_UNUSED,
	SPECIAL_CHAR_LEVEL_UNUSED,
} NonNumericSpecialChar;

typedef struct {
	void*                 ncgrBuffer;
	NNSG2dCharacterData*  charData;
	charcode_t            charcodes[16];
	u32                   bgColor;
} FontSpecialCharsContext;

FontSpecialCharsContext* FontSpecialChars_Init(u32 fgColor, u32 shadowColor, u32 bgColor, HeapID heapID);
void FontSpecialChars_Free(FontSpecialCharsContext* context);
void FontSpecialChars_DrawPartyScreenLevelText(FontSpecialCharsContext* context, NonNumericSpecialChar tableIdx, Window* window, u32 x, u32 y);
void FontSpecialChars_DrawPartyScreenHPText(FontSpecialCharsContext* context, s32 displayValue, u32 digits, PaddingMode paddingMode, Window* window, u32 x, u32 y);
void FontSpecialChars_DrawPartyScreenText(
	FontSpecialCharsContext* context,
	NonNumericSpecialChar tableIdx,
	s32 displayValue,
	u32 digits,
	PaddingMode paddingMode,
	Window* window,
	u32 x,
	u32 y);

#endif /* MPR_FONT_SPECIAL_CHARS_H */
