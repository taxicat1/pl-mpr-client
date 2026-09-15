// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_02037A18.h"

#include "constants/items.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_02031F6C.h"
#include "applications/pc_boxes/box_02035AAC.h"

#include "bg_window.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "item.h"
#include "sprite.h"
#include "sys_task.h"
#include "sys_task_manager.h"

struct BoxItemIconManager {
	BOOL unk_00;
	BoxApplicationDisplay* unk_04;
	const BoxApplication* unk_08;
	BgConfig* unk_0C;
	SpriteList* unk_10;
	BoxIconManager* unk_14;
	BoxCursorManager* unk_18;
	Sprite* unk_1C[3];
	int unk_28[3];
	int unk_34;
	int unk_38;
};

typedef struct {
	BoxItemIconManager* unk_00;
	Sprite* unk_04;
	VecFx32 unk_08;
	VecFx32 unk_14;
	fx32 unk_20;
	fx32 unk_24;
	int unk_28;
	int unk_2C;
	int unk_30;
} BoxItemIcon_UnkStruct0;

static void BoxItemIcon_02037A80(BoxItemIconManager* param0);
static void BoxItemIcon_02037B4C(BoxItemIconManager* param0);
static int BoxItemIcon_02037B68(BoxItemIconManager* param0, int param1);
static int BoxItemIcon_02037B90(BoxItemIconManager* param0, int param1);
static void BoxItemIcon_02037BAC(BoxItemIconManager* param0, int param1, int param2);
static void BoxItemIcon_02037BCC(BoxItemIconManager* param0);
static void BoxItemIcon_02037C04(BoxItemIconManager* param0);
static void BoxItemIcon_02037C9C(BoxItemIconManager* param0, VecFx32* param1, u32* param2);
static void BoxItemIcon_02037D20(BoxItemIconManager* param0, int param1, int param2);
static void BoxItemIcon_02037E60(BoxItemIconManager* param0, int param1, const VecFx32* param2, int param3, int param4);
static void BoxItemIcon_02037EE8(SysTask* param0, void* param1);

static const struct {
	u32  unk_00;
	u32  unk_04;
} sDAT_020627E0[] = {
	{ 1536 * 0x20, 7 },
	{ 1552 * 0x20, 8 },
	{ 1568 * 0x20, 9 },
};

BOOL BoxItemIcon_02037A18(BoxItemIconManager** param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4) {
	BoxItemIconManager* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxItemIconManager));
	if (v0 != NULL) {
		v0->unk_04 = param1;
		v0->unk_08 = param2;
		v0->unk_0C = param3;
		v0->unk_10 = param4;
		v0->unk_14 = BoxGraphics_02030880(param1);
		v0->unk_18 = BoxGraphics_020308A0(param1);
		v0->unk_34 = 0;
		v0->unk_00 = (BoxApp_GetBoxMode(param2) == PC_MODE_MOVE_ITEMS);
		
		BoxItemIcon_02037A80(v0);
		*param0 = v0;
		
		return TRUE;
	}
	
	return FALSE;
}


void BoxItemIcon_02037A70(BoxItemIconManager* param0) {
	BoxItemIcon_02037B4C(param0);
	Heap_Free(param0);
}


static void BoxItemIcon_02037A80(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		SpriteResourcesHeader v0;
		NNSG2dImageProxy v1;
		NNSG2dCharacterData* v2;
		
		BoxGraphics_020308E8(&v0, &v1, BoxGraphics_02030878(param0->unk_04), param0->unk_14->unk_1C, param0->unk_14->unk_20, 0);
		
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_ICON;
		} else {
			narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__ITEM_ICON;
		}
		void* v3 = Graphics_GetCharData(narc, Item_GetFileID(1, ITEM_FILE_TYPE_ICON), 0, &v2, HEAP_ID_BOX_GRAPHICS);
		if (v3 != NULL) {
			v2->mappingType = GX_GetOBJVRamModeChar();
			
			for (int i = 0; i < 3; i++) {
				NNS_G2dInitImageProxy(&v1);
				NNS_G2dLoadImage1DMapping(v2, sDAT_020627E0[i].unk_00, NNS_G2D_VRAM_TYPE_2DMAIN, &v1);
				
				param0->unk_1C[i] = BoxGraphics_02030908(param0->unk_10, &v0, 0, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN);
				
				Sprite_SetExplicitPalette(param0->unk_1C[i], sDAT_020627E0[i].unk_04);
				Sprite_SetDrawFlag(param0->unk_1C[i], FALSE);
				Sprite_SetAffineOverwriteMode(param0->unk_1C[i], 1);
				
				param0->unk_28[i] = 0;
			}
			
			Heap_Free(v3);
		}
	} else {
		for (int i = 0; i < 3; i++) {
			param0->unk_1C[i] = NULL;
			param0->unk_28[i] = 0;
		}
	}
}


static void BoxItemIcon_02037B4C(BoxItemIconManager* param0) {
	for (int i = 0; i < 3; i++) {
		if (param0->unk_1C[i]) {
			Sprite_Delete(param0->unk_1C[i]);
		}
	}
}


static int BoxItemIcon_02037B68(BoxItemIconManager* param0, int param1) {
	for (int i = 0; i < 3; i++) {
		if (param0->unk_28[i] == 0) {
			param0->unk_28[i] = param1;
			return i;
		}
	}
	
	return -1;
}


static int BoxItemIcon_02037B90(BoxItemIconManager* param0, int param1) {
	for (int i = 0; i < 3; i++) {
		if (param0->unk_28[i] == param1) {
			return i;
		}
	}
	
	return -1;
}


static void BoxItemIcon_02037BAC(BoxItemIconManager* param0, int param1, int param2) {
	param0->unk_28[param1] = param2;
}


void BoxItemIcon_02037BB4(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		BoxItemIcon_02037BCC(param0);
		BoxItemIcon_02037C04(param0);
	}
}


static void BoxItemIcon_02037BCC(BoxItemIconManager* param0) {
	u32 v0 = BoxApp_GetPreviewedMonHeldItem(param0->unk_08);
	if (v0 != 0) {
		int v1 = BoxItemIcon_02037B68(param0, 1);
		BoxItemIcon_02037D20(param0, v1, v0);
		Sprite_SetAnim(param0->unk_1C[v1], 6);
		Sprite_SetDrawFlag(param0->unk_1C[v1], TRUE);
	}
}


static void BoxItemIcon_02037C04(BoxItemIconManager* param0) {
	u32 item = BoxApp_GetCursorItem(param0->unk_08);
	if (item != ITEM_NONE) {
		VecFx32 v1;
		u32 v2;
		int v3 = BoxItemIcon_02037B68(param0, 3);
		
		BoxCursor_02032F24(param0->unk_18, &v1);
		
		//v1.x += (0 << FX32_SHIFT); /* No effect */
		v1.y += (8 << FX32_SHIFT);
		
		if (BoxApp_GetCursorLocation(param0->unk_08) == CURSOR_IN_PARTY) {
			v2 = 1;
		} else {
			v2 = 2;
		}
		
		// Present in Platinum. Why is this missing?
		//Graphics_LoadObjectTiles(16, Item_GetFileID(item, ITEM_FILE_TYPE_ICON), 0, sDAT_020627E0[v3].unk_00, 0, 0, HEAP_ID_BOX_GRAPHICS);
		
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_ICON;
		} else {
			narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__ITEM_ICON;
		}
		Graphics_LoadPalette(narc, Item_GetFileID(item, ITEM_FILE_TYPE_PALETTE), 1, sDAT_020627E0[v3].unk_04 * 0x20, 0x20, HEAP_ID_BOX_GRAPHICS);
		Sprite_SetExplicitPriority(param0->unk_1C[v3], v2);
		
		BoxGraphics_SetSpritePriority(param0->unk_1C[v3], 2);
		
		Sprite_SetPosition(param0->unk_1C[v3], &v1);
		Sprite_SetAnim(param0->unk_1C[v3], 0);
		Sprite_SetDrawFlag(param0->unk_1C[v3], TRUE);
	}
}


static void BoxItemIcon_02037C9C(BoxItemIconManager* param0, VecFx32* param1, u32* param2) {
	param1->z = 0;
	
	if (BoxApp_GetCursorLocation(param0->unk_08) == CURSOR_IN_PARTY) {
		s32 v0, v1;
		BoxParty_020361E4(BoxGraphics_02030894(param0->unk_04), BoxApp_GetCursorPartyPosition(param0->unk_08), &v0, &v1);
		
		param1->x = (v0 + 10) << FX32_SHIFT;
		param1->y = (v1 + 8) << FX32_SHIFT;
		*param2 = 1;
	} else {
		param1->x = 112 + BoxGraphics_020308CC(param0->unk_04) + BoxApp_GetCursorBoxCol(param0->unk_08) * 24 + 10;
		param1->y = 40 + BoxApp_GetCursorBoxRow(param0->unk_08) * 24 + 8;
		param1->x <<= FX32_SHIFT;
		param1->y <<= FX32_SHIFT;
		*param2 = 2;
	}
}


static void BoxItemIcon_02037D20(BoxItemIconManager* param0, int param1, int param2) {
	if (param1 >= 0) {
		VecFx32 v0;
		u32 v1;
		BoxItemIcon_02037C9C(param0, &v0, &v1);
		
		Graphics_LoadObjectTiles(16, Item_GetFileID(param2, ITEM_FILE_TYPE_ICON), 0, sDAT_020627E0[param1].unk_00, 0, 0, HEAP_ID_BOX_GRAPHICS);
		
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_ITEMTOOL__ITEMDATA__ITEM_ICON;
		} else {
			narc = NARC_INDEX_PL_ITEMTOOL__ITEMDATA__ITEM_ICON;
		}
		Graphics_LoadPalette(narc, Item_GetFileID(param2, ITEM_FILE_TYPE_PALETTE), 1, sDAT_020627E0[param1].unk_04 * 0x20, 0x20, HEAP_ID_BOX_GRAPHICS);
		Sprite_SetExplicitPriority(param0->unk_1C[param1], v1);
		
		BoxGraphics_SetSpritePriority(param0->unk_1C[param1], 3);
		Sprite_SetPosition(param0->unk_1C[param1], &v0);
	}
}


void BoxItemIcon_02037DA8(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0) {
			BoxItemIcon_02037BAC(param0, v0, 2);
			Sprite_SetAnim(param0->unk_1C[v0], 2);
		}
		
		int v1 = BoxApp_GetPreviewedMonHeldItem(param0->unk_08);
		if (v1 != 0) {
			v0 = BoxItemIcon_02037B68(param0, 1);
			if (v0 >= 0) {
				BoxItemIcon_02037D20(param0, v0, v1);
				Sprite_SetAnim(param0->unk_1C[v0], 1);
				Sprite_SetDrawFlag(param0->unk_1C[v0], TRUE);
			}
		}
	}
}


BOOL BoxItemIcon_02037E08(BoxItemIconManager* param0) {
	if (param0->unk_00 == FALSE) {
		return TRUE;
	} else {
		int v0 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0) {
			if (Sprite_IsAnimated(param0->unk_1C[v0])) {
				return FALSE;
			}
		}
		
		v0 = BoxItemIcon_02037B90(param0, 2);
		if (v0 >= 0) {
			if (Sprite_IsAnimated(param0->unk_1C[v0])) {
				return FALSE;
			}
			
			BoxItemIcon_02037BAC(param0, v0, 0);
		}
		
		return TRUE;
	}
}


static void BoxItemIcon_02037E60(BoxItemIconManager* param0, int param1, const VecFx32* param2, int param3, int param4) {
	BoxItemIcon_UnkStruct0* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxItemIcon_UnkStruct0));
	if (v0 != NULL) {
		v0->unk_00 = param0;
		v0->unk_04 = param0->unk_1C[param1];
		v0->unk_08 = *(Sprite_GetPosition(v0->unk_04));
		v0->unk_14 = *param2;
		v0->unk_20 = (v0->unk_14.x - v0->unk_08.x) / param4;
		v0->unk_24 = (v0->unk_14.y - v0->unk_08.y) / param4;
		v0->unk_2C = param4;
		v0->unk_28 = param3;
		v0->unk_30 = 0;
		
		if (BoxGraphics_02030870(BoxItemIcon_02037EE8, v0, 0)) {
			param0->unk_34++;
		} else {
			Heap_Free(v0);
		}
	}
}


static void BoxItemIcon_02037EE8(SysTask* param0, void* param1) {
	BoxItemIcon_UnkStruct0* v0 = param1;
	switch (v0->unk_30) {
		case 0:
			if (v0->unk_2C) {
				v0->unk_08.x += v0->unk_20;
				v0->unk_08.y += v0->unk_24;
				Sprite_SetPosition(v0->unk_04, &v0->unk_08);
				v0->unk_2C--;
			} else {
				Sprite_SetPosition(v0->unk_04, &v0->unk_14);
				v0->unk_30++;
			}
			break;
		
		case 1:
			if (Sprite_IsAnimated(v0->unk_04) == 0) {
				BoxGraphics_SetSpritePriority(v0->unk_04, v0->unk_28);
				v0->unk_00->unk_34--;
				SysTask_Done(param0);
				BoxGraphics_02030A64(v0, NULL, NULL);
			}
			break;
	}
}


void BoxItemIcon_02037F5C(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0) {
			VecFx32 v1;
			BoxCursor_02032F24(param0->unk_18, &v1);
			
			v1.x += (0 << FX32_SHIFT);
			v1.y += (8 << FX32_SHIFT);
			
			BoxItemIcon_02037BAC(param0, v0, 3);
			Sprite_SetAnim(param0->unk_1C[v0], 3);
			BoxItemIcon_02037E60(param0, v0, &v1, 2, 12);
		}
	}
}


BOOL BoxItemIcon_02037FB0(BoxItemIconManager* param0) {
	return (param0->unk_34 == 0);
}


void BoxItemIcon_02037FC0(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0) {
			BoxItemIcon_02037BAC(param0, v0, 2);
			Sprite_SetAnim(param0->unk_1C[v0], 2);
		}
	}
}


BOOL BoxItemIcon_02037FEC(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 2);
		if (v0 >= 0) {
			if (Sprite_IsAnimated(param0->unk_1C[v0])) {
				return FALSE;
			}
			
			BoxItemIcon_02037BAC(param0, v0, 0);
		}
	}
	
	return TRUE;
}


void BoxItemIcon_02038020(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 3);
		if (v0 >= 0) {
			VecFx32 v1;
			u32 v2;
			BoxItemIcon_02037C9C(param0, &v1, &v2);
			BoxItemIcon_02037BAC(param0, v0, 1);
			Sprite_SetAnim(param0->unk_1C[v0], 4);
			BoxItemIcon_02037E60(param0, v0, &v1, 3, 12);
		}
	}
}


BOOL BoxItemIcon_0203806C(BoxItemIconManager* param0) {
	return (param0->unk_34 == 0);
}


void BoxItemIcon_0203807C(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 3);
		int v1 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0 && v1 >= 0) {
			VecFx32 v2;
			u32 v3;
			BoxItemIcon_02037C9C(param0, &v2, &v3);
			BoxItemIcon_02037BAC(param0, v0, 1);
			Sprite_SetAnim(param0->unk_1C[v0], 4);
			
			BoxItemIcon_02037E60(param0, v0, &v2, 3, 12);
			BoxCursor_02032F24(param0->unk_18, &v2);
			
			v2.x += (0 << FX32_SHIFT);
			v2.y += (8 << FX32_SHIFT);
			
			BoxItemIcon_02037BAC(param0, v1, 3);
			Sprite_SetAnim(param0->unk_1C[v1], 3);
			BoxItemIcon_02037E60(param0, v1, &v2, 2, 12);
		}
	}
}


BOOL BoxItemIcon_02038110(BoxItemIconManager* param0) {
	return (param0->unk_34 == 0);
}


void BoxItemIcon_02038120(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 3);
		if (v0 >= 0) {
			Sprite_SetAnim(param0->unk_1C[v0], 5);
			param0->unk_38 = v0;
			return;
		}
		
		v0 = BoxItemIcon_02037B90(param0, 1);
		if (v0 >= 0) {
			Sprite_SetAnim(param0->unk_1C[v0], 2);
			param0->unk_38 = v0;
			return;
		}
		
		param0->unk_38 = -1;
	}
}


BOOL BoxItemIcon_02038168(BoxItemIconManager* param0) {
	if (param0->unk_00) {
		if (param0->unk_38 != -1) {
			if (Sprite_IsAnimated(param0->unk_1C[param0->unk_38])) {
				return FALSE;
			}
			
			BoxItemIcon_02037BAC(param0, param0->unk_38, 0);
			param0->unk_38 = -1;
		}
	}
	
	return TRUE;
}


void BoxItemIcon_020381A4(BoxItemIconManager* param0, const VecFx32* param1) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 3);
		if (v0 >= 0) {
			VecFx32 v1 = *param1;
			v1.x += (0 << FX32_SHIFT);
			v1.y += (8 << FX32_SHIFT);
			
			Sprite_SetPosition(param0->unk_1C[v0], &v1);
		}
	}
}


void BoxItemIcon_020381E4(BoxItemIconManager* param0, u32 param1) {
	if (param0->unk_00) {
		int v0 = BoxItemIcon_02037B90(param0, 3);
		if (v0 >= 0) {
			Sprite_SetExplicitPriority(param0->unk_1C[v0], param1);
		}
	}
}
