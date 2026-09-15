#ifndef MPR_BG_WINDOW_H
#define MPR_BG_WINDOW_H

#include <nitro/types.h>
#include <nitro/fx/fx.h>
#include <nitro/gx.h>

#include "constants/heap.h"

#define TILEMAP_COPY_SRC_FLAT  0 // source dimensions are equal to dest dimensions
#define TILEMAP_COPY_SRC_RECT  1 // dest dimensions carve a window from source

#define TILEMAP_FILL_VAL_KEEP_PALETTE      16 // do not replace the selected palette index
#define TILEMAP_FILL_VAL_INCLUDES_PALETTE  17

#define TILEMAP_PALETTE_SHIFT(i)  (i << 12)
#define TILEMAP_MASK_PALETTE      0xF000
#define TILEMAP_MASK_INDEX        0x0FFF

#define PIXEL_FILL(color)  (((color) << 4) | (color))

typedef enum {
	BG_LAYER_MAIN_0 = 0,
	BG_LAYER_MAIN_1,
	BG_LAYER_MAIN_2,
	BG_LAYER_MAIN_3,
	BG_LAYER_SUB_0,
	BG_LAYER_SUB_1,
	BG_LAYER_SUB_2,
	BG_LAYER_SUB_3,
	BG_LAYER_MAX
} BgLayer;

typedef enum {
	BG_TYPE_STATIC = 0,
	BG_TYPE_AFFINE,
	BG_TYPE_STATIC_WITH_AFFINE
} BgType;

typedef enum {
	BG_OFFSET_UPDATE_SET_X = 0,
	BG_OFFSET_UPDATE_ADD_X,
	BG_OFFSET_UPDATE_SUB_X,
	BG_OFFSET_UPDATE_SET_Y,
	BG_OFFSET_UPDATE_ADD_Y,
	BG_OFFSET_UPDATE_SUB_Y
} BgOffsetUpdateOp;

typedef enum {
	BG_SCREEN_SIZE_128x128 = 0,
	BG_SCREEN_SIZE_256x256,
	BG_SCREEN_SIZE_256x512,
	BG_SCREEN_SIZE_512x256,
	BG_SCREEN_SIZE_512x512,
	BG_SCREEN_SIZE_1024x1024
} BgScreenSize;

typedef enum {
	BG_COLOR_MODE_4BPP = 0,
	BG_COLOR_MODE_8BPP
} BgColorMode;

typedef enum {
	SCROLL_DIRECTION_UP = 0,
	SCROLL_DIRECTION_DOWN,
	SCROLL_DIRECTION_LEFT,
	SCROLL_DIRECTION_RIGHT
} ScrollDirection;

typedef struct {
	const u8*  pixels;
	u16        width;
	u16        height;
} Bitmap;

typedef struct {
	u32   x;
	u32   y;
	u32   bufferSize;
	u32   baseTile;
	u8    screenSize;
	u8    colorMode;
	u8    screenBase;
	u8    charBase;
	u8    bgExtPltt;
	u8    priority;
	u8    areaOver;
	BOOL  mosaic;
} BgTemplate;

typedef struct {
	void* tilemapBuffer;
	
	u32  bufferSize;
	u32  baseTile;
	
	int  xOffset;
	int  yOffset;
	
	u8  type;
	u8  screenSize;
	u8  colorMode;
	u8  tileSize;
	
	u16 rotation;
	
	fx32  xScale;
	fx32  yScale;
	
	int  xCenter;
	int  yCenter;
} Background;

typedef struct {
	HeapID      heapID;
	u16         scrollScheduled;
	u16         bufferTransferScheduled;
	Background  bgs[8];
} BgConfig;

typedef struct {
	u8   bgLayer;
	u8   tilemapLeft;
	u8   tilemapTop;
	u8   width;
	u8   height;
	u8   palette;
	u16  baseTile;
} WindowTemplate;

typedef struct {
	BgConfig*  bgConfig;
	u8         bgLayer;
	u8         tilemapLeft;
	u8         tilemapTop;
	u8         width;
	u8         height;
	u8         palette;
	u16        baseTile  : 15;
	u16        colorMode : 1;
	void*      pixels;
} Window;

typedef struct {
	GXDispMode  displayMode;
	GXBGMode    mainBgMode;
	GXBGMode    subBgMode;
	GXBG0As     bg0As2DOr3D;
} GraphicsModes;

BgConfig* BgConfig_New(HeapID heapID);
u32 BgConfig_GetHeapID(BgConfig* bgConfig);
void SetAllGraphicsModes(const GraphicsModes* graphicsModes);
void Bg_InitFromTemplate(BgConfig* bgConfig, u8 bgLayer, const BgTemplate* bgTemplate, u8 bgType);
void Bg_FreeTilemapBuffer(BgConfig* bgConfig, u8 bgLayer);
void Bg_SetPriority(u8 bgLayer, u8 priority);
void Bg_ToggleLayer(u8 bgLayer, u8 enable);
void Bg_SetOffset(BgConfig* bgConfig, u8 bgLayer, u8 bgUpdateOffsetOp, int val);
int Bg_GetXOffset(BgConfig* bgConfig, BgLayer bgLayer);
int Bg_GetYOffset(BgConfig* bgConfig, BgLayer bgLayer);
void Bg_SetAffineParams(BgConfig* bgConfig, u8 bgLayer, const MtxFx22* mtx, int centerX, int centerY);
void Bg_CopyTilemapBufferToVRAM(BgConfig* bgConfig, u8 bgLayer);
void Bg_CopyTilemapBufferRangeToVRAM(BgConfig* bgConfig, u8 bgLayer, void* src, u32 size, u32 offset);
void Bg_LoadTilemapBuffer(BgConfig* bgConfig, u8 bgLayer, void* src, u32 size);
void Bg_LoadTiles(BgConfig* bgConfig, u8 bgLayer, void* src, u32 size, u32 tileStart);
void Bg_LoadTilesToVRAM(BgConfig* bgConfig, u8 bgLayer, void* src, u32 size, u32 offset);
void Bg_ClearTilesRange(u8 bgLayer, u32 size, u32 offset, HeapID heapID);
void Bg_FillTilesRange(BgConfig* bgConfig, u32 bgLayer, u32 fillVal, u32 numTiles, u32 offset);
void Bg_LoadPalette(u8 bgLayer, void* src, u16 size, u16 offset);
void Bg_MaskPalette(u8 bgLayer, u16 mask);
void Bg_LoadToTilemapRect(BgConfig* bgConfig, u8 bgLayer, const void* src, u8 destX, u8 destY, u8 destWidth, u8 destHeight);
void Bg_CopyToTilemapRect(BgConfig* bgConfig, u8 bgLayer, u8 destX, u8 destY, u8 destWidth, u8 destHeight, const void* src, u8 srcX, u8 srcY, u8 srcWidth, u8 srcHeight);
void Bg_FillTilemapRect(BgConfig* bgConfig, u8 bgLayer, u16 fillVal, u8 x, u8 y, u8 width, u8 height, u8 palette);
void Bg_ChangeTilemapRectPalette(BgConfig* bgConfig, u8 bgLayer, u8 x, u8 y, u8 width, u8 height, u8 palette);
void Bg_ClearTilemap(BgConfig* bgConfig, u8 bgLayer);
void* Bg_GetCharPtr(u8 bgLayer);
void* Bg_GetTilemapBuffer(BgConfig* bgConfig, u8 bgLayer);
void Bitmap_BlitRect4bpp(const Bitmap* src, const Bitmap* dest, u16 srcX, u16 srcY, u16 destX, u16 destY, u16 width, u16 height, u16 transparent);
void Bitmap_BlitRect8bpp(const Bitmap* src, const Bitmap* dest, u16 srcX, u16 srcY, u16 destX, u16 destY, u16 width, u16 height, u16 transparent);
void Bitmap_FillRect4bpp(const Bitmap* bitmap, u16 x, u16 y, u16 width, u16 height, u8 fillVal);
void Bitmap_FillRect8bpp(const Bitmap* bitmap, u16 x, u16 y, u16 width, u16 height, u8 fillVal);
Window* Window_New(HeapID heapID, u8 winCount);
void Window_Init(Window* window);
void Window_Add(BgConfig* bgConfig, Window* window, u8 bgLayer, u8 tilemapLeft, u8 tilemapTop, u8 width, u8 height, u8 palette, u16 baseTile);
void Window_AddToTopLeftCorner(BgConfig* bgConfig, Window* window, u8 width, u8 height, u16 baseTile, u8 fillVal);
void Window_AddFromTemplate(BgConfig* bgConfig, Window* window, const WindowTemplate* windowTemplate);
void Window_Remove(Window* window);
void Windows_Delete(Window* window, u8 winCount);
void Window_CopyToVRAM(Window* window);
void Window_ScheduleCopyToVRAM(Window* window);
void Window_PutToTilemap(Window* window);
void Window_PutRectToTilemap(Window* window, u32 width, u32 height);
void Window_ClearTilemap(Window* window);
void Window_LoadTiles(Window* window);
void Window_ClearAndCopyToVRAM(Window* window);
void Window_ClearAndScheduleCopyToVRAM(Window* window);
void Window_FillTilemap(Window* window, u8 val);
void Window_BlitBitmapRect(Window* window, void* src, u16 srcX, u16 srcY, u16 srcWidth, u16 srcHeight, u16 destX, u16 destY, u16 destWidth, u16 destHeight);
void Window_BlitBitmapRectWithTransparency(Window* window, void* src, u16 srcX, u16 srcY, u16 srcWidth, u16 srcHeight, u16 destX, u16 destY, u16 destWidth, u16 destHeight, u16 transparent);
void Window_FillRectWithColor(Window* window, u8 color, u16 x, u16 y, u16 width, u16 height);
void Window_CopyGlyph(Window* window, const u8* glyphPixels, u16 srcWidth, u16 srcHeight, u16 destX, u16 destY, u16 table);
void Window_Scroll(Window* window, u8 direction, u8 distance, u8 fillVal);
u8 Window_GetBgLayer(Window* window);
u8 Window_GetWidth(Window* window);
u8 Window_GetHeight(Window* window);
u8 Window_GetXPos(Window* window);
u8 Window_GetYPos(Window* window);
void Window_SetXPos(Window* window, u8 x);
void Window_SetYPos(Window* window, u8 y);
void Bg_RunScheduledUpdates(BgConfig* bgConfig);
void Bg_ScheduleTilemapTransfer(BgConfig* bgConfig, u8 bgLayer);
void Bg_ScheduleScroll(BgConfig* bgConfig, u8 bgLayer, u8 bgUpdateOffsetOp, int val);
u8 Bg_DoesPixelAtXYMatchVal(BgConfig* bgConfig, u8 bgLayer, u16 x, u16 y, u16* src);

#endif /* MPR_BG_WINDOW_H */
