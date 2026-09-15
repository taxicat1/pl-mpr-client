#ifndef MPR_FONT_MANAGER_H
#define MPR_FONT_MANAGER_H

#include "charcode.h"
#include "narc.h"
#include "render_text.h"

typedef enum {
	GLYPH_SHAPE_8x8 = 0,
	GLYPH_SHAPE_8x16,
	GLYPH_SHAPE_16x8,
	GLYPH_SHAPE_16x16,
	
	GLYPH_SHAPE_MAX,
} GlyphShape;

typedef enum {
    GLYPH_ACCESS_MODE_IMMEDIATE = 0,
    GLYPH_ACCESS_MODE_LAZY,
} GlyphAccessMode;

typedef struct FontManager FontManager;

typedef void (*GlyphBitmapFunc)(const FontManager* fontManager, charcode_t c, TextGlyph* outGlyph);
typedef u8 (*GlyphWidthFunc)(const FontManager* fontManager, u32 glyph);

typedef struct {
	u32  size;
	u32  widthTableOffset;
	u32  numGlyphs;
	u8   maxWidth;
	u8   maxHeight;
	u8   glyphWidth;
	u8   glyphHeight;
} FontHeader;

struct FontManager {
    int              glyphAccessMode;
    GlyphBitmapFunc  glyphBitmapFunc;
    GlyphShape       glyphShape;
    u32              glyphSize;
    u8*              narcBuf;
    u8               glyphBuf[64];
    NARC*            narc;
    u32              arcFileIdx;
    FontHeader       header;
    BOOL             isMonospace;
    GlyphWidthFunc   glyphWidthFunc;
    u8*              glyphWidths;
};

FontManager* FontManager_New(NarcID narcID, u32 arcFileIdx, GlyphAccessMode glyphAccessMode, BOOL isMonospace, HeapID heapID);
void FontManager_Delete(FontManager* fontManager);
void FontManager_SwitchGlyphAccessMode(FontManager* fontManager, GlyphAccessMode glyphAccessMode, u32 heapID);
void FontManager_TryLoadGlyph(const FontManager* fontManager, charcode_t c, TextGlyph* outGlyph);
u32 FontManager_CalcStringWidth(const FontManager* fontManager, const charcode_t* str, u32 letterSpacing);

#endif /* MPR_FONT_MANAGER_H */
