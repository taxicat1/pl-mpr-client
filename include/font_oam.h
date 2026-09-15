#ifndef MPR_FONT_OAM_H
#define MPR_FONT_OAM_H

#include <nnsys/g2d.h>

#include "sprite.h"
#include "bg_window.h"
#include "heap.h"

typedef struct {
	Sprite*  unk_00;
	int      unk_04;
	int      unk_08;
} FontOAMUnkSubstruct1;

typedef struct {
	FontOAMUnkSubstruct1*  unk_00;
	int                    unk_04;
	const Sprite*          unk_08;
	int  x;
	int  y;
} FontOAM;

typedef struct {
	void*                unk_00[12];
	NNSG2dCellDataBank*  unk_30[12];
	FontOAM*             unk_60;
	int                  unk_64;
} FontOAMManager;

typedef struct {
	const FontOAMManager* unk_00;
	const Window* unk_04;
	SpriteList* unk_08;
	const NNSG2dImagePaletteProxy* unk_0C;
	const Sprite* unk_10;
	int unk_14;
	int unk_18;
	int unk_1C;
	int unk_20;
	int unk_24;
	int unk_28;
	HeapID heapID;
} FontOAMTemplate;

FontOAMManager* FontOAMManager_New(int numFontOAM, HeapID heapID);
void FontOAMManager_Free(FontOAMManager* manager);
FontOAM* FontOAM_New(const FontOAMTemplate* template);
void FontOAM_Free(FontOAM* fontOAM);
int FontOAM_GetCharSizeFromWindow(const Window* window, int param1, HeapID heapID);
void FontOAM_SetXY(FontOAM* fontOAM, int x, int y);
void FontOAM_GetXY(const FontOAM* fontOAM, int* x, int* y);
void FontOAM_SetExplicitPalette(FontOAM* fontOAM, u32 palette);
void FontOAM_CopyFromWindow(
	const Window* window,
	int width,
	int height,
	int x,
	int y,
	char* output);

#endif /* MPR_FONT_OAM_H */
