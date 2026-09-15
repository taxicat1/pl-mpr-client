#ifndef MPR_FONT_H
#define MPR_FONT_H

#include "charcode.h"
#include "graphics.h"
#include "render_text.h"
#include "string_gf.h"

#define MAX_LETTER_HEIGHT       16
#define MAX_LETTER_HEIGHT_TILES ((MAX_LETTER_HEIGHT + 7) / 8)

#define TEXT_LINES(n)                             ((n) * MAX_LETTER_HEIGHT)
#define TEXT_LINES_TILES(n)                       ((n) * MAX_LETTER_HEIGHT_TILES)
#define TEXT_LINES_WITH_SPACING(n, spacing)       ((n) * MAX_LETTER_HEIGHT + ((n) - 1) * spacing)
#define TEXT_LINES_WITH_SPACING_TILES(n, spacing) ((n) * MAX_LETTER_HEIGHT_TILES + ((n) - 1) * spacing)

typedef enum {
	FONT_SYSTEM = 0,
	FONT_MESSAGE,
	FONT_SUBSCREEN,
	FONT_UNOWN,
	
	FONT_MAX
} Font;

typedef enum {
	FONTATTR_MAX_LETTER_WIDTH = 0,
	FONTATTR_MAX_LETTER_HEIGHT,
	FONTATTR_LETTER_SPACING,
	FONTATTR_LINE_SPACING,
	FONTATTR_DUMMY,
	FONTATTR_FG_COLOR,
	FONTATTR_BG_COLOR,
	FONTATTR_SHADOW_COLOR
} FontAttribute;

typedef struct {
	u8  maxLetterWidth;
	u8  maxLetterHeight;
	u8  letterSpacing;
	u8  lineSpacing;
	u8  dummy;
	u8  fgColor;
	u8  bgColor;
	u8  shadowColor;
} FontAttributes;

void Fonts_Init(void);
void Font_InitManager(Font font, HeapID heapID);
void Font_UseImmediateGlyphAccess(Font font, HeapID heapID);
void Font_UseLazyGlyphAccess(Font font);
void Font_Free(Font font);
const TextGlyph* Font_TryLoadGlyph(Font font, charcode_t c);
RenderResult Font_RenderText(Font font, TextPrinter* printer);
u32 Font_CalcCharArrayWidth(Font font, const charcode_t* str, u32 letterSpacing);
u32 Font_CalcStringWidth(Font font, const String* string, u32 letterSpacing);
u8 Font_GetAttribute(u8 font, u8 attribute);
void Font_LoadTextPalette(PaletteLoadLocation palLocation, u32 palSlotOffset, HeapID heapID);
void Font_LoadScreenIndicatorsPalette(PaletteLoadLocation palLocation, u32 palSlotOffset, HeapID heapID);

#endif /* MPR_FONT_H */
