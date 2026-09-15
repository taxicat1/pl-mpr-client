// File not documented. This file controls icons within the boxes

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_020336AC.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"

#include "assert.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "narc.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "constants/species.h"


BOOL BoxIcon_020336AC(BoxIconManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3) {
	NNSG2dImagePaletteProxy v0;
	NNSG2dPaletteData* v1;
	void* v2;
	
	NarcID iconNarc;
	if (gIsDiamondPearl) {
		iconNarc = NARC_INDEX_DP_POKETOOL__ICONGRA__POKE_ICON;
	} else {
		iconNarc = NARC_INDEX_PL_POKETOOL__ICONGRA__PL_POKE_ICON;
	}
	NNS_G2dInitImagePaletteProxy(&v0);
	v2 = LoadMemberFromNARC(iconNarc, PokeIconPalettesFileIndex(), FALSE, HEAP_ID_BOX_GRAPHICS, TRUE);
	NNS_G2dGetUnpackedPaletteData(v2, &v1);
	v1->szByte = 2 * 2 * 0x20;
	NNS_G2dLoadPalette(v1, 2 * 0x20, NNS_G2D_VRAM_TYPE_2DMAIN, &v0);
	Heap_Free(v2);
	
	// TODO naix
	NarcID boxNarc;
	if (gIsDiamondPearl) {
		boxNarc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		boxNarc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	param0->unk_08 = Graphics_GetCellBank(boxNarc, 21, 1, &(param0->unk_10), HEAP_ID_BOX_GRAPHICS);
	param0->unk_00 = Graphics_GetAnimBank(boxNarc, 22, 1, &(param0->unk_04), HEAP_ID_BOX_GRAPHICS);
	param0->unk_14 = Graphics_GetCellBank(boxNarc, 23, 1, &(param0->unk_1C), HEAP_ID_BOX_GRAPHICS);
	param0->unk_18 = Graphics_GetAnimBank(boxNarc, 24, 1, &(param0->unk_20), HEAP_ID_BOX_GRAPHICS);
	
	param0->unk_48 = NARC_ctor(iconNarc, HEAP_ID_BOX_GRAPHICS);
	
	if ((param0->unk_08 == NULL) || (param0->unk_00 == NULL) || (param0->unk_48 == NULL)) {
		return FALSE;
	}
	
	param0->unk_24 = param3;
	param0->unk_40 = param2;
	param0->unk_44 = param1;
	
	return TRUE;
}


void BoxIcon_02033770(BoxIconManager* param0) {
	if (param0->unk_08 != NULL) {
		Heap_Free(param0->unk_08);
		param0->unk_08 = NULL;
	}
	
	if (param0->unk_00 != NULL) {
		Heap_Free(param0->unk_00);
		param0->unk_00 = NULL;
	}
	
	if (param0->unk_14 != NULL) {
		Heap_Free(param0->unk_14);
		param0->unk_14 = NULL;
	}
	
	if (param0->unk_18 != NULL) {
		Heap_Free(param0->unk_18);
		param0->unk_18 = NULL;
	}
	
	if (param0->unk_48 != NULL) {
		NARC_dtor(param0->unk_48);
	}
}


void BoxIcon_020337B8(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	while (param2--) {
		param1->unk_00 = NULL;
		NNS_G2dInitImageProxy(&(param1->unk_04));
		param1++;
	}
}


void BoxIcon_020337DC(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	while (param2--) {
		if (param1->unk_00 != NULL) {
			Sprite_Delete(param1->unk_00);
			param1->unk_00 = NULL;
		}
		
		param1++;
	}
}


void BoxIcon_02033804(const BoxIcon* param0, BoxIcon* param1, u32 param2) {
	MI_CpuCopy32(param0, param1, sizeof(BoxIcon) * param2);
}


void BoxIcon_02033814(BoxIconManager* param0, BoxPokemon* boxMon, s32 param2, s32 param3, u32 param4, u32 param5, u32 param6, BoxIcon* param7) {
	NNSG2dCharacterData* v0;
	SpriteResourcesHeader v1;
	u16 species;
	u8 isEgg;
	u8 form;
	u8 reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	BoxGraphics_020308E8(&v1, NULL, BoxGraphics_02030878(param0->unk_44), param0->unk_10, param0->unk_04, param4);
	
	species = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	isEgg = BoxPokemon_GetValue(boxMon, MON_DATA_IS_EGG, NULL);
	form = BoxPokemon_GetValue(boxMon, MON_DATA_FORM, NULL);
	param7->unk_28 = BoxPokemon_IconSpriteIndex(boxMon);
	param7->unk_2C = PokeIconPaletteIndex(species, form, isEgg);
	param7->unk_30 = BoxPokemon_GetValue(boxMon, MON_DATA_HELD_ITEM, NULL);
	param7->unk_2E = BoxPokemon_GetValue(boxMon, MON_DATA_MARKINGS, NULL);
	
	NNS_G2dInitImageProxy(&(param7->unk_04));
	NARC_ReadFromMember(param0->unk_48, param7->unk_28, 0, ((4 * 4) * 0x20 + 0x80), param0->unk_4C);
	NNS_G2dGetUnpackedCharacterData(param0->unk_4C, &v0);
	
	v0->mappingType = GX_GetOBJVRamModeChar();
	v0->szByte = (4 * 4) * 0x20;
	
	NNS_G2dLoadImage1DMapping(v0, param6 * 0x20, NNS_G2D_VRAM_TYPE_2DMAIN, &(param7->unk_04));
	
	v1.imageProxy = &(param7->unk_04);
	param7->unk_00 = BoxGraphics_02030908(param0->unk_24, &v1, param2, param3, param5, NNS_G2D_VRAM_TYPE_2DMAIN);
	
	GF_ASSERT(param7->unk_00 != NULL);
	
	Sprite_SetExplicitPalette(param7->unk_00, 2 + param7->unk_2C);
	BoxIcon_02033A28(param0, param7, BoxApp_GetMonSpriteTransparencyMask(param0->unk_40));
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


void BoxIcon_02033934(BoxIconManager* param0, BoxPokemon* boxMon, u32 param2, u32 param3, u8* param4, s32 param5, s32 param6, u32 param7, u32 param8, u32 param9, BoxIcon* param10) {
	NNSG2dCharacterData* v0;
	SpriteResourcesHeader v1;
	
	BOOL reencrypt = BoxPokemon_EnterDecryptionContext(boxMon);
	u8 isEgg = BoxPokemon_GetValue(boxMon, MON_DATA_IS_EGG, NULL);
	u8 form = BoxPokemon_GetValue(boxMon, MON_DATA_FORM, NULL);
	
	BoxGraphics_020308E8(&v1, NULL, BoxGraphics_02030878(param0->unk_44), param0->unk_10, param0->unk_04, param7);
	
	param10->unk_28 = param3;
	param10->unk_2C = PokeIconPaletteIndex(param2, form, isEgg);
	param10->unk_30 = BoxPokemon_GetValue(boxMon, MON_DATA_HELD_ITEM, NULL);
	param10->unk_2E = BoxPokemon_GetValue(boxMon, MON_DATA_MARKINGS, NULL);
	
	NNS_G2dInitImageProxy(&(param10->unk_04));
	NNS_G2dGetUnpackedCharacterData(param4, &v0);
	
	v0->mappingType = GX_GetOBJVRamModeChar();
	v0->szByte = (4 * 4) * 0x20;
	
	NNS_G2dLoadImage1DMapping(v0, param9 * 0x20, NNS_G2D_VRAM_TYPE_2DMAIN, &(param10->unk_04));
	
	v1.imageProxy = &(param10->unk_04);
	param10->unk_00 = BoxGraphics_02030908(param0->unk_24, &v1, param5, param6, param8, NNS_G2D_VRAM_TYPE_2DMAIN);
	
	GF_ASSERT(param10->unk_00 != NULL);
	
	Sprite_SetExplicitPalette(param10->unk_00, 2 + param10->unk_2C);
	BoxIcon_02033A28(param0, param10, BoxApp_GetMonSpriteTransparencyMask(param0->unk_40));
	BoxPokemon_ExitDecryptionContext(boxMon, reencrypt);
}


void BoxIcon_02033A28(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	if (param2 != 0) {
		if (param2 & 1) {
			if (param1->unk_30 == 0) {
				Sprite_SetExplicitOAMMode(param1->unk_00, GX_OAM_MODE_XLU);
				return;
			}
		}
		
		for (int i = 0; i < 6; i++) {
			if ((param2 & (2 << i)) && ((param1->unk_2E & (1 << i)) == 0)) {
				Sprite_SetExplicitOAMMode(param1->unk_00, GX_OAM_MODE_XLU);
				return;
			}
		}
	}
	
	Sprite_SetExplicitOAMMode(param1->unk_00, GX_OAM_MODE_NORMAL);
}


void BoxIcon_02033A78(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	param1->unk_2E = param2;
}


void BoxIcon_02033A7C(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	param1->unk_30 = param2;
}


void BoxIcon_02033A80(BoxIconManager* param0, void* param1, u32 param2, u32 param3) {
	NARC_ReadFromMember(param0->unk_48, param2, 0, param3, param1);
}


void BoxIcon_02033A90(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	if (param1->unk_00 != NULL) {
		NNS_G2dInitImageProxy(&(param1->unk_04));
		NARC_ReadFromMember(param0->unk_48, param1->unk_28, 0, ((4 * 4) * 0x20 + 0x80), param0->unk_4C);
		
		NNSG2dCharacterData* v0;
		NNS_G2dGetUnpackedCharacterData(param0->unk_4C, &v0);
		v0->mappingType = GX_GetOBJVRamModeChar();
		v0->szByte = (4 * 4) * 0x20;
		
		NNS_G2dLoadImage1DMapping(v0, param2 * 0x20, NNS_G2D_VRAM_TYPE_2DMAIN, &(param1->unk_04));
		
		Sprite_SetImageProxy(param1->unk_00, &(param1->unk_04));
	}
}


void BoxIcon_02033AF4(BoxIconManager* param0, BoxIcon* param1, u32 param2) {
	if ((param2 == 1) || (param2 == 2)) {
		Sprite_SetAffineOverwriteMode(param1->unk_00, 1);
	} else {
		Sprite_SetAffineOverwriteMode(param1->unk_00, 0);
	}
	
	Sprite_SetAnim(param1->unk_00, param2);
}


BOOL BoxIcon_02033B1C(BoxIconManager* param0, BoxIcon* param1) {
	return Sprite_IsAnimated(param1->unk_00) == 0;
}


void BoxGraphics_ApplyMonInMultiSelectShading(BoxIconManager* param0, BoxIcon* param1, BOOL isSelected) {
	u32 v0 = ((isSelected) ? 6 : 2) + param1->unk_2C;
	
	Sprite_SetExplicitPalette(param1->unk_00, v0);
}


NNSG2dCellDataBank* BoxIcon_02033B48(BoxIconManager* param0) {
	return param0->unk_10;
}


NNSG2dAnimBankData* BoxIcon_02033B4C(BoxIconManager* param0) {
	return param0->unk_04;
}
