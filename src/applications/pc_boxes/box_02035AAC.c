// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_02035AAC.h"

#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_02030AC4.h"

#include "assert.h"
#include "bg_window.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "narc.h"
#include "party.h"
#include "pokemon.h"
#include "sprite.h"
#include "sys_task.h"
#include "sys_task_manager.h"

static const struct {
	u16  unk_00;
	u16  unk_02;
} sDAT_02062548[] = {
	{ 144,  28 },
	{ 192,  36 },
	{ 144,  68 },
	{ 192,  76 },
	{ 144, 108 },
	{ 192, 116 },
};

static void BoxParty_02035BC4(SysTask* param0, void* param1);
static void BoxParty_02035C58(SysTask* param0, void* param1);
static void BoxParty_02035DA4(SysTask* param0, void* param1);
static void BoxParty_02035ED4(SysTask* param0, void* param1);
static void BoxParty_02036218(BoxPartyManager* param0, u32 param1);
static void BoxParty_02036268(BoxPartyManager* param0, VecFx32* param1);
static void BoxParty_02036304(BoxPartyManager* param0, fx32 param1);
static void BoxParty_0203636C(BoxPartyManager* param0, BOOL param1);
static void BoxParty_02036404(BoxPartyManager* param0);


BOOL BoxParty_02035AAC(BoxPartyManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4) {
	param0->unk_00 = param1;
	param0->unk_0C = param2;
	param0->unk_08 = param4;
	param0->unk_04 = param3;
	param0->unk_38 = BoxGraphics_02030880(param1);
	param0->unk_10 = BoxGraphics_02030888(param1);
	
	if (BoxApp_GetBoxMode(param2) != PC_MODE_COMPARE) {
		param0->unk_30 = 14;
		param0->unk_34 = 0;
	} else {
		param0->unk_30 = 9;
		param0->unk_34 = (9 - 14) * 8;
	}
	
	param0->unk_2B = 0;
	param0->unk_2C = 0;
	
	BoxIcon_020337B8(param0->unk_38, param0->unk_3C, 6);
	
	param0->unk_1C = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(VecFx32) * 6 * 8);
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	// TODO naix
	param0->unk_14 = Graphics_GetScrnData(narc, 6, 1, &(param0->unk_18), HEAP_ID_BOX_GRAPHICS);
	
	if ((param0->unk_14 == NULL) || (param0->unk_1C == NULL)) {
		return FALSE;
	}
	
	BoxParty_02036268(param0, param0->unk_1C);
	return TRUE;
}


void BoxParty_02035B3C(BoxPartyManager* param0) {
	if (param0->unk_14) {
		Heap_Free(param0->unk_14);
	}
	
	if (param0->unk_1C != NULL) {
		Heap_Free(param0->unk_1C);
	}
	
	BoxParty_02036404(param0);
}


void BoxParty_02035B5C(BoxPartyManager* param0) {
	param0->unk_2B = 0;
	BoxParty_02036218(param0, param0->unk_2B);
	BoxParty_0203636C(param0, 1);
	Bg_CopyTilemapBufferToVRAM(param0->unk_04, 2);
}


void BoxParty_02035B84(BoxPartyManager* param0) {
	BoxParty_0203636C(param0, 0);
	
	param0->unk_2B = 26;
	param0->unk_2C = (BoxGraphics_02030870(BoxParty_02035BC4, param0, 0) != NULL);
}


BOOL BoxParty_02035BB4(BoxPartyManager* param0) {
	return (param0->unk_2C == 0);
}


static void BoxParty_02035BC4(SysTask* param0, void* param1) {
	BoxPartyManager* v0 = (BoxPartyManager*)param1;
	fx32 v1;
	
	if (v0->unk_2B > 2) {
		v1 = -((2 * 8) << FX32_SHIFT);
		v0->unk_2B -= 2;
	} else {
		v1 = -((v0->unk_2B * 8) << FX32_SHIFT);
		v0->unk_2B = 0;
	}
	
	BoxParty_02036218(v0, v0->unk_2B);
	Bg_CopyTilemapBufferToVRAM(v0->unk_04, 2);
	BoxParty_02036304(v0, v1);
	
	if (v0->unk_2B == 0) {
		v0->unk_2C = 0;
		SysTask_Done(param0);
	}
}


void BoxParty_02035C24(BoxPartyManager* param0) {
	param0->unk_2C = (BoxGraphics_02030870(BoxParty_02035C58, param0, 0) != NULL);
}


BOOL BoxParty_02035C44(BoxPartyManager* param0) {
	if (param0->unk_2C == 0) {
		BoxParty_02036404(param0);
		return TRUE;
	}
	
	return FALSE;
}


static void BoxParty_02035C58(SysTask* param0, void* param1) {
	BoxPartyManager* v0 = (BoxPartyManager*)param1;
	fx32 v1;
	
	if (v0->unk_2B < (26 - 2)) {
		v1 = ((2 * 8) << FX32_SHIFT);
		v0->unk_2B += 2;
	} else {
		v1 = ((26 - v0->unk_2B) * 8) << FX32_SHIFT;
		v0->unk_2B = 26;
	}
	
	if (v0->unk_2B > 2) {
		u32 v2, v3;
		v3 = (u32)((v1) >> FX32_SHIFT) / 8;
		v2 = v0->unk_2B - 2 - v3;
		Bg_FillTilemapRect(v0->unk_04, 2, 0x0, v0->unk_30, v2, 15, v3, 17);
	}
	
	BoxParty_02036218(v0, v0->unk_2B);
	Bg_CopyTilemapBufferToVRAM(v0->unk_04, 2);
	BoxParty_02036304(v0, v1);
	
	if (v0->unk_2B == 26) {
		v0->unk_2C = 0;
		SysTask_Done(param0);
	}
}


void BoxParty_02035CF0(BoxPartyManager* param0) {
	int partyCount = Party_GetCurrentCount(param0->unk_0C->party);
	if (param0->unk_28 < partyCount) {
		int v2 = partyCount - param0->unk_28;
		
		for (int i = param0->unk_28; i < MAX_PARTY_SIZE - 1; i++) {
			BoxIcon_02033804(&(param0->unk_3C[i + 1]), &(param0->unk_3C[i]), 1);
			BoxIcon_020337B8(param0->unk_38, &(param0->unk_3C[i + 1]), 1);
			
			if (param0->unk_3C[i].unk_00 != NULL) {
				BoxIcon_02033A90(param0->unk_38, &(param0->unk_3C[i]), 1048 + 32 * i);
			}
		}
		
		param0->unk_29 = v2;
		param0->unk_2A = 0;
		param0->unk_2C = (BoxGraphics_02030870(BoxParty_02035DA4, param0, 0) != NULL);
	} else {
		param0->unk_2C = 0;
	}
}


static void BoxParty_02035DA4(SysTask* param0, void* param1) {
	BoxPartyManager* v0;
	VecFx32* v1;
	int v2, v3;
	
	v0 = (BoxPartyManager*)param1;
	v2 = v0->unk_28;
	v3 = v0->unk_29;
	v1 = ((VecFx32*)(v0->unk_1C)) + ((v2 * 8) + v0->unk_2A);
	
	for (int i = 0; i < v3; i++) {
		if (v0->unk_3C[v2].unk_00 != NULL) {
			Sprite_SetPosition(v0->unk_3C[v2].unk_00, v1);
		}
		
		v2++;
		v1 += 8;
	}
	
	v0->unk_2A++;
	if (v0->unk_2A >= 8) {
		v0->unk_2C = 0;
		SysTask_Done(param0);
	}
}


BOOL BoxParty_02035E18(BoxPartyManager* param0) {
	return (param0->unk_2C == 0);
}


void BoxParty_02035E28(BoxPartyManager* param0) {
	u32 v0 = Party_GetCurrentCount(param0->unk_0C->party) - 1;
	
	if (param0->unk_28 > v0) {
		VecFx32 v1 = *(Sprite_GetPosition(param0->unk_3C[v0].unk_00));
		param0->unk_20 = (((sDAT_02062548[v0].unk_00 + param0->unk_34) << FX32_SHIFT) - v1.x) / 8;
		param0->unk_24 = ((sDAT_02062548[v0].unk_02 << FX32_SHIFT) - v1.y) / 8;
		param0->unk_2A = 0;
		param0->unk_28 = v0;
		param0->unk_2C = (BoxGraphics_02030870(BoxParty_02035ED4, param0, 0) != NULL);
	} else {
		param0->unk_2C = 0;
	}
}


BOOL BoxParty_02035EC4(BoxPartyManager* param0) {
	return (param0->unk_2C == 0);
}


static void BoxParty_02035ED4(SysTask* param0, void* param1) {
	BoxPartyManager* v0;
	VecFx32 v1;
	BOOL v2;
	
	v0 = (BoxPartyManager*)param1;
	v1 = *(Sprite_GetPosition(v0->unk_3C[v0->unk_28].unk_00));
	
	v0->unk_2A++;
	if (v0->unk_2A >= 8) {
		v1.x = (sDAT_02062548[v0->unk_28].unk_00 + v0->unk_34) << FX32_SHIFT;
		v1.y = sDAT_02062548[v0->unk_28].unk_02 << FX32_SHIFT;
		v2 = TRUE;
	} else {
		v1.x += v0->unk_20;
		v1.y += v0->unk_24;
		v2 = FALSE;
	}
	
	Sprite_SetPosition(v0->unk_3C[v0->unk_28].unk_00, &v1);
	
	if (v2) {
		v0->unk_2C = 0;
		SysTask_Done(param0);
	}
}


void BoxParty_02035F7C(BoxPartyManager* param0) {
	if (param0->unk_0C->selectedBoxID == BoxApp_GetCurrentBox(param0->unk_0C)) {
		BoxPanel_02031720(param0->unk_10);
	}
	
	u32 v0 = BoxApp_GetCursorPartyPosition(param0->unk_0C);
	BoxIcon_020337DC(param0->unk_38, &param0->unk_3C[v0], 1);
	param0->unk_28 = v0;
}


void BoxParty_02035FB8(BoxPartyManager* param0, u32 param1) {
	param0->unk_174 = 0;
	param0->unk_28 = param1;
	BoxIcon_02033AF4(param0->unk_38, &(param0->unk_3C[param1]), 1);
}


BOOL BoxParty_02035FDC(BoxPartyManager* param0) {
	BoxIcon* v0 = &(param0->unk_3C[param0->unk_28]);
	
	switch (param0->unk_174) {
		case 0:
			if (BoxIcon_02033B1C(param0->unk_38, v0)) {
				const BoxApplicationManager* v1 = BoxGraphics_02030A10(param0->unk_00);
				
				if (BoxAppMan_HasCheckedCanReleaseMon(v1)) {
					if (BoxAppMan_CanReleaseMon(v1)) {
						BoxIcon_020337DC(param0->unk_38, v0, 1);
						return 1;
					} else {
						BoxIcon_02033AF4(param0->unk_38, v0, 2);
						param0->unk_174++;
					}
				}
			}
			break;
		
		case 1:
			if (BoxIcon_02033B1C(param0->unk_38, v0)) {
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}


void BoxParty_0203605C(BoxPartyManager* param0) {
	if (BoxApp_GetCursorLocation(param0->unk_0C) == CURSOR_IN_PARTY) {
		u32 v0 = BoxApp_GetMonSpriteTransparencyMask(param0->unk_0C);
		for (int i = 0; i < 6; i++) {
			if (param0->unk_3C[i].unk_00 != NULL) {
				BoxIcon_02033A28(param0->unk_38, &(param0->unk_3C[i]), v0);
			}
		}
	}
}


void BoxParty_02036098(BoxPartyManager* param0, u32 param1, u32 param2) {
	if (param0->unk_3C[param1].unk_00 != NULL) {
		BoxIcon_02033A7C(param0->unk_38, &param0->unk_3C[param1], param2);
		BoxIcon_02033A28(param0->unk_38, &param0->unk_3C[param1], BoxApp_GetMonSpriteTransparencyMask(param0->unk_0C));
	}
}


void BoxParty_020360C8(BoxPartyManager* param0) {
	u32 v0 = BoxApp_GetCursorPartyPosition(param0->unk_0C);
	u32 v1 = BoxApp_GetPreviewedMonMarkings(param0->unk_0C);
	
	if (param0->unk_3C[v0].unk_00 != NULL) {
		BoxIcon_02033A78(param0->unk_38, &(param0->unk_3C[v0]), v1);
	}
}


void BoxParty_020360F8(BoxPartyManager* param0, u32 param1, u32 param2, BoxIcon* param3) {
	BoxIcon_02033804(&param0->unk_3C[param1], param3, 1);
	BoxIcon_02033A90(param0->unk_38, param3, param2);
	BoxIcon_020337B8(param0->unk_38, &param0->unk_3C[param1], 1);
	
	param0->unk_28 = param1;
}


void BoxParty_02036134(BoxPartyManager* param0, u32 param1, const BoxIcon* param2) {
	u32 v0;
	u32 v1;
	BoxIcon* v2;
	
	v0 = Party_GetCurrentCount(param0->unk_0C->party) - 1;
	v1 = 1048 + 32 * v0;
	v2 = &param0->unk_3C[v0];
	
	BoxIcon_02033804(param2, v2, 1);
	BoxIcon_02033A90(param0->unk_38, v2, v1);
	Sprite_SetExplicitPriority(v2->unk_00, 1);
	BoxGraphics_SetSpritePriority(v2->unk_00, 4 + v0);
	
	param0->unk_28 = param1;
}


void BoxParty_02036188(BoxPartyManager* param0, u32 param1, const BoxIcon* param2) {
	u32 v0;
	u32 v1;
	BoxIcon* v2;
	
	v0 = param1;
	v1 = 1048 + 32 * v0;
	v2 = &param0->unk_3C[v0];
	
	BoxIcon_02033804(param2, v2, 1);
	BoxIcon_02033A90(param0->unk_38, v2, v1);
	Sprite_SetExplicitPriority(v2->unk_00, 1);
	BoxGraphics_SetSpritePriority(v2->unk_00, 4 + v0);
	
	param0->unk_28 = param1;
}


BoxIcon* BoxParty_020361CC(BoxPartyManager* param0, u32 param1) {
	GF_ASSERT(param1 < 6);
	return &param0->unk_3C[param1];
}


void BoxParty_020361E4(BoxPartyManager* param0, u32 param1, s32* param2, s32* param3) {
	if (param1 == 6) {
		*param2 = 192 + param0->unk_34;
		*param3 = 184;
	} else {
		*param2 = sDAT_02062548[param1].unk_00 + param0->unk_34;
		*param3 = sDAT_02062548[param1].unk_02;
	}
}


static void BoxParty_02036218(BoxPartyManager* param0, u32 param1) {
	u32 v0, v1, v2;
	
	if (param1 <= 2) {
		v0 = ((2 - param1) * 15) * 2;
		v1 = 24;
		v2 = 0;
	} else {
		if (param1 >= 26) {
			return;
		}
		
		v0 = 0;
		v1 = 26 - param1;
		v2 = 24 - v1;
	}
	
	Bg_LoadToTilemapRect(param0->unk_04, 2, ((u8*)param0->unk_18->rawData) + v0, param0->unk_30, v2, 15, v1);
}


static void BoxParty_02036268(BoxPartyManager* param0, VecFx32* param1) {
	for (u32 i = 0; i < (6 - 1); i++) {
		fx32 v0 = ((sDAT_02062548[i].unk_00 - sDAT_02062548[i + 1].unk_00) * FX32_ONE) / 8;
		fx32 v1 = ((sDAT_02062548[i].unk_02 - sDAT_02062548[i + 1].unk_02) * FX32_ONE) / 8;
		
		u32 v3;
		for (v3 = 0; v3 < (8 - 1); v3++) {
			param1[v3].x = ((sDAT_02062548[i + 1].unk_00 + param0->unk_34) * FX32_ONE) + (v0 * (v3 + 1));
			param1[v3].y = (sDAT_02062548[i + 1].unk_02 * FX32_ONE) + (v1 * (v3 + 1));
			param1[v3].z = 0;
		}
		
		param1[v3].x = ((sDAT_02062548[i].unk_00 + param0->unk_34) * FX32_ONE);
		param1[v3].y = (sDAT_02062548[i].unk_02 * FX32_ONE);
		param1[v3].z = 0;
		param1 += 8;
	}
}


static void BoxParty_02036304(BoxPartyManager* param0, fx32 param1) {
	for (int v1 = 0; v1 < 6; v1++) {
		if (param0->unk_3C[v1].unk_00 != NULL) {
			VecFx32 v0 = *(Sprite_GetPosition(param0->unk_3C[v1].unk_00));
			v0.y += param1;
			Sprite_SetPosition(param0->unk_3C[v1].unk_00, &v0);
			
			if (param1 < 0) {
				if (v0.y < 0xD0000) {
					Sprite_SetDrawFlag(param0->unk_3C[v1].unk_00, TRUE);
				}
			} else {
				if (v0.y >= 0xD0000) {
					Sprite_SetDrawFlag(param0->unk_3C[v1].unk_00, FALSE);
				}
			}
		} else {
			break;
		}
	}
}


static void BoxParty_0203636C(BoxPartyManager* param0, BOOL param1) {
	int v3 = (param1) ? 0 : 208;
	int v2 = Party_GetCurrentCount(param0->unk_0C->party);
	
	for (int i = 0; i < v2; i++) {
		BoxPokemon* v0 = &(Party_GetPokemonBySlotIndex(param0->unk_0C->party, i))->box;
		BoxIcon_02033814(param0->unk_38, v0, sDAT_02062548[i].unk_00 + param0->unk_34, sDAT_02062548[i].unk_02 + v3, 1, 4 + i, 1048 + 32 * i, &param0->unk_3C[i]);
		Sprite_SetDrawFlag(param0->unk_3C[i].unk_00, param1);
	}
}


static void BoxParty_02036404(BoxPartyManager* param0) {
	int v1 = Party_GetCurrentCount(param0->unk_0C->party);
	for (int i = 0; i < v1; i++) {
		if (param0->unk_3C[i].unk_00) {
			BoxIcon_020337DC(param0->unk_38, &(param0->unk_3C[i]), 1);
		}
	}
}
