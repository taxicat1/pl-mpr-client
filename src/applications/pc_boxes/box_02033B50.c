// File not documented. This controls the "Deposit All / Close" and "Withdraw All / Close"
// buttons in the bottom right of the PC.

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_02033B50.h"

#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"

#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "narc.h"
#include "sprite.h"

#include "fs/local.h"

static const u8* sBoxButtonGraphic = LOCAL_FILE(NARC_box_m_obj_EN_ncg);


void BoxButton_SetLanguage(void) {
	static BOOL setLanguage = FALSE;
	
	if (!setLanguage) {
		switch (gGameLanguage) {
			default:
			case LANGUAGE_ENGLISH:
				// Default
				break;
			
			case LANGUAGE_GERMAN:
				sBoxButtonGraphic = LOCAL_FILE(NARC_box_m_obj_DE_ncg);
				break;
			
			case LANGUAGE_ITALIAN:
				sBoxButtonGraphic = LOCAL_FILE(NARC_box_m_obj_IT_ncg);
				break;
			
			case LANGUAGE_SPANISH:
				sBoxButtonGraphic = LOCAL_FILE(NARC_box_m_obj_ES_ncg);
				break;
			
			case LANGUAGE_FRENCH:
				sBoxButtonGraphic = LOCAL_FILE(NARC_box_m_obj_FR_ncg);
				break;
		}
		
		setLanguage = TRUE;
	}
}

BOOL BoxButton_02033B50(BoxButtonManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3) {
	param0->unk_44 = param1;
	param0->unk_3C = param3;
	param0->unk_40 = param2;
	param0->unk_00 = param3;
	param0->unk_04 = NULL;
	
	// TODO naix
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	param0->unk_08 = Graphics_GetCellBank(narc, 10, 1, &(param0->unk_10), HEAP_ID_BOX_GRAPHICS);
	param0->unk_0C = Graphics_GetAnimBank(narc, 11, 1, &(param0->unk_14), HEAP_ID_BOX_GRAPHICS);
	
	if ((param0->unk_08 == NULL) || (param0->unk_0C == NULL)) {
		return FALSE;
	}
	
	return TRUE;
}


void BoxButton_02033BA0(BoxButtonManager* param0) {
	NNS_G2dInitImageProxy(&(param0->unk_18));
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	Graphics_LoadImageMapping(narc, 9, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 1256 * 0x20, 10, &(param0->unk_18));
	
	const u8* v1 = NULL;
	switch (BoxApp_GetBoxMode(param0->unk_40)) {
		case PC_MODE_RANCH_DEPOSIT:
			v1 = sBoxButtonGraphic + 0x24;
			break;
		
		case PC_MODE_RANCH_WITHDRAW:
			v1 = sBoxButtonGraphic + 0x1B00 + 0x24;
			break;
	}
	
	if (v1 != NULL) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				GX_LoadOBJ(v1 + (((i * 4) + j) * 0x240),         0x9D00 + (((i * 0x48)        + (j * 8)) * 32), 0x100);
				GX_LoadOBJ(v1 + (((i * 4) + j) * 0x240) + 0x100, 0x9D00 + (((i * 0x48) + 0x20 + (j * 8)) * 32), 0x100);
				GX_LoadOBJ(v1 + (((i * 4) + j) * 0x240) + 0x200, 0x9D00 + (((i * 0x48) + 0x40 + (j * 2)) * 32), 0x40);
			}
		}
	}
	
	SpriteResourcesHeader v0;
	BoxGraphics_020308E8(&v0, &(param0->unk_18), BoxGraphics_02030878(param0->unk_44), param0->unk_10, param0->unk_14, 2);
	param0->unk_04 = BoxGraphics_02030908(param0->unk_00, &v0, 183 + BoxGraphics_020308CC(param0->unk_44), 176, 44, NNS_G2D_VRAM_TYPE_2DMAIN);
	Sprite_SetExplicitPalette(param0->unk_04, 1);
}


void BoxButton_02033CE0(BoxButtonManager* param0) {
	if (param0->unk_08 != NULL) {
		Heap_Free(param0->unk_08);
		param0->unk_08 = NULL;
	}
	
	if (param0->unk_0C != NULL) {
		Heap_Free(param0->unk_0C);
		param0->unk_0C = NULL;
	}
}


void BoxButton_02033D04(BoxButtonManager* param0, int param1) {
	switch (param1) {
		case 0:
			Sprite_SetAnim(param0->unk_04, 0);
			break;
		
		case 1:
			Sprite_SetAnim(param0->unk_04, 1);
			break;
		
		case 2:
			Sprite_SetAnim(param0->unk_04, 2);
			break;
	}
}
