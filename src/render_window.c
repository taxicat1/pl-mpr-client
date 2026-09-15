#include <nitro.h>
#include <string.h>

#include "render_window.h"

#include "bg_window.h"
#include "sys_task.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "render_text.h"

#include "constants/graphics.h"
#include "fs/graphic/winframe.naix"

#define WAIT_DIAL_FRAME_WIDTH_TILES  2
#define WAIT_DIAL_FRAME_HEIGHT_TILES 2
#define WAIT_DIAL_TILES_PER_FRAME    (WAIT_DIAL_FRAME_WIDTH_TILES * WAIT_DIAL_FRAME_HEIGHT_TILES)
#define WAIT_DIAL_FRAME_OFFSET(i)    (WAIT_DIAL_FRAME_SIZE * i)
#define WAIT_DIAL_FRAME_COUNT        8
#define WAIT_DIAL_FRAME_SIZE         (TILE_SIZE_4BPP * WAIT_DIAL_TILES_PER_FRAME)
#define WAIT_DIAL_WHOLE_SIZE         (WAIT_DIAL_FRAME_SIZE * WAIT_DIAL_FRAME_COUNT)

typedef enum {
	DIAL_DRAW_MODE_LOAD_ONLY = 0,
	DIAL_DRAW_MODE_LOAD_AND_DRAW,
	DIAL_DRAW_MODE_CLEAR,
} WaitDialDrawMode;

typedef enum {
	DIAL_DELETE_MODE_NONE = 0,
	DIAL_DELETE_MODE_CLEAR,
	DIAL_DELETE_MODE_DESTROY,
} WaitDialDeleteMode;

typedef struct {
    Window*  window;
    u8       pixels[WAIT_DIAL_WHOLE_SIZE];
    u8       messageBoxPixels[WAIT_DIAL_FRAME_SIZE];
    u16      messageBoxTile;
    u8       counter;
    u8       curFrame    : 7;
    u8       bitpad1     : 1;
    u8       deleteMode  : 2;
    u8       bitpad2     : 6;
} WaitDial;

static void DrawStandardWindowFrame(BgConfig* bgConfig, u8 bgLayer, u8 x, u8 y, u8 width, u8 height, u8 palette, u16 tile);
static void DrawMessageBoxFrame(BgConfig* bgConfig, u8 bgLayer, u8 x, u8 y, u8 width, u8 height, u8 palette, u16 tile);
static void BlitRectToBitmap(
	void* srcPixels,
	u16 srcX,
	u16 srcY,
	u16 srcWidth,
	u16 srcHeight,
	void* destPixels,
	u16 destWidth,
	u16 destHeight,
	u16 destX,
	u16 destY,
	u16 blitWidth,
	u16 blitHeight);
static void DrawMessageBoxScrollCursor(Window* window, u16 baseTile);
static void DrawWaitDial(WaitDial* dial, u32 drawMode);
static void SysTask_TickWaitDial(SysTask* task, void* data);
static void SysTask_CleanupWaitDial(SysTask* task, void* data);
void DestroyWaitDial(void* taskData);


void LoadStandardWindowGraphics(BgConfig* bgConfig, u8 bgLayer, u16 tileOffset, u8 palOffset, u8 standardWindowType, u32 heapID) {
	u32 narcMemberIdx;
	if (standardWindowType == STANDARD_WINDOW_SYSTEM) {
		narcMemberIdx = standard_system_NCGR;
	} else {
		narcMemberIdx = standard_field_NCGR;
	}
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__WINFRAME;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__PL_WINFRAME;
	}
	
	Graphics_LoadTilesToBgLayer(narc,
		narcMemberIdx,
		bgConfig,
		bgLayer,
		tileOffset,
		0,
		FALSE,
		heapID);
	
	if (standardWindowType == STANDARD_WINDOW_UNDERGROUND) {
		narcMemberIdx = standard_underground_NCLR;
	} else {
		narcMemberIdx = standard_system_NCLR;
	}
	
	if (bgLayer < BG_LAYER_SUB_0) {
		Graphics_LoadPalette(narc,
			narcMemberIdx,
			PAL_LOAD_MAIN_BG,
			palOffset * PALETTE_SIZE_BYTES,
			PALETTE_SIZE_BYTES,
			heapID);
	} else {
		Graphics_LoadPalette(narc,
			narcMemberIdx,
			PAL_LOAD_SUB_BG,
			palOffset * PALETTE_SIZE_BYTES,
			PALETTE_SIZE_BYTES,
			heapID);
	}
}


static void DrawStandardWindowFrame(BgConfig* bgConfig, u8 bgLayer, u8 x, u8 y, u8 width, u8 height, u8 palette, u16 tile) {
	Bg_FillTilemapRect(bgConfig, bgLayer, tile,     x - 1,     y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 1, x,         y - 1,      width, 1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 2, x + width, y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 3, x - 1,     y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 5, x + width, y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 6, x - 1,     y + height, 1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 7, x,         y + height, width, 1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 8, x + width, y + height, 1,     1,      palette);
}


void Window_DrawStandardFrame(Window* window, u8 skipTransfer, u16 tile, u8 palette) {
	DrawStandardWindowFrame(window->bgConfig,
		Window_GetBgLayer(window),
		Window_GetXPos(window),
		Window_GetYPos(window),
		Window_GetWidth(window),
		Window_GetHeight(window),
		palette,
		tile);
	
	if (skipTransfer == FALSE) {
		Window_CopyToVRAM(window);
	}
}


u32 GetMessageBoxTilesNARCMember(u32 messageBoxFrame) {
	return message_box_00_NCGR + messageBoxFrame;
}


u32 GetMessageBoxPaletteNARCMember(u32 messageBoxFrame) {
	return message_box_00_NCLR + messageBoxFrame;
}


void LoadMessageBoxGraphics(BgConfig* bgConfig, u8 bgLayer, u16 tileOffset, u8 palOffset, u8 messageBoxFrame, u32 heapID) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__WINFRAME;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__PL_WINFRAME;
	}
	
	Graphics_LoadTilesToBgLayer(
		narc,
		GetMessageBoxTilesNARCMember(messageBoxFrame),
		bgConfig,
		bgLayer,
		tileOffset,
		0,
		FALSE,
		heapID);
	
	if (bgLayer < BG_LAYER_SUB_0) {
		Graphics_LoadPalette(
			narc,
			GetMessageBoxPaletteNARCMember(messageBoxFrame),
			PAL_LOAD_MAIN_BG,
			palOffset * PALETTE_SIZE_BYTES,
			PALETTE_SIZE_BYTES,
			heapID);
	} else {
		Graphics_LoadPalette(
			narc,
			GetMessageBoxPaletteNARCMember(messageBoxFrame),
			PAL_LOAD_SUB_BG,
			palOffset * PALETTE_SIZE_BYTES,
			PALETTE_SIZE_BYTES,
			heapID);
	}
}


static void DrawMessageBoxFrame(BgConfig* bgConfig, u8 bgLayer, u8 x, u8 y, u8 width, u8 height, u8 palette, u16 tile) {
	Bg_FillTilemapRect(bgConfig, bgLayer, tile,      x - 2,         y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 1,  x - 1,         y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 2,  x,             y - 1,      width, 1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 3,  x + width,     y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 4,  x + width + 1, y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 5,  x + width + 2, y - 1,      1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 6,  x - 2,         y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 7,  x - 1,         y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 9,  x + width,     y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 10, x + width + 1, y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 11, x + width + 2, y,          1,     height, palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 12, x - 2,         y + height, 1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 13, x - 1,         y + height, 1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 14, x,             y + height, width, 1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 15, x + width,     y + height, 1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 16, x + width + 1, y + height, 1,     1,      palette);
	Bg_FillTilemapRect(bgConfig, bgLayer, tile + 17, x + width + 2, y + height, 1,     1,      palette);
}


void Window_DrawMessageBox(Window* window, u32 tile, u32 palette) {
	DrawMessageBoxFrame(
		window->bgConfig,
		Window_GetBgLayer(window),
		Window_GetXPos(window),
		Window_GetYPos(window),
		Window_GetWidth(window),
		Window_GetHeight(window),
		palette,
		tile);
}


void Window_DrawMessageBoxWithScrollCursor(Window* window, u8 skipTransfer, u16 tile, u8 palette) {
	Window_DrawMessageBox(window, tile, palette);
	
	if (skipTransfer == FALSE) {
		Window_CopyToVRAM(window);
	}
	
	DrawMessageBoxScrollCursor(window, tile);
}


static void BlitRectToBitmap(
	void* srcPixels,
	u16 srcX,
	u16 srcY,
	u16 srcWidth,
	u16 srcHeight,
	void* destPixels,
	u16 destWidth,
	u16 destHeight,
	u16 destX,
	u16 destY,
	u16 blitWidth,
	u16 blitHeight
) {
	Bitmap src, dest;
	
	src.pixels = (u8*)srcPixels;
	src.width = srcWidth;
	src.height = srcHeight;
	
	dest.pixels = (u8*)destPixels;
	dest.width = destWidth;
	dest.height = destHeight;
	
	Bitmap_BlitRect4bpp(&src, &dest, srcX, srcY, destX, destY, blitWidth, blitHeight, 0);
}


static void DrawMessageBoxScrollCursor(Window* window, u16 baseTile) {
	// must forward-declare these to match
	u8* cursorTiles;
	void* cursorCharPtr;
	
	u32 heapID = BgConfig_GetHeapID(window->bgConfig);
	u8 bgLayer = Window_GetBgLayer(window);
	u8* cursorBlit = Heap_Alloc(heapID, SCROLL_CURSOR_GRAPHICS_SIZE);
	u8* bgGfx = Bg_GetCharPtr(bgLayer);
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__WINFRAME;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__PL_WINFRAME;
	}
	
	NNSG2dCharacterData* cursorCharData;
	cursorCharPtr = Graphics_GetCharData(narc,
		scroll_cursor_NCGR,
		FALSE,
		&cursorCharData,
		heapID);
	
	cursorTiles = cursorCharData->pRawData;
	
	// Copy the window frame tiles into the blit window as a background
	for (u8 i = 0; i < SCROLL_CURSOR_FRAME_COUNT; i++) {
		u32 frameOffset = i * SCROLL_CURSOR_FRAME_SIZE;
		
		memcpy(&cursorBlit[frameOffset + SCROLL_CURSOR_TILE_OFFSET(0)], &bgGfx[(baseTile + 10) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
		memcpy(&cursorBlit[frameOffset + SCROLL_CURSOR_TILE_OFFSET(1)], &bgGfx[(baseTile + 11) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
		memcpy(&cursorBlit[frameOffset + SCROLL_CURSOR_TILE_OFFSET(2)], &bgGfx[(baseTile + 10) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
		memcpy(&cursorBlit[frameOffset + SCROLL_CURSOR_TILE_OFFSET(3)], &bgGfx[(baseTile + 11) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
	}
	
	BlitRectToBitmap(cursorTiles,
		4,
		0,
		12,
		16 * 3,
		cursorBlit,
		12,
		16 * 3,
		1,
		0,
		12,
		16 * 3);
	
	Bg_LoadTiles(window->bgConfig, bgLayer, cursorBlit, SCROLL_CURSOR_GRAPHICS_SIZE, baseTile + 18);
	TextPrinter_SetScrollArrowBaseTile(baseTile);
	Heap_Free(cursorCharPtr);
	Heap_Free(cursorBlit);
}


void* Window_AddWaitDial(Window* window, u32 baseTile) {
	WaitDial* dial;
	u32 heapID;
	u8* bgCharPtr;
	u8* dialTiles;
	u8* tmp;
	void* dialTilesRaw;
	u8 bgLayer;
	u8 i;
	NNSG2dCharacterData* dialCharData;
	
	heapID = BgConfig_GetHeapID(window->bgConfig);
	bgLayer = Window_GetBgLayer(window);
	bgCharPtr = Bg_GetCharPtr(bgLayer);
	dial = Heap_Alloc(heapID, sizeof(WaitDial));
	
	memcpy(dial->messageBoxPixels, &bgCharPtr[(baseTile + 18) * TILE_SIZE_4BPP], WAIT_DIAL_FRAME_SIZE);
	
	tmp = Heap_Alloc(heapID, WAIT_DIAL_FRAME_SIZE);
	
	memcpy(&tmp[TILE_SIZE_4BPP * 0], &bgCharPtr[(baseTile + 10) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
	memcpy(&tmp[TILE_SIZE_4BPP * 1], &bgCharPtr[(baseTile + 11) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
	memcpy(&tmp[TILE_SIZE_4BPP * 2], &bgCharPtr[(baseTile + 10) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
	memcpy(&tmp[TILE_SIZE_4BPP * 3], &bgCharPtr[(baseTile + 11) * TILE_SIZE_4BPP], TILE_SIZE_4BPP);
	
	for (i = 0; i < WAIT_DIAL_FRAME_COUNT; i++) {
		memcpy(dial->pixels + WAIT_DIAL_FRAME_OFFSET(i), tmp, WAIT_DIAL_FRAME_SIZE);
	}
	
	Heap_Free(tmp);
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__WINFRAME;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__PL_WINFRAME;
	}
	dialTilesRaw = Graphics_GetCharData(narc,
		wait_dial_NCGR,
		FALSE,
		&dialCharData,
		heapID);
	dialTiles = dialCharData->pRawData;
	
	BlitRectToBitmap(dialTiles,
		0,
		0,
		WAIT_DIAL_FRAME_WIDTH_TILES * 8,
		(WAIT_DIAL_FRAME_HEIGHT_TILES * 8) * WAIT_DIAL_FRAME_COUNT,
		dial->pixels,
		WAIT_DIAL_FRAME_WIDTH_TILES * 8,
		(WAIT_DIAL_FRAME_HEIGHT_TILES * 8) * WAIT_DIAL_FRAME_COUNT,
		0,
		0,
		WAIT_DIAL_FRAME_WIDTH_TILES * 8,
		(WAIT_DIAL_FRAME_HEIGHT_TILES * 8) * WAIT_DIAL_FRAME_COUNT);
	Heap_Free(dialTilesRaw);
	
	dial->window = window;
	dial->messageBoxTile = baseTile;
	dial->counter = 0;
	dial->curFrame = 0;
	dial->deleteMode = 0;
	
	SysTask_ExecuteOnVBlank(SysTask_TickWaitDial, dial, 0);
	DrawWaitDial(dial, DIAL_DRAW_MODE_LOAD_AND_DRAW);
	
	return dial;
}


static void DrawWaitDial(WaitDial* dial, u32 drawMode) {
	u8 bgLayer = Window_GetBgLayer(dial->window);
	u8 x = Window_GetXPos(dial->window);
	u8 y = Window_GetYPos(dial->window);
	u8 width = Window_GetWidth(dial->window);
	
	if (drawMode == DIAL_DRAW_MODE_CLEAR) {
		Bg_LoadTiles(dial->window->bgConfig, bgLayer, dial->messageBoxPixels, WAIT_DIAL_FRAME_SIZE, dial->messageBoxTile + 18);
		Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 10, x + width + 1, y + 2, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
		Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 11, x + width + 2, y + 2, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
		Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 10, x + width + 1, y + 3, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
		Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 11, x + width + 2, y + 3, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
		Bg_CopyTilemapBufferToVRAM(dial->window->bgConfig, bgLayer);
		
		return;
	}
	
	Bg_LoadTiles(dial->window->bgConfig, bgLayer, &dial->pixels[WAIT_DIAL_FRAME_SIZE * dial->curFrame], WAIT_DIAL_FRAME_SIZE, dial->messageBoxTile + 18);
	
	if (drawMode == DIAL_DRAW_MODE_LOAD_ONLY) {
		return;
	}

	Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 18, x + width + 1, y + 2, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
	Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 19, x + width + 2, y + 2, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
	Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 20, x + width + 1, y + 3, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
	Bg_FillTilemapRect(dial->window->bgConfig, bgLayer, dial->messageBoxTile + 21, x + width + 2, y + 3, 1, 1, TILEMAP_FILL_VAL_KEEP_PALETTE);
	Bg_CopyTilemapBufferToVRAM(dial->window->bgConfig, bgLayer);
}


static void SysTask_TickWaitDial(SysTask* task, void* data) {
	WaitDial* dial = data;
	
	if (dial->deleteMode != DIAL_DELETE_MODE_NONE) {
		if (dial->deleteMode == DIAL_DELETE_MODE_CLEAR) {
			DrawWaitDial(dial, DIAL_DRAW_MODE_CLEAR);
		}
		
		SysTask_Done(task);
		return;
	}
	
	dial->counter++;
	
	if (dial->counter == 16) {
		dial->counter = 0;
		dial->curFrame = (dial->curFrame + 1) & 7;
		DrawWaitDial(dial, DIAL_DRAW_MODE_LOAD_ONLY);
	}
}


static void SysTask_CleanupWaitDial(SysTask* task, void* data) {
	Heap_Free(data);
	SysTask_Done(task);
}


void DestroyWaitDial(void* taskData) {
	WaitDial* dial = taskData;
	
	SysTask_ExecuteAfterVBlank(SysTask_CleanupWaitDial, dial, 0);
	dial->deleteMode = DIAL_DELETE_MODE_CLEAR;
}
