#include <nitro.h>
#include <nnsys.h>

#include "applications/pc_boxes/box_mpr.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_mon_bucket_sorting.h"
#include "applications/pc_boxes/box_wallpaper_pltt.h"
#include "applications/pc_boxes/box_0202F090.h"

#include "mpr_client/mpr_text.h"
#include "mpr_client/mpr_comm.h"

#include "assert.h"
#include "bg_window.h"
#include "common.h"
#include "char_transfer.h"
#include "font.h"
#include "font_oam.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "message.h"
#include "pltt_transfer.h"
#include "sprite.h"
#include "string_gf.h"
#include "text.h"

#include "constants/charcode.h"
#include "constants/natures.h"
#include "constants/species.h"

#include "fs/local.h"

struct MPRBoxDisplay {
	BoxApplicationDisplay* unk_00;
	const BoxApplication* unk_04; // Unused
	BgConfig* unk_08;
	SpriteList* unk_0C;
	u32 unk_10;
	NNSG2dImagePaletteProxy unk_14;
	Sprite* unk_28[10];
	String* unk_50;
	Window unk_54;
	void* unk_64;
	NNSG2dCellDataBank* unk_68;
	void* unk_6C;
	NNSG2dAnimBankData* unk_70;
	FontOAMManager* unk_74;
	FontOAM* unk_78[32];
	CharTransferAllocation unk_F8[32];
	void* unk_278;
	NNSG2dCharacterData* unk_27C;
};

static inline const void* NGC_GetData(const u8* ngc);
static inline u32 NGC_GetSize(const u8* ngc);
static inline const void* NSC_GetData(const u8* nsc);
static inline u32 NSC_GetSize(const u8* nsc);
static inline const void* NCL_GetData(const u8* ncl);
static void BoxMPR_02039868(MPRBoxDisplay* mprDisplay);
static void BoxMPR_020398A0(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, String* param5, BOOL param6);
static void BoxMPR_02039978(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, String* param5);
static void BoxMPR_02039990(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, const charcode_t* param5, BOOL param6);
static void BoxMPR_020399C0(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, const charcode_t* param5);
static void BoxMPR_020399D8(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, u32 param5);
static void BoxMPR_02039AF8(MPRBoxDisplay* mprDisplay);
static void BoxMPR_02039C7C(MPRBoxDisplay* mprDisplay);
static void AppendChars(String* dest, const charcode_t* src);
static void BoxMPR_02039CEC(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A06C(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A218(MPRBoxDisplay* mprDisplay, const int* buckets, int listCount);
static void BoxMPR_0203A3E8(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A480(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A6F8(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A8F4(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203A990(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203ABF0(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203AC6C(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203AEC8(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203B0C4(MPRBoxDisplay* mprDisplay);
static void BoxMPR_0203B23C(u32 index, TouchScreenButtonState state, void* param);

const int gTypeNamesInAlphabeticalOrder[] = {
	TYPE_DARK,
	TYPE_ROCK,
	TYPE_PSYCHIC,
	TYPE_FIGHTING,
	TYPE_GRASS,
	TYPE_ICE,
	TYPE_GHOST,
	TYPE_GROUND,
	TYPE_ELECTRIC,
	TYPE_POISON,
	TYPE_DRAGON,
	TYPE_NORMAL,
	TYPE_STEEL,
	TYPE_FLYING,
	TYPE_FIRE,
	TYPE_WATER,
	TYPE_BUG
};

const int gNatureNamesInAlphabeticalOrder[] = {
	NATURE_ADAMANT,
	NATURE_RASH,
	NATURE_TIMID,
	NATURE_CALM,
	NATURE_MILD,
	NATURE_GENTLE,
	NATURE_HARDY,
	NATURE_QUIRKY,
	NATURE_LONELY,
	NATURE_CAREFUL,
	NATURE_DOCILE,
	NATURE_BOLD,
	NATURE_HASTY,
	NATURE_BASHFUL,
	NATURE_SASSY,
	NATURE_LAX,
	NATURE_RELAXED,
	NATURE_MODEST,
	NATURE_SERIOUS,
	NATURE_NAIVE,
	NATURE_NAUGHTY,
	NATURE_BRAVE,
	NATURE_JOLLY,
	NATURE_QUIET,
	NATURE_IMPISH
};


void BoxMPR_020395E4(BoxApplication* boxApp, BoxMenuItem item) {
	BoxMenu* menu = &boxApp->boxMenu;
	
	if (menu->type != BOX_MENU_TYPE_MARKING) {
		for (int i = 0; i < 8; i++) {
			if (menu->menuItems[i] == item) {
				menu->markings |= (1 << i);
			}
		}
	}
}


BOOL BoxMPR_02039618(MPRBoxDisplay** outMprDisplay, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4) {
	MPRBoxDisplay* mprDisplay = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(MPRBoxDisplay));
	if (mprDisplay != NULL) {
		MI_CpuClear16(mprDisplay, sizeof(MPRBoxDisplay));
		mprDisplay->unk_08 = param3;
		mprDisplay->unk_00 = param1;
		mprDisplay->unk_04 = param2;
		mprDisplay->unk_0C = param4;
		mprDisplay->unk_10 = 0;
		
		// NOTE: in localized versions, 32 chars is not enough for some of the text
		mprDisplay->unk_50 = String_Init(32, HEAP_ID_BOX_GRAPHICS);
		mprDisplay->unk_64 = NULL;
		mprDisplay->unk_6C = NULL;
		
		for (int i = 0; i < 10; i++) {
			mprDisplay->unk_28[i] = NULL;
		}
		
		// TODO naix
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_GRAPHIC__BOX;
		} else {
			narc = NARC_INDEX_PL_GRAPHIC__BOX;
		}
		mprDisplay->unk_278 = Graphics_GetCharData(narc, 25, TRUE, &mprDisplay->unk_27C, HEAP_ID_BOX_GRAPHICS);
		GF_ASSERT(mprDisplay->unk_278 != NULL);
		
		*outMprDisplay = mprDisplay;
		return TRUE;
	}
	
	return FALSE;
}


void BoxMPR_0203969C(MPRBoxDisplay* mprDisplay) {
	int i;
	
	if (mprDisplay->unk_54.pixels != NULL) {
		Window_Remove(&mprDisplay->unk_54);
	}
	
	if (mprDisplay->unk_278 != NULL) {
		Heap_Free(mprDisplay->unk_278);
	}
	
	if (mprDisplay->unk_64 != NULL) {
		Heap_Free(mprDisplay->unk_64);
	}
	
	if (mprDisplay->unk_6C != NULL) {
		Heap_Free(mprDisplay->unk_6C);
	}
	
	if (mprDisplay->unk_50 != NULL) {
		String_Free(mprDisplay->unk_50);
	}
	
	for (i = 0; i < 10; i++) {
		if (mprDisplay->unk_28[i] != NULL) {
			Sprite_Delete(mprDisplay->unk_28[i]);
		}
	}
	
	for (i = 0; i < 32; i++) {
		if (mprDisplay->unk_78[i] != NULL) {
			FontOAM_Free(mprDisplay->unk_78[i]);
		}
	}
	
	if (mprDisplay->unk_74 != NULL) {
		FontOAMManager_Free(mprDisplay->unk_74);
	}
	
	CharTransfer_Free();
	
	Heap_Free(mprDisplay);
}


static inline const void* NGC_GetData(const u8* ngc) {
	return (const void*)(ngc + 0x24);
}


static inline u32 NGC_GetSize(const u8* ngc) {
	return *(const u32*)(ngc + 0x14) - 0x14;
}


static inline const void* NSC_GetData(const u8* nsc) {
	return (const void*)(nsc + 0x28);
}


static inline u32 NSC_GetSize(const u8* nsc) {
	return *(u32*)(nsc + 0x14) - 0x18;
}


static inline const void* NCL_GetData(const u8* ncl) {
	return (const void*)(ncl + 0x20);
}


void BoxMPR_0203971C(MPRBoxDisplay* mprDisplay) {
	Bg_LoadTiles(
		mprDisplay->unk_08,
		BG_LAYER_SUB_3,
		NGC_GetData(LOCAL_FILE(ds_back_ncg)),
		NGC_GetSize(LOCAL_FILE(ds_back_ncg)),
		0);
	
	Bg_CopyTilemapBufferRangeToVRAM(
		mprDisplay->unk_08,
		BG_LAYER_SUB_3,
		NSC_GetData(LOCAL_FILE(ds_back_2_nsc)),
		NSC_GetSize(LOCAL_FILE(ds_back_2_nsc)),
		0);
	
	Bg_FillTilemapRect(mprDisplay->unk_08, BG_LAYER_SUB_2, 0, 0, 0, 0x20, 0x20, 0x11);
	Bg_FillTilemapRect(mprDisplay->unk_08, BG_LAYER_SUB_1, 0, 0, 0, 0x20, 0x20, 0x11);
	
	Bg_ToggleLayer(BG_LAYER_SUB_2, FALSE);
	Bg_ToggleLayer(BG_LAYER_SUB_1, FALSE);
	
	Bg_FillTilesRange(mprDisplay->unk_08, BG_LAYER_SUB_0, 0, 1, 0);
	Bg_FillTilemapRect(mprDisplay->unk_08, BG_LAYER_SUB_0, 0, 0, 0, 0x20, 0x20, 0x11);
	Bg_CopyTilemapBufferToVRAM(mprDisplay->unk_08, BG_LAYER_SUB_0);
	
	Window_Add(mprDisplay->unk_08, &mprDisplay->unk_54, BG_LAYER_SUB_0, 0, 0, 0x20, 0x18, 2, 0);
	Window_FillTilemap(&mprDisplay->unk_54, 0);
	Window_CopyToVRAM(&mprDisplay->unk_54);
	
	Bg_ToggleLayer(BG_LAYER_SUB_0, FALSE);
	
	GXS_LoadBGPltt(NCL_GetData(LOCAL_FILE(ds_back_ncl)), 0, 0x60);
	
	CharTransferTemplate charTransferTemplate = {
		.maxTasks  = 32,
		.sizeMain  = 0x10000,
		.sizeSub   = 0x4000
	};
	CharTransferAllocation allocation;
	charTransferTemplate.heapID = HEAP_ID_BOX_GRAPHICS;
	
	CharTransfer_InitWithVramModes(&charTransferTemplate, GX_OBJVRAMMODE_CHAR_1D_64K, GX_OBJVRAMMODE_CHAR_1D_32K);
	CharTransfer_SetOffsetSub(0);
	CharTransfer_AllocRange(0x1300, 1, NNS_G2D_VRAM_TYPE_2DSUB, &allocation);
	
	mprDisplay->unk_74 = FontOAMManager_New(32, HEAP_ID_BOX_GRAPHICS);
	MI_CpuClear16(mprDisplay->unk_78, sizeof(FontOAM*) * 32);
	
	BoxMPR_02039868(mprDisplay);
}


static void BoxMPR_02039868(MPRBoxDisplay* mprDisplay) {
	BoxMPR_02039AF8(mprDisplay);
	
	NNSG2dImageProxy proxy;
	NNS_G2dInitImageProxy(&proxy);
	
	// TODO naix
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__ZUKAN;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__ZUKAN;
	}
	Graphics_LoadImageMapping(narc, 125, TRUE, IMAGE_MAPPING_LAYOUT_1D, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0, HEAP_ID_BOX_GRAPHICS, &proxy);
	
	BoxMPR_02039CEC(mprDisplay);
}


static void BoxMPR_020398A0(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, String* param5, BOOL param6) {
	int width = Font_CalcStringWidth(FONT_SUBSCREEN, param5, 0);
	
	Window window;
	Window_Init(&window);
	Window_AddToTopLeftCorner(mprDisplay->unk_08, &window, ((width + 7) / 8), 2, 0, 0);
	
	TextColor color;
	if (param6) {
		color = TEXT_COLOR(3, 3, 1);
	} else {
		color = TEXT_COLOR(2, 2, 1);
	}
	
	Text_AddPrinterWithParamsAndColor(
		&window,
		2,
		param5,
		0,
		0,
		TEXT_SPEED_NO_TRANSFER,
		color,
		NULL);
	
	int size = FontOAM_GetCharSizeFromWindow(&window, 2, HEAP_ID_BOX_GRAPHICS);
	CharTransfer_AllocRange(size, TRUE, NNS_G2D_VRAM_TYPE_2DSUB, &mprDisplay->unk_F8[param1]);
	
	FontOAMTemplate oamTemplate;
	oamTemplate.unk_00 = mprDisplay->unk_74;
	oamTemplate.unk_04 = &window;
	oamTemplate.unk_08 = mprDisplay->unk_0C;
	oamTemplate.unk_0C = &mprDisplay->unk_14;
	oamTemplate.unk_10 = param2;
	oamTemplate.unk_14 = mprDisplay->unk_F8[param1].offset;
	oamTemplate.unk_18 = param3 - (width / 2);
	oamTemplate.unk_1C = param4 - 6;
	oamTemplate.unk_20 = 0;
	oamTemplate.unk_24 = 0;
	oamTemplate.unk_28 = 2;
	oamTemplate.heapID = HEAP_ID_BOX_GRAPHICS;
	
	mprDisplay->unk_78[param1] = FontOAM_New(&oamTemplate);
	
	Window_Remove(&window);
}


static void BoxMPR_02039978(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, String* param5) {
	BoxMPR_020398A0(mprDisplay, param1, param2, param3, param4, param5, TRUE);
}


static void BoxMPR_02039990(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, const charcode_t* param5, BOOL param6) {
	String_CopyChars(mprDisplay->unk_50, param5);
	BoxMPR_020398A0(mprDisplay, param1, param2, param3, param4, mprDisplay->unk_50, param6);
}


static void BoxMPR_020399C0(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, const charcode_t* param5) {
	BoxMPR_02039990(mprDisplay, param1, param2, param3, param4, param5, TRUE);
}


static void BoxMPR_020399D8(MPRBoxDisplay* mprDisplay, u32 param1, Sprite* param2, u32 param3, u32 param4, u32 param5) {
	int i, count = 0;
	for (i = 0; i < 6; i++) {
		if ((1 << i) & param5) {
			count++;
		}
	}
	
	int width = count * 8;
	
	Window window;
	Window_Init(&window);
	Window_AddToTopLeftCorner(mprDisplay->unk_08, &window, ((width + 7) / 8), 2, 0, 0);
	
	count = 0;
	for (i = 0; i < 6; i++) {
		if ((1 << i) & param5) {
			Window_BlitBitmapRectWithTransparency(&window, mprDisplay->unk_27C->pRawData, i * 8, 8, 0x30, 0x10, count * 8, 4, 8, 8, 15);
			count++;
		}
	}
	
	int size = FontOAM_GetCharSizeFromWindow(&window, 2, HEAP_ID_BOX_GRAPHICS);
	CharTransfer_AllocRange(size, TRUE, NNS_G2D_VRAM_TYPE_2DSUB, &mprDisplay->unk_F8[param1]);
	
	FontOAMTemplate oamTemplate;
	oamTemplate.unk_00 = mprDisplay->unk_74;
	oamTemplate.unk_04 = &window;
	oamTemplate.unk_08 = mprDisplay->unk_0C;
	oamTemplate.unk_0C = &mprDisplay->unk_14;
	oamTemplate.unk_10 = param2;
	oamTemplate.unk_14 = mprDisplay->unk_F8[param1].offset;
	oamTemplate.unk_18 = param3 - (width / 2);
	oamTemplate.unk_1C = param4 - 6;
	oamTemplate.unk_20 = 0;
	oamTemplate.unk_24 = 0;
	oamTemplate.unk_28 = 2;
	oamTemplate.heapID = HEAP_ID_BOX_GRAPHICS;
	
	mprDisplay->unk_78[param1] = FontOAM_New(&oamTemplate);
	
	Window_Remove(&window);
}


static void BoxMPR_02039AF8(MPRBoxDisplay* mprDisplay) {
	u32 ncerSize = LOCAL_FILE_SIZE(box_wallpaper_ncer);
	mprDisplay->unk_64 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, ncerSize);
	MI_CpuCopy16(LOCAL_FILE(box_wallpaper_ncer), mprDisplay->unk_64, ncerSize);
	NNS_G2dGetUnpackedCellBank(mprDisplay->unk_64, &mprDisplay->unk_68);
	
	u32 nanrSize = LOCAL_FILE_SIZE(box_wallpaper_nanr);
	mprDisplay->unk_6C = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, nanrSize);
	MI_CpuCopy16(LOCAL_FILE(box_wallpaper_nanr), mprDisplay->unk_6C, nanrSize);
	NNS_G2dGetUnpackedAnimBank(mprDisplay->unk_6C, &mprDisplay->unk_70);
	
	NNS_G2dInitImagePaletteProxy(&mprDisplay->unk_14);
	
	// TODO naix
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__ZUKAN;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__ZUKAN;
	}
	Graphics_LoadPartialPalette(narc, 3, NNS_G2D_VRAM_TYPE_2DSUB, 0, HEAP_ID_BOX_GRAPHICS, &mprDisplay->unk_14);
	
	NNSG2dPaletteData g2dPal = {
		3, 0, 2, &gWallpaperPltt
	};
	
	NNS_G2dLoadPalette(&g2dPal, 4, NNS_G2D_VRAM_TYPE_2DSUB, &mprDisplay->unk_14);
}


void BoxMPR_02039B98(MPRBoxDisplay* mprDisplay) {
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	
	OSIntrMode intrBak = OS_DisableInterrupts();
	
	BoxMPR_02039C7C(mprDisplay);
	
	if (mprFilter->unk_00 == 0) {
		Bg_CopyTilemapBufferRangeToVRAM(
			mprDisplay->unk_08,
			BG_LAYER_SUB_3,
			NSC_GetData(LOCAL_FILE(ds_back_2_nsc)),
			NSC_GetSize(LOCAL_FILE(ds_back_2_nsc)),
			0);
	} else {
		Bg_CopyTilemapBufferRangeToVRAM(
			mprDisplay->unk_08,
			BG_LAYER_SUB_3,
			NSC_GetData(LOCAL_FILE(ds_back_1_nsc)),
			NSC_GetSize(LOCAL_FILE(ds_back_1_nsc)),
			0);
	}
	
	switch (mprFilter->unk_00) {
		case 0:
			BoxMPR_02039CEC(mprDisplay);
			break;
		
		case 1:
			BoxMPR_0203A06C(mprDisplay);
			break;
		
		case 2:
			BoxMPR_0203A3E8(mprDisplay);
			break;
		
		case 3:
			BoxMPR_0203A480(mprDisplay);
			break;
		
		case 4:
			BoxMPR_0203A6F8(mprDisplay);
			break;
		
		case 5:
			BoxMPR_0203A8F4(mprDisplay);
			break;
		
		case 6:
			BoxMPR_0203A990(mprDisplay);
			break;
		
		case 7:
			BoxMPR_0203ABF0(mprDisplay);
			break;
		
		case 8:
			BoxMPR_0203AC6C(mprDisplay);
			break;
		
		case 9:
			BoxMPR_0203AEC8(mprDisplay);
			break;
		
		case 10:
			BoxMPR_0203B0C4(mprDisplay);
			break;
	}
	
	OS_RestoreInterrupts(intrBak);
}


static void BoxMPR_02039C7C(MPRBoxDisplay* mprDisplay) {
	for (int i = 0; i < 32; i++) {
		if (mprDisplay->unk_78[i] != NULL) {
			CharTransfer_ClearRange(&mprDisplay->unk_F8[i]);
			FontOAM_Free(mprDisplay->unk_78[i]);
			mprDisplay->unk_78[i] = NULL;
		}
	}
	
	for (int i = 0; i < 10; i++) {
		if (mprDisplay->unk_28[i] != NULL) {
			Sprite_Delete(mprDisplay->unk_28[i]);
			mprDisplay->unk_28[i] = NULL;
		}
	}
}


static void AppendChars(String* dest, const charcode_t* src) {
	for (int i = 0; src[i] != CHAR_EOS; i++) {
		String_AppendChar(dest, src[i]);
	}
}


static void BoxMPR_02039CEC(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	String* dstString = mprDisplay->unk_50;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	mprDisplay->unk_28[0] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x80, 0x1AC, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	
	MPRText_CopyChars(dstString, MPR_TEXT_PokedexNo - 1 + mprFilter->currSettings.unk_00);
	BoxMPR_02039978(mprDisplay, 0, mprDisplay->unk_28[0], 0, 0, dstString);
	
	Sprite_SetAnim(mprDisplay->unk_28[0], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[0], FALSE);
	mprDisplay->unk_28[1] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x48, 0x1E8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.species < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Name);
		BoxMPR_02039978(mprDisplay, 1, mprDisplay->unk_28[1], 0, 0, dstString);
	} else {
		MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SPECIES_NAME, HEAP_ID_BOX_GRAPHICS);
		MessageLoader_GetString(msgLoader, mprFilter->currSettings.species, dstString);
		MessageLoader_Free(msgLoader);
		BoxMPR_02039978(mprDisplay, 1, mprDisplay->unk_28[1], 0, 0, dstString);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[1], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[1], FALSE);
	mprDisplay->unk_28[2] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xB8, 0x1E8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.type < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Type);
		BoxMPR_02039978(mprDisplay, 2, mprDisplay->unk_28[2], 0, 0, dstString);
	} else {
		MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_POKEMON_TYPE_NAMES, HEAP_ID_BOX_GRAPHICS);
		MessageLoader_GetString(msgLoader, mprFilter->currSettings.type, dstString);
		MessageLoader_Free(msgLoader);
		BoxMPR_02039978(mprDisplay, 2, mprDisplay->unk_28[2], 0, 0, dstString);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[2], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[2], FALSE);
	mprDisplay->unk_28[3] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x48, 0x204, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.move < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Move);
		BoxMPR_02039978(mprDisplay, 3, mprDisplay->unk_28[3], 0, 0, dstString);
	} else {
		MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_MOVE_NAMES, HEAP_ID_BOX_GRAPHICS);
		MessageLoader_GetString(msgLoader, mprFilter->currSettings.move, dstString);
		MessageLoader_Free(msgLoader);
		BoxMPR_02039978(mprDisplay, 3, mprDisplay->unk_28[3], 0, 0, dstString);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[3], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[3], FALSE);
	mprDisplay->unk_28[4] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xB8, 0x204, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.ability < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Ability);
		BoxMPR_02039978(mprDisplay, 4, mprDisplay->unk_28[4], 0, 0, dstString);
	} else {
		MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_ABILITY_NAMES, HEAP_ID_BOX_GRAPHICS);
		MessageLoader_GetString(msgLoader, mprFilter->currSettings.ability, dstString);
		MessageLoader_Free(msgLoader);
		BoxMPR_02039978(mprDisplay, 4, mprDisplay->unk_28[4], 0, 0, dstString);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[4], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[4], FALSE);
	mprDisplay->unk_28[5] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x48, 0x220, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.nature < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Nature);
		BoxMPR_02039978(mprDisplay, 5, mprDisplay->unk_28[5], 0, 0, dstString);
	} else {
		MessageLoader* msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_NATURE_NAMES, HEAP_ID_BOX_GRAPHICS);
		MessageLoader_GetString(msgLoader, mprFilter->currSettings.nature, dstString);
		MessageLoader_Free(msgLoader);
		BoxMPR_02039978(mprDisplay, 5, mprDisplay->unk_28[5], 0, 0, dstString);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[5], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[5], FALSE);
	mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xB8, 0x220, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	if (mprFilter->currSettings.mark < 0) {
		MPRText_CopyChars(dstString, MPR_TEXT_Mark);
		BoxMPR_02039978(mprDisplay, 6, mprDisplay->unk_28[6], 0, 0, dstString);
	} else {
		BoxMPR_020399D8(mprDisplay, 6, mprDisplay->unk_28[6], 0, 0, mprFilter->currSettings.mark);
	}
	
	Sprite_SetAnim(mprDisplay->unk_28[6], 2);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	
	MPRText_CopyChars(dstString, MPR_TEXT_SortPokemon);
	BoxMPR_02039978(mprDisplay, 7, 0, 0x80, 0x18E, dstString);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[7], plttOffset + 3);
	
	MPRText_CopyChars(dstString, MPR_TEXT_RefineSearch);
	BoxMPR_02039978(mprDisplay, 8, 0, 0x80, 0x1CD, dstString);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[8], plttOffset + 3);
}


static void BoxMPR_0203A06C(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	u32 plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	
	for (int i = 0; i < 6; i++) {
		if (i + (mprFilter->activePage * 6) < (MPR_TEXT_Random - MPR_TEXT_PokedexNo)) {
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			BoxMPR_020399C0(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, MPRText_GetChars(i + ((mprFilter->activePage * 6) + MPR_TEXT_PokedexNo - 1)));
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
		} else {
			break;
		}
	}
	
	BoxMPR_020399C0(mprDisplay, 6, NULL, 0x80, 0x18E, MPRText_GetChars(mprFilter->activePage + MPR_TEXT_SortBasic));
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (mprFilter->activePage < 2) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


BOOL BoxMPR_0203A1EC(const int* buckets, int listCount, int currentBucket) {
    if (currentBucket < 9) {
        return (buckets[currentBucket] < buckets[currentBucket + 1]);
    } else {
        return (buckets[currentBucket] < listCount);
    }
}


static void BoxMPR_0203A218(MPRBoxDisplay* mprDisplay, const int* buckets, int listCount) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	u32 plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	
	for (int i = 0; i < 6; i++) {
		if (i + (mprFilter->activePage * 6) <= MPR_TEXT_SortAlpha10 - MPR_TEXT_SortAlpha1) {
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039990(
				mprDisplay,
				i,
				mprDisplay->unk_28[i],
				0,
				0,
				MPRText_GetChars(i + ((mprFilter->activePage * 6) + MPR_TEXT_SortAlpha1)),
				BoxMPR_0203A1EC(buckets, listCount, i + (mprFilter->activePage * 6)));
			
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
		} else {
			break;
		}
	}
	
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (mprFilter->activePage < 1) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203A3E8(MPRBoxDisplay* mprDisplay) {
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Name));
	
	BoxMPR_0203A218(mprDisplay, gSpeciesNameBuckets, NATIONAL_DEX_COUNT);
}


u16 BoxMPR_AdvanceSpeciesIndex(u16 speciesIndex, int amt, const u16* speciesNamesInAlphabeticalOrder) {
	while (TRUE) {
		while (TRUE) {
			u16 species = speciesNamesInAlphabeticalOrder[speciesIndex];
			
			if (species == SPECIES_SHAYMIN) {
				if (MPRComm_GetDisplayConfig() & MPR_CONFIG_SHOW_SPECIES_SHAYMIN) {
					break;
				}
			} else if (species == SPECIES_ARCEUS) {
				if (MPRComm_GetDisplayConfig() & MPR_CONFIG_SHOW_SPECIES_ARCEUS) {
					break;
				}
			} else {
				break;
			}
			
			speciesIndex++;
		}
		
		if (amt-- == 0) {
			return speciesIndex;
		}
		
		speciesIndex++;
	}
}


static void BoxMPR_0203A480(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	MessageLoader* msgLoader;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID zukanNarc, msgNarc;
	if (gIsDiamondPearl) {
		zukanNarc = NARC_INDEX_DP_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
		msgNarc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		zukanNarc = NARC_INDEX_PL_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
		msgNarc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	
	int speciesIndex, maxSpeciesIndex;
	int speciesNameFileSize; /* Unused */
	u16* speciesNamesInAlphabeticalOrder = LoadMemberFromNARC_OutFileSize(zukanNarc, 13, 0, HEAP_ID_BOX_GRAPHICS, 0, &speciesNameFileSize);
	msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, msgNarc, TEXT_BANK_UNIFIED_SPECIES_NAME, HEAP_ID_BOX_GRAPHICS);
	speciesIndex = BoxMPR_AdvanceSpeciesIndex(gSpeciesNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage, speciesNamesInAlphabeticalOrder);
	int currentBucket = mprFilter->activeBucket;
	
	if (currentBucket == 9) {
		maxSpeciesIndex = NATIONAL_DEX_COUNT;
	} else {
		currentBucket++;
		maxSpeciesIndex = gSpeciesNameBuckets[currentBucket];
	}
	
	for (int i = 0; i < 6; i++) {
		if (speciesIndex < maxSpeciesIndex) {
			MessageLoader_GetString(msgLoader, speciesNamesInAlphabeticalOrder[speciesIndex], mprDisplay->unk_50);
			
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039978(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, mprDisplay->unk_50);
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
			
			speciesIndex = BoxMPR_AdvanceSpeciesIndex(speciesIndex, 1, speciesNamesInAlphabeticalOrder);
		} else {
			break;
		}
	}
	
	MessageLoader_Free(msgLoader);
	Heap_Free(speciesNamesInAlphabeticalOrder);
	
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Name));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Dash));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_SortAlpha1 + mprFilter->activeBucket));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (speciesIndex < maxSpeciesIndex) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203A6F8(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	MessageLoader* msgLoader;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_POKEMON_TYPE_NAMES, HEAP_ID_BOX_GRAPHICS);
	
	for (int i = 0; i < 6; i++) {
		if (i + (6 * mprFilter->activePage) < TYPE_MAX - 1) {
			MessageLoader_GetString(msgLoader, gTypeNamesInAlphabeticalOrder[i + (6 * mprFilter->activePage)], mprDisplay->unk_50);
			
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039978(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, mprDisplay->unk_50);
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
		} else {
			break;
		}
	}
	
	MessageLoader_Free(msgLoader);
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Type));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, 2);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (mprFilter->activePage < 2) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, 2);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, 2);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, 2);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203A8F4(MPRBoxDisplay* mprDisplay) {
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Move));
	
	BoxMPR_0203A218(mprDisplay, gMoveNameBuckets, VALID_MOVE_COUNT - 1); // Exclude Struggle
}


u16 BoxMPR_AdvanceMoveIndex(u16 moveIndex, int amt) {
	while (TRUE) {
		while (TRUE) {
			u16 move = gMoveNamesInAlphabeticalOrder[moveIndex];
			
			if (move == MOVE_JUDGMENT) {
				if (MPRComm_GetDisplayConfig() & MPR_CONFIG_SHOW_MOVE_JUDGMENT) {
					break;
				}
			} else if (move == MOVE_SEED_FLARE) {
				if (MPRComm_GetDisplayConfig() & MPR_CONFIG_SHOW_MOVE_SEED_FLARE) {
					break;
				}
			} else {
				break;
			}
			
			moveIndex++;
		}
		
		if (amt-- == 0) {
			return moveIndex;
		}
		
		moveIndex++;
	}
}


static void BoxMPR_0203A990(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	MessageLoader* msgLoader;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	
	int moveIndex, maxMoveIndex;
	msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_MOVE_NAMES, HEAP_ID_BOX_GRAPHICS);
	moveIndex = BoxMPR_AdvanceMoveIndex(gMoveNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage);
	int currentBucket = mprFilter->activeBucket;
	
	if (currentBucket == 9) {
		maxMoveIndex = VALID_MOVE_COUNT - 1;
	} else {
		currentBucket++;
		maxMoveIndex = gMoveNameBuckets[currentBucket];
	}
	
	for (int i = 0; i < 6; i++) {
		if (moveIndex < maxMoveIndex) {
			MessageLoader_GetString(msgLoader, gMoveNamesInAlphabeticalOrder[moveIndex], mprDisplay->unk_50);
			
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039978(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, mprDisplay->unk_50);
			Sprite_SetAnim(mprDisplay->unk_28[i], 2);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
			
			moveIndex = BoxMPR_AdvanceMoveIndex(moveIndex, 1);
		} else {
			break;
		}
	}
	
	MessageLoader_Free(msgLoader);
	
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Move));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Dash));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_SortAlpha1 + mprFilter->activeBucket));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (moveIndex < maxMoveIndex) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203ABF0(MPRBoxDisplay* mprDisplay) {
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Ability));
	
	BoxMPR_0203A218(mprDisplay, gAbilityNameBuckets, ABILITY_COUNT);
}


u16 BoxMPR_AdvanceAbilityIndex(u16 abilityIndex, int amt) {
	while (TRUE) {
		while (TRUE) {
			u16 ability = gAbilityNamesInAlphabeticalOrder[abilityIndex];
			
			if (ability == ABILITY_MULTITYPE) {
				if (MPRComm_GetDisplayConfig() & MPR_CONFIG_SHOW_ABILITY_MULTITYPE) {
					break;
				}
			} else {
				break;
			}
			
			abilityIndex++;
		}
		
		if (amt-- == 0) {
			return abilityIndex;
		}
		
		abilityIndex++;
	}
}


static void BoxMPR_0203AC6C(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	MessageLoader* msgLoader;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	
	int abilityIndex, maxAbilityIndex;
	msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_ABILITY_NAMES, HEAP_ID_BOX_GRAPHICS);
	abilityIndex = BoxMPR_AdvanceAbilityIndex(gAbilityNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage);
	int currentBucket = mprFilter->activeBucket;
	
	if (currentBucket == 9) {
		maxAbilityIndex = ABILITY_COUNT;
	} else {
		currentBucket++;
		maxAbilityIndex = gAbilityNameBuckets[currentBucket];
	}
	
	for (int i = 0; i < 6; i++) {
		if (abilityIndex < maxAbilityIndex) {
			MessageLoader_GetString(msgLoader, gAbilityNamesInAlphabeticalOrder[abilityIndex], mprDisplay->unk_50);
			
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039978(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, mprDisplay->unk_50);
			Sprite_SetAnim(mprDisplay->unk_28[i], 2);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
			
			abilityIndex = BoxMPR_AdvanceAbilityIndex(abilityIndex, 1);
		} else {
			break;
		}
	}
	
	MessageLoader_Free(msgLoader);
	
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Ability));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Dash));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_SortAlpha1 + mprFilter->activeBucket));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (abilityIndex < maxAbilityIndex) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203AEC8(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	MessageLoader* msgLoader;
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_NATURE_NAMES, HEAP_ID_BOX_GRAPHICS);
	
	for (int i = 0; i < 6; i++) {
		if (i + (6 * mprFilter->activePage) < NATURE_MAX) {
			MessageLoader_GetString(msgLoader, gNatureNamesInAlphabeticalOrder[i + (6 * mprFilter->activePage)], mprDisplay->unk_50);
			
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_02039978(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, mprDisplay->unk_50);
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
		} else {
			break;
		}
	}
	
	MessageLoader_Free(msgLoader);
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Nature));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	if (mprFilter->activePage > 0) {
		mprDisplay->unk_28[6] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x10, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[6], 4);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[6], FALSE);
	}
	
	if (mprFilter->activePage < 4) {
		mprDisplay->unk_28[7] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xF0, 0x1D8, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetAnim(mprDisplay->unk_28[7], 5);
		Sprite_SetAnimateFlag(mprDisplay->unk_28[7], FALSE);
	}
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203B0C4(MPRBoxDisplay* mprDisplay) {
	NNSG2dImageProxy imgProxy;
	SpriteResourcesHeader spriteHeader;
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	int plttOffset = PlttTransfer_GetPlttOffset(&mprDisplay->unk_14, NNS_G2D_VRAM_TYPE_2DSUB);
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	
	NNS_G2dInitImageProxy(&imgProxy);
	BoxGraphics_020308E8(&spriteHeader, &imgProxy, &mprDisplay->unk_14, mprDisplay->unk_68, mprDisplay->unk_70, 3);
	
	for (int i = 0; i < 6; i++) {
		if (i + (6 * mprFilter->activePage) < 25) {
			u32 v0 = ((i % 2) * 0x60) + 0x50;
			u32 v1 = ((i / 2) * 0x20) + 0x1B8;
			mprDisplay->unk_28[i] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, v0, v1, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
			
			BoxMPR_020399D8(mprDisplay, i, mprDisplay->unk_28[i], 0, 0, (1 << i));
			Sprite_SetAnim(mprDisplay->unk_28[i], 0);
			Sprite_SetAnimateFlag(mprDisplay->unk_28[i], FALSE);
		} else {
			break;
		}
	}
	
	String_CopyChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_RefineSearch));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Colon));
	AppendChars(mprDisplay->unk_50, MPRText_GetChars(MPR_TEXT_Mark));
	BoxMPR_02039978(mprDisplay, 6, NULL, 0x80, 0x18E, mprDisplay->unk_50);
	FontOAM_SetExplicitPalette(mprDisplay->unk_78[6], plttOffset + 3);
	
	mprDisplay->unk_28[8] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0x40, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 8, mprDisplay->unk_28[8], 0, 0, MPRText_GetChars(MPR_TEXT_Reset));
	Sprite_SetAnim(mprDisplay->unk_28[8], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[8], FALSE);
	
	mprDisplay->unk_28[9] = BoxGraphics_02030908(mprDisplay->unk_0C, &spriteHeader, 0xC0, 0x228, 0x31, NNS_G2D_VRAM_TYPE_2DSUB);
	BoxMPR_020399C0(mprDisplay, 9, mprDisplay->unk_28[9], 0, 0, MPRText_GetChars(MPR_TEXT_OK));
	Sprite_SetAnim(mprDisplay->unk_28[9], 0);
	Sprite_SetAnimateFlag(mprDisplay->unk_28[9], FALSE);
}


static void BoxMPR_0203B23C(u32 index, TouchScreenButtonState state, void* param) {
	MPRFilterMenu* mprFilter = param;
	if (!mprFilter->unk_4C) {
		mprFilter->buttonStates[index] = state;
		
		if (state == TOUCH_BUTTON_PRESSED && *(mprFilter->touchScreenButtonPressedPtr) == 0xFFFF) {
			*(mprFilter->touchScreenButtonPressedPtr) = index;
		}
	}
}


TouchScreenActions* BoxMPR_0203B260(MPRFilterMenu* mprFilter) {
	static const TouchScreenRect sTouchHitBoxes1[] = {
		{ 0x20, 0x38, 0x40, 0xC0 },
		{ 0x5C, 0x74, 0x00, 0x7F },
		{ 0x5C, 0x74, 0x80, 0xFF },
		{ 0x78, 0x90, 0x00, 0x7F },
		{ 0x78, 0x90, 0x80, 0xFF },
		{ 0x94, 0xAC, 0x00, 0x7F },
		{ 0x94, 0xAC, 0x80, 0xFF },
		{ TOUCHSCREEN_TABLE_TERMINATOR }
	};
	
	static const TouchScreenRect sTouchHitBoxes2[] = {
		{ 0x2C, 0x44, 0x20, 0x7F },
		{ 0x2C, 0x44, 0x80, 0xDF },
		{ 0x4C, 0x64, 0x20, 0x7F },
		{ 0x4C, 0x64, 0x80, 0xDF },
		{ 0x6C, 0x84, 0x20, 0x7F },
		{ 0x6C, 0x84, 0x80, 0xDF },
		{ 0x4C, 0x64, 0x00, 0x1F },
		{ 0x4C, 0x64, 0xE0, 0xFF },
		{ 0x9C, 0xB4, 0x10, 0x6F },
		{ 0x9C, 0xB4, 0x90, 0xEF },
		{ TOUCHSCREEN_TABLE_TERMINATOR }
	};
	
	for (int i = 0; i < 10; i++) {
		mprFilter->buttonStates[i] = TOUCH_BUTTON_INVALID;
	}
	
	switch (mprFilter->unk_00) {
		case 0:
			return TouchScreenActions_RegisterHandler(sTouchHitBoxes1, NELEMS(sTouchHitBoxes1), BoxMPR_0203B23C, mprFilter, HEAP_ID_BOX_DATA);
		
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			return TouchScreenActions_RegisterHandler(sTouchHitBoxes2, NELEMS(sTouchHitBoxes2), BoxMPR_0203B23C, mprFilter, HEAP_ID_BOX_DATA);
		
		default:
			GF_ASSERT(FALSE);
			return NULL;
	}
}


void BoxMPR_0203B2D0(MPRBoxDisplay* mprDisplay) {
	MPRFilterMenu* mprFilter = BoxGraphics_GetMPRFilter(mprDisplay->unk_00);
	int i;
	BOOL animateButton;
	
	for (i = 0; i < 10; i++) {
		mprFilter->buttonStates[i] = TOUCH_BUTTON_HELD_OUT_OF_BOUNDS;
	}
	
	switch (mprFilter->unk_00) {
		case 0:
			if (mprFilter->currSettings.species >= 0 && mprFilter->buttonStates[1] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[1] = TOUCH_BUTTON_RELEASED;
			}
			
			if (mprFilter->currSettings.type >= 0 && mprFilter->buttonStates[2] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[2] = TOUCH_BUTTON_RELEASED;
			}
			
			if (mprFilter->currSettings.move >= 0 && mprFilter->buttonStates[3] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[3] = TOUCH_BUTTON_RELEASED;
			}
			
			if (mprFilter->currSettings.ability >= 0 && mprFilter->buttonStates[4] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[4] = TOUCH_BUTTON_RELEASED;
			}
			
			if (mprFilter->currSettings.nature >= 0 && mprFilter->buttonStates[5] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[5] = TOUCH_BUTTON_RELEASED;
			}
			
			if (mprFilter->currSettings.mark >= 0 && mprFilter->buttonStates[6] != TOUCH_BUTTON_HELD) {
				mprFilter->buttonStates[6] = TOUCH_BUTTON_RELEASED;
			}
			break;
		
		case 1:
			if (mprFilter->activePage == (mprFilter->currSettings.unk_00 / 6)) {
				int buttonIdx = mprFilter->currSettings.unk_00 - (mprFilter->activePage * 6);
				if (mprFilter->buttonStates[buttonIdx] != TOUCH_BUTTON_HELD) {
					mprFilter->buttonStates[buttonIdx] = TOUCH_BUTTON_RELEASED;
				}
			}
			
			animateButton = (mprFilter->currSettings.unk_00 >= 0);
			break;
		
		case 2:
			animateButton = (mprFilter->currSettings.species >= 0);
			break;
		
		case 3:
			{
				// TODO naix
				NarcID narc;
				if (gIsDiamondPearl) {
					narc = NARC_INDEX_DP_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
				} else {
					narc = NARC_INDEX_PL_APPLICATION__ZUKANLIST__ZKN_DATA__ZUKAN_DATA;
				}
				int speciesNameFileSize; /* Unused */
				u16* speciesNamesInAlphabeticalOrder = LoadMemberFromNARC_OutFileSize(narc, 13, 0, HEAP_ID_BOX_GRAPHICS, 0, &speciesNameFileSize);
				int speciesIndex = BoxMPR_AdvanceSpeciesIndex(gSpeciesNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage, speciesNamesInAlphabeticalOrder);
				
				for (i = 0; i < 6; i++) {
					if (mprFilter->currSettings.species == speciesNamesInAlphabeticalOrder[speciesIndex] && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
						mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
					}
					speciesIndex = BoxMPR_AdvanceSpeciesIndex(speciesIndex, 1, speciesNamesInAlphabeticalOrder);
				}
				
				Heap_Free(speciesNamesInAlphabeticalOrder);
			}
			
			animateButton = (mprFilter->currSettings.species >= 0);
			break;
		
		case 4:
			for (i = 0; i < 6; i++) {
				if (mprFilter->currSettings.type == gTypeNamesInAlphabeticalOrder[i + (mprFilter->activePage * 6)] && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
					mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
				}
			}
			
			animateButton = (mprFilter->currSettings.type >= 0);
			break;
		
		case 5:
			animateButton = (mprFilter->currSettings.move >= 0);
			break;
		
		case 6:
			{
				int moveIndex = BoxMPR_AdvanceMoveIndex(gMoveNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage);
				for (i = 0; i < 6; i++) {
					if (mprFilter->currSettings.move == gMoveNamesInAlphabeticalOrder[moveIndex] && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
						mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
					}
					moveIndex = BoxMPR_AdvanceMoveIndex(moveIndex, 1);
				}
			}
			
			animateButton = (mprFilter->currSettings.move >= 0);
			break;
		
		case 7:
			animateButton = (mprFilter->currSettings.ability >= 0);
			break;
		
		case 8:
			{
				int abilityIndex = BoxMPR_AdvanceAbilityIndex(gAbilityNameBuckets[mprFilter->activeBucket], 6 * mprFilter->activePage);
				for (i = 0; i < 6; i++) {
					if (mprFilter->currSettings.ability == gAbilityNamesInAlphabeticalOrder[abilityIndex] && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
						mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
					}
					abilityIndex = BoxMPR_AdvanceAbilityIndex(abilityIndex, 1);
				}
			}
			
			animateButton = (mprFilter->currSettings.ability >= 0);
			break;
		
		case 9:
			for (i = 0; i < 6; i++) {
				if (mprFilter->currSettings.nature == gNatureNamesInAlphabeticalOrder[i + (mprFilter->activePage * 6)] && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
					mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
				}
			}
			
			animateButton = (mprFilter->currSettings.nature >= 0);
			break;
		
		case 10:
			for (i = 0; i < 6; i++) {
				if (mprFilter->currSettings.mark >= 0 && (mprFilter->currSettings.mark & (1 << i)) && mprFilter->buttonStates[i] != TOUCH_BUTTON_HELD) {
					mprFilter->buttonStates[i] = TOUCH_BUTTON_RELEASED;
				}
			}
			
			animateButton = (mprFilter->currSettings.mark >= 0);
			break;
	}
	
	if (mprDisplay->unk_28[9] != NULL && mprFilter->unk_48 != 0 && mprFilter->buttonStates[9] != TOUCH_BUTTON_HELD) {
		mprFilter->buttonStates[9] = TOUCH_BUTTON_RELEASED;
	}
	
	for (i = 0; i < 10; i++) {
		if (i == 6 && mprFilter->unk_00 != 0) {
			continue;
		}
		
		if (i != 7 && i != 8 && mprDisplay->unk_28[i] != NULL) {
			switch (mprFilter->buttonStates[i]) {
				case TOUCH_BUTTON_PRESSED:
					Sprite_SetAnimFrame(mprDisplay->unk_28[i], 1);
					break;
				
				case TOUCH_BUTTON_HELD:
					Sprite_SetAnimFrame(mprDisplay->unk_28[i], 2);
					break;
				
				case TOUCH_BUTTON_RELEASED:
					Sprite_SetAnimFrame(mprDisplay->unk_28[i], 3);
					break;
				
				case TOUCH_BUTTON_HELD_OUT_OF_BOUNDS:
				case TOUCH_BUTTON_INVALID:
					if (Sprite_GetAnimFrame(mprDisplay->unk_28[i]) != 0) {
						Sprite_UpdateAnim(mprDisplay->unk_28[i], -0x2000);
					}
					break;
			}
			
			int frame = Sprite_GetAnimFrame(mprDisplay->unk_28[i]);
			int x, y;
			FontOAM_GetXY(mprDisplay->unk_78[i], &x, &y);
			
			switch (frame) {
				case 0:
					FontOAM_SetXY(mprDisplay->unk_78[i], x, -6);
					break;
				
				case 1:
					FontOAM_SetXY(mprDisplay->unk_78[i], x, -8);
					break;
				
				case 2:
					FontOAM_SetXY(mprDisplay->unk_78[i], x, -10);
					break;
				
				case 3:
					FontOAM_SetXY(mprDisplay->unk_78[i], x, -8);
					break;
			}
		}
	}
	
	if (mprDisplay->unk_28[8] != NULL) {
		Sprite_SetAnim(mprDisplay->unk_28[8], animateButton ? 1 : 0);
	}
}
