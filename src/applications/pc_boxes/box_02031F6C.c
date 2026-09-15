// File not documented

#include <nitro.h>

#include "applications/pc_boxes/box_02031F6C.h"

#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_02030AC4.h"
#include "applications/pc_boxes/box_02037A18.h"
#include "applications/pc_boxes/box_02035AAC.h"

#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "math_util.h"
#include "narc.h"
#include "sprite.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "constants/items.h"

typedef struct {
	Sprite* unk_00;
	u16 unk_04;
	u16 unk_06;
	u32 unk_08;
	fx32 unk_0C;
	fx32 unk_10;
	fx32 unk_14;
	fx32 unk_18;
	fx32 unk_1C;
	fx32 unk_20;
	fx32 unk_24;
	BoxCursorManager* unk_28;
} BoxCursor_UnkStruct;

static void BoxCursor_02032214(BoxCursorManager* param0);
static void BoxCursor_020322F4(const BoxCursorManager* param0, BOOL* param1, BOOL* param2);
static void BoxCursor_0203249C(SysTask* param0, void* param1);
static void BoxCursor_020326A0(SysTask* param0, void* param1);
static void BoxCursor_020327E4(SysTask* param0, void* param1);
static void BoxCursor_02032968(BoxCursorManager* param0, Sprite* param1, fx32 param2, s32 param3, u32 param4);
static BOOL BoxCursor_020329F4(BoxCursorManager* param0);
static void BoxCursor_02032A08(void* param0);
static void BoxCursor_02032A14(SysTask* param0, void* param1);
static void BoxCursor_02032B2C(SysTask* param0, void* param1);
static void BoxCursor_02032CA8(SysTask* param0, void* param1);
static void BoxCursor_02032F80(SysTask* param0, void* param1);
static void BoxCursor_02033020(SysTask* param0, void* param1);
static void BoxCursor_020330BC(SysTask* param0, void* param1);
static void BoxCursor_02033184(s32* param0, s32* param1, u8* param2, BoxCursorManager* param3);
static void BoxCursor_02033240(BoxCursorManager* param0, VecFx32* param1);
static void BoxCursor_020332EC(BoxCursorManager* param0);
static void BoxCursor_02033474(BoxCursorManager* param0);
static void BoxCursor_020334A8(BoxCursorManager* param0);
static void BoxCursor_0203354C(BoxCursorManager* param0);
static void BoxCursor_02033634(BoxCursorManager* param0);
static void BoxCursor_02033640(BoxCursorManager* param0);
static Sprite* BoxCursor_02033660(BoxCursorManager* param0);


BOOL BoxCursor_02031F6C(BoxCursorManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3) {
	param0->unk_794 = param1;
	param0->unk_790 = param2;
	param0->unk_00 = param3;
	param0->unk_758 = BoxGraphics_02030888(param1);
	param0->unk_75C = BoxGraphics_02030894(param1);
	param0->unk_48 = BoxGraphics_02030880(param1);
	param0->unk_785 = BoxApp_GetCursorLocation(param2);
	param0->unk_789 = BoxApp_GetCursorBoxCol(param2);
	param0->unk_78A = BoxApp_GetCursorBoxRow(param2);
	param0->unk_78B = BoxApp_GetCursorPartyPosition(param2);
	param0->unk_04 = NULL;
	param0->unk_08 = NULL;
	param0->unk_0C = NULL;
	param0->unk_10 = NULL;
	param0->unk_754 = 0;
	param0->unk_78E = 1;
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	param0->unk_14 = Graphics_GetCellBank(narc, 13, TRUE, &(param0->unk_1C), HEAP_ID_BOX_GRAPHICS);
	param0->unk_18 = Graphics_GetAnimBank(narc, 14, TRUE, &(param0->unk_20), HEAP_ID_BOX_GRAPHICS);
	
	if ((param0->unk_14 == NULL) || (param0->unk_18 == NULL)) {
		return FALSE;
	}
	
	return TRUE;
}


void BoxCursor_02032034(BoxCursorManager* param0) {
	SpriteResourcesHeader v0;
	u32 cursorLocation, pcMode;
	
	NNS_G2dInitImageProxy(&(param0->unk_24));
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	Graphics_LoadImageMapping(narc, 12, TRUE, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 0 * 0x20, 10, &(param0->unk_24));
	BoxGraphics_020308E8(&v0, &(param0->unk_24), BoxGraphics_02030878(param0->unk_794), param0->unk_1C, param0->unk_20, ((param0->unk_785 == 1) ? 1 : 2));
	BoxCursor_02033184(&param0->unk_764, &param0->unk_768, &param0->unk_785, param0);
	
	param0->unk_04 = BoxGraphics_02030908(param0->unk_00, &v0, param0->unk_764, param0->unk_768, 1, NNS_G2D_VRAM_TYPE_2DMAIN);
	param0->unk_08 = BoxGraphics_02030908(param0->unk_00, &v0, param0->unk_764, param0->unk_768 + 24, 42, NNS_G2D_VRAM_TYPE_2DMAIN);
	
	cursorLocation = BoxApp_GetCursorLocation(param0->unk_790);
	pcMode = BoxApp_GetBoxMode(param0->unk_790);
	
	int v3, v4;
	if (pcMode != PC_MODE_COMPARE) {
		v3 = 108;
		v4 = 236;
	} else {
		v3 = 108 + -44;
		v4 = 236 + -44;
	}
	
	v0.priority = 2;
	param0->unk_0C = BoxGraphics_02030908(param0->unk_00, &v0, v3, 20, 43, NNS_G2D_VRAM_TYPE_2DMAIN);
	param0->unk_10 = BoxGraphics_02030908(param0->unk_00, &v0, v4, 20, 43, NNS_G2D_VRAM_TYPE_2DMAIN);
	
	Sprite_SetAnim(param0->unk_08, 5);
	Sprite_SetExplicitPriority(param0->unk_08, 2);
	
	cursorLocation = BoxApp_GetCursorLocation(param0->unk_790);
	
	if (cursorLocation != CURSOR_ON_BOX_HEADER) {
		Sprite_SetAnim(param0->unk_0C, 6);
		Sprite_SetAnim(param0->unk_10, 7);
	} else {
		Sprite_SetAnim(param0->unk_0C, 8);
		Sprite_SetAnim(param0->unk_10, 9);
	}
	
	if (cursorLocation != CURSOR_IN_BOX) {
		Sprite_SetDrawFlag(param0->unk_08, FALSE);
	}
	
	if (pcMode != PC_MODE_MOVE_ITEMS) {
		if (BoxApp_GetPreviewMonSource(param0->unk_790) == PREVIEW_MON_UNDER_CURSOR) {
			Sprite_SetAnim(param0->unk_04, 0);
			BoxIcon_020337B8(param0->unk_48, param0->unk_4C, (5 * 6));
			param0->unk_78C = 0;
		} else {
			Sprite_SetAnim(param0->unk_04, 2);
			BoxCursor_02032214(param0);
			param0->unk_78C = 1;
		}
		
		param0->unk_78D = 0;
	} else {
		if (BoxApp_GetCursorItem(param0->unk_790) == ITEM_NONE) {
			Sprite_SetAnim(param0->unk_04, 0);
			param0->unk_78D = 0;
		} else {
			Sprite_SetAnim(param0->unk_04, 2);
			param0->unk_78D = 1;
		}
		
		BoxIcon_020337B8(param0->unk_48, param0->unk_4C, (5 * 6));
		param0->unk_78C = 0;
	}
	
	param0->unk_760 = BoxGraphics_020308AC(param0->unk_794);
	
	BoxCursor_02032EF4(param0);
}


static void BoxCursor_02032214(BoxCursorManager* param0) {
	VecFx32 v0 = *(Sprite_GetPosition(param0->unk_04));
	s32 v1 = (v0.x >> FX32_SHIFT) - 0;
	s32 v2 = (v0.y >> FX32_SHIFT) - -4;
	
	BoxIcon_020337B8(param0->unk_48, param0->unk_4C, 1);
	BoxIcon_02033814(param0->unk_48, BoxApp_GetPreviewedBoxMon(param0->unk_790), v1, v2, Sprite_GetExplicitPriority(param0->unk_04), 2, 1240, param0->unk_4C);
	
	param0->unk_664[0].unk_00 = (0 * FX32_ONE);
	param0->unk_664[0].unk_04 = (-4 * FX32_ONE);
	param0->unk_754 = 1;
	param0->unk_78C = 1;
}


void BoxCursor_02032298(BoxCursorManager* param0) {
	if (param0->unk_14) {
		Heap_Free(param0->unk_14);
		param0->unk_14 = NULL;
	}
	
	if (param0->unk_18) {
		Heap_Free(param0->unk_18);
		param0->unk_18 = NULL;
	}
	
	if (param0->unk_04 != NULL) {
		Sprite_Delete(param0->unk_04);
		param0->unk_04 = NULL;
	}
	
	if (param0->unk_08 != NULL) {
		Sprite_Delete(param0->unk_08);
		param0->unk_08 = NULL;
	}
	
	if (param0->unk_0C != NULL) {
		Sprite_Delete(param0->unk_0C);
		param0->unk_0C = NULL;
	}
	
	if (param0->unk_10 != NULL) {
		Sprite_Delete(param0->unk_10);
		param0->unk_10 = NULL;
	}
}


static void BoxCursor_020322F4(const BoxCursorManager* param0, BOOL* param1, BOOL* param2) {
	*param1 = *param2 = 0;
	
	switch (param0->unk_785) {
		case 0:
			if (param0->unk_786 == 0) {
				u32 col = BoxApp_GetCursorBoxCol(param0->unk_790);
				
				if ((param0->unk_789 == 0) && (col == 5) || (param0->unk_789 == 5) && (col == 0)) {
					*param1 = 1;
				}
			}
			break;
		
		case 2:
			if ((param0->unk_786 == 4) || (param0->unk_786 == 3)) {
				*param2 = 1;
			}
			break;
		
		case 4:
		case 3:
			if (param0->unk_786 == 2) {
				*param2 = 1;
			}
			break;
	}
}


void BoxCursor_02032374(BoxCursorManager* param0) {
	BOOL v0, v1;
	s32 v2, v3;
	s32 v4, v5;
	s32 v6;
	
	BoxCursor_02033184(&(param0->unk_76C), &(param0->unk_770), &(param0->unk_786), param0);
	BoxCursor_020322F4(param0, &v0, &v1);
	
	if (param0->unk_786 != 0) {
		Sprite_SetDrawFlag(param0->unk_08, FALSE);
	}
	
	param0->unk_774 = param0->unk_764 << FX32_SHIFT;
	param0->unk_778 = param0->unk_768 << FX32_SHIFT;
	
	v2 = param0->unk_76C << FX32_SHIFT;
	v3 = param0->unk_770 << FX32_SHIFT;
	
	if (!v0) {
		v4 = v2 - param0->unk_774;
	} else {
		if (v2 > param0->unk_774) {
			v4 = -((272 - param0->unk_76C) + (param0->unk_764 - 70));
		} else {
			v4 = (272 - param0->unk_764) + (param0->unk_76C - 70);
		}
		
		v4 <<= FX32_SHIFT;
	}
	
	if (!v1) {
		v5 = v3 - param0->unk_778;
	} else {
		if (v3 > param0->unk_778) {
			v5 = -((208 - param0->unk_770) + (param0->unk_768 - -24));
		} else {
			v5 = (208 - param0->unk_768) + (param0->unk_770 - -24);
		}
		
		v5 <<= FX32_SHIFT;
	}
	
	if (v0 || v1) {
		v6 = 12;
	} else {
		v6 = 6;
	}
	
	param0->unk_77C = v4 / v6;
	param0->unk_780 = v5 / v6;
	param0->unk_787 = v6;
	
	BoxGraphics_02030870(BoxCursor_0203249C, param0, 0);
}


BOOL BoxCursor_02032488(BoxCursorManager* param0) {
	return (param0->unk_787 == 0);
}


static void BoxCursor_0203249C(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = (BoxCursorManager*)param1;
	VecFx32 v1;
	v1.z = 0;
	
	if (v0->unk_787) {
		v0->unk_774 += v0->unk_77C;
		
		if ((v0->unk_77C > 0) && (v0->unk_774 > 0x110000)) {
			v0->unk_774 = 0x46000 + (0x110000 - v0->unk_774);
		}
		
		if ((v0->unk_77C < 0) && (v0->unk_774 < 0x46000)) {
			v0->unk_774 = 0x110000 - (0x46000 - v0->unk_774);
		}
		
		v0->unk_778 += v0->unk_780;
		
		if ((v0->unk_780 > 0) && (v0->unk_778 > 0xD0000)) {
			v0->unk_778 = -0x18000 + (0xD0000 - v0->unk_778);
		}
		
		if ((v0->unk_780 < 0) && (v0->unk_778 < -0x18000)) {
			v0->unk_778 = 0xD0000 - (-0x18000 - v0->unk_778);
		}
		
		v1.x = v0->unk_774;
		v1.y = v0->unk_778;
		v1.z = 0;
		
		BoxCursor_02033240(v0, &v1);
		
		v0->unk_787--;
		return;
	}

	v1.x = v0->unk_76C << FX32_SHIFT;
	v1.y = v0->unk_770 << FX32_SHIFT;
	v1.z = 0;

	BoxCursor_02033240(v0, &v1);

	v0->unk_764 = v0->unk_76C;
	v0->unk_768 = v0->unk_770;
	v0->unk_785 = v0->unk_786;
	v0->unk_789 = BoxApp_GetCursorBoxCol(v0->unk_790);
	v0->unk_78A = BoxApp_GetCursorBoxRow(v0->unk_790);
	v0->unk_78B = BoxApp_GetCursorPartyPosition(v0->unk_790);

	if (v0->unk_786 == 0) {
		Sprite_SetDrawFlag(v0->unk_08, TRUE);
	}

	if (v0->unk_786 != 2) {
		Sprite_SetAnim(v0->unk_0C, 6);
		Sprite_SetAnim(v0->unk_10, 7);
	} else {
		Sprite_SetAnim(v0->unk_0C, 8);
		Sprite_SetAnim(v0->unk_10, 9);
	}

	SysTask_Done(param0);
}


void BoxCursor_02032644(BoxCursorManager* param0) {
	Sprite_SetAnim(param0->unk_04, 1);
	
	param0->unk_780 = (12 << FX32_SHIFT) / 6;
	param0->unk_770 = (param0->unk_768 + 12);
	param0->unk_787 = 6;
	param0->unk_784 = 0;
	
	BoxCursor_020332EC(param0);
	BoxGraphics_02030870(BoxCursor_020326A0, param0, 0);
}


BOOL BoxCursor_0203268C(BoxCursorManager* param0) {
	return (param0->unk_784 == 255);
}


static void BoxCursor_020326A0(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = (BoxCursorManager*)param1;
	VecFx32 v1 = *(Sprite_GetPosition(v0->unk_04));
	
	switch (v0->unk_784) {
		case 0:
			v0->unk_78E = 0;
			
			if (v0->unk_787) {
				v1.y += v0->unk_780;
				BoxCursor_02033240(v0, &v1);
				v0->unk_787--;
				break;
			}
			
			v1.y = v0->unk_770 << FX32_SHIFT;
			
			BoxCursor_02033240(v0, &v1);
			Sprite_SetAnim(v0->unk_04, 2);
			BoxCursor_02033474(v0);
			
			v0->unk_787 = 6;
			v0->unk_784++;
			// Fall through
		
		case 1:
			if (v0->unk_787) {
				v1.y -= v0->unk_780;
				BoxCursor_02033240(v0, &v1);
				v0->unk_787--;
				break;
			}
			
			v1.y = v0->unk_768 << FX32_SHIFT;
			BoxCursor_02033240(v0, &v1);
			v0->unk_784 = 255;
			v0->unk_78E = 1;
			SysTask_Done(param0);
	}
}


void BoxCursor_0203277C(BoxCursorManager* param0) {
	param0->unk_780 = (12 << FX32_SHIFT) / 6;
	param0->unk_770 = (param0->unk_768 + 12);
	param0->unk_787 = 6;
	param0->unk_784 = 0;
	
	BoxGraphics_02030870(BoxCursor_020327E4, param0, 0);
}


BOOL BoxCursor_020327B0(BoxCursorManager* param0) {
	if (param0->unk_784 == 255) {
		BoxCursor_020334A8(param0);
		BoxIcon_020337B8(param0->unk_48, param0->unk_4C, param0->unk_754);
		param0->unk_754 = 0;
		return TRUE;
	}
	
	return FALSE;
}


static void BoxCursor_020327E4(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = (BoxCursorManager*)param1;
	VecFx32 v1 = *(Sprite_GetPosition(v0->unk_04));
	
	switch (v0->unk_784) {
		case 0:
			v0->unk_78E = 0;
			
			if (v0->unk_787) {
				v1.y += v0->unk_780;
				BoxCursor_02033240(v0, &v1);
				v0->unk_787--;
				break;
			}
			
			v1.y = v0->unk_770 << FX32_SHIFT;
			
			BoxCursor_02033240(v0, &v1);
			Sprite_SetAnim(v0->unk_04, 1);
			BoxCursor_02033634(v0);
			
			v0->unk_787 = 6;
			v0->unk_784++;
			// Fall through
		
		case 1:
			if (v0->unk_787) {
				v1.y -= v0->unk_780;
				BoxCursor_02033240(v0, &v1);
				v0->unk_787--;
				break;
			}
			
			v1.y = v0->unk_768 << FX32_SHIFT;
			
			BoxCursor_02033240(v0, &v1);
			Sprite_SetAnim(v0->unk_04, 0);
			
			v0->unk_784 = 255;
			v0->unk_78E = 1;
			
			SysTask_Done(param0);
	}
}


void BoxCursor_020328C8(BoxCursorManager* param0) {
	VecFx32 v0;
	Sprite* v1;
	
	Sprite_SetAnim(param0->unk_04, 1);
	
	param0->unk_788 = 0;
	
	v1 = BoxCursor_02033660(param0);
	v0 = *(Sprite_GetPosition(v1));
	BoxCursor_02032968(param0, param0->unk_4C->unk_00, v0.y, 1, Sprite_GetPriority(v1));
	
	v0 = *(Sprite_GetPosition(param0->unk_4C->unk_00));
	BoxCursor_02032968(param0, v1, v0.y, -1, Sprite_GetPriority(param0->unk_4C->unk_00));
	
	param0->unk_784 = 0;
}


BOOL BoxCursor_02032944(BoxCursorManager* param0) {
	if (BoxCursor_020329F4(param0)) {
		Sprite_SetAnim(param0->unk_04, 2);
		BoxCursor_0203354C(param0);
		return TRUE;
	}
	
	return FALSE;
}


static void BoxCursor_02032968(BoxCursorManager* param0, Sprite* param1, fx32 param2, s32 param3, u32 param4) {
	BoxCursor_UnkStruct* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxCursor_UnkStruct));
	if (v0) {
		VecFx32 v1 = *(Sprite_GetPosition(param1));
		
		v0->unk_00 = param1;
		v0->unk_04 = 12;
		v0->unk_06 = 12 / 2;
		v0->unk_08 = param4;
		v0->unk_0C = v1.y;
		v0->unk_14 = v1.x;
		v0->unk_10 = param2;
		v0->unk_18 = 0;
		v0->unk_24 = param3 * 12;
		v0->unk_1C = (v0->unk_10 - v0->unk_0C) / 12;
		v0->unk_20 = (180 << FX32_SHIFT) / 12;
		v0->unk_28 = param0;
		
		if (BoxGraphics_02030870(BoxCursor_02032A14, v0, 0)) {
			param0->unk_788 = 1;
			return;
		} else {
			Heap_Free(v0);
		}
	}
}


static BOOL BoxCursor_020329F4(BoxCursorManager* param0) {
	return (param0->unk_788 == 0);
}


static void BoxCursor_02032A08(void* param0) {
	BoxCursorManager* v0 = param0;
	v0->unk_788 = 0;
}


static void BoxCursor_02032A14(SysTask* param0, void* param1) {
	BoxCursor_UnkStruct* v0 = (BoxCursor_UnkStruct*)param1;
	VecFx32 v1;
	
	v1 = *(Sprite_GetPosition(v0->unk_00));
	
	if (v0->unk_04) {
		fx32 v2;
		
		v0->unk_04--;
		
		if (v0->unk_04 == v0->unk_06) {
			BoxGraphics_SetSpritePriority(v0->unk_00, v0->unk_08);
		}
		
		v0->unk_0C += v0->unk_1C;
		v1.y = v0->unk_0C;
		v0->unk_18 += v0->unk_20;
		v2 = v0->unk_24 * CalcSineDegrees(v0->unk_18 >> FX32_SHIFT);
		v1.x = v0->unk_14 + v2;
		
		Sprite_SetPosition(v0->unk_00, &v1);
	} else {
		v1.x = v0->unk_14;
		v1.y = v0->unk_10;
		
		Sprite_SetPosition(v0->unk_00, &v1);
		BoxGraphics_02030A64(param1, BoxCursor_02032A08, v0->unk_28);
		SysTask_Done(param0);
	}
}


void BoxCursor_02032AA4(BoxCursorManager* param0) {
	VecFx32 v0;
	
	if ((param0->unk_78C == 0) && (param0->unk_78D == 0)) {
		Sprite_SetAnim(param0->unk_04, 3);
	}
	
	v0 = *(Sprite_GetPosition(param0->unk_04));
	v0.y += (-2 * FX32_ONE);
	
	BoxCursor_02033240(param0, &v0);
	
	param0->unk_784 = 0;
	param0->unk_787 = 0;
	
	if (BoxGraphics_02030870(BoxCursor_02032B2C, param0, 0)) {
		param0->unk_788 = 1;
	}
}


BOOL BoxCursor_02032B18(BoxCursorManager* param0) {
	return (param0->unk_788 == 0);
}


static void BoxCursor_02032B2C(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = (BoxCursorManager*)param1;
	
	switch (v0->unk_784) {
		case 0:
			if (++(v0->unk_787) >= 4) {
				VecFx32 v1 = *(Sprite_GetPosition(v0->unk_04));

				v0->unk_770 = (v1.y >> FX32_SHIFT) + 6;
				v0->unk_780 = (6 * FX32_ONE) / 2;
				v0->unk_787 = 2;

				if ((v0->unk_78C == 0) && (v0->unk_78D == 0)) {
					Sprite_SetAnim(v0->unk_04, 4);
				}

				v0->unk_784++;
			}
			break;
		case 1:
			{
				VecFx32 v2 = *(Sprite_GetPosition(v0->unk_04));
				
				if (v0->unk_787) {
					v2.y += v0->unk_780;
					BoxCursor_02033240(v0, &v2);
					v0->unk_787--;
				} else {
					v2.y = v0->unk_770 << FX32_SHIFT;
					BoxCursor_02033240(v0, &v2);
					v0->unk_788 = 0;
					SysTask_Done(param0);
				}
			}
			break;
	}
}


void BoxCursor_02032C10(BoxCursorManager* param0) {
	VecFx32 v0 = *(Sprite_GetPosition(param0->unk_04));
	
	param0->unk_770 = (v0.y >> FX32_SHIFT) + -4;
	param0->unk_780 = (-4 * FX32_ONE) / 4;
	param0->unk_787 = 4;
	param0->unk_784 = 0;
	
	if (BoxGraphics_02030870(BoxCursor_02032CA8, param0, 0)) {
		param0->unk_788 = 1;
	}
}


BOOL BoxCursor_02032C6C(BoxCursorManager* param0) {
	if (param0->unk_788 == 0) {
		if ((param0->unk_78C == 0) && (param0->unk_78D == 0)) {
			if (Sprite_GetActiveAnim(param0->unk_04) != 0) {
				Sprite_SetAnim(param0->unk_04, 0);
			}
		}
		
		return TRUE;
	}

	return FALSE;
}


static void BoxCursor_02032CA8(SysTask* param0, void* param1) {
	BoxCursorManager* v0;
	VecFx32 v1;
	
	v0 = (BoxCursorManager*)param1;
	v1 = *(Sprite_GetPosition(v0->unk_04));
	
	if (v0->unk_787) {
		v1.y += v0->unk_780;
		BoxCursor_02033240(v0, &v1);
		v0->unk_787--;
	} else {
		v1.y = v0->unk_770 << FX32_SHIFT;
		BoxCursor_02033240(v0, &v1);
		v0->unk_788 = 0;
		SysTask_Done(param0);
	}
}


void BoxCursor_02032D14(BoxCursorManager* param0) {
	if (param0->unk_790->selectedBoxID == BoxApp_GetCurrentBox(param0->unk_790)) {
		BoxPanel_02031720(param0->unk_758);
	}
	
	BoxCursor_02033640(param0);
	Sprite_SetAnim(param0->unk_04, 0);
}


void BoxCursor_02032D4C(BoxCursorManager* param0) {
	Sprite_SetAnim(param0->unk_04, 1);
	BoxIcon_02033AF4(param0->unk_48, param0->unk_4C, 1);
	
	param0->unk_784 = 0;
}


BOOL BoxCursor_02032D70(BoxCursorManager* param0) {
	const BoxApplicationManager* v0 = BoxGraphics_02030A10(param0->unk_794);
	
	switch (param0->unk_784) {
		case 0:
			if (BoxIcon_02033B1C(param0->unk_48, param0->unk_4C)) {
				param0->unk_784++;
			}
			break;
		
		case 1:
			if (BoxAppMan_HasCheckedCanReleaseMon(v0)) {
				if (BoxAppMan_CanReleaseMon(v0)) {
					Sprite_SetAnim(param0->unk_04, 0);
					BoxCursor_02033640(param0);
					return TRUE;
				} else {
					BoxIcon_02033AF4(param0->unk_48, param0->unk_4C, 2);
					param0->unk_784++;
				}
			}
			break;
		
		case 2:
			if (BoxIcon_02033B1C(param0->unk_48, param0->unk_4C)) {
				Sprite_SetAnim(param0->unk_04, 2);
				return TRUE;
			}

			break;
	}

	return FALSE;
}


void BoxCursor_02032E08(BoxCursorManager* param0) {
	Sprite_SetExplicitPriority(param0->unk_04, 1);
	
	if (param0->unk_78C) {
		Sprite_SetExplicitPriority(param0->unk_4C->unk_00, 1);
	}
	
	if (param0->unk_78D) {
		BoxItemIcon_020381E4(param0->unk_760, 1);
	}
}


void BoxCursor_02032E40(BoxCursorManager* param0) {
	Sprite_SetExplicitPriority(param0->unk_04, 2);
	
	if (param0->unk_78C) {
		Sprite_SetExplicitPriority(param0->unk_4C->unk_00, 2);
	}
	
	if (param0->unk_78D) {
		BoxItemIcon_020381E4(param0->unk_760, 2);
	}
}


void BoxCursor_02032E78(BoxCursorManager* param0) {
	if (param0->unk_78C) {
		u32 v0 = BoxApp_GetPreviewedMonMarkings(param0->unk_790);
		BoxIcon_02033A78(param0->unk_48, param0->unk_4C, v0);
	}
}


void BoxCursor_02032EA0(BoxCursorManager* param0) {
	u32 v0 = BoxApp_GetMonSpriteTransparencyMask(param0->unk_790);
	
	if (param0->unk_4C->unk_00 != NULL) {
		BoxIcon_02033A28(param0->unk_48, param0->unk_4C, v0);
	}
}


void BoxCursor_02032EC4(BoxCursorManager* param0, u32 param1) {
	if (param0->unk_4C->unk_00 != NULL) {
		BoxIcon_02033A7C(param0->unk_48, param0->unk_4C, param1);
		BoxIcon_02033A28(param0->unk_48, param0->unk_4C, BoxApp_GetMonSpriteTransparencyMask(param0->unk_790));
	}
}


void BoxCursor_02032EF4(BoxCursorManager* param0) {
	if (BoxApp_IsCursorFastMode(param0->unk_790)) {
		Sprite_SetExplicitPalette(param0->unk_04, 1);
	} else {
		Sprite_SetExplicitPalette(param0->unk_04, 0);
	}
}


void BoxGraphics_OpenMultiSelectCursor(BoxCursorManager* param0) {
	Sprite_SetAnim(param0->unk_04, 1);
}


void BoxCursor_02032F24(BoxCursorManager* param0, VecFx32* param1) {
	*param1 = *(Sprite_GetPosition(param0->unk_04));
}


void BoxCursor_02032F3C(BoxCursorManager* param0) {
	param0->unk_784 = 0;
	
	if (SysTask_Start(BoxCursor_02032F80, param0, 0)) {
		param0->unk_78D = 1;
		param0->unk_788 = 1;
	}
}


BOOL BoxCursor_02032F6C(BoxCursorManager* param0) {
	return (param0->unk_788 == 0);
}


static void BoxCursor_02032F80(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = param1;
	
	switch (v0->unk_784) {
		case 0:
			Sprite_SetAnim(v0->unk_04, 1);
			BoxItemIcon_02037F5C(v0->unk_760);
			v0->unk_784++;
			break;
		
		case 1:
			if (BoxItemIcon_02037FB0(v0->unk_760)) {
				Sprite_SetAnim(v0->unk_04, 2);
				v0->unk_788 = 0;
				SysTask_Done(param0);
			}
			break;
	}
}


void BoxCursor_02032FDC(BoxCursorManager* param0) {
	param0->unk_784 = 0;
	
	if (SysTask_Start(BoxCursor_02033020, param0, 0)) {
		param0->unk_78D = 0;
		param0->unk_788 = 1;
	}
}


BOOL BoxCursor_0203300C(BoxCursorManager* param0) {
	return (param0->unk_788 == 0);
}


static void BoxCursor_02033020(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = param1;
	
	switch (v0->unk_784) {
		case 0:
			Sprite_SetAnim(v0->unk_04, 1);
			BoxItemIcon_02038020(v0->unk_760);
			v0->unk_784++;
			break;
		
		case 1:
			if (BoxItemIcon_0203806C(v0->unk_760)) {
				Sprite_SetAnim(v0->unk_04, 0);
				v0->unk_788 = 0;
				SysTask_Done(param0);
			}
	}
}


void BoxCursor_0203307C(BoxCursorManager* param0) {
	param0->unk_784 = 0;
	
	if (SysTask_Start(BoxCursor_020330BC, param0, 0)) {
		param0->unk_788 = 1;
	}
}


BOOL BoxCursor_020330A8(BoxCursorManager* param0) {
	return (param0->unk_788 == 0);
}


static void BoxCursor_020330BC(SysTask* param0, void* param1) {
	BoxCursorManager* v0 = param1;
	
	switch (v0->unk_784) {
		case 0:
			Sprite_SetAnim(v0->unk_04, 1);
			BoxItemIcon_0203807C(v0->unk_760);
			v0->unk_784++;
			break;
		
		case 1:
			if (BoxItemIcon_02038110(v0->unk_760)) {
				Sprite_SetAnim(v0->unk_04, 2);
				v0->unk_788 = 0;
				SysTask_Done(param0);
			}
			break;
	}
}


void BoxCursor_02033118(BoxCursorManager* param0) {
	if (param0->unk_78D) {
		Sprite_SetAnim(param0->unk_04, 1);
	}
}


void BoxCursor_02033130(BoxCursorManager* param0) {
	if (param0->unk_78D) {
		Sprite_SetAnim(param0->unk_04, 0);
		param0->unk_78D = 0;
	}
}


void BoxCursor_02033150(BoxCursorManager* param0) {
	BOOL v0 = BoxApp_0202E9C8(param0->unk_790, -1);
	BOOL v1 = BoxApp_0202E9C8(param0->unk_790, 1);
	
	Sprite_SetDrawFlag(param0->unk_0C, v0);
	Sprite_SetDrawFlag(param0->unk_10, v1);
}


static void BoxCursor_02033184(s32* param0, s32* param1, u8* cursorLocation, BoxCursorManager* param3) {
	const BoxApplication* v0 = param3->unk_790;
	
	*cursorLocation = BoxApp_GetCursorLocation(v0);
	switch (*cursorLocation) {
		case CURSOR_IN_BOX:
			*param0 = 112 + BoxGraphics_020308CC(param3->unk_794) + BoxApp_GetCursorBoxCol(v0) * 24 + 0;
			*param1 = 40 + BoxApp_GetCursorBoxRow(v0) * 24 + -16;
			break;
		
		case CURSOR_IN_PARTY:
			{
				u32 partyIndex = BoxApp_GetCursorPartyPosition(v0);
				
				BoxParty_020361E4(BoxGraphics_02030894(param3->unk_794), partyIndex, param0, param1);
				(*param0) += 0;
				(*param1) += -16;
			}
			break;
		
		case CURSOR_ON_BOX_HEADER:
			*param0 = 168 + BoxGraphics_020308CC(param3->unk_794);
			*param1 = 8;
			break;
		
		case CURSOR_ON_PARTY_BUTTON:
			*param0 = 159 + BoxGraphics_020308CC(param3->unk_794);
			*param1 = 160;
			break;
		
		case CURSOR_ON_CLOSE_BUTTON:
			*param0 = 235 + BoxGraphics_020308CC(param3->unk_794);
			*param1 = 160;
			break;
	}
}


static void BoxCursor_02033240(BoxCursorManager* param0, VecFx32* param1) {
	Sprite_SetPosition(param0->unk_04, param1);
	
	if (param0->unk_78C) {
		VecFx32 v0;
		int v1;
		
		v0.z = 0;
		
		for (v1 = 0; v1 < param0->unk_754; v1++) {
			v0.x = param1->x + param0->unk_664[v1].unk_00;
			v0.y = param1->y - param0->unk_664[v1].unk_04;
			Sprite_SetPosition(param0->unk_4C[v1].unk_00, &v0);
		}
	}
	
	if (param0->unk_78D) {
		BoxItemIcon_020381A4(param0->unk_760, param1);
	}
	
	if (param0->unk_78E) {
		VecFx32 v2 =* param1;

		v2.y += (24 << FX32_SHIFT);
		Sprite_SetPosition(param0->unk_08, &v2);
	}
}


static void BoxCursor_020332EC(BoxCursorManager* param0) {
	if (BoxApp_GetCursorLocation(param0->unk_790) == CURSOR_IN_BOX) {
		if (BoxApp_IsMultiSelectSingleSelect(param0->unk_790)) {
			u32 posInBox = BoxApp_GetCursorBoxPosition(param0->unk_790);
			BoxPanel_02031650(param0->unk_758, posInBox, 1240, param0->unk_4C);
			param0->unk_754 = 1;
			param0->unk_664[0].unk_00 = (0 * FX32_ONE);
			param0->unk_664[0].unk_04 = (-4 * FX32_ONE);
		} else {
			u32 v1, v2, v3, v4, v5, v6, v7, col, row;
			
			BoxApp_GetMultiSelectBoundingBox(param0->unk_790, &v4, &v5, &v6, &v7);
			
			col = BoxApp_GetCursorBoxCol(param0->unk_790);
			row = BoxApp_GetCursorBoxRow(param0->unk_790);
			v1 = 0;
			
			for (v3 = v6; v3 <= v7; v3++) {
				for (v2 = v4; v2 <= v5; v2++) {
					BoxPanel_02031650(param0->unk_758, v3 * 6 + v2, 1520 + v1 * 16, &param0->unk_4C[v1]);

					if (param0->unk_4C[v1].unk_00 != NULL) {
						param0->unk_664[v1].unk_00 = (0 + (v2 - col) * 24) * FX32_ONE;
						param0->unk_664[v1].unk_04 = (-4 - (v3 - row) * 24) * FX32_ONE;
						v1++;
					}
				}
			}
			
			param0->unk_754 = v1;
		}
	} else {
		u32 partyIndex = BoxApp_GetCursorPartyPosition(param0->unk_790);
		
		BoxParty_020360F8(param0->unk_75C, partyIndex, 1240, param0->unk_4C);
		
		param0->unk_754 = 1;
		param0->unk_664[0].unk_00 = (0 * FX32_ONE);
		param0->unk_664[0].unk_04 = (-4 * FX32_ONE);
	}
}


static void BoxCursor_02033474(BoxCursorManager* param0) {
	for (int v0 = 0; v0 < param0->unk_754; v0++) {
		BoxGraphics_SetSpritePriority(param0->unk_4C[v0].unk_00, 2);
	}

	param0->unk_78C = 1;
}


static void BoxCursor_020334A8(BoxCursorManager* param0) {
	if (BoxApp_GetCursorLocation(param0->unk_790) == CURSOR_IN_BOX) {
		if (BoxApp_IsMultiSelectSingleSelect(param0->unk_790)) {
			u32 posInBox = BoxApp_GetCursorBoxPosition(param0->unk_790);
			BoxPanel_020316A4(param0->unk_758, posInBox, param0->unk_4C);
		} else {
			int v1, v2, v3;
			
			v2 = BoxApp_GetMultiSelectTopLeftPos(param0->unk_790);
			
			for (v1 = 0; v1 < param0->unk_754; v1++) {
				v3 = v2 + BoxApp_GetRelativeMonPosInMultiSelection(param0->unk_790, v1);
				BoxPanel_020316A4(param0->unk_758, v3, &param0->unk_4C[v1]);
			}
		}
	} else {
		u32 partyIndex = BoxApp_GetCursorPartyPosition(param0->unk_790);
		BoxParty_02036134(param0->unk_75C, partyIndex, param0->unk_4C);
	}
}


static void BoxCursor_0203354C(BoxCursorManager* param0) {
	if (BoxApp_GetCursorLocation(param0->unk_790) == CURSOR_IN_BOX) {
		u32 posInBox = BoxApp_GetCursorBoxPosition(param0->unk_790);
		
		BoxIcon* v1 = BoxPanel_02031708(param0->unk_758, posInBox);
		
		BoxIcon_02033A90(param0->unk_48, param0->unk_4C, 1520);
		BoxIcon_02033A90(param0->unk_48, v1, 1520 + 16);
		
		OS_WaitVBlankIntr();
		
		BoxPanel_02031650(param0->unk_758, posInBox, 1240, &param0->unk_4C[1]);
		BoxPanel_020316A4(param0->unk_758, posInBox, param0->unk_4C);
		BoxIcon_02033804(&(param0->unk_4C[1]), &(param0->unk_4C[0]), 1);
	} else {
		u32 v2 = BoxApp_GetCursorPartyPosition(param0->unk_790);
		
		BoxIcon* v3 = BoxParty_020361CC(param0->unk_75C, v2);
		
		BoxIcon_02033A90(param0->unk_48, param0->unk_4C, 1520);
		BoxIcon_02033A90(param0->unk_48, v3, 1520 + 16);
		
		OS_WaitVBlankIntr();
		
		BoxParty_020360F8(param0->unk_75C, v2, 1240, &param0->unk_4C[1]);
		BoxParty_02036188(param0->unk_75C, v2, param0->unk_4C);
		BoxIcon_02033804(&(param0->unk_4C[1]), &(param0->unk_4C[0]), 1);
	}
}


static void BoxCursor_02033634(BoxCursorManager* param0) {
	param0->unk_78C = 0;
}


static void BoxCursor_02033640(BoxCursorManager* param0) {
	BoxIcon_020337DC(param0->unk_48, param0->unk_4C, 1);
	param0->unk_78C = 0;
	param0->unk_754 = 0;
}


static Sprite* BoxCursor_02033660(BoxCursorManager* param0) {
	BoxIcon* v0;
	u32 cursorPosition;
	if (BoxApp_GetCursorLocation(param0->unk_790) == CURSOR_IN_BOX) {
		cursorPosition = BoxApp_GetCursorBoxPosition(param0->unk_790);
		v0 = BoxPanel_02031708(param0->unk_758, cursorPosition);
		return v0->unk_00;
	} else {
		cursorPosition = BoxApp_GetCursorPartyPosition(param0->unk_790);
		v0 = BoxParty_020361CC(param0->unk_75C, cursorPosition);
		return v0->unk_00;
	}
}
