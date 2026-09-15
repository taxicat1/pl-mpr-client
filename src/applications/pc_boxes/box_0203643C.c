// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_0203643C.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_02034D5C.h"

#include "assert.h"
#include "bg_window.h"
#include "common.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "math_util.h"
#include "narc.h"
#include "pc_boxes.h"
#include "sprite.h"
#include "string_gf.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "text.h"

typedef struct {
	BoxTouchSelectorManager* unk_00;
	u32 unk_04;
	Sprite* unk_08;
	u16 unk_0C;
	s16 unk_0E;
	fx32 unk_10;
	fx32 unk_14;
	int unk_18;
} BoxTouchSelector_UnkStruct0;

typedef struct {
	void* unk_00;
	Sprite* unk_04;
	fx32 unk_08;
	fx32 unk_0C;
	fx32 unk_10;
	u16 unk_14;
	u16 unk_16;
	BOOL unk_18;
	fx32 unk_1C;
	fx32 unk_20;
	fx32 unk_24;
	u16 unk_28;
	u16 unk_2A;
} BoxTouchSelector_UnkStruct1;

typedef struct {
	BoxTouchSelectorManager* unk_00;
	Sprite** unk_04;
	BoxTouchSelector_UnkStruct1 unk_08[10];
	u8 unk_1C0[10];
	u8 unk_1CA;
	u8 unk_1CB;
	u8 unk_1CC;
	u8 unk_1CD;
} BoxTouchSelector_UnkStruct2;

struct BoxTouchSelectorManager {
	BoxApplicationDisplay* unk_00;
	const BoxApplication* unk_04;
	BgConfig* unk_08;
	SpriteList* unk_0C;
	u32 unk_10;
	SysTask* unk_14;
	NNSG2dImagePaletteProxy unk_18;
	Sprite* unk_2C;
	Sprite* unk_30;
	Sprite* unk_34[8];
	Sprite* unk_54[10];
	int unk_7C;
	int unk_80;
	String* unk_84;
	Window unk_88;
	void* unk_98;
	NNSG2dCellDataBank* unk_9C;
	void* unk_A0;
	NNSG2dAnimBankData* unk_A4;
	void* unk_A8;
	NNSG2dCellDataBank* unk_AC;
	void* unk_B0;
	NNSG2dAnimBankData* unk_B4;
	BoxTouchSelector_UnkStruct0 unk_B8;
	u8 unk_D4[16];
	u8 unk_E4[16];
};

static void BoxTouchSelector_020364D8(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036778(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036838(BoxTouchSelectorManager* param0);
static SysTask* BoxTouchSelector_02036948(BoxTouchSelectorManager* param0, SysTaskFunc param1, void* param2);
static void BoxTouchSelector_0203695C(BoxTouchSelectorManager* param0);
static BOOL BoxTouchSelector_0203696C(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036A70(SysTask* param0, void* param1);
static void BoxTouchSelector_02036B48(SysTask* param0, void* param1);
static void BoxTouchSelector_02036C74(SysTask* param0, void* param1);
static void BoxTouchSelector_02036D98(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036DEC(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036E58(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036EFC(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02036F9C(BoxTouchSelectorManager* param0);
static u32 BoxTouchSelector_02036FBC(u32 param0, u32 param1);
static void BoxTouchSelector_02036FE0(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_020370AC(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_020370CC(SysTask* param0, void* param1);
static void BoxTouchSelector_02037150(SysTask* param0, void* param1);
static void BoxTouchSelector_02037200(SysTask* param0, BoxTouchSelector_UnkStruct1* param1);
static void BoxTouchSelector_02037228(BoxTouchSelector_UnkStruct2* param0, Sprite* param1, BoxTouchSelector_UnkStruct1* param2, u32 param3, u32 param4, u32 param5, u32 param6, BOOL param7);
static void BoxTouchSelector_020372A4(BoxTouchSelector_UnkStruct2* param0, BoxTouchSelector_UnkStruct1* param1, u32 param2, u32 param3, u32 param4);
static void BoxTouchSelector_020372F0(SysTask* param0, void* param1);
static void BoxTouchSelector_020373A8(void* param0);
static void BoxTouchSelector_020373B0(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02037400(SysTask* param0, void* param1);
static void BoxTouchSelector_020375D8(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_0203762C(SysTask* param0, void* param1);
static void BoxTouchSelector_020376D0(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02037720(SysTask* param0, void* param1);
static void BoxTouchSelector_020378D4(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02037954(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02037960(BoxTouchSelectorManager* param0);
static void BoxTouchSelector_02037980(BoxTouchSelectorManager* param0);

static const fx32 sDAT_0206256C[] = {
	0x0,
	0xFFFA0000,
	0x58000
};

static const int sDAT_02062560[] = {
	0x0,
	0xFFFFFF80,
	0x80
};

static const struct {
	s16  x;
	s16  y;
} sDAT_020625A0[] = {
	{ 0x6A, 0x248 },
	{ 0x6A, 0x242 },
	{ 0x6A, 0x23E },
	{ 0x69, 0x23A },
	{ 0x69, 0x238 },
	{ 0x69, 0x236 },
	{ 0x69, 0x234 },
	{ 0x69, 0x232 },
	{ 0x69, 0x230 },
	{ 0x68, 0x22E },
	{ 0x68, 0x22C },
	{ 0x68, 0x22A },
	{ 0x68, 0x228 },
	{ 0x67, 0x226 },
	{ 0x67, 0x224 },
	{ 0x67, 0x223 },
	{ 0x66, 0x221 },
	{ 0x66, 0x21F },
	{ 0x65, 0x21D },
	{ 0x65, 0x21B },
	{ 0x64, 0x219 },
	{ 0x64, 0x217 },
	{ 0x63, 0x216 },
	{ 0x62, 0x214 },
	{ 0x62, 0x212 },
	{ 0x61, 0x210 },
	{ 0x60, 0x20E },
	{ 0x60, 0x20D },
	{ 0x5F, 0x20B },
	{ 0x5E, 0x209 },
	{ 0x5E, 0x208 },
	{ 0x5D, 0x206 },
	{ 0x5C, 0x204 },
	{ 0x5B, 0x203 },
	{ 0x5A, 0x201 },
	{ 0x59, 0x1FF },
	{ 0x58, 0x1FE },
	{ 0x57, 0x1FC },
	{ 0x56, 0x1FB },
	{ 0x56, 0x1F9 },
	{ 0x55, 0x1F8 },
	{ 0x53, 0x1F6 },
	{ 0x52, 0x1F5 },
	{ 0x51, 0x1F3 },
	{ 0x50, 0x1F2 },
	{ 0x4F, 0x1F0 },
	{ 0x4E, 0x1EF },
	{ 0x4D, 0x1EE },
	{ 0x4C, 0x1EC },
	{ 0x4B, 0x1EB },
	{ 0x49, 0x1EA },
	{ 0x48, 0x1E8 },
	{ 0x47, 0x1E7 },
	{ 0x46, 0x1E6 },
	{ 0x44, 0x1E5 },
	{ 0x43, 0x1E4 },
	{ 0x42, 0x1E3 },
	{ 0x41, 0x1E2 },
	{ 0x3F, 0x1E1 },
	{ 0x3E, 0x1DF },
	{ 0x3D, 0x1DF },
	{ 0x3B, 0x1DE },
	{ 0x3A, 0x1DD },
	{ 0x38, 0x1DC },
	{ 0x37, 0x1DB },
	{ 0x36, 0x1DA },
	{ 0x34, 0x1D9 },
	{ 0x33, 0x1D8 },
	{ 0x31, 0x1D8 },
	{ 0x30, 0x1D7 },
	{ 0x2E, 0x1D6 },
	{ 0x2D, 0x1D6 },
	{ 0x2B, 0x1D5 },
	{ 0x2A, 0x1D4 },
	{ 0x28, 0x1D4 },
	{ 0x27, 0x1D3 },
	{ 0x25, 0x1D3 },
	{ 0x24, 0x1D2 },
	{ 0x22, 0x1D2 },
	{ 0x21, 0x1D2 },
	{ 0x1F, 0x1D1 },
	{ 0x1E, 0x1D1 },
	{ 0x1C, 0x1D1 },
	{ 0x1A, 0x1D0 },
	{ 0x19, 0x1D0 },
	{ 0x17, 0x1D0 },
	{ 0x16, 0x1D0 },
	{ 0x14, 0x1D0 },
	{ 0x13, 0x1D0 },
	{ 0x11, 0x1D0 },
	{ 0x10, 0x1D0 },
	{ 0x10, 0x1CE },
	{ 0x10, 0x1CC },
	{ 0x10, 0x1CA },
	{ 0x10, 0x1C8 },
	{ 0x10, 0x1C6 },
	{ 0x10, 0x1C4 },
	{ 0x10, 0x1C2 },
	{ 0x10, 0x1C0 },
	{ 0x10, 0x1BF },
	{ 0x10, 0x1BD },
	{ 0x10, 0x1BC },
	{ 0x10, 0x1BA },
	{ 0x10, 0x1B9 },
	{ 0x10, 0x1B7 },
	{ 0x10, 0x1B6 },
	{ 0x10, 0x1B4 },
	{ 0x10, 0x1B3 },
	{ 0x10, 0x1B1 },
	{ 0x10, 0x1B0 },
	{ 0x10, 0x1AE },
	{ 0x10, 0x1AD },
	{ 0x10, 0x1AB },
	{ 0x10, 0x1AA },
	{ 0x10, 0x1A8 },
	{ 0x10, 0x1A7 },
	{ 0x10, 0x1A6 },
	{ 0x10, 0x1A4 },
	{ 0x10, 0x1A2 },
	{ 0x10, 0x1A0 },
	{ 0x10, 0x19E },
	{ 0x10, 0x19C },
	{ 0x10, 0x19A },
	{ 0x10, 0x198 },
	{ 0x10, 0x196 },
	{ 0x10, 0x194 },
	{ 0x10, 0x192 },
	{ 0x10, 0x190 },
	{ 0x10, 0x18E },
	{ 0x10, 0x18C },
	{ 0x10, 0x18A },
	{ 0x10, 0x188 },
	{ 0x10, 0x186 },
	{ 0x10, 0x184 },
	{ 0x10, 0x182 },
	{ 0x10, 0x180 },
	{ 0x10, 0x17E },
	{ 0x10, 0x17C },
	{ 0x10, 0x17A },
	{ 0x10, 0x178 },
	{ 0x10, 0x176 },
	{ 0x10, 0x174 },
	{ 0x10, 0x172 },
	{ 0x10, 0x170 }
};

static const u16 sDAT_02062578[] = {
	0x8F,
	0x7F,
	0x6A,
	0x4A,
	0x33,
	0x1F,
	0xC,
	0x0
};


BOOL BoxTouchSelector_0203643C(BoxTouchSelectorManager** param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4) {
	if (BoxApp_GetBoxMode(param2) == PC_MODE_COMPARE) {
		*param0 = NULL;
		return TRUE;
	} else {
		BoxTouchSelectorManager* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTouchSelectorManager));
		if (v0 != NULL) {
			v0->unk_08 = param3;
			v0->unk_00 = param1;
			v0->unk_04 = param2;
			v0->unk_0C = param4;
			v0->unk_14 = NULL;
			v0->unk_84 = String_Init(32, HEAP_ID_BOX_GRAPHICS);
			v0->unk_10 = param2->touchDialHelper.selectedTouchDial;
			v0->unk_98 = NULL;
			v0->unk_A0 = NULL;
			v0->unk_A8 = NULL;
			v0->unk_B0 = NULL;
			v0->unk_2C = NULL;
			v0->unk_30 = NULL;
			
			int i;
			for (i = 0; i < 8; i++) {
				v0->unk_34[i] = NULL;
			}
			
			for (i = 0; i < 10; i++) {
				v0->unk_54[i] = NULL;
			}
			
			v0->unk_80 = 0;
			BoxTouchSelector_020364D8(v0);
			*param0 = v0;
			
			return TRUE;
		}
		
		return FALSE;
	}
}


static void BoxTouchSelector_020364D8(BoxTouchSelectorManager* param0) {
	fx32 v0 = 0;
	fx32 v1 = (90 << FX32_SHIFT) / 16;
	
	param0->unk_D4[0] = 0;
	param0->unk_E4[0] = 16;
	
	int i;
	for (i = 1; i < (16 - 1); i++) {
		v0 += v1;
		param0->unk_D4[i] = (gFxSinCosTable[v0 >> FX32_SHIFT] * 16) >> FX32_SHIFT;
		param0->unk_E4[i] = 16 - param0->unk_D4[i];
	}
	
	param0->unk_D4[i] = 16;
	param0->unk_E4[i] = 0;
}


void BoxTouchSelector_02036534(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		if (param0->unk_88.pixels != NULL) {
			Window_Remove(&(param0->unk_88));
		}
		
		if (param0->unk_98 != NULL) {
			Heap_Free(param0->unk_98);
		}
		
		if (param0->unk_A0 != NULL) {
			Heap_Free(param0->unk_A0);
		}
		
		if (param0->unk_A8 != NULL) {
			Heap_Free(param0->unk_A8);
		}
		
		if (param0->unk_B0 != NULL) {
			Heap_Free(param0->unk_B0);
		}
		
		if (param0->unk_2C != NULL) {
			Sprite_Delete(param0->unk_2C);
		}
		
		if (param0->unk_30 != NULL) {
			Sprite_Delete(param0->unk_30);
		}
		
		if (param0->unk_84 != NULL) {
			String_Free(param0->unk_84);
		}
		
		int i;
		for (i = 0; i < 8; i++) {
			if (param0->unk_34[i]) {
				Sprite_Delete(param0->unk_34[i]);
			}
		}
		
		for (i = 0; i < 10; i++) {
			if (param0->unk_54[i]) {
				Sprite_Delete(param0->unk_54[i]);
			}
		}
		
		Heap_Free(param0);
	}
}


void BoxTouchSelector_020365D8(BoxTouchSelectorManager* param0) {
	// TODO naix
	if (param0) {
		if (gIsDiamondPearl) {
			NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
			Graphics_LoadTilesToBgLayer(narc, 103, param0->unk_08, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 100, param0->unk_08, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 101, param0->unk_08, 6, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 102, param0->unk_08, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
		} else {
			NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
			Graphics_LoadTilesToBgLayer(narc, 127, param0->unk_08, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 124, param0->unk_08, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 125, param0->unk_08, 6, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 126, param0->unk_08, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
		}
		
		Bg_FillTilesRange(param0->unk_08, 4, 0x0, 1, 0);
		Bg_FillTilemapRect(param0->unk_08, 4, 0x0, 0, 0, 32, 32, 17);
		Bg_CopyTilemapBufferToVRAM(param0->unk_08, 4);
		Bg_SetOffset(param0->unk_08, BG_LAYER_SUB_1, 0, sDAT_0206256C[param0->unk_10]);
		Window_Add(param0->unk_08, &(param0->unk_88), 4, 0, 0, 32, 24, 2, 0);
		Window_FillTilemap(&(param0->unk_88), 0x0);
		Window_CopyToVRAM(&param0->unk_88);
		BoxTouchSelector_02037954(param0);
		
		if (gIsDiamondPearl) {
			NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
			Graphics_LoadPalette(narc, 104, 4, 0, 0x20 * 3, HEAP_ID_BOX_GRAPHICS);
		} else {
			NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
			Graphics_LoadPalette(narc, 128, 4, 0, 0x20 * 3, HEAP_ID_BOX_GRAPHICS);
		}
		BoxTouchSelector_02036778(param0);
	}
}


static void BoxTouchSelector_02036778(BoxTouchSelectorManager* param0) {
	SpriteResourcesHeader v0;
	NNSG2dImageProxy v1;
	
	BoxTouchSelector_02036838(param0);
	
	NNS_G2dInitImageProxy(&v1);
	
	// TODO naix
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		Graphics_LoadImageMapping(narc, 114, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0 * 0x20, 10, &(v1));
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		Graphics_LoadImageMapping(narc, 138, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0 * 0x20, 10, &(v1));
	}
	
	BoxGraphics_020308E8(&v0, &v1, &(param0->unk_18), param0->unk_9C, param0->unk_A4, 3);
	
	param0->unk_2C = BoxGraphics_02030908(param0->unk_0C, &v0, 0, 568, 49, NNS_G2D_VRAM_TYPE_2DSUB);
	param0->unk_30 = BoxGraphics_02030908(param0->unk_0C, &v0, 256, 568, 49, NNS_G2D_VRAM_TYPE_2DSUB);
	
	Sprite_SetFlipMode(param0->unk_30, 1);
	
	if (BoxApp_GetBoxMode(param0->unk_04) == PC_MODE_MOVE_ITEMS) {
		Sprite_SetAnim(param0->unk_30, 1);
	}
}


static void BoxTouchSelector_02036838(BoxTouchSelectorManager* param0) {
	// TODO naix
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		param0->unk_98 = Graphics_GetCellBank(narc, 115, 1, &(param0->unk_9C), HEAP_ID_BOX_GRAPHICS);
		param0->unk_A0 = Graphics_GetAnimBank(narc, 116, 1, &(param0->unk_A4), HEAP_ID_BOX_GRAPHICS);
		param0->unk_A8 = Graphics_GetCellBank(narc, 118, 1, &(param0->unk_AC), HEAP_ID_BOX_GRAPHICS);
		param0->unk_B0 = Graphics_GetAnimBank(narc, 119, 1, &(param0->unk_B4), HEAP_ID_BOX_GRAPHICS);
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		param0->unk_98 = Graphics_GetCellBank(narc, 139, 1, &(param0->unk_9C), HEAP_ID_BOX_GRAPHICS);
		param0->unk_A0 = Graphics_GetAnimBank(narc, 140, 1, &(param0->unk_A4), HEAP_ID_BOX_GRAPHICS);
		param0->unk_A8 = Graphics_GetCellBank(narc, 142, 1, &(param0->unk_AC), HEAP_ID_BOX_GRAPHICS);
		param0->unk_B0 = Graphics_GetAnimBank(narc, 143, 1, &(param0->unk_B4), HEAP_ID_BOX_GRAPHICS);
	}
	
	NNS_G2dInitImagePaletteProxy(&(param0->unk_18));
	
	// TODO naix
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		Graphics_LoadPartialPalette(narc, 120, NNS_G2D_VRAM_TYPE_2DSUB, 0, 10, &(param0->unk_18));
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		Graphics_LoadPartialPalette(narc, 144, NNS_G2D_VRAM_TYPE_2DSUB, 0, 10, &(param0->unk_18));
	}
}


static SysTask* BoxTouchSelector_02036948(BoxTouchSelectorManager* param0, SysTaskFunc param1, void* param2) {
	param0->unk_14 = BoxGraphics_02030870(param1, param2, 0);
	return param0->unk_14;
}


static void BoxTouchSelector_0203695C(BoxTouchSelectorManager* param0) {
	SysTask_Done(param0->unk_14);
	param0->unk_14 = NULL;
}


static BOOL BoxTouchSelector_0203696C(BoxTouchSelectorManager* param0) {
	return (param0->unk_14 == NULL);
}


void BoxTouchSelector_0203697C(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		u32 selectedTouchDialID = param0->unk_04->touchDialHelper.selectedTouchDial;
		
		if (param0->unk_10 != selectedTouchDialID) {
			SysTaskFunc v1;
			BoxTouchSelector_UnkStruct0* v2 = &(param0->unk_B8);
			
			v2->unk_00 = param0;
			v2->unk_08 = NULL;
			v2->unk_04 = 0;
			
			switch (selectedTouchDialID) {
				case TOUCH_DIALS_NONE:
					v2->unk_0C = 8;
					v1 = BoxTouchSelector_02036C74;
					BoxTouchSelector_02036F9C(param0);
					BoxTouchSelector_020370AC(param0);
					BoxTouchSelector_02037954(param0);
					break;
				
				case TOUCH_DIALS_BOX_JUMP:
					v2->unk_0C = 8;
					
					if (param0->unk_10 == 0) {
						v1 = BoxTouchSelector_02036A70;
					} else {
						v1 = BoxTouchSelector_02036B48;
						v2->unk_0C += 8;
					}
					
					BoxTouchSelector_020370AC(param0);
					v2->unk_08 = param0->unk_2C;
					break;
				
				case TOUCH_DIALS_MARKINGS:
					v2->unk_0C = 8;
					
					if (param0->unk_10 == 0) {
						v1 = BoxTouchSelector_02036A70;
					} else {
						v1 = BoxTouchSelector_02036B48;
						v2->unk_0C += 8;
					}
					
					v2->unk_08 = param0->unk_30;
					BoxTouchSelector_02036F9C(param0);
					BoxTouchSelector_02037954(param0);
					break;
				
				default:
					Heap_Free(v2);
					return;
			}
			
			param0->unk_10 = selectedTouchDialID;
			v2->unk_10 = (Bg_GetXOffset(param0->unk_08, 5) * FX32_ONE);
			v2->unk_14 = (sDAT_0206256C[selectedTouchDialID] - v2->unk_10) / v2->unk_0C;
			v2->unk_18 = sDAT_0206256C[selectedTouchDialID] >> FX32_SHIFT;
			
			if (v2->unk_08 != NULL) {
				Sprite_SetAnim(v2->unk_08, 1);
			}
			
			BoxTouchSelector_02036948(param0, v1, v2);
		}
	}
}


BOOL BoxTouchSelector_02036A60(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		return BoxTouchSelector_0203696C(param0);
	} else {
		return TRUE;
	}
}


static void BoxTouchSelector_02036A70(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct0* v0 = (BoxTouchSelector_UnkStruct0*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_04) {
		case 0:
			if (Sprite_IsAnimated(v0->unk_08) == 0) {
				v0->unk_04++;
			}
			break;
		
		case 1:
			if (v0->unk_0C) {
				v0->unk_10 += v0->unk_14;
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, (v0->unk_10 >> FX32_SHIFT));
				v0->unk_0C--;
			} else {
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, v0->unk_18);
				G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 0, 16);
				v0->unk_04++;
			}
			break;
		
		case 2:
			Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_2, 0, sDAT_02062560[v1->unk_10]);
			v0->unk_0E = 0;
			v0->unk_04++;
			break;
		
		case 3:
			G2S_ChangeBlendAlpha(v1->unk_D4[v0->unk_0E], v1->unk_E4[v0->unk_0E]);
			v0->unk_0E++;
			if (v0->unk_0E >= 16) {
				Sprite_SetAnim(v0->unk_08, 0);
				BoxTouchSelector_0203695C(v1);
			}
			break;
	}
}


static void BoxTouchSelector_02036B48(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct0* v0 = (BoxTouchSelector_UnkStruct0*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_04) {
		case 0:
			if (Sprite_IsAnimated(v0->unk_08) == 0) {
				G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 0);
				v0->unk_0E = (16 - 1);
				v0->unk_04++;
			}
			break;
		
		case 1:
			v0->unk_0E--;
			G2S_ChangeBlendAlpha(v1->unk_D4[v0->unk_0E], v1->unk_E4[v0->unk_0E]);

			if (v0->unk_0E == 0) {
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_2, 0, sDAT_02062560[v1->unk_10]);
				v0->unk_04++;
			}
			break;
		
		case 2:
			if (v0->unk_0C) {
				v0->unk_10 += v0->unk_14;
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, (v0->unk_10 >> FX32_SHIFT));
				v0->unk_0C--;
			} else {
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, v0->unk_18);
				G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 0, 16);
				v0->unk_04++;
			}
			break;
		
		case 3:
			Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_2, 0, sDAT_02062560[v1->unk_10]);
			v0->unk_0E = 0;
			v0->unk_04++;
			break;
		
		case 4:
			G2S_ChangeBlendAlpha(v1->unk_D4[v0->unk_0E], v1->unk_E4[v0->unk_0E]);

			if (++(v0->unk_0E) >= 16) {
				Sprite_SetAnim(v0->unk_08, 0);
				BoxTouchSelector_0203695C(v1);
			}
			break;
	}
}


static void BoxTouchSelector_02036C74(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct0* v0 = (BoxTouchSelector_UnkStruct0*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_04) {
		case 0:
			v0->unk_0E = (16 - 1);
			v0->unk_04++;
			// Fall through
		
		case 1:
			v0->unk_0E--;
			G2S_ChangeBlendAlpha(v1->unk_D4[v0->unk_0E], v1->unk_E4[v0->unk_0E]);

			if (v0->unk_0E == 0) {
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_2, 0, sDAT_02062560[0]);
				v0->unk_04++;
			}
			break;
		
		case 2:
			if (v0->unk_0C) {
				v0->unk_10 += v0->unk_14;
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, (v0->unk_10 >> FX32_SHIFT));
				v0->unk_0C--;
			} else {
				Bg_SetOffset(v1->unk_08, BG_LAYER_SUB_1, 0, v0->unk_18);
				v0->unk_04++;
			}
			break;
		
		case 3:
			BoxTouchSelector_0203695C(v1);
			break;
	}
}


void BoxTouchSelector_02036D18(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		switch (param0->unk_10) {
			case 1:
				BoxTouchSelector_02036E58(param0);
				BoxTouchSelector_020373B0(param0);
				break;
			
			case 2:
				BoxTouchSelector_02036FE0(param0);
				BoxTouchSelector_020376D0(param0);
				break;
		}
	}
}


BOOL BoxTouchSelector_02036D44(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		if (BoxTouchSelector_0203696C(param0)) {
			if (param0->unk_10 == 1) {
				BoxTouchSelector_02037980(param0);
			}
			
			return TRUE;
		}
		
		return FALSE;
	}
	
	return TRUE;
}


void BoxTouchSelector_02036D6C(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		switch (param0->unk_10) {
			case 1:
				BoxTouchSelector_02037954(param0);
				BoxTouchSelector_02036EFC(param0);
				BoxTouchSelector_020375D8(param0);
				break;
			
			case 2:
				BoxTouchSelector_020378D4(param0);
				break;
		}
	}
}


static void BoxTouchSelector_02036D98(BoxTouchSelectorManager* param0) {
	int v0, v1, v2, v3;
	
	v3 = BoxApp_GetTouchDialScrollDelta(param0->unk_04);
	
	if (v3 > 0) {
		v0 = param0->unk_80 + (6 - 1);
		
		if (v0 >= 10) {
			v0 -= 10;
		}
		
		v1 = -1;
		
		param0->unk_80 -= v3;
		
		if (param0->unk_80 < 0) {
			param0->unk_80 += 10;
		}
	} else {
		v0 = param0->unk_80;
		v1 = 1;
		
		param0->unk_80 -= v3;
		
		if (param0->unk_80 >= 10) {
			param0->unk_80 -= 10;
		}
	}
}


static void BoxTouchSelector_02036DEC(BoxTouchSelectorManager* param0) {
	int v0 = BoxApp_GetTouchDialScrollDelta(param0->unk_04);
	param0->unk_80 += v0;
	
	if (param0->unk_80 < 0) {
		param0->unk_80 += 8;
	} else if (param0->unk_80 >= 8) {
		param0->unk_80 -= 8;
	}
}


BOOL BoxTouchSelector_02036E24(BoxTouchSelectorManager* param0) {
	if (param0 != NULL) {
		if (BoxTouchSelector_0203696C(param0)) {
			if (param0->unk_10 == 1) {
				BoxTouchSelector_02037980(param0);
				BoxTouchSelector_02036D98(param0);
			} else {
				BoxTouchSelector_02036DEC(param0);
			}
			
			return TRUE;
		}
		
		return FALSE;
	}
	
	return TRUE;
}


static void BoxTouchSelector_02036E58(BoxTouchSelectorManager* param0) {
	SpriteResourcesHeader v0;
	BoxAltWindowManager* v1;
	NNSG2dImageProxy v2;
	int v3, v4;
	
	v1 = BoxGraphics_020308B8(param0->unk_00);
	v4 = BoxApp_GetTouchDialOffset(param0->unk_04) + 3;
	
	if (v4 >= 18) {
		v4 -= 18;
	}
	
	for (v3 = 0; v3 < 10; v3++) {
		NNS_G2dInitImageProxy(&v2);
		BoxAltWindow_0203501C(v1, v4, 192 + v3 * 32, NNS_G2D_VRAM_TYPE_2DSUB, &v2);
		BoxGraphics_020308E8(&v0, &v2, &(param0->unk_18), BoxAltWindow_020351F8(v1), NULL, 2);
		
		param0->unk_54[v3] = BoxGraphics_02030908(param0->unk_0C, &v0, 16, 368, 48, NNS_G2D_VRAM_TYPE_2DSUB);
		
		GF_ASSERT(param0->unk_54[v3] != NULL);
		
		v4--;
		if (v4 < 0) {
			v4 = 18 - 1;
		}
	}
	
	param0->unk_80 = 0;
}


static void BoxTouchSelector_02036EFC(BoxTouchSelectorManager* param0) {
	SpriteResourcesHeader v0;
	BoxAltWindowManager* v1;
	int v2, v3, v4, v5, v6, v7, v8;
	
	v1 = BoxGraphics_020308B8(param0->unk_00);
	v4 = BoxApp_GetTouchDialOffset(param0->unk_04);
	
	if (param0->unk_04->touchDialHelper.scrollDelta > 0) {
		v4 = (v4 - param0->unk_04->touchDialHelper.scrollDelta) + (3 + 1);
		if (v4 >= 18) {
			v4 -= 18;
		}
		
		v5 = 1;
		
		v8 = param0->unk_80 - 1;
		if (v8 < 0) {
			v8 += 10;
		}
		
		v3 = param0->unk_04->touchDialHelper.scrollDelta;
		v6 = 16;
		v7 = 368;
	} else {
		v4 = (v4 - param0->unk_04->touchDialHelper.scrollDelta) - (2 + 1);
		if (v4 < 0) {
			v4 += 18;
		}
		
		v5 = -1;
		
		v8 = param0->unk_80 + 6;
		if (v8 >= 10) {
			v8 -= 10;
		}
		
		v3 = param0->unk_04->touchDialHelper.scrollDelta * -1;
		v6 = 106;
		v7 = 592;
	}
	
	param0->unk_7C = v3;
	
	for (v2 = 0; v2 < v3; v2++) {
		BoxAltWindow_0203501C(v1, v4, 192 + v8 * 32, NNS_G2D_VRAM_TYPE_2DSUB, NULL);
		
		v4 += v5;
		if (v4 < 0) {
			v4 = 18 - 1;
		} else if (v4 >= 18) {
			v4 = 0;
		}
		
		v8 -= v5;
		if (v8 < 0) {
			v8 = (10 - 1);
		} else if (v8 >= 10) {
			v8 = 0;
		}
	}
}


static void BoxTouchSelector_02036F9C(BoxTouchSelectorManager* param0) {
	for (int i = 0; i < 10; i++) {
		if (param0->unk_54[i]) {
			Sprite_Delete(param0->unk_54[i]);
			param0->unk_54[i] = NULL;
		}
	}
}


static u32 BoxTouchSelector_02036FBC(u32 param0, u32 param1) {
	if (param0 == 0) {
		return (param1 == 0) ? 1 : 0;
	}
	
	if (param1 & (1 << (param0 - 1))) {
		return param0 * 2 + 1;
	} else {
		return param0 * 2;
	}
}


static void BoxTouchSelector_02036FE0(BoxTouchSelectorManager* param0) {
	SpriteResourcesHeader v0;
	NNSG2dImageProxy v1;
	
	u32 v2 = BoxApp_GetMonSpriteTransparencyMask(param0->unk_04);
	
	NNS_G2dInitImageProxy(&v1);
	
	// TODO naix
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		Graphics_LoadImageMapping(narc, 117, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 192 * 0x20, 10, &v1);
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		Graphics_LoadImageMapping(narc, 141, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 192 * 0x20, 10, &v1);
	}
	
	for (int i = 0; i < 8; i++) {
		BoxGraphics_020308E8(&v0, &v1, &(param0->unk_18), param0->unk_AC, param0->unk_B4, 2);

		param0->unk_34[i] = BoxGraphics_02030908(param0->unk_0C, &v0, 240, 368, 48, NNS_G2D_VRAM_TYPE_2DSUB);
		GF_ASSERT(param0->unk_34[i] != NULL);

		Sprite_SetAnim(param0->unk_34[i], BoxTouchSelector_02036FBC(i, v2));
	}
	
	param0->unk_80 = BoxApp_GetMarkingsButtonsScrollOffset(param0->unk_04);
}


static void BoxTouchSelector_020370AC(BoxTouchSelectorManager* param0) {
	for (int i = 0; i < 8; i++) {
		if (param0->unk_34[i]) {
			Sprite_Delete(param0->unk_34[i]);
			param0->unk_34[i] = NULL;
		}
	}
}


static void BoxTouchSelector_020370CC(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct1* v0 = (BoxTouchSelector_UnkStruct1*)param1;
	VecFx32 v1;
	
	if (v0->unk_16) {
		v0->unk_16--;
		return;
	}
	
	if (v0->unk_14) {
		v0->unk_14--;
		v0->unk_08 += v0->unk_10;
		
		u32 v2 = v0->unk_08 >> FX32_SHIFT;
		v1.x = sDAT_020625A0[v2].x << FX32_SHIFT;
		v1.y = sDAT_020625A0[v2].y << FX32_SHIFT;
		v1.z = 0;
		Sprite_SetPosition(v0->unk_04, &v1);
	} else {
		u32 v3 = v0->unk_0C >> FX32_SHIFT;
		v1.x = sDAT_020625A0[v3].x << FX32_SHIFT;
		v1.y = sDAT_020625A0[v3].y << FX32_SHIFT;
		v1.z = 0;
		
		Sprite_SetPosition(v0->unk_04, &v1);
		BoxTouchSelector_02037200(param0, v0);
	}
}


static void BoxTouchSelector_02037150(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct1* v0 = (BoxTouchSelector_UnkStruct1*)param1;
	VecFx32 v1;
	
	if (v0->unk_16) {
		v0->unk_16--;
		return;
	}
	
	if (v0->unk_14) {
		v0->unk_14--;
		v0->unk_08 += v0->unk_10;
		
		u32 v2 = v0->unk_08 >> FX32_SHIFT;
		v1.x = (256 - sDAT_020625A0[v2].x) << FX32_SHIFT;
		v1.y = sDAT_020625A0[v2].y << FX32_SHIFT;
		v1.z = 0;
		Sprite_SetPosition(v0->unk_04, &v1);
	} else {
		u32 v3 = v0->unk_0C >> FX32_SHIFT;
		v1.x = (256 - sDAT_020625A0[v3].x) << FX32_SHIFT;
		v1.y = sDAT_020625A0[v3].y << FX32_SHIFT;
		v1.z = 0;
		Sprite_SetPosition(v0->unk_04, &v1);
		
		if (v0->unk_18) {
			v0->unk_14 = v0->unk_28;
			v0->unk_08 = v0->unk_1C;
			v0->unk_10 = v0->unk_24;
			v0->unk_0C = v0->unk_20;
			v0->unk_16 = v0->unk_2A;
			v0->unk_18 = 0;
		} else {
			BoxTouchSelector_02037200(param0, v0);
		}
	}
}


static void BoxTouchSelector_02037200(SysTask* param0, BoxTouchSelector_UnkStruct1* param1) {
	BoxTouchSelector_UnkStruct2* v0 = param1->unk_00;
	
	GF_ASSERT(v0->unk_1CB);
	v0->unk_1CB--;
	SysTask_Done(param0);
}


static void BoxTouchSelector_02037228(BoxTouchSelector_UnkStruct2* param0, Sprite* param1, BoxTouchSelector_UnkStruct1* param2, u32 param3, u32 param4, u32 param5, u32 param6, BOOL param7) {
	GF_ASSERT(param1 != NULL);
	
	if (param6) {
		param6--;
	}
	
	param2->unk_00 = param0;
	param2->unk_04 = param1;
	param2->unk_14 = param5;
	param2->unk_16 = param6;
	
	SysTaskFunc v0 = (param7) ? BoxTouchSelector_02037150 : BoxTouchSelector_020370CC;
	
	param2->unk_08 = sDAT_02062578[param3] << FX32_SHIFT;
	param2->unk_0C = sDAT_02062578[param4] << FX32_SHIFT;
	param2->unk_10 = (param2->unk_0C - param2->unk_08) / (s32)param5;
	param2->unk_18 = 0;
	
	if (BoxGraphics_02030870(v0, param2, 0) != NULL) {
		param0->unk_1CB++;
	}
}


static void BoxTouchSelector_020372A4(BoxTouchSelector_UnkStruct2* param0, BoxTouchSelector_UnkStruct1* param1, u32 param2, u32 param3, u32 param4) {
	param1->unk_18 = 1;
	param1->unk_2A = param2;
	
	if (param2) {
		param1->unk_2A--;
	}
	
	int v0 = param4 - param3;
	if (v0 < 0) {
		v0 *= -1;
	}
	
	v0 *= 6;
	
	param1->unk_28 = v0;
	param1->unk_1C = sDAT_02062578[param3] << FX32_SHIFT;
	param1->unk_20 = sDAT_02062578[param4] << FX32_SHIFT;
	param1->unk_24 = (param1->unk_20 - param1->unk_1C) / (s32)v0;
}


static void BoxTouchSelector_020372F0(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct2* v0 = (BoxTouchSelector_UnkStruct2*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_1CC) {
		case 0:
			v0->unk_1CA++;
			int v2 = 6 - v0->unk_1CA;
			BoxTouchSelector_02037228(v0, v1->unk_54[v2], &v0->unk_08[v2], 0, 7 - v0->unk_1CA, (v2 + 1) * 6, 0, 0);
			v0->unk_1CD = 6 - (1 * 2);
			
			if (v0->unk_1CA >= 6) {
				v0->unk_1CC = 2;
			} else {
				if (v0->unk_1CD) {
					v0->unk_1CC = 1;
				}
			}
			break;
		
		case 1:
			v0->unk_1CD -= 2;
			if (v0->unk_1CD == 0) {
				v0->unk_1CC = 0;
			}
			break;
		
		case 2:
			if (v0->unk_1CB == 0) {
				v0->unk_1CC++;
				BoxGraphics_02030A64(v0, BoxTouchSelector_020373A8, v1);
			}
			break;
	}
}


static void BoxTouchSelector_020373A8(void* param0) {
	BoxTouchSelectorManager* v0 = param0;
	BoxTouchSelector_0203695C(v0);
}


static void BoxTouchSelector_020373B0(BoxTouchSelectorManager* param0) {
	BoxTouchSelector_UnkStruct2* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTouchSelector_UnkStruct2));
	if (v0 != NULL) {
		v0->unk_00 = param0;
		v0->unk_1CC = 0;
		v0->unk_1CA = 0;
		v0->unk_1CB = 0;
		
		for (int i = 0; i < 8; i++) {
			v0->unk_1C0[i] = 0;
		}
		
		if (BoxTouchSelector_02036948(param0, BoxTouchSelector_020372F0, v0) == NULL) {
			Heap_Free(v0);
		}
	}
}


static void BoxTouchSelector_02037400(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct2* v0 = (BoxTouchSelector_UnkStruct2*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_1CC) {
		case 0:
			{
				int v2, v3, v4, v5, v6, v7, v8;
				if (v1->unk_04->touchDialHelper.scrollDelta > 0) {
					v4 = v1->unk_80 + 6 - 1;
					if (v4 >= 10) {
						v4 -= 10;
					}
					
					v5 = -1;
					v7 = 6;
					v8 = 7;
				} else {
					v4 = v1->unk_80;
					v5 = 1;
					v7 = 1;
					v8 = 0;
				}
				
				for (v2 = 0; v2 < v1->unk_7C; v2++) {
					BoxTouchSelector_02037228(v0, v0->unk_04[v4], &v0->unk_08[v4], v7, v8, (v2 + 1) * 6, 0, 0);
					
					v7 += v5;
					v4 += v5;
					if (v4 >= 10) {
						v4 = 0;
					} else if (v4 < 0) {
						v4 = 10 - 1;
					}
				}
				
				v3 = (6 - v1->unk_7C);
				
				for (v2 = 0; v2 < v3; v2++) {
					v8 = v7 - (v5 * v1->unk_7C);
					
					BoxTouchSelector_02037228(v0, v0->unk_04[v4], &v0->unk_08[v4], v7, v8, v1->unk_7C * 6, 0, 0);
					
					v7 += v5;
					v4 += v5;
					if (v4 >= 10) {
						v4 = 0;
					} else if (v4 < 0) {
						v4 = 10 - 1;
					}
				}
				
				if (v1->unk_04->touchDialHelper.scrollDelta > 0) {
					v7 = 0;
					v8 = v1->unk_7C;
				} else {
					v7 = 7;
					v8 = v7 - v1->unk_7C;
				}
				
				v3 = v1->unk_7C;
				
				for (v2 = 0; v2 < v3; v2++) {
					BoxTouchSelector_02037228(v0, v0->unk_04[v4], &v0->unk_08[v4], v7, v8, (v1->unk_7C - v2) * 6, (v2 + 1) * 6, 0);
					
					v8 += v5;
					v4 += v5;
					if (v4 >= 10) {
						v4 -= 10;
					} else if (v4 < 0) {
						v4 += 10;
					}
				}
				
				v0->unk_1CC++;
			}
			break;
		
		case 1:
			if (v0->unk_1CB == 0) {
				v0->unk_1CC++;
				BoxGraphics_02030A64(v0, BoxTouchSelector_020373A8, v1);
			}
			break;
	}
}


static void BoxTouchSelector_020375D8(BoxTouchSelectorManager* param0) {
	BoxTouchSelector_UnkStruct2* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTouchSelector_UnkStruct2));
	if (v0 != NULL) {
		v0->unk_00 = param0;
		v0->unk_04 = param0->unk_54;
		v0->unk_1CC = 0;
		v0->unk_1CA = 0;
		v0->unk_1CB = 0;
		
		for (int i = 0; i < 10; i++) {
			v0->unk_1C0[i] = 0;
		}
		
		if (BoxTouchSelector_02036948(param0, BoxTouchSelector_02037400, v0) == NULL) {
			Heap_Free(v0);
		}
	}
}


static void BoxTouchSelector_0203762C(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct2* v0 = (BoxTouchSelector_UnkStruct2*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_1CC) {
		case 0:
			{
				int v2, v3, v4;
				v4 = 6;
				v3 = v1->unk_80;
				
				for (v2 = 0; v2 < 6; v2++) {
					BoxTouchSelector_02037228(v0, v1->unk_34[v3], &(v0->unk_08[v3]), 0, v4, 6 * v4, 6 * v2, 1);
					v4--;
					v3++;
					if (v3 >= 8) {
						v3 = 0;
					}
				}
				
				v0->unk_1CC++;
			}
			break;
		
		case 1:
			if (v0->unk_1CB == 0) {
				v0->unk_1CC++;
				BoxGraphics_02030A64(v0, BoxTouchSelector_020373A8, v1);
			}
			break;
	}
}


static void BoxTouchSelector_020376D0(BoxTouchSelectorManager* param0) {
	BoxTouchSelector_UnkStruct2* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTouchSelector_UnkStruct2));
	if (v0 != NULL) {
		SysTask* v2;
		
		v0->unk_00 = param0;
		v0->unk_1CC = 0;
		v0->unk_1CA = 0;
		v0->unk_1CB = 0;
		
		for (int i = 0; i < 8; i++) {
			v0->unk_1C0[i] = 0;
		}
		
		if (BoxTouchSelector_02036948(param0, BoxTouchSelector_0203762C, v0) == NULL) {
			Heap_Free(v0);
		}
	}
}


static void BoxTouchSelector_02037720(SysTask* param0, void* param1) {
	BoxTouchSelector_UnkStruct2* v0 = (BoxTouchSelector_UnkStruct2*)param1;
	BoxTouchSelectorManager* v1 = v0->unk_00;
	
	switch (v0->unk_1CC) {
		case 0:
			{
				int v2, v3, v4, v5, v6, v7, v8, v9, v10;
				v3 = BoxApp_GetTouchDialScrollDelta(v1->unk_04);
				v2 = v1->unk_80;
				
				if (v3 < 0) {
					v2 += (6 - 1);
					if (v2 >= 8) {
						v2 -= 8;
					}
					
					v4 = -1;
					v9 = -v3;
					v6 = 1;
					v7 = 0;
				} else {
					v4 = 1;
					v9 = v3;
					v6 = 7 - 1;
					v7 = 7;
				}
				
				v8 = 6 + v9;
				
				for (v5 = 0; v5 < v9; v5++) {
					BoxTouchSelector_02037228(v0, v1->unk_34[v2], &(v0->unk_08[v2]), v6, v7, 6 * (v5 + 1), 0, 1);
					
					v6 -= v4;
					v2 += v4;
					if (v2 < 0) {
						v2 += 8;
					} else if (v2 >= 8) {
						v2 -= 8;
					}
				}
				
				v10 = 0;
				
				for (; v5 < 8; v5++) {
					v7 -= v4;
					
					BoxTouchSelector_02037228(v0, v1->unk_34[v2], &(v0->unk_08[v2]), v6, v7, 6 * (v9), v10, 1);
					
					if ((v6 != 0) && (v6 != 7)) {
						v6 -= v4;
					} else {
						v10 += 6;
					}
					
					v2 += v4;
					if (v2 < 0) {
						v2 += 8;
					} else if (v2 >= 8) {
						v2 -= 8;
					}
				}
				
				if (v3 < 0) {
					v6 = 7;
					v7 = v6 - (v9 - 2);
				} else {
					v6 = 0;
					v7 = v6 + (v9 - 2);
				}
				
				for (; v5 < v8; v5++) {
					BoxTouchSelector_020372A4(v0, &(v0->unk_08[v2]), 6, v6, v7);
					
					v7 -= v4;
					v2 += v4;
					if (v2 < 0) {
						v2 += 8;
					} else if (v2 >= 8) {
						v2 -= 8;
					}
				}
				
				v0->unk_1CC++;
			}
			break;
		
		case 1:
			if (v0->unk_1CB == 0) {
				v0->unk_1CC++;
				BoxGraphics_02030A64(v0, BoxTouchSelector_020373A8, v1);
			}
			break;
	}
}


static void BoxTouchSelector_020378D4(BoxTouchSelectorManager* param0) {
	BoxTouchSelector_UnkStruct2* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTouchSelector_UnkStruct2));
	if (v0 != NULL) {
		v0->unk_00 = param0;
		v0->unk_1CC = 0;
		v0->unk_1CA = 0;
		v0->unk_1CB = 0;
		
		for (int i = 0; i < 8; i++) {
			v0->unk_1C0[i] = 0;
		}
		
		if (BoxTouchSelector_02036948(param0, BoxTouchSelector_02037720, v0) == NULL) {
			Heap_Free(v0);
		}
	}
}


void BoxTouchSelector_02037924(BoxTouchSelectorManager* param0) {
	if (param0 != NULL && param0->unk_34[0] != NULL) {
		u32 v1 = BoxApp_GetMonSpriteTransparencyMask(param0->unk_04);
		for (int i = 0; i < 8; i++) {
			Sprite_SetAnim(param0->unk_34[i], BoxTouchSelector_02036FBC(i, v1));
		}
	}
}


static void BoxTouchSelector_02037954(BoxTouchSelectorManager* param0) {
	Bg_ToggleLayer(BG_LAYER_SUB_0, FALSE);
}


static void BoxTouchSelector_02037960(BoxTouchSelectorManager* param0) {
	G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 14, 2);
	Bg_ToggleLayer(BG_LAYER_SUB_0, TRUE);
}


static void BoxTouchSelector_02037980(BoxTouchSelectorManager* param0) {
	static const struct {
		s16  unk_00;
		s16  unk_02;
	} v0[] = {
		{ 36,    4 },
		{ 36,   36 },
		{ 50,   69 },
		{ 92,   92 },
		{ 114, 124 },
		{ 124, 156 },
	};
	
	int v2 = BoxApp_GetTouchDialOffset(param0->unk_04) + 3;
	if (v2 >= MAX_PC_BOXES) {
		v2 -= MAX_PC_BOXES;
	}
	
	for (int i = 0; i < NELEMS(v0); i++) {
		PCBoxes_BufferBoxName(param0->unk_04->pcBoxes, v2, param0->unk_84);
		Window_FillRectWithColor(&param0->unk_88, 0, v0[i].unk_00, v0[i].unk_02, 104, 16);
		
		Text_AddPrinterWithParamsAndColor(
			&param0->unk_88,
			FONT_SYSTEM,
			param0->unk_84,
			v0[i].unk_00 + 4,
			v0[i].unk_02 + 2,
			TEXT_SPEED_NO_TRANSFER,
			TEXT_COLOR(1, 2, 0),
			NULL);
		
		v2--;
		if (v2 < 0) {
			v2 = MAX_PC_BOXES - 1;
		}
	}
	
	Window_LoadTiles(&param0->unk_88);
	BoxTouchSelector_02037960(param0);
}
