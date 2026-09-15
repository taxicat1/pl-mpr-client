// File not documented. This file controls rendering the box itself, including the wallpaper, name text, and
// animations when changing boxes or changing wallpapers.

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_02030AC4.h"

#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_02035AAC.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_def.h"

#include "assert.h"
#include "bg_window.h"
#include "font.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "text.h"

#include "constants/species.h"

#include "fs/local.h"

typedef struct {
	u8  nscr;
	u8  ncgr;
	u8  nclr;
} BoxWallpaperNarcID;

// TODO naix
static const BoxWallpaperNarcID sBoxWallpaperNarcIDs[] = {
	{ 30,  29,  28  },
	{ 33,  32,  31  },
	{ 36,  35,  34  },
	{ 39,  38,  37  },
	{ 42,  41,  40  },
	{ 45,  44,  43  },
	{ 48,  47,  46  },
	{ 51,  50,  49  },
	{ 54,  53,  52  },
	{ 57,  56,  55  },
	{ 60,  59,  58  },
	{ 63,  62,  61  },
	{ 66,  65,  64  },
	{ 69,  68,  67  },
	{ 72,  71,  70  },
	{ 75,  74,  73  },
	{ 78,  77,  76  },
	{ 81,  80,  79  },
	{ 84,  83,  82  },
	{ 87,  86,  85  },
	{ 90,  89,  88  },
	{ 93,  92,  91  },
	{ 96,  95,  94  },
	{ 99,  98,  97  },
	{ 102, 101, 100 },
	{ 105, 104, 103 },
	{ 108, 107, 106 },
	{ 111, 110, 109 },
	{ 114, 113, 112 },
	{ 117, 116, 115 },
	{ 120, 119, 118 },
	{ 123, 122, 121 },
};

typedef struct {
	BoxPanelManager* unk_00;
	s16 unk_04;
	s16 unk_06;
	s16 unk_08;
	s16 unk_0A;
	u32 unk_0C;
} BoxPanel_UnkStruct0;

static void BoxPanel_02030BC0(const BoxPanelManager* param0, int param1, u32* param2, u32* param3, u32* param4);
static void BoxPanel_02030C8C(BoxPanelManager* param0, const BoxCustomization* customization, int param2, BOOL param3);
static void BoxPanel_02030D78(BoxPanelManager* param0, const BoxCustomization* customization, int param2);
static void BoxPanel_02030ED8(BoxPanelManager* param0, const BoxCustomization* customization, u32 param2, u32 param3, u32 param4);
static void BoxPanel_02031034(SysTask* param0, void* param1);
static void BoxPanel_0203107C(SysTask* param0, void* param1);
static void BoxPanel_02031124(BoxPanelManager* param0, fx32 param1, s32 param2);
static void BoxPanel_020311AC(BoxPanelManager* param0, fx32 param1);
static void BoxPanel_0203124C(BoxPanelManager* param0, BoxIcon* param1, int param2, int param3);
static void BoxPanel_02031340(BoxPanelManager* param0, BoxIcon* param1, int param2);
static void BoxPanel_02031368(BoxPanelManager* param0, u32 boxID);
static void BoxPanel_02031400(BoxPanelManager* param0, int param1, int param2, u32 param3);
static void BoxPanel_02031458(SysTask* param0, void* param1);
static void BoxPanel_020314B4(void* param0);
static void BoxPanel_020314BC(SysTask* param0, void* param1);
static void BoxPanel_02031560(BoxPanelManager* param0);
static u32 BoxPanel_020317F4(u32 param0);
static void BoxPanel_02031804(BoxPanelManager* param0, BoxIconManager* param1, u32 param2, u32 param3, s32 param4, BoxIcon* param5);
static void BoxPanel_020318C0(BoxPanelManager* param0, BoxIcon* param1, u32 param2);
static void BoxPanel_02031918(BoxPanelManager* param0, BoxIcon* param1, u32 param2);
static void BoxPanel_02031970(BoxPanelManager* param0, BoxIcon* param1);
static void BoxPanel_020319C8(u16* param0, const u16* param1, u32 param2, u32 param3, u32 param4);

static const u16 sWallpaperPaletteIdx[] = {
	9,
	10,
};


BOOL BoxPanel_02030AC4(BoxPanelManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4) {
	param0->boxID = param2->customization.boxID;
	param0->unk_01 = 11;
	param0->unk_02 = 0;
	param0->unk_585C = BoxGraphics_020308CC(param1);
	param0->unk_5854 = 72 + param0->unk_585C;
	param0->unk_5858 = (256 + 16) + param0->unk_585C;
	param0->unk_58F0 = BoxGraphics_02030880(param1);
	
	BoxIcon_020337B8(param0->unk_58F0, param0->unk_A8[0], MAX_MONS_PER_BOX);
	BoxIcon_020337B8(param0->unk_58F0, param0->unk_A8[1], MAX_MONS_PER_BOX);
	
	param0->unk_58F4 = param3;
	param0->unk_58F8 = param2;
	param0->unk_58FC = param1;
	
	return TRUE;
}


void BoxPanel_02030B40(BoxPanelManager* param0) {
	BoxIcon_020337DC(param0->unk_58F0, param0->unk_A8[0], MAX_MONS_PER_BOX);
	BoxIcon_020337DC(param0->unk_58F0, param0->unk_A8[1], MAX_MONS_PER_BOX);
}


void BoxPanel_02030B68(BoxPanelManager* param0) {
	param0->unk_0C = Bg_GetXOffset(param0->unk_58F4, 3);
	param0->unk_0C -= param0->unk_585C;
	param0->unk_0C &= (REG_G2_BG3HOFS_OFFSET_MASK);
	
	Bg_SetOffset(param0->unk_58F4, BG_LAYER_MAIN_3, 0, param0->unk_0C);
	Bg_FillTilemapRect(param0->unk_58F4, 3, 0x18, 0, 0, 64, 32, 9);
}


static void BoxPanel_02030BC0(const BoxPanelManager* param0, int param1, u32* param2, u32* param3, u32* param4) {
	if (param1 != 0) {
		*param2 = (param0->unk_02 ^ 1) * (21 * 20);
		*param3 = (param0->unk_01 + (param1 * (21 + 2))) & 63;
		*param4 = sWallpaperPaletteIdx[param0->unk_02 ^ 1];
	} else {
		*param2 = param0->unk_02 * (21 * 20);
		*param3 = param0->unk_01;
		*param4 = sWallpaperPaletteIdx[param0->unk_02];
	}
}


void BoxPanel_02030C18(BoxPanelManager* param0, const BoxCustomization* customization, int boxChangeDirection, BOOL param3) {
	u32 v0, v1, v2;
	BoxPanel_02030BC0(param0, boxChangeDirection, &v0, &v1, &v2);
	BoxPanel_02030C8C(param0, customization, v2, param3);
	BoxPanel_02030D78(param0, customization, v0);
	BoxPanel_02030ED8(param0, customization, v1, v0, v2);
	
	if (param3) {
		if (boxChangeDirection == 0) {
			BoxPanel_02031804(param0, param0->unk_58F0, customization->boxID, param0->unk_02, boxChangeDirection * ((21 + 2) * 8), param0->unk_A8[param0->unk_02]);
		}
	}
}


static void BoxPanel_02030C8C(BoxPanelManager* param0, const BoxCustomization* customization, int param2, BOOL param3) {
	NNSG2dPaletteData* v0;
	void* v1;
	if (BoxApp_GetBoxMode(param0->unk_58F8) != PC_MODE_RANCH_WITHDRAW) {
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_GRAPHIC__BOX;
		} else {
			narc = NARC_INDEX_PL_GRAPHIC__BOX;
		}
		v1 = Graphics_GetPlttData(narc, sBoxWallpaperNarcIDs[customization->wallpaper].nclr, &v0, HEAP_ID_BOX_GRAPHICS);
	} else {
		u32 size = LOCAL_FILE_SIZE(box_wallpaper_nclr);
		v1 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, size);
		if (v1 == NULL) {
			return;
		}
		
		memcpy(v1, LOCAL_FILE(box_wallpaper_nclr), size);
		if (!NNS_G2dGetUnpackedPaletteData(v1, &v0)) {
			OS_Terminate();
			Heap_Free(v1);
			return;
		}
	}
	
	if (v1 != NULL) {
		int v2;
		fx32 v3;
		
		if (param3) {
			DC_FlushRange(v0->pRawData, 0x20);
			GX_LoadBGPltt(v0->pRawData, param2 * 0x20, 0x20);
		}
		
		for (v2 = 0, v3 = 0; v2 < 8; v2++) {
			BoxGraphics_02030974(v0->pRawData, param0->unk_5860[v2], 16, 0x7fff, (v3 >> FX32_SHIFT));
			v3 += ((16 << FX32_SHIFT) / 8);
		}
		
		BoxGraphics_02030974(v0->pRawData, param0->unk_5860[v2], 16, 0x7fff, 16);
		DC_FlushRange(param0->unk_5860, sizeof(param0->unk_5860));
		Heap_Free(v1);
	}
}


static void BoxPanel_02030D78(BoxPanelManager* param0, const BoxCustomization* customization, int param2) {
	void* v0;
	if (BoxApp_GetBoxMode(param0->unk_58F8) != PC_MODE_RANCH_WITHDRAW) {
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_GRAPHIC__BOX;
		} else {
			narc = NARC_INDEX_PL_GRAPHIC__BOX;
		}
		v0 = LoadMemberFromNARC(narc, sBoxWallpaperNarcIDs[customization->wallpaper].ncgr, TRUE, HEAP_ID_BOX_GRAPHICS, TRUE);
	} else {
		// LZ decompressed size from compression header
		int size = *(u32*)(LOCAL_FILE(box_wallpaper_ncgr_LZ)) >> 8;
		v0 = Heap_AllocAtEnd(HEAP_ID_BOX_GRAPHICS, size);
		if (v0 == NULL) {
			return;
		}
		MI_UncompressLZ8(LOCAL_FILE(box_wallpaper_ncgr_LZ), v0);
	}
	
	if (v0 != NULL) {
		NNSG2dCharacterData* v1;
		if (NNS_G2dGetUnpackedBGCharacterData(v0, &v1)) {
			Window* v2 = Window_New(HEAP_ID_BOX_GRAPHICS, 1);
			if (v2) {
				u32 v3, v4;
				
				v2->bgConfig = param0->unk_58F4;
				v2->width = 21;
				v2->height = 4;
				v2->pixels = v1->pRawData;
				
				v4 = Font_CalcStringWidth(FONT_SYSTEM, customization->name, 0);
				v3 = 84 - (v4 / 2);
				
				if (BoxApp_GetBoxMode(param0->unk_58F8) != PC_MODE_RANCH_WITHDRAW) {
					Text_AddPrinterWithParamsAndColor(
						v2,
						FONT_SYSTEM,
						customization->name,
						v3,
						13,
						TEXT_SPEED_NO_TRANSFER,
						TEXT_COLOR(2, 1, 0),
						NULL);
				
				} else {
					// 4 copies printed at (-1,-1), (+1,-1), (-1,+1), (+1,+1) offset from the base location
					// to create an outline effect
					for (int i = 12; i <= 14; i++) {
						for (int j = v3 - 1; j <= v3 + 1; j++) {
							if (j == v3 && i == 13) {
								continue;
							}
							
							Text_AddPrinterWithParamsAndColor(
								v2,
								FONT_SYSTEM,
								customization->name,
								j,
								i,
								TEXT_SPEED_NO_TRANSFER,
								TEXT_COLOR(1, 0, 0),
								NULL);
						}
					}
					
					// Final print in the center
					Text_AddPrinterWithParamsAndColor(
						v2,
						FONT_SYSTEM,
						customization->name,
						v3,
						13,
						TEXT_SPEED_NO_TRANSFER,
						TEXT_COLOR(2, 0, 0),
						NULL);
				}
				
				Heap_Free(v2);
			}
			
			Bg_LoadTiles(param0->unk_58F4, 3, v1->pRawData, v1->szByte, param2);
		}
		
		Heap_Free(v0);
	}
}


static void BoxPanel_02030ED8(BoxPanelManager* param0, const BoxCustomization* customization, u32 param2, u32 param3, u32 param4) {
	void* v0;
	if (BoxApp_GetBoxMode(param0->unk_58F8) != PC_MODE_RANCH_WITHDRAW) {
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_GRAPHIC__BOX;
		} else {
			narc = NARC_INDEX_PL_GRAPHIC__BOX;
		}
		v0 = LoadMemberFromNARC(narc, sBoxWallpaperNarcIDs[customization->wallpaper].nscr, TRUE, HEAP_ID_BOX_GRAPHICS, TRUE);
	} else {
		u32 size = LOCAL_FILE_SIZE(box_wallpaper_nscr);
		v0 = Heap_AllocAtEnd(HEAP_ID_BOX_GRAPHICS, size);
		if (v0 == NULL) {
			return;
		}
		
		memcpy(v0, LOCAL_FILE(box_wallpaper_nscr), size);
	}
	
	if (v0 != NULL) {
		u16* v1;
		NNSG2dScreenData* v2;
		NNS_G2dGetUnpackedScreenData(v0, &v2);
		v1 = Bg_GetTilemapBuffer(param0->unk_58F4, 3);
		
		if (v1 != NULL) {
			BoxPanel_020319C8(v1, (const u16*)(v2->rawData), param2, param3, param4);
		}
		
		Heap_Free(v0);
		Bg_CopyTilemapBufferRangeToVRAM(param0->unk_58F4, 3, v1, 0x1000, 0);
	}
}


void BoxPanel_02030F80(BoxPanelManager* param0, const BoxCustomization* customization, int boxChangeDirection) {
	param0->unk_08 = (Bg_GetXOffset(param0->unk_58F4, 3) & (REG_G2_BG3HOFS_OFFSET_MASK)) << FX32_SHIFT;
	param0->unk_14 = param0->unk_08 + ((((21 + 2) * 8) * boxChangeDirection) << FX32_SHIFT);
	param0->unk_10 = (param0->unk_14 - param0->unk_08) / (15 * 2);
	param0->unk_14 >>= FX32_SHIFT;
	param0->unk_14 &= (REG_G2_BG3HOFS_OFFSET_MASK);
	param0->unk_04 = (15 * 2);
	param0->unk_03 = param0->unk_02;
	param0->unk_01 += ((21 + 2) * boxChangeDirection);
	param0->unk_01 &= 63;
	param0->unk_02 ^= 1;
	
	BoxPanel_02031368(param0, customization->boxID);
	BoxPanel_02031124(param0, param0->unk_10, boxChangeDirection * ((21 + 2) * 8));
	
	param0->unk_30 = customization->boxID;
	param0->unk_34 = param0->unk_02;
	param0->unk_98 = 1;
	param0->unk_9C = 0;
	
	SysTask_Start(BoxPanel_02031034, param0, 0);
	BoxGraphics_02030870(BoxPanel_0203107C, param0, 1);
}


BOOL BoxPanel_0203102C(BoxPanelManager* param0) {
	return param0->unk_9C;
}


static void BoxPanel_02031034(SysTask* param0, void* param1) {
	BoxPanelManager* v0 = (BoxPanelManager*)param1;
	
	if (v0->unk_04) {
		if (v0->unk_98 == 2) {
			BoxPanel_020311AC(v0, v0->unk_10);
			v0->unk_98 = 0;
		}
		
		return;
	}
	
	v0->unk_0C = v0->unk_14;
	v0->boxID = BoxApp_GetCurrentBox(v0->unk_58F8);
	SysTask_Done(param0);
	v0->unk_9C = 1;
}


static void BoxPanel_0203107C(SysTask* param0, void* param1) {
	BoxPanelManager* v0 = (BoxPanelManager*)param1;
	u32 v1 = v0->unk_03;
	u32 v2 = v1 ^ 1;
	
	if (v0->unk_04) {
		if (v0->unk_98 < 2) {
			BoxPanel_020318C0(v0, v0->unk_A8[v1], v0->unk_98);
			BoxPanel_02031918(v0, v0->unk_A8[v2], v0->unk_98);
			
			v0->unk_98++;
			v0->unk_0C = (v0->unk_08 >> FX32_SHIFT) & (REG_G2_BG3HOFS_OFFSET_MASK);
			v0->unk_08 += v0->unk_10;
			Bg_SetOffset(v0->unk_58F4, BG_LAYER_MAIN_3, 0, v0->unk_0C);
			v0->unk_04--;
		}
	} else {
		Bg_SetOffset(v0->unk_58F4, BG_LAYER_MAIN_3, 0, v0->unk_14);
		BoxPanel_02031970(v0, v0->unk_A8[v2]);
		SysTask_Done(param0);
	}
}


static void BoxPanel_02031124(BoxPanelManager* param0, fx32 param1, s32 param2) {
	for (int i = 0; i < 6; i++) {
		param0->unk_68[0][i] = (112 + param0->unk_585C + 24 * i) * FX32_ONE;
		param0->unk_68[1][i] = param0->unk_68[0][i] - param1;
		param0->unk_38[0][i] = (112 + param0->unk_585C + 24 * i + param2) * FX32_ONE;
		param0->unk_38[1][i] = param0->unk_38[0][i] - param1;
		
		int v1 = param0->unk_38[1][i] >> FX32_SHIFT;
		if ((v1 > param0->unk_5854) && (v1 < param0->unk_5858)) {
			int v2 = param0->unk_03 ^ 1;
			BoxPanel_0203124C(param0, param0->unk_A8[v2], v1, i);
		}
	}
}


static void BoxPanel_020311AC(BoxPanelManager* param0, fx32 param1) {
	int v0, v1, v2, v3;
	
	v3 = param0->unk_03;
	v2 = v3 ^ 1;
	
	for (v0 = 0; v0 < 6; v0++) {
		v1 = param0->unk_68[1][v0] >> FX32_SHIFT;
		
		if ((v1 <= param0->unk_5854) || (v1 >= param0->unk_5858)) {
			BoxPanel_02031340(param0, param0->unk_A8[v3], v0);
		}
		
		param0->unk_68[0][v0] = param0->unk_68[1][v0] - param1;
		param0->unk_68[1][v0] = param0->unk_68[0][v0] - param1;
		param0->unk_38[0][v0] = param0->unk_38[1][v0] - param1;
		param0->unk_38[1][v0] = param0->unk_38[0][v0] - param1;
		
		v1 = param0->unk_38[1][v0] >> FX32_SHIFT;
		
		if ((v1 > param0->unk_5854) && (v1 < param0->unk_5858)) {
			BoxPanel_0203124C(param0, param0->unk_A8[v2], v1, v0);
		}
	}
}


static void BoxPanel_0203124C(BoxPanelManager* param0, BoxIcon* param1, int param2, int param3) {
	PCBoxes* pcBoxes;
	BoxPokemon* v1;
	int v2, v3;
	u32 v4;
	
	pcBoxes = BoxApp_GetPCBoxes(param0->unk_58F8);
	v4 = (88 + (param0->unk_34 * 480)) + param3 * (4 * 4);
	param1 += param3;
	v3 = param3;
	
	for (v2 = 0; v2 < 5; v2++) {
		if (param1->unk_00 == NULL) {
			if (BoxApp_GetBoxMode(param0->unk_58F8) == PC_MODE_RANCH_WITHDRAW) {
				v1 = BoxApp_0202E99C(param0->unk_58F8, v3);
			} else {
				v1 = PCBoxes_GetBoxMonAt(pcBoxes, param0->unk_30, v3);
			}
			
			if (BoxPokemon_GetValue(v1, MON_DATA_SPECIES_EXISTS, NULL)) {
				BoxIcon_02033934(param0->unk_58F0, v1, param0->unk_5814[v3], param0->unk_57D8[v3], param0->unk_CD8[v3], param2, 40 + 24 * v2, 2, BoxPanel_020317F4(v3), v4, param1);
			}
		}
		
		v4 += (6 * (4 * 4));
		param1 += 6;
		v3 += 6;
	}
}


static void BoxPanel_02031340(BoxPanelManager* param0, BoxIcon* param1, int param2) {
	param1 += param2;
	
	for (int i = 0; i < 5; i++) {
		if (param1->unk_00 != NULL) {
			Sprite_Delete(param1->unk_00);
			param1->unk_00 = NULL;
		}
		
		param1 += 6;
	}
}


static void BoxPanel_02031368(BoxPanelManager* param0, u32 boxID) {
	PCBoxes* pcBoxes = BoxApp_GetPCBoxes(param0->unk_58F8);
	
	for (u32 i = 0; i < MAX_MONS_PER_BOX; i++) {
		BoxPokemon* boxMon;
		if (BoxApp_GetBoxMode(param0->unk_58F8) == PC_MODE_RANCH_WITHDRAW) {
			boxMon = BoxApp_0202E99C(param0->unk_58F8, i);
		} else {
			boxMon = PCBoxes_GetBoxMonAt(pcBoxes, boxID, i);
		}
		
		u32 species = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
		if (species != SPECIES_NONE) {
			param0->unk_5814[i] = species;
			param0->unk_57D8[i] = BoxPokemon_IconSpriteIndex(boxMon);
			BoxIcon_02033A80(param0->unk_58F0, param0->unk_CD8[i], param0->unk_57D8[i], ((4 * 4) * 0x20 + 0x80));
		}
	}
}


static void BoxPanel_02031400(BoxPanelManager* param0, int param1, int param2, u32 param3) {
	param0->unk_A3 = 0;
	
	BoxPanel_UnkStruct0* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxPanel_UnkStruct0));
	if (v0 != NULL) {
		v0->unk_00 = param0;
		v0->unk_04 = param1;
		v0->unk_06 = param2;
		v0->unk_0C = param3;
		v0->unk_0A = 2;
		v0->unk_08 = (param2 > param1) ? 1 : -1;
		
		if (BoxGraphics_02030870(BoxPanel_02031458, v0, 0) != NULL) {
			param0->unk_A3 = 1;
		} else {
			Heap_Free(v0);
		}
	}
}


static void BoxPanel_02031458(SysTask* param0, void* param1) {
	BoxPanel_UnkStruct0* v0 = (BoxPanel_UnkStruct0*)param1;
	
	if (v0->unk_0A) {
		v0->unk_0A--;
	} else {
		if (v0->unk_04 != v0->unk_06) {
			v0->unk_04 += v0->unk_08;
			GX_LoadBGPltt(v0->unk_00->unk_5860[v0->unk_04], v0->unk_0C, 0x20);
			v0->unk_0A = 2;
		} else {
			BoxGraphics_02030A64(v0, BoxPanel_020314B4, v0->unk_00);
			SysTask_Done(param0);
		}
	}
}


static void BoxPanel_020314B4(void* param0) {
	BoxPanelManager* v0 = param0;
	v0->unk_A3 = 0;
}


static int sWallpaperChangeState = 0;

static void BoxPanel_020314BC(SysTask* param0, void* param1) {
	static u32 v0 = 0, v1 = 0, v2 = 0;
	static const BoxCustomization* customization;
	
	BoxPanelManager* v4 = param1;
	
	switch (sWallpaperChangeState) {
		case 0:
			BoxPanel_02030BC0(v4, 0, &v0, &v1, &v2);
			customization = BoxApp_GetBoxCustomization(v4->unk_58F8);
			BoxPanel_02030C8C(v4, customization, v2, FALSE);
			sWallpaperChangeState++;
			break;
		
		case 1:
			BoxPanel_02030ED8(v4, customization, v1, v0, v2);
			BoxPanel_02030D78(v4, customization, v0);
			sWallpaperChangeState++;
			break;
		
		case 2:
			v4->unk_A3 = 0;
			sWallpaperChangeState = 0;
			SysTask_Done(param0);
			break;
	}
}


static void BoxPanel_02031560(BoxPanelManager* param0) {
	param0->unk_A3 = (SysTask_ExecuteAfterVBlank(BoxPanel_020314BC, param0, 0) != NULL);
	if (param0->unk_A3) {
		sWallpaperChangeState = 0;
	}
}


void BoxPanel_02031598(BoxPanelManager* param0) {
	param0->unk_A2 = 0;
	BoxPanel_02031400(param0, 0, 8, sWallpaperPaletteIdx[param0->unk_02] * 0x20);
}


BOOL BoxPanel_020315B8(BoxPanelManager* param0) {
	switch (param0->unk_A2) {
		case 0:
			if (param0->unk_A3 == 0) {
				param0->unk_A2++;
			}
			break;
		
		case 1:
			BoxPanel_02031560(param0);
			param0->unk_A2++;
			break;
		
		case 2:
			if (param0->unk_A3 == 0) {
				param0->unk_A2++;
			}
			break;
		
		case 3:
			BoxPanel_02031400(param0, 8, 0, sWallpaperPaletteIdx[param0->unk_02] * 0x20);
			param0->unk_A2++;
			break;
		
		case 4:
			if (param0->unk_A3 == 0) {
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


void BoxPanel_02031650(BoxPanelManager* param0, u32 param1, s32 param2, BoxIcon* param3) {
	BoxIcon_02033804(&param0->unk_A8[param0->unk_02][param1], param3, 1);
	BoxIcon_02033A90(param0->unk_58F0, param3, param2);
	BoxIcon_020337B8(param0->unk_58F0, &param0->unk_A8[param0->unk_02][param1], 1);
}


void BoxPanel_020316A4(BoxPanelManager* param0, u32 param1, const BoxIcon* param2) {
	u32 v0 = 88 + (param0->unk_02 * 480) + ((4 * 4) * param1);
	BoxIcon* v1 = &param0->unk_A8[param0->unk_02][param1];
	
	BoxIcon_02033804(param2, v1, 1);
	BoxGraphics_ApplyMonInMultiSelectShading(param0->unk_58F0, v1, FALSE);
	BoxIcon_02033A90(param0->unk_58F0, v1, v0);
	BoxGraphics_SetSpritePriority(v1->unk_00, BoxPanel_020317F4(param1));
}


BoxIcon* BoxPanel_02031708(BoxPanelManager* param0, u32 posInBox) {
	return &param0->unk_A8[param0->unk_02][posInBox];
}


void BoxPanel_02031720(BoxPanelManager* param0) {
	int boxID = PCBoxes_GetCurrentBoxID(param0->unk_58F8->pcBoxes);
	
	for (int i = 0; i < MAX_MONS_PER_BOX; i++) {
		BoxPokemon* boxMon = PCBoxes_GetBoxMonAt(param0->unk_58F8->pcBoxes, boxID, i);
		
		if (BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES_EXISTS, NULL)) {
			if (param0->unk_A8[param0->unk_02][i].unk_00 == NULL) {
				int boxRow = i / MAX_PC_COLS;
				int boxCol = i % MAX_PC_COLS;
				int v5 = (88 + (param0->unk_02 * 480)) + ((4 * 4) * i);
				
				BoxIcon_02033814(param0->unk_58F0, boxMon, 112 + param0->unk_585C + 24 * boxCol, 40 + 24 * boxRow, 2, BoxPanel_020317F4(i), v5, &(param0->unk_A8[param0->unk_02][i]));
			}
		}
	}
}


static u32 BoxPanel_020317F4(u32 param0) {
	static const u16 v0[MAX_MONS_PER_BOX] = {
		5,   4,  3,  2,  1,  0,
		11, 10,  9,  8,  7,  6,
		17, 16, 15, 14, 13, 12,
		23, 22, 21, 20, 19, 18,
		29, 28, 27, 26, 25, 24,
	};
	
	return 11 + v0[param0];
}


static void BoxPanel_02031804(BoxPanelManager* param0, BoxIconManager* param1, u32 param2, u32 param3, s32 param4, BoxIcon* param5) {
	BoxPokemon* v0;
	u32 v1, v2, v3, v4, v5;
	u64 v6, v7;
	
	v5 = (88 + (param3 * 480));
	v4 = 0;
	
	for (v3 = 0; v3 < 5; v3++) {
		for (v2 = 0; v2 < 6; v2++) {
			if (BoxApp_GetBoxMode(param0->unk_58F8) != PC_MODE_RANCH_WITHDRAW) {
				GF_ASSERT(PCBoxes_GetCurrentBoxID(param0->unk_58F8->pcBoxes) == param2);
			}
			
			v0 = BoxApp_0202E96C(param1->unk_40, v4);
			v1 = BoxPokemon_GetValue((BoxPokemon*)v0, MON_DATA_SPECIES, NULL);
			
			if (v1 != 0) {
				BoxIcon_02033814(param1, v0, 112 + param0->unk_585C + 24 * v2 + param4, 40 + 24 * v3, 2, BoxPanel_020317F4(v4), v5, param5);
			}
			
			param5++;
			v4++;
			v5 += (4 * 4);
		}
	}
}


static void BoxPanel_020318C0(BoxPanelManager* param0, BoxIcon* param1, u32 param2) {
	for (int v2 = 0; v2 < 5; v2++) {
		for (int v1 = 0; v1 < 6; v1++) {
			if (param1->unk_00 != NULL) {
				VecFx32 v0;
				v0 = *(Sprite_GetPosition(param1->unk_00));
				v0.x = param0->unk_68[param2][v1];
				Sprite_SetPosition(param1->unk_00, &v0);
			}
			
			param1++;
		}
	}
}


static void BoxPanel_02031918(BoxPanelManager* param0, BoxIcon* param1, u32 param2) {
	for (int v2 = 0; v2 < 5; v2++) {
		for (int v1 = 0; v1 < 6; v1++) {
			if (param1->unk_00 != NULL) {
				VecFx32 v0;
				v0 = *(Sprite_GetPosition(param1->unk_00));
				v0.x = param0->unk_38[param2][v1];
				Sprite_SetPosition(param1->unk_00, &v0);
			}
			
			param1++;
		}
	}
}


static void BoxPanel_02031970(BoxPanelManager* param0, BoxIcon* param1) {
	VecFx32 v0;
	v0.z = 0;
	
	for (int v2 = 0; v2 < 5; v2++) {
		v0.y = (40 + v2 * 24) * FX32_ONE;
		
		for (int v1 = 0; v1 < 6; v1++) {
			v0.x = (112 + param0->unk_585C + v1 * 24) * FX32_ONE;
			
			if (param1->unk_00 != NULL) {
				Sprite_SetPosition(param1->unk_00, &v0);
			}
			
			param1++;
		}
	}
}


static void BoxPanel_020319C8(u16* param0, const u16* param1, u32 param2, u32 param3, u32 param4) {
	u32 v0, v1;
	u16* v2;
	u16 v3;
	
	if (param2 >= 32) {
		param0 += 1024;
	}
	
	for (v0 = 0; v0 < 21; v0++) {
		v2 = param0 + ((32 * 0) + (param2 & 31));
		
		for (v1 = 0; v1 < 20; v1++) {
			v3 = param1[v1 * 21 + v0];
			v3 = ((GX_SCRFMT_TEXT_CHARNAME_MASK & v3) + param3) | ((GX_SCRFMT_TEXT_HF_MASK | GX_SCRFMT_TEXT_VF_MASK) & v3) | (param4 << GX_SCRFMT_TEXT_COLORPLTT_SHIFT);
			*v2 = v3;
			v2 += 32;
		}
		
		for (v1 = 0; v1 < 4; v1++) {
			*v2 = (param4 << GX_SCRFMT_TEXT_COLORPLTT_SHIFT) | (param3 + 0);
			v2 += 32;
		}
		
		param2 = (param2 + 1) & 63;
		
		if (param2 == 0) {
			param0 -= 1024;
		}
		
		if (param2 == 32) {
			param0 += 1024;
		}
	}
	
	for (v0 = 0; v0 < 2; v0++) {
		v2 = param0 + ((32 * 0) + (param2 & 31));
		
		for (v1 = 0; v1 < (20 + 4); v1++) {
			*v2 = (param4 << GX_SCRFMT_TEXT_COLORPLTT_SHIFT) | (param3 + 0);
			v2 += 32;
		}
		
		param2 = (param2 + 1) & 63;
		
		if (param2 == 0) {
			param0 -= 1024;
		}
		
		if (param2 == 32) {
			param0 += 1024;
		}
	}
}


void BoxPanel_02031AC4(BoxPanelManager* param0, u32 boxCursorPosition) {
	param0->unk_A0 = boxCursorPosition;
	param0->unk_A4 = 0;
	
	BoxIcon_02033AF4(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][boxCursorPosition]), 1);
}


BOOL BoxPanel_02031AFC(BoxPanelManager* param0) {
	BoxIcon* v0 = &(param0->unk_A8[param0->unk_02][param0->unk_A0]);
	
	switch (param0->unk_A4) {
		case 0:
			if (BoxIcon_02033B1C(param0->unk_58F0, v0)) {
				const BoxApplicationManager* v1 = BoxGraphics_02030A10(param0->unk_58FC);
				
				if (BoxAppMan_HasCheckedCanReleaseMon(v1)) {
					if (BoxAppMan_CanReleaseMon(v1)) {
						BoxIcon_020337DC(param0->unk_58F0, v0, 1);
						return TRUE;
					} else {
						BoxIcon_02033AF4(param0->unk_58F0, v0, 2);
						param0->unk_A4++;
					}
				}
			}
			
			break;
		
		case 1:
			if (BoxIcon_02033B1C(param0->unk_58F0, v0)) {
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


void BoxPanel_02031B9C(BoxPanelManager* param0) {
	u32 v0 = BoxApp_GetMonSpriteTransparencyMask(param0->unk_58F8);
	
	for (int monPosInBox = 0; monPosInBox < MAX_MONS_PER_BOX; monPosInBox++) {
		if (param0->unk_A8[param0->unk_02][monPosInBox].unk_00 != NULL) {
			BoxIcon_02033A28(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][monPosInBox]), v0);
		}
	}
}


void BoxPanel_02031BEC(BoxPanelManager* param0, u32 posInBox, u32 item) {
	if (param0->unk_A8[param0->unk_02][posInBox].unk_00 != NULL) {
		BoxIcon_02033A7C(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][posInBox]), item);
		BoxIcon_02033A28(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][posInBox]), BoxApp_GetMonSpriteTransparencyMask(param0->unk_58F8));
	}
}


void BoxPanel_02031C40(BoxPanelManager* param0) {
	u32 posInBox = BoxApp_GetCursorBoxPosition(param0->unk_58F8);
	u32 monMarkings = BoxApp_GetPreviewedMonMarkings(param0->unk_58F8);
	
	if (param0->unk_A8[param0->unk_02][posInBox].unk_00 != NULL) {
		BoxIcon_02033A78(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][posInBox]), monMarkings);
	}
}


void BoxGraphics_ApplyMultiSelectMonShading(BoxPanelManager* param0) {
	u32 leftCol, rightCol, topRow, bottomRow;
	BoxApp_GetMultiSelectBoundingBox(param0->unk_58F8, &leftCol, &rightCol, &topRow, &bottomRow);
	
	u32 posInBox = 0;
	for (u32 row = 0; row < MAX_PC_ROWS; row++) {
		for (u32 col = 0; col < MAX_PC_COLS; col++) {
			if (param0->unk_A8[param0->unk_02][posInBox].unk_00 != NULL) {
				if (((u32)(col - leftCol) <= (u32)(rightCol - leftCol)) & ((u32)(row - topRow) <= (u32)(bottomRow - topRow))) {
					BoxGraphics_ApplyMonInMultiSelectShading(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][posInBox]), TRUE);
				} else {
					BoxGraphics_ApplyMonInMultiSelectShading(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][posInBox]), FALSE);
				}
			}
			
			posInBox++;
		}
	}
}


void BoxPanel_02031D30(BoxPanelManager* param0) {
	for (int i = 0; i < MAX_MONS_PER_BOX; i++) {
		if (param0->unk_A8[param0->unk_02][i].unk_00 != NULL) {
			BoxGraphics_ApplyMonInMultiSelectShading(param0->unk_58F0, &(param0->unk_A8[param0->unk_02][i]), FALSE);
		}
	}
}


// New with MPR below here (controls animations of deposit/withdraw)

void BoxPanel_02031D70(BoxPanelManager* param0, int param1) {
	BoxIcon* icon = &param0->unk_A8[param0->unk_02][param1];
	icon->unk_32 = 0;
	Sprite_SetAffineOverwriteMode(icon->unk_00, AFFINE_OVERWRITE_MODE_DOUBLE);
}


BOOL BoxPanel_02031D98(BoxPanelManager* param0, int param1, BOOL param2) {
	BoxIcon* icon = &param0->unk_A8[param0->unk_02][param1];
	VecFx32 scaleVec;
	
	switch (icon->unk_32) {
		default:
			icon->unk_32++;
			Sprite_SetAffineZRotation(icon->unk_00, ((icon->unk_32 * FX32_CONST(16)) / 30) % FX32_CONST(16));
			
			scaleVec.x = scaleVec.y = ((27 - icon->unk_32) << FX32_SHIFT) / 27;
			scaleVec.z = FX32_ONE;
			Sprite_SetAffineScale(icon->unk_00, &scaleVec);
			break;
		
		case 26:
			if (BoxIcon_02033B1C(param0->unk_58F0, icon)) {
				BoxGraphics_02030A10(param0->unk_58FC);
				
				if (!param2) {
					BoxIcon_020337DC(param0->unk_58F0, icon, 1);
					return TRUE;
				}
				
				Sprite_SetAffineZRotation(icon->unk_00, 0);
				
				scaleVec.x = scaleVec.y = scaleVec.z = FX32_ONE;
				Sprite_SetAffineScale(icon->unk_00, &scaleVec);
				
				BoxIcon_02033AF4(param0->unk_58F0, icon, 2);
				icon->unk_32++;
			}
			break;
		
		case 27:
			if (BoxIcon_02033B1C(param0->unk_58F0, icon)) {
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


void BoxPanel_02031E84(BoxPanelManager* param0, int param1) {
	BoxIcon* icon = &param0->unk_A8[param0->unk_02][param1];
	icon->unk_32 = 0;
	
	VecFx32 position = *(Sprite_GetPosition(icon->unk_00));
	position.y -= FX32_ONE;
	Sprite_SetPosition(icon->unk_00, &position);
}


BOOL BoxPanel_02031ED0(BoxPanelManager* param0, int param1) {
	BoxIcon* icon = &param0->unk_A8[param0->unk_02][param1];
	
	if (icon->unk_32 != 26) {
		if (icon->unk_32 == 10) {
			Sprite_SetExplicitOAMMode(icon->unk_00, GX_OAM_MODE_XLU);
		}
		
		if (icon->unk_32 > 21) {
			Sprite_SetDrawFlag(icon->unk_00, (icon->unk_32 % 2 != 0));
		}
		
		VecFx32 position = *(Sprite_GetPosition(icon->unk_00));
		position.y -= (icon->unk_32 << FX32_SHIFT) / 5;
		Sprite_SetPosition(icon->unk_00, &position);
		icon->unk_32++;
	} else {
		BoxIcon_020337DC(param0->unk_58F0, icon, 1);
		return TRUE;
	}
	
	return FALSE;
}
