#ifndef MPR_RENDER_TEXT_H
#define MPR_RENDER_TEXT_H

#include "bg_window.h"
#include "charcode.h"
#include "string_gf.h"

#define COLOR_CACHE_OFFSET                 100
#define COLOR_CACHE_RANGE                  7
#define COLOR_CACHE_MAX                    (COLOR_CACHE_OFFSET + COLOR_CACHE_RANGE)
#define COLOR_CACHE_IS_VALID(param)        (param >= COLOR_CACHE_OFFSET && param < COLOR_CACHE_MAX)
#define CHAR_CONTROL_SET_COLOR_FROM_CACHE  0xFF

typedef struct TextGlyph {
	u8  gfx[128];
	u8  width;
	u8  height;
} TextGlyph;

typedef union {
	const charcode_t*  raw;
	const String*      string;
} StringToPrint;

typedef struct {
	StringToPrint  toPrint;
	Window*        window;
	u8             dummy08;
	u8             fontID;
	u8             x;
	u8             y;
	u8             currX;
	u8             currY;
	u8             letterSpacing;
	u8             lineSpacing;
	u8             dummy14;
	u8             fgColor;
	u8             bgColor;
	u8             shadowColor;
	u16            glyphTable;
	u8             dummy1A;
	u8             dummy1B;
} TextPrinterTemplate;

typedef BOOL (*TextPrinterCallback)(TextPrinterTemplate*, u16);

typedef struct {
	u8  fontID              : 4;
	u8  speedUp             : 1;
	u8  dummy               : 3;
	u8  scrollArrowDelay    : 5;
	u8  scrollArrowYPosIdx  : 2;
	u8  fontIDSet           : 1;
	u8  autoScrollDelay     : 8;
} TextPrinterSubstruct;

typedef struct {
	TextPrinterTemplate  printerTemplate;
	TextPrinterCallback  callback;
	
	u8     substruct[7];
	u8     active;
	u8     state;
	u8     textSpeedBottom : 7;
	u8     textSpeedTop : 1;
	u8     delayCounter;
	u8     scrollDistance;
	u8     id;
	u8     callbackResult;
	u16    callbackParam;
	void*  iconGfx;
} TextPrinter;

typedef enum {
	RENDER_PRINT,
	RENDER_FINISH,
	RENDER_REPEAT, // Run render function again, if e.g. a control code is encountered.
	RENDER_UPDATE
} RenderResult;

// The NO_WAIT flag is dependent on ENABLED, so it does not have a unique constant for bitflag 2.
#define AUTO_SCROLL_DISABLED  0
#define AUTO_SCROLL_ENABLED   (1 << 0)
#define AUTO_SCROLL_NO_WAIT   ((1 << 1) | AUTO_SCROLL_ENABLED)

RenderResult RenderText(TextPrinter* printer);
void TextPrinter_SetScrollArrowBaseTile(u16 tile);
void TextPrinter_InitScrollArrowAnim(TextPrinter* printer);
void TextPrinter_DrawScrollArrow(TextPrinter* printer);
void TextPrinter_ClearScrollArrow(TextPrinter* printer);
BOOL TextPrinter_WaitAutoMode(TextPrinter* printer);
BOOL TextPrinter_WaitWithScrollArrow(TextPrinter* printer);
BOOL TextPrinter_Wait(TextPrinter* printer);

#endif /* MPR_RENDER_TEXT_H */
