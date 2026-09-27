// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_02033D34.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"

#include "assert.h"
#include "bg_window.h"
#include "font_special_chars.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "message.h"
#include "narc.h"
#include "pokemon.h"
#include "pokemon_sprite.h"
#include "sprite.h"
#include "string_gf.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "text.h"
#include "type_icon.h"

typedef struct {
	u16 unk_00;
	u16 unk_02;
	u16 unk_04;
	u16 unk_06;
	BoxMonPreviewManager* unk_08;
} BoxMonPreview_UnkStruct0;

static void BoxMonPreview_02033F4C(BoxMonPreviewManager* param0);
static void BoxMonPreview_02034054(BoxMonPreviewManager* param0);
static void BoxMonPreview_02034098(BoxMonPreviewManager* param0);
static void BoxMonPreview_020340B8(BoxMonPreviewManager* param0);
static void BoxMonPreview_020340E4(BoxMonPreviewManager* param0);
static void BoxMonPreview_02034100(SysTask* param0, void* param1);
static void BoxMonPreview_020341C8(BoxMonPreviewManager* param0, fx32 param1);
static void BoxMonPreview_0203420C(BoxMonPreviewManager* param0);
static void BoxMonPreview_02034228(BoxMonPreview_UnkStruct0* param0);
static void BoxMonPreview_0203425C(Window* param0, BoxMonPreviewManager* param1, u32 param2, u32 param3);
static void BoxMonPreview_020344E0(BoxMonPreviewManager* param0);
static void BoxMonPreview_02034648(BoxMonPreviewManager* param0, u8 param1);


BOOL BoxMonPreview_02033D34(BoxMonPreviewManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4, const MessageLoader* param5) {
	if (BoxApp_GetBoxMode(param2) == PC_MODE_COMPARE) {
		param0->unk_00 = 0;
		return TRUE;
	}
	
	param0->unk_14 = param1;
	param0->unk_08 = param3;
	param0->unk_10 = param2;
	param0->unk_0C = param4;
	param0->unk_04 = NULL;
	param0->unk_4C = NULL;
	param0->unk_24 = NULL;
	param0->unk_20 = BoxGraphics_020308C4(param1);
	param0->unk_18 = FontSpecialChars_Init(9, 6, 15, HEAP_ID_BOX_GRAPHICS);
	param0->unk_1C = FontSpecialChars_Init(1, 2, 15, HEAP_ID_BOX_GRAPHICS);
	
	// TODO naix
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	param0->unk_28 = Graphics_GetCellBank(narc, 16, 1, &(param0->unk_2C), HEAP_ID_BOX_GRAPHICS);
	param0->unk_30 = NULL;
	param0->unk_44 = MessageLoader_GetNewString(param5, 21);
	param0->unk_48 = MessageLoader_GetNewString(param5, 22);
	param0->unk_34 = Graphics_GetCellBank(TypeIcon_GetNARC(), TypeIcon_GetCell(), 1, &(param0->unk_38), HEAP_ID_BOX_GRAPHICS);
	
	for (int i = 0; i < 2; i++) {
		param0->unk_3C[i] = NULL;
	}
	
	param0->unk_00 = 1;
	return TRUE;
}


void BoxMonPreview_02033DE8(BoxMonPreviewManager* param0) {
	int i;
	
	if (param0->unk_00 == 0) {
		return;
	}
	
	if (param0->unk_04) {
		for (i = 0; i < 4; i++) {
			Window_Remove(&param0->unk_04[i]);
		}
		
		Heap_Free(param0->unk_04);
		param0->unk_04 = NULL;
	}
	
	if (param0->unk_18) {
		FontSpecialChars_Free(param0->unk_18);
	}
	
	if (param0->unk_1C) {
		FontSpecialChars_Free(param0->unk_1C);
	}
	
	if (param0->unk_44) {
		String_Free(param0->unk_44);
	}
	
	if (param0->unk_48) {
		String_Free(param0->unk_48);
	}
	
	if (param0->unk_24) {
		PokemonSprite_Delete(param0->unk_24);
		param0->unk_24 = NULL;
	}
	
	if (param0->unk_28) {
		Heap_Free(param0->unk_28);
	}
	
	if (param0->unk_30) {
		Sprite_Delete(param0->unk_30);
	}
	
	if (param0->unk_34) {
		Heap_Free(param0->unk_34);
	}
	
	for (i = 0; i < 2; i++) {
		if (param0->unk_3C[i] != NULL) {
			Sprite_Delete(param0->unk_3C[i]);
		}
	}
	
	BoxMonPreview_02034098(param0);
}


void BoxMonPreview_02033E88(BoxMonPreviewManager* param0) {
	if (param0->unk_00) {
		PokemonSpriteManager_UpdateCharAndPltt(param0->unk_20);
	}
}


void BoxMonPreview_02033E98(BoxMonPreviewManager* param0) {
	PokemonSpriteTemplate v0;
	BuildPokemonSpriteTemplate(&v0, 1, 0, 0, 0, 0, 0);
	param0->unk_24 = PokemonSpriteManager_CreateSprite(param0->unk_20, &v0, 44, 84, 0, 0, NULL, NULL);
}


BOOL BoxMonPreview_02033ECC(BoxMonPreviewManager* param0) {
	if (param0->unk_24) {
		PokemonSprite_Delete(param0->unk_24);
		param0->unk_24 = NULL;
		return FALSE;
	}
	
	return TRUE;
}


void BoxMonPreview_02033EE4(BoxMonPreviewManager* param0) {
	static const WindowTemplate v0[] = {
		{ 2, 0, 1,  10, 2, 1, 288 },
		{ 2, 5, 3,   5, 1, 1, 308 },
		{ 2, 0, 16, 10, 3, 3, 313 },
		{ 2, 1, 21, 12, 4, 1, 343 }
	};
	
	if (param0->unk_00 == 0) {
		return;
	}
	
	param0->unk_04 = Window_New(HEAP_ID_BOX_GRAPHICS, 4);
	
	if (param0->unk_04) {
		for (int i = 0; i < 4; i++) {
			Window_AddFromTemplate(param0->unk_08, &param0->unk_04[i], &v0[i]);
			
			if (i != 3) {
				Window_PutToTilemap(&param0->unk_04[i]);
			} else {
				Window_PutRectToTilemap(&param0->unk_04[i], 12, 2);
			}
		}
		
		Bg_CopyTilemapBufferToVRAM(param0->unk_08, 2);
	}
	
	BoxMonPreview_02033F4C(param0);
	BoxMonPreview_02034054(param0);
}


static void BoxMonPreview_02033F4C(BoxMonPreviewManager* param0) {
	NNSG2dImageProxy v0;
	SpriteResourcesHeader v1;
	NarcID narcID;
	u32 v3, i;
	
	narcID = TypeIcon_GetNARC();
	v3 = TypeIcon_GetChar(2);
	
	Graphics_LoadPalette(narcID, TypeIcon_GetPlttSrc(), 1, 10 * 0x20, 0x20 * 3, HEAP_ID_BOX_GRAPHICS);
	
	for (i = 0; i < 2; i++) {
		NNS_G2dInitImageProxy(&v0);
		Graphics_LoadImageMappingAndSetVramMode(narcID, v3, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, (1504 + (8 * i)) * 0x20, 10, &v0);
		BoxGraphics_020308E8(&v1, &v0, BoxGraphics_02030878(param0->unk_14), param0->unk_38, NULL, 3);

		param0->unk_3C[i] = BoxGraphics_02030908(param0->unk_0C, &v1, 24 + 36 * i, 176, 46, NNS_G2D_VRAM_TYPE_2DMAIN);
		GF_ASSERT(param0->unk_3C[i] != NULL);

		Sprite_SetDrawFlag(param0->unk_3C[i], FALSE);
	}
	
	NNS_G2dInitImageProxy(&v0);
	
	Graphics_LoadImageMapping(18, 15, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 1480 * 0x20, 10, &v0);
	BoxGraphics_020308E8(&v1, &v0, BoxGraphics_02030878(param0->unk_14), param0->unk_2C, NULL, 3);
	
	param0->unk_30 = BoxGraphics_02030908(param0->unk_0C, &v1, 56, 176, 47, NNS_G2D_VRAM_TYPE_2DMAIN);
}


static void BoxMonPreview_02034054(BoxMonPreviewManager* param0) {
	if (param0->unk_4C == NULL) {
		BoxMonPreview_UnkStruct0* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxMonPreview_UnkStruct0));
		
		if (v0 != NULL) {
			const PCMonPreview* preview = BoxApp_GetPCMonPreview(param0->unk_10);
			
			v0->unk_00 = (preview->isEgg) ? 4 : 0;
			v0->unk_08 = param0;
			param0->unk_4C = SysTask_Start(BoxMonPreview_02034100, v0, 1);
		} else {
			GF_ASSERT(FALSE);
		}
	}
}


static void BoxMonPreview_02034098(BoxMonPreviewManager* param0) {
	if (param0->unk_4C) {
		Heap_Free(SysTask_GetParam(param0->unk_4C));
		SysTask_Done(param0->unk_4C);
		param0->unk_4C = NULL;
	}
}


static void BoxMonPreview_020340B8(BoxMonPreviewManager* param0) {
	BoxMonPreview_UnkStruct0* v0 = SysTask_GetParam(param0->unk_4C);
	if (v0 != NULL) {
		const PCMonPreview* preview = BoxApp_GetPCMonPreview(param0->unk_10);
		
		v0->unk_00 = (preview->isEgg) ? 4 : 0;
		BoxMonPreview_0203420C(param0);
	}
}


static void BoxMonPreview_020340E4(BoxMonPreviewManager* param0) {
	BoxMonPreview_UnkStruct0* v0 = SysTask_GetParam(param0->unk_4C);
	if (v0 != NULL) {
		v0->unk_00 = 4;
		BoxMonPreview_0203420C(param0);
	}
}


static void BoxMonPreview_02034100(SysTask* param0, void* param1) {
	BoxMonPreview_UnkStruct0* v0 = (BoxMonPreview_UnkStruct0*)param1;
	BoxMonPreviewManager* v1 = v0->unk_08;
	const PCMonPreview* preview;
	
	switch (v0->unk_00) {
		case 0:
			if (BoxApp_IsMonAvailableToCursor(v1->unk_10) == FALSE) {
				break;
			}

			v0->unk_02 = 0;
			v0->unk_00 = 1;
			// Fall through
		
		case 1:
			BoxMonPreview_0203425C(&v1->unk_04[3], v1, v0->unk_02, 0);
			Window_LoadTiles(&v1->unk_04[3]);
			v0->unk_04 = 0;
			v0->unk_00 = 2;
			break;
		
		case 2:
			v0->unk_04++;
			if (v0->unk_04 > 80) {
				BoxMonPreview_02034228(v0);
				v0->unk_04 = 0;
				v0->unk_06 = 0;
				v0->unk_00 = 3;
			}
			break;
		
		case 3:
			v0->unk_06 += 4;
			
			BoxMonPreview_020341C8(v1, 4 << FX32_SHIFT);
			Window_Scroll(&v1->unk_04[3], 0, 4, 0x0);
			Window_LoadTiles(&v1->unk_04[3]);
			
			if (v0->unk_06 >= 16) {
				if (v0->unk_02 == 1) {
					BoxMonPreview_0203420C(v1);
				}
				
				v0->unk_02++;
				if (v0->unk_02 >= 4) {
					v0->unk_02 = 0;
				}
				
				v0->unk_06 = 0;
				v0->unk_04 = 0;
				v0->unk_00 = 2;
			}
			break;
		
		case 4:
			break;
	}
}


static void BoxMonPreview_020341C8(BoxMonPreviewManager* param0, fx32 param1) {
	for (int i = 0; i < 2; i++) {
		VecFx32 v1 = *(Sprite_GetPosition(param0->unk_3C[i]));
		v1.y -= param1;
		
		Sprite_SetPosition(param0->unk_3C[i], &v1);
		
		if (i == 0 && Sprite_GetDrawFlag(param0->unk_3C[i])) {
			u32 v2, v3;
			
			v2 = v1.x >> FX32_SHIFT;
			v3 = v1.y >> FX32_SHIFT;
		}
	}
}


static void BoxMonPreview_0203420C(BoxMonPreviewManager* param0) {
	for (int i = 0; i < 2; i++) {
		Sprite_SetDrawFlag(param0->unk_3C[i], FALSE);
	}
}


static void BoxMonPreview_02034228(BoxMonPreview_UnkStruct0* param0) {
	u32 v0;
	Window* v1 = &(param0->unk_08->unk_04[3]);
	
	Window_FillTilemap(v1, 0);
	BoxMonPreview_0203425C(v1, param0->unk_08, param0->unk_02, 0);
	
	v0 = param0->unk_02 + 1;
	
	if (v0 >= 4) {
		v0 = 0;
	}
	
	BoxMonPreview_0203425C(v1, param0->unk_08, v0, 1);
}


static void BoxMonPreview_0203425C(Window* param0, BoxMonPreviewManager* param1, u32 param2, u32 param3) {
	const String* v0;
	u32 v1 = (param3 * 16);
	
	switch (param2) {
		case 0:
			v0 = param1->unk_10->pcMonPreview.heldItemName;
			break;
		
		case 2:
			v0 = param1->unk_10->pcMonPreview.nature;
			break;
		
		case 3:
			v0 = param1->unk_10->pcMonPreview.ability;
			break;
		
		case 1:
			{
				VecFx32 v2;
				u32 v3 = TypeIcon_GetNARC();
				u32 v4 = TypeIcon_GetChar(param1->unk_10->pcMonPreview.type1);
				
				Graphics_LoadObjectTiles(TypeIcon_GetNARC(), TypeIcon_GetChar(param1->unk_10->pcMonPreview.type1), 0, 1504 * 0x20, 0, 1, HEAP_ID_BOX_GRAPHICS);
				
				v2 = *(Sprite_GetPosition(param1->unk_3C[0]));
				v2.y = (176 + (16 * param3)) << FX32_SHIFT;
				
				Sprite_SetPosition(param1->unk_3C[0], &v2);
				Sprite_SetExplicitPalette(param1->unk_3C[0], 10 + TypeIcon_GetPltt(param1->unk_10->pcMonPreview.type1));
				Sprite_SetDrawFlag(param1->unk_3C[0], TRUE);
				
				if (param1->unk_10->pcMonPreview.type1 != param1->unk_10->pcMonPreview.type2) {
					Graphics_LoadObjectTiles(TypeIcon_GetNARC(), TypeIcon_GetChar(param1->unk_10->pcMonPreview.type2), 0, (1504 + 8) * 0x20, 0, 1, HEAP_ID_BOX_GRAPHICS);
					
					v2.x += (36 << FX32_SHIFT);
					
					Sprite_SetPosition(param1->unk_3C[1], &v2);
					Sprite_SetExplicitPalette(param1->unk_3C[1], 10 + TypeIcon_GetPltt(param1->unk_10->pcMonPreview.type2));
					Sprite_SetDrawFlag(param1->unk_3C[1], TRUE);
				} else {
					Sprite_SetDrawFlag(param1->unk_3C[1], FALSE);
				}
			}
			return;
		
		default:
			return;
	}
	
	Text_AddPrinterWithParamsAndColor(
		param0,
		FONT_SYSTEM,
		v0,
		0,
		v1,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(9, 6, 0),
		NULL);
}


void BoxMonPreview_02034394(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return;
	}
	
	Window_FillTilemap(&param0->unk_04[0], 15);
	Window_FillTilemap(&param0->unk_04[1], 15);
	Window_FillTilemap(&param0->unk_04[2], 15);
	Window_FillTilemap(&param0->unk_04[3], 0);
	
	if (BoxApp_IsMonAvailableToCursor(param0->unk_10)) {
		BoxMonPreview_020344E0(param0);
	}
	
	Window_LoadTiles(&param0->unk_04[0]);
	Window_LoadTiles(&param0->unk_04[1]);
	Window_LoadTiles(&param0->unk_04[2]);
	Window_LoadTiles(&param0->unk_04[3]);
}


void BoxMonPreview_020343F4(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return;
	}
	
	Window_FillTilemap(&param0->unk_04[0], 15);
	Window_FillTilemap(&param0->unk_04[1], 15);
	Window_FillTilemap(&param0->unk_04[2], 15);
	Window_FillTilemap(&param0->unk_04[3], 0);
	
	if (param0->unk_24) {
		PokemonSprite_Delete(param0->unk_24);
		param0->unk_24 = NULL;
	}
	
	if (BoxApp_IsMonAvailableToCursor(param0->unk_10)) {
		BoxMonPreview_020344E0(param0);
	} else {
		BoxMonPreview_02034648(param0, 0);
	}
	
	Window_LoadTiles(&param0->unk_04[0]);
	Window_LoadTiles(&param0->unk_04[1]);
	Window_LoadTiles(&param0->unk_04[2]);
	Window_LoadTiles(&param0->unk_04[3]);
	
	BoxMonPreview_020340B8(param0);
}


void BoxMonPreview_02034474(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return;
	}
	
	Window_FillTilemap(&param0->unk_04[0], 15);
	Window_FillTilemap(&param0->unk_04[1], 15);
	Window_FillTilemap(&param0->unk_04[2], 15);
	Window_FillTilemap(&param0->unk_04[3], 0);
	
	if (param0->unk_24) {
		PokemonSprite_Delete(param0->unk_24);
		param0->unk_24 = NULL;
	}
	
	BoxMonPreview_02034648(param0, 0);
	
	Window_LoadTiles(&param0->unk_04[0]);
	Window_LoadTiles(&param0->unk_04[1]);
	Window_LoadTiles(&param0->unk_04[2]);
	Window_LoadTiles(&param0->unk_04[3]);
	
	BoxMonPreview_020340E4(param0);
}


static void BoxMonPreview_020344E0(BoxMonPreviewManager* param0) {
	PokemonSpriteTemplate v0;
	const PCMonPreview* preview = BoxApp_GetPCMonPreview(param0->unk_10);
	
	Text_AddPrinterWithParamsAndColor(
		&param0->unk_04[0],
		FONT_SYSTEM,
		preview->speciesName,
		2,
		0,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(9, 6, 15),
		NULL);
	
	Text_AddPrinterWithParamsAndColor(
		&param0->unk_04[2],
		FONT_SYSTEM,
		preview->nickname,
		2,
		0,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(1, 2, 15),
		NULL);
	
	Text_AddPrinterWithParamsAndColor(
		&param0->unk_04[2],
		FONT_SYSTEM,
		preview->nickname,
		2,
		0,
		TEXT_SPEED_NO_TRANSFER,
		TEXT_COLOR(1, 2, 15),
		NULL);
	
	switch (preview->gender) {
		case 0:
			Text_AddPrinterWithParamsAndColor(
				&param0->unk_04[2], 
				FONT_SYSTEM,
				param0->unk_44,
				70,
				0,
				TEXT_SPEED_NO_TRANSFER,
				TEXT_COLOR(7, 8, 15),
				NULL);
			
			break;
		
		case 1:
			Text_AddPrinterWithParamsAndColor(
				&param0->unk_04[2],
				FONT_SYSTEM,
				param0->unk_48,
				70,
				0,
				TEXT_SPEED_NO_TRANSFER,
				TEXT_COLOR(3, 4, 15),
				NULL);
			
			break;
	}
	
	if (preview->isEgg == FALSE) {
		Text_AddPrinterWithParamsAndColor(&param0->unk_04[3], FONT_SYSTEM, preview->heldItemName, 0, 0, TEXT_SPEED_NO_TRANSFER, TEXT_COLOR(9, 6, 0), NULL);
		
		if (preview->dexNum) {
			FontSpecialChars_DrawPartyScreenText(param0->unk_18, 2, preview->dexNum, 3, 2, &(param0->unk_04[1]), 0, 0);
		}
		
		FontSpecialChars_DrawPartyScreenText(param0->unk_1C, 1, preview->level, 3, 1, &(param0->unk_04[2]), 0, 16);
	}
	
	BoxPokemon_BuildSpriteTemplate(&v0, preview->mon, 2);
	param0->unk_24 = PokemonSpriteManager_CreateSprite(param0->unk_20, &v0, 44, 84, 0, 0, NULL, NULL);
	BoxMonPreview_02034648(param0, preview->markings);
}


BOOL BoxMonPreview_0203461C(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return TRUE;
	}
	return TRUE;
}


void BoxMonPreview_02034620(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return;
	}
	
	if (BoxApp_IsMonAvailableToCursor(param0->unk_10)) {
		const PCMonPreview* preview = BoxApp_GetPCMonPreview(param0->unk_10);
		BoxMonPreview_02034648(param0, preview->markings);
	}
}


static void BoxMonPreview_02034648(BoxMonPreviewManager* param0, u8 param1) {
	u16* v0;
	u32 v1;
	int v2;
	
	v1 = (19 * 32) + 4;
	v0 = Bg_GetTilemapBuffer(param0->unk_08, 2);
	v0 += v1;
	
	for (v2 = 0; v2 < 6; v2++) {
		if (param1 & (1 << v2)) {
			v0[v2] = (2 << 12) | (152 + v2);
		} else {
			v0[v2] = (2 << 12) | (132 + v2);
		}
	}
	
	Bg_CopyTilemapBufferRangeToVRAM(param0->unk_08, 2, v0, 6 * sizeof(u16), v1);
}


void BoxMonPreview_020346AC(BoxMonPreviewManager* param0) {
	if (param0->unk_00 == 0) {
		return;
	}
	
	const PCMonPreview* preview = BoxApp_GetPCMonPreview(param0->unk_10);
	Window_FillTilemap(&param0->unk_04[3], 0);
	
	if (preview->isEgg == FALSE) {
		Text_AddPrinterWithParamsAndColor(
			&param0->unk_04[3],
			FONT_SYSTEM,
			preview->heldItemName,
			0,
			0,
			TEXT_SPEED_NO_TRANSFER,
			TEXT_COLOR(9, 6, 0),
			NULL);
	}
	
	BoxMonPreview_020340B8(param0);
}
