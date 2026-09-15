#include <nitro.h>
#include <string.h>

#include "font_oam.h"

#include "assert.h"
#include "heap.h"
#include "graphics.h"
#include "sprite.h"
#include "bg_window.h"
#include "char_transfer.h"

typedef struct {
	int  unk_00;
	int  unk_04;
	int  unk_08;
	int  unk_0C;
} FontOAM_UnkStruct0;

typedef struct {
	FontOAM_UnkStruct0  unk_00;
	FontOAM_UnkStruct0  unk_10;
	
	u8  unk_20;
} FontOAM_UnkStruct1;

typedef struct FontOAM_UnkStruct2 {
	int  unk_00;
	int  unk_04;
	int  unk_08;
	
	struct FontOAM_UnkStruct2*  unk_0C;
	struct FontOAM_UnkStruct2*  unk_10;
} FontOAM_UnkStruct2;

typedef struct {
	FontOAM_UnkStruct2 unk_00;
	int unk_14;
} FontOAM_UnkStruct3;

static void FontOAM_0200C8A0(FontOAM* param0);
static FontOAM* FontOAM_0200C8B0(const FontOAMManager* param0);
static int FontOAM_0200C8D8(int param0, int param1);
static BOOL FontOAM_0200C904(FontOAM_UnkStruct1* param0, FontOAM_UnkStruct2* param1, HeapID heapID);
static int FontOAM_0200C9B4(int param0, int param1, HeapID heapID, FontOAM_UnkStruct2* param3);
static void FontOAM_0200CA3C(const Window* param0, const FontOAM_UnkStruct2* param1, NNSG2dImageProxy* param2, int param3, int param4, HeapID heapID);
static int FontOAM_0200CAB0(const Window* param0, const FontOAM_UnkStruct2* param1, NNSG2dImageProxy* param2, int param3, int param4, int param5, int param6, HeapID heapID);
static int FontOAM_0200CB68(const FontOAM_UnkStruct2* param0, int param1);
static void FontOAM_0200CBBC(const FontOAMTemplate* param0, const FontOAM_UnkStruct2* param1, const NNSG2dImageProxy* param2, FontOAM* param3);
static void FontOAM_0200CC10(FontOAM* param0);
static Sprite* FontOAM_0200CC34(const FontOAMTemplate* param0, const FontOAM_UnkStruct2* param1, const NNSG2dImageProxy* param2);
static FontOAM_UnkStruct2* FontOAM_0200CCBC(HeapID heapID);
static void FontOAM_0200CCD8(FontOAM_UnkStruct2* param0);
static void FontOAM_0200CCEC(FontOAM_UnkStruct2* param0);
static void FontOAM_0200CD04(FontOAM_UnkStruct2* param0, FontOAM_UnkStruct2* param1);

static const u8 sUnk_0205DAF8[12][2] = {
	{ 0x08, 0x08 },
	{ 0x08, 0x04 },
	{ 0x04, 0x08 },
	{ 0x04, 0x04 },
	{ 0x04, 0x02 },
	{ 0x04, 0x01 },
	{ 0x02, 0x04 },
	{ 0x02, 0x02 },
	{ 0x02, 0x01 },
	{ 0x01, 0x04 },
	{ 0x01, 0x02 },
	{ 0x01, 0x01 }
};


FontOAMManager* FontOAMManager_New(int numFontOAM, HeapID heapID) {
	FontOAMManager* v0;
	int v1;
	
	v0 = Heap_Alloc(heapID, sizeof(FontOAMManager));
	GF_ASSERT(v0);
	
	for (v1 = 0; v1 < 12; v1++) {
		v0->unk_00[v1] = Graphics_GetCellBank(35, v1, 0, &v0->unk_30[v1], heapID);
		GF_ASSERT(v0->unk_00[v1]);
	}
	
	v0->unk_60 = Heap_Alloc(heapID, sizeof(FontOAM) * numFontOAM);
	GF_ASSERT(v0->unk_60);
	
	v0->unk_64 = numFontOAM;
	memset(v0->unk_60, 0, sizeof(FontOAM) * numFontOAM);
	
	return v0;
}


void FontOAMManager_Free(FontOAMManager* manager) {
	int v0;
	
	GF_ASSERT(manager);
	
	for (v0 = 0; v0 < 12; v0++) {
		Heap_Free(manager->unk_00[v0]);
	}
	
	Heap_Free(manager->unk_60);
	Heap_Free(manager);
}


FontOAM* FontOAM_New(const FontOAMTemplate* template) {
	FontOAM* v0;
	FontOAM_UnkStruct2 v1;
	int v2;
	NNSG2dImageProxy* v3;
	
	GF_ASSERT(template);
	
	v0 = FontOAM_0200C8B0(template->unk_00);
	GF_ASSERT(v0);
	
	v0->unk_08 = template->unk_10;
	v0->x = template->unk_18;
	v0->y = template->unk_1C;
	
	v1.unk_0C = &v1;
	v1.unk_10 = &v1;
	
	v2 = FontOAM_0200C9B4(template->unk_04->width, template->unk_04->height, template->heapID, &v1);
	v3 = Heap_AllocAtEnd(template->heapID, sizeof(NNSG2dImageProxy) * v2);
	
	v0->unk_00 = Heap_Alloc(template->heapID, sizeof(FontOAMUnkSubstruct1) * v2);
	v0->unk_04 = v2;
	
	FontOAM_0200CA3C(template->unk_04, &v1, v3, template->unk_14, template->unk_28, template->heapID);
	FontOAM_0200CBBC(template, &v1, v3, v0);
	Heap_Free(v3);
	FontOAM_0200CCEC(&v1);
	
	return v0;
}


void FontOAM_Free(FontOAM* fontOAM) {
	GF_ASSERT(fontOAM);
	GF_ASSERT(fontOAM->unk_00);
	
	FontOAM_0200CC10(fontOAM);
	Heap_Free(fontOAM->unk_00);
	FontOAM_0200C8A0(fontOAM);
}


int FontOAM_GetCharSizeFromWindow(const Window* window, int param1, HeapID heapID) {
	FontOAM_UnkStruct2 v0;
	int v1;
	
	v0.unk_0C = &v0;
	v0.unk_10 = &v0;
	
	FontOAM_0200C9B4(window->width, window->height, heapID, &v0);
	v1 = FontOAM_0200CB68(&v0, param1);
	FontOAM_0200CCEC(&v0);
	
	return v1;
}


void FontOAM_SetXY(FontOAM* fontOAM, int x, int y) {
	GF_ASSERT(fontOAM);
	
	fontOAM->x = x;
	fontOAM->y = y;
	
	x *= FX32_ONE;
	y *= FX32_ONE;
	
	if (fontOAM->unk_08) {
		const VecFx32* fontPos = Sprite_GetPosition(fontOAM->unk_08);
		
		x += fontPos->x;
		y += fontPos->y;
	}
	
	VecFx32 spritePos;
	spritePos.z = 0;
	
	for (int v0 = 0; v0 < fontOAM->unk_04; v0++) {
		spritePos.x = x + (fontOAM->unk_00[v0].unk_04 << FX32_SHIFT);
		spritePos.y = y + (fontOAM->unk_00[v0].unk_08 << FX32_SHIFT);
		
		Sprite_SetPosition(fontOAM->unk_00[v0].unk_00, &spritePos);
	}
}


void FontOAM_GetXY(const FontOAM* fontOAM, int* x, int* y) {
	GF_ASSERT(fontOAM);
	GF_ASSERT(x);
	GF_ASSERT(y);
	
	*x = fontOAM->x;
	*y = fontOAM->y;
}


void FontOAM_SetExplicitPalette(FontOAM* fontOAM, u32 palette) {
	int v0;
	
	GF_ASSERT(fontOAM);
	
	for (v0 = 0; v0 < fontOAM->unk_04; v0++) {
		Sprite_SetExplicitPalette(fontOAM->unk_00[v0].unk_00, palette);
	}
}


void FontOAM_CopyFromWindow(
	const Window* window,
	int width,
	int height,
	int x,
	int y,
	char* output
) {
	// this function copies a rectangle of pixels from the window
	// to the output starting at (x,y) and spanning (width,height)
	int i;
	int dstOffset;
	int srcOffset;
	
	GF_ASSERT(window->width >= (width + x));
	GF_ASSERT(window->height >= (height + y));
	
	for (i = 0; i < height; i++) {
		dstOffset = i * width;
		dstOffset *= 32;
		srcOffset = ((i + y) * window->width);
		srcOffset += x;
		srcOffset *= 32;
		
		memcpy(output + dstOffset, (char*)(window->pixels) + srcOffset, 32 * width);
	}
}


static void FontOAM_0200C8A0(FontOAM* param0) {
	memset(param0, 0, sizeof(FontOAM));
}


static FontOAM* FontOAM_0200C8B0(const FontOAMManager* param0) {
	int v0;
	
	for (v0 = 0; v0 < param0->unk_64; v0++) {
		if (param0->unk_60[v0].unk_00 == NULL) {
			return param0->unk_60 + v0;
		}
	}
	
	return NULL;
}


static int FontOAM_0200C8D8(int param0, int param1) {
	int v0;
	
	for (v0 = 0; v0 < 12; v0++) {
		if ((sUnk_0205DAF8[v0][0] <= param0) && (sUnk_0205DAF8[v0][1] <= param1)) {
			return v0;
		}
	}
	
	return 12;
}


static BOOL FontOAM_0200C904(FontOAM_UnkStruct1* param0, FontOAM_UnkStruct2* param1, HeapID heapID) {
	FontOAM_UnkStruct2* v0;
	int v1;
	int v2;
	
	v0 = FontOAM_0200CCBC(heapID);
	FontOAM_0200CD04(v0, param1->unk_10);
	
	v0->unk_08 = FontOAM_0200C8D8(param0->unk_00.unk_08, param0->unk_00.unk_0C);
	v0->unk_00 = param0->unk_00.unk_04;
	v0->unk_04 = param0->unk_00.unk_00;
	
	v2 = param0->unk_00.unk_08 - sUnk_0205DAF8[v0->unk_08][0];
	v1 = param0->unk_00.unk_0C - sUnk_0205DAF8[v0->unk_08][1];
	
	if (v2 > 0) {
		param0->unk_10.unk_0C = param0->unk_00.unk_0C;
		param0->unk_10.unk_08 = v2;
		param0->unk_10.unk_00 = param0->unk_00.unk_00;
		param0->unk_10.unk_04 = param0->unk_00.unk_04 + sUnk_0205DAF8[v0->unk_08][0];
		
		GF_ASSERT(param0->unk_20 != 1);
		param0->unk_20 = 1;
	}
	
	if (v1 > 0) {
		param0->unk_00.unk_00 = param0->unk_00.unk_00 + sUnk_0205DAF8[v0->unk_08][1];
		param0->unk_00.unk_0C = v1;
	} else {
		if (param0->unk_20 == 1) {
			param0->unk_00 = param0->unk_10;
			param0->unk_20 = 0;
		} else {
			return 1;
		}
	}
	
	return 0;
}


static int FontOAM_0200C9B4(int param0, int param1, HeapID heapID, FontOAM_UnkStruct2* param3) {
	FontOAM_UnkStruct0 v0;
	FontOAM_UnkStruct1 v1;
	int v2;
	int v3;
	
	GF_ASSERT(param0);
	GF_ASSERT(param1);
	
	v3 = 0;
	
	v1.unk_00.unk_00 = 0;
	v1.unk_00.unk_04 = 0;
	v1.unk_00.unk_08 = param0;
	v1.unk_00.unk_0C = param1;
	v1.unk_20 = 0;
	
	v0.unk_04 = 0;
	v0.unk_08 = param0;
	
	while (v1.unk_00.unk_0C != 0) {
		v2 = FontOAM_0200C8D8(v1.unk_00.unk_08, v1.unk_00.unk_0C);
		
		v0.unk_00 = v1.unk_00.unk_00 + sUnk_0205DAF8[v2][1];
		v0.unk_0C = v1.unk_00.unk_0C - sUnk_0205DAF8[v2][1];
		v1.unk_00.unk_0C = sUnk_0205DAF8[v2][1];
		
		do {
			v3++;
		} while (FontOAM_0200C904(&v1, param3, heapID) == 0);
		
		v1.unk_00 = v0;
	}
	
	return v3;
}


static void FontOAM_0200CA3C(const Window* param0, const FontOAM_UnkStruct2* param1, NNSG2dImageProxy* param2, int param3, int param4, HeapID heapID) {
	FontOAM_UnkStruct2* v0;
	int v1;
	int v2;
	GXOBJVRamModeChar v3;
	
	if (param4 == NNS_G2D_VRAM_TYPE_2DMAIN) {
		v3 = GX_GetOBJVRamModeChar();
	} else {
		v3 = GXS_GetOBJVRamModeChar();
	}
	
	v2 = CharTransfer_GetBlockSize(v3);
	v1 = 0;
	v0 = param1->unk_0C;
	
	while (v0 != param1) {
		NNS_G2dInitImageProxy(param2 + v1);
		param3 = FontOAM_0200CAB0(param0, v0, param2 + v1, v2, v3, param3, param4, heapID);
		v0 = v0->unk_0C;
		v1++;
	}
}


static int FontOAM_0200CAB0(const Window* param0, const FontOAM_UnkStruct2* param1, NNSG2dImageProxy* param2, int param3, int param4, int param5, int param6, HeapID heapID) {
	char* v0;
	int v1;
	int v2 = sUnk_0205DAF8[param1->unk_08][0];
	int v3 = sUnk_0205DAF8[param1->unk_08][1];
	v1 = v2;
	v1 *= v3;
	
	if (v1 < param3) {
		v1 = param3;
	}
	
	v1 *= 32;
	v0 = Heap_AllocAtEnd(heapID, v1);
	
	FontOAM_CopyFromWindow(param0, v2, v3, param1->unk_00, param1->unk_04, v0);
	DC_FlushRange(v0, v1);
	
	if (param6 == NNS_G2D_VRAM_TYPE_2DMAIN) {
		GX_LoadOBJ(v0, param5, v1);
		param2->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN] = param5;
		param2->attr.mappingType = GX_GetOBJVRamModeChar();
	} else {
		GXS_LoadOBJ(v0, param5, v1);
		param2->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DSUB] = param5;
		param2->attr.mappingType = GXS_GetOBJVRamModeChar();
	}
	
	param2->attr.sizeS = NNS_G2D_1D_MAPPING_CHAR_SIZE;
	param2->attr.sizeT = NNS_G2D_1D_MAPPING_CHAR_SIZE;
	param2->attr.fmt = GX_TEXFMT_PLTT16;
	param2->attr.bExtendedPlt = 0;
	param2->attr.plttUse = GX_TEXPLTTCOLOR0_TRNS;
	param2->attr.mappingType = param4;
	
	Heap_Free(v0);
	
	return param5 + v1;
}


static int FontOAM_0200CB68(const FontOAM_UnkStruct2* param0, int param1) {
	FontOAM_UnkStruct2* v0;
	int v1;
	GXOBJVRamModeChar v2;
	int v3;
	int v4;
	int v5, v6;
	
	if (param1 == NNS_G2D_VRAM_TYPE_2DMAIN) {
		v2 = GX_GetOBJVRamModeChar();
	} else {
		v2 = GXS_GetOBJVRamModeChar();
	}
	
	v1 = CharTransfer_GetBlockSize(v2);
	v3 = 0;
	v0 = param0->unk_0C;
	
	while (v0 != param0) {
		v5 = sUnk_0205DAF8[v0->unk_08][0];
		v6 = sUnk_0205DAF8[v0->unk_08][1];
		v4 = v5 * v6;
		
		if (v4 < v1) {
			v4 = v1;
		}
		
		v3 += v4 * 32;
		v0 = v0->unk_0C;
	}
	
	return v3;
}


static void FontOAM_0200CBBC(const FontOAMTemplate* param0, const FontOAM_UnkStruct2* param1, const NNSG2dImageProxy* param2, FontOAM* param3) {
	FontOAM_UnkStruct2* v0;
	int v1 = 0;
	v0 = param1->unk_0C;
	
	while (v0 != param1) {
		param3->unk_00[v1].unk_00 = FontOAM_0200CC34(param0, v0, param2 + v1);
		GF_ASSERT(param3->unk_00[v1].unk_00);
		
		param3->unk_00[v1].unk_04 = v0->unk_00 * 8;
		param3->unk_00[v1].unk_08 = v0->unk_04 * 8;
		
		v0 = v0->unk_0C;
		v1++;
	}
}


static void FontOAM_0200CC10(FontOAM* param0) {
	int v0;
	
	for (v0 = 0; v0 < param0->unk_04; v0++) {
		Sprite_Delete(param0->unk_00[v0].unk_00);
	}
}


static Sprite* FontOAM_0200CC34(const FontOAMTemplate* param0, const FontOAM_UnkStruct2* param1, const NNSG2dImageProxy* param2) {
	SpriteListTemplate v0;
	SpriteResourcesHeader v1;
	
	v1.imageProxy = param2;
	v1.charData = NULL;
	v1.paletteProxy = param0->unk_0C;
	v1.cellBank = param0->unk_00->unk_30[param1->unk_08];
	v1.cellAnimBank = NULL;
	v1.multiCellBank = NULL;
	v1.multiCellAnimBank = NULL;
	v1.isVRamTransfer = 0;
	v1.priority = param0->unk_20;
	
	v0.list = param0->unk_08;
	v0.resourceData = &v1;
	v0.priority = param0->unk_24;
	v0.vramType = param0->unk_28;
	v0.heapID = param0->heapID;
	v0.position.x = 0;
	v0.position.y = 0;
	v0.position.z = 0;
	
	if (param0->unk_10) {
		const VecFx32* v2;
		
		v2 = Sprite_GetPosition(param0->unk_10);
		v0.position = *v2;
	}
	
	v0.position.x += (param0->unk_18 << FX32_SHIFT) + ((param1->unk_00 * 8) << FX32_SHIFT);
	v0.position.y += (param0->unk_1C << FX32_SHIFT) + ((param1->unk_04 * 8) << FX32_SHIFT);
	
	return SpriteList_Add(&v0);
}


static FontOAM_UnkStruct2* FontOAM_0200CCBC(HeapID heapID) {
	FontOAM_UnkStruct2* v0 = Heap_AllocAtEnd(heapID, sizeof(FontOAM_UnkStruct2));
	GF_ASSERT(v0);
	
	v0->unk_0C = NULL;
	v0->unk_10 = NULL;
	
	return v0;
}


static void FontOAM_0200CCD8(FontOAM_UnkStruct2* param0) {
	GF_ASSERT(param0);
	Heap_Free(param0);
}


static void FontOAM_0200CCEC(FontOAM_UnkStruct2* param0) {
	FontOAM_UnkStruct2* v0;
	FontOAM_UnkStruct2* v1;
	
	v0 = param0->unk_0C;
	
	while (v0 != param0) {
		v1 = v0->unk_0C;
		FontOAM_0200CCD8(v0);
		v0 = v1;
	}
}


static void FontOAM_0200CD04(FontOAM_UnkStruct2* param0, FontOAM_UnkStruct2* param1) {
	param0->unk_0C = param1->unk_0C;
	param0->unk_10 = param1;
	param1->unk_0C->unk_10 = param0;
	param1->unk_0C = param0;
}
