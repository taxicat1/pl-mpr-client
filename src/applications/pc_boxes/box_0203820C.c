// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_0203820C.h"

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_0202F090.h"
#include "applications/pc_boxes/box_020336AC.h"

#include "bg_window.h"
#include "common.h"
#include "font.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "message.h"
#include "narc.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "string_gf.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "text.h"

#include "constants/pokemon_contest_types.h"
#include "fs/msgdata/box_messages.h"

struct BoxComparisonManager {
	BOOL unk_00;
	BoxApplicationDisplay* unk_04;
	const BoxApplication* unk_08;
	BgConfig* unk_0C;
	SpriteList* unk_10;
	BoxIconManager* unk_14;
	u32 unk_18[2];
	Sprite* unk_20[2];
	Sprite* unk_28[2];
	Sprite* unk_30;
	Sprite* unk_34[2][5];
	void* unk_5C;
	NNSG2dCellDataBank* unk_60;
	void* unk_64;
	NNSG2dAnimBankData* unk_68;
	MessageLoader* unk_6C;
	MessageLoader* unk_70;
	String* boxDisplayText;
	Window unk_78[7];
	u8 unk_E8[640];
	NNSG2dScreenData* unk_368;
	NNSG2dScreenData* unk_36C;
	void* unk_370;
	void* unk_374;
	CompareMode unk_378;
	int unk_37C;
	SysTask* unk_380;
};

static void BoxComparison_0203858C(BoxComparisonManager* param0);
static void BoxComparison_020385F4(BoxComparisonManager* param0);
static void BoxComparison_02038658(BoxComparisonManager* param0);
static void BoxComparison_020386CC(BoxComparisonManager* param0);
static void BoxComparison_020386F4(BoxComparisonManager* param0);
static void BoxComparison_0203875C(BoxComparisonManager* param0, int param1);
static void BoxComparison_02038798(BoxComparisonManager* param0);
static void BoxComparison_020387D0(BoxComparisonManager* param0);
static void BoxComparison_02038808(BoxComparisonManager* param0, int param1);
static void BoxComparison_020388E0(BoxComparisonManager* param0, int param1);
static void BoxComparison_02038900(BoxComparisonManager* param0);
static void BoxComparison_02038938(BoxComparisonManager* param0);
static inline u32 BoxComparison_Inline0(Window* param0, u32 param1, const String* param2);
static void BoxComparison_02038970(BoxComparisonManager* param0);
static void BoxComparison_02038A24(BoxComparisonManager* param0);
static void BoxComparison_02038A3C(BoxComparisonManager* param0, int param1);
static void BoxComparison_02038B60(BoxComparisonManager* param0, int param1);
static void BoxComparison_02038C60(BoxComparisonManager* param0);
static void BoxComparison_02038E30(BoxComparisonManager* param0, u32 param1);
static void BoxComparison_02038ED0(BoxComparisonManager* param0);
static void BoxComparison_02038F34(BoxComparisonManager* param0);
static void BoxComparison_02038FC8(BoxComparisonManager* param0, int param1);
static void BoxComparison_02039030(SysTask* param0, void* param1);


BOOL BoxComparison_0203820C(BoxComparisonManager** param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4, MessageLoader* param5) {
	if (BoxApp_GetBoxMode(param2) != PC_MODE_COMPARE) {
		*param0 = NULL;
		return TRUE;
	} else {
		BoxComparisonManager* v0 = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxComparisonManager));
		if (v0 != NULL) {
			v0->unk_04 = param1;
			v0->unk_08 = param2;
			v0->unk_0C = param3;
			v0->unk_10 = param4;
			v0->unk_14 = BoxGraphics_02030880(param1);
			
			// TODO naix
			NarcID narc;
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_GRAPHIC__BOX;
			} else {
				narc = NARC_INDEX_PL_GRAPHIC__BOX;
			}
			v0->unk_370 = Graphics_GetScrnData(narc, 7, TRUE, &(v0->unk_368), HEAP_ID_BOX_GRAPHICS);
			v0->unk_374 = Graphics_GetScrnData(narc, 8, TRUE, &(v0->unk_36C), HEAP_ID_BOX_GRAPHICS);
			v0->unk_6C = param5;
			
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_MSGDATA__MSG;
			} else {
				narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
			}
			v0->unk_70 = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_MOVE_NAMES, HEAP_ID_BOX_GRAPHICS);
			v0->boxDisplayText = String_Init(32, HEAP_ID_BOX_GRAPHICS);
			v0->unk_380 = NULL;
			*param0 = v0;
			return TRUE;
		}
		
		return FALSE;
	}
}


void BoxComparison_020382B8(BoxComparisonManager* param0) {
	if (param0 != NULL) {
		Heap_Free(param0->unk_370);
		Heap_Free(param0->unk_374);
		MessageLoader_Free(param0->unk_70);
		BoxComparison_02038ED0(param0);
		BoxComparison_020386CC(param0);
		String_Free(param0->boxDisplayText);
		Heap_Free(param0);
	}
}


void BoxComparison_020382F4(BoxComparisonManager* param0) {
	if (param0 != NULL) {
		BoxComparison_0203858C(param0);
		
		// TODO naix
		if (gIsDiamondPearl) {
			NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
			Graphics_LoadTilesToBgLayer(narc, 108, param0->unk_0C, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 105, param0->unk_0C, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 106, param0->unk_0C, 6, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 107, param0->unk_0C, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadPalette(narc, 109, 4, 0, 0x20 * 4, HEAP_ID_BOX_GRAPHICS);
		} else {
			NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
			Graphics_LoadTilesToBgLayer(narc, 132, param0->unk_0C, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 129, param0->unk_0C, 5, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 130, param0->unk_0C, 6, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadTilemapToBgLayer(narc, 131, param0->unk_0C, 7, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
			Graphics_LoadPalette(narc, 133, 4, 0, 0x20 * 4, HEAP_ID_BOX_GRAPHICS);
		}
		
		NarcID narc;
		if (gIsDiamondPearl) {
			narc = NARC_INDEX_DP_POKETOOL__ICONGRA__POKE_ICON;
		} else {
			narc = NARC_INDEX_PL_POKETOOL__ICONGRA__PL_POKE_ICON;
		}
		Graphics_LoadPalette(narc, PokeIconPalettesFileIndex(), 5, 4 * 0x20, 4 * 0x20, HEAP_ID_BOX_GRAPHICS);
		Bg_ClearTilesRange(4, 0x20, 0, HEAP_ID_BOX_GRAPHICS);
		Bg_FillTilemapRect(param0->unk_0C, 4, 0x0, 0, 0, 32, 32, 17);
		
		BoxComparison_020385F4(param0);
		BoxComparison_02038658(param0);
		
		param0->unk_378 = BoxApp_GetCompareMode(param0->unk_08);
		
		Bg_CopyTilemapBufferToVRAM(param0->unk_0C, 4);
		BoxComparison_02038C60(param0);
	}
}


void BoxComparison_0203846C(BoxComparisonManager* param0, BOOL param1) {
	if (param0 != NULL) {
		int v0 = BoxApp_GetCompareMonSlot(param0->unk_08);
		
		if (param1) {
			Sprite_SetAnim(param0->unk_28[v0 ^ 1], 2);
			Sprite_SetAnim(param0->unk_28[v0], 0);
		} else {
			Sprite_SetAnim(param0->unk_28[v0 ^ 1], 1);
			Sprite_SetAnim(param0->unk_28[v0], 0);
		}
		
		BoxComparison_0203858C(param0);
	}
}


void BoxComparison_020384C4(BoxComparisonManager* param0) {
	BoxComparison_02038F34(param0);
	
	if (param0 != NULL) {
		int compareMonSlot = BoxApp_GetCompareMonSlot(param0->unk_08);
		
		BoxComparison_02038FC8(param0, compareMonSlot);
		
		switch (BoxApp_GetCompareMode(param0->unk_08)) {
			case COMPARE_BATTLE_STATS:
				BoxComparison_02038A3C(param0, compareMonSlot);
				break;
			
			case COMPARE_CONTEST_STATS:
				BoxComparison_02038808(param0, compareMonSlot);
				break;
			
			case COMPARE_MOVES:
				BoxComparison_02038B60(param0, compareMonSlot);
				break;
		}
	}
}


BOOL BoxComparison_02038514(BoxComparisonManager* param0) {
	return TRUE;
}


void BoxComparison_02038518(BoxComparisonManager* param0) {
	if (param0 != NULL) {
		param0->unk_37C = 0;
		BoxComparison_0203875C(param0, param0->unk_378);
		Bg_CopyTilemapBufferToVRAM(param0->unk_0C, 4);
	}
}


BOOL BoxComparison_02038538(BoxComparisonManager* param0) {
	if (param0 != NULL) {
		switch (param0->unk_37C) {
			case 0:
				BoxComparison_020385F4(param0);
				param0->unk_37C++;
				break;
				// Fall through
			
			case 1:
				BoxComparison_02038970(param0);
				BoxComparison_020386F4(param0);
				Bg_CopyTilemapBufferToVRAM(param0->unk_0C, 4);
				param0->unk_37C++;
				break;
			
			case 2:
				return TRUE;
		}
		
		return  FALSE;
	}
	
	return TRUE;
}


static void BoxComparison_0203858C(BoxComparisonManager* param0) {
	NNSG2dScreenData* v0 = (BoxApp_GetCompareMonSlot(param0->unk_08) == 0) ? param0->unk_368 : param0->unk_36C;
	
	Bg_CopyToTilemapRect(param0->unk_0C, 2, 0, 0, 6, 32, v0->rawData, 0, 0, 32, 32);
	Bg_CopyToTilemapRect(param0->unk_0C, 2, 26, 0, 6, 32, v0->rawData, 26, 0, 32, 32);
	Bg_CopyTilemapBufferToVRAM(param0->unk_0C, 2);
}


static void BoxComparison_020385F4(BoxComparisonManager* param0) {
	switch (BoxApp_GetCompareMode(param0->unk_08)) {
		case COMPARE_BATTLE_STATS:
			Bg_ToggleLayer(BG_LAYER_SUB_1, TRUE);
			Bg_ToggleLayer(BG_LAYER_SUB_2, FALSE);
			Bg_ToggleLayer(BG_LAYER_SUB_3, FALSE);
			break;
		
		case COMPARE_CONTEST_STATS:
			Bg_ToggleLayer(BG_LAYER_SUB_2, TRUE);
			Bg_ToggleLayer(BG_LAYER_SUB_1, FALSE);
			Bg_ToggleLayer(BG_LAYER_SUB_3, FALSE);
			break;
		
		case COMPARE_MOVES:
			Bg_ToggleLayer(BG_LAYER_SUB_3, TRUE);
			Bg_ToggleLayer(BG_LAYER_SUB_1, FALSE);
			Bg_ToggleLayer(BG_LAYER_SUB_2, FALSE);
			break;
	}
}


static void BoxComparison_02038658(BoxComparisonManager* param0) {
	static const WindowTemplate v0[] = {
		{ 4,  3, 1, 9,  2, 0,  1 },
		{ 4, 20, 1, 9,  2, 1, 19 },
		{ 4, 13, 4, 6, 16, 0, 37 }
	};
	
	int v1;
	for (v1 = 0; v1 < 7; v1++) {
		param0->unk_78[v1].pixels = NULL;
	}

	for (v1 = 0; v1 < NELEMS(v0); v1++) {
		Window_AddFromTemplate(param0->unk_0C, &param0->unk_78[0 + v1], &v0[v1]);
		Window_PutToTilemap(&param0->unk_78[0 + v1]);
	}
	
	BoxComparison_02038970(param0);
	Window_FillTilemap(&param0->unk_78[0], 0);
	Window_FillTilemap(&param0->unk_78[1], 0);
	
	for (v1 = 0; v1 < NELEMS(v0); v1++) {
		Window_LoadTiles(&param0->unk_78[0 + v1]);
	}
	
	BoxComparison_020386F4(param0);
}


static void BoxComparison_020386CC(BoxComparisonManager* param0) {
	for (int i = 0; i < 7; i++) {
		if (param0->unk_78[i].pixels) {
			Window_Remove(&(param0->unk_78[i]));
		}
	}
}


static void BoxComparison_020386F4(BoxComparisonManager* param0) {
	int v0 = BoxApp_GetCompareMode(param0->unk_08);
	
	switch (BoxApp_GetCompareMode(param0->unk_08)) {
		case COMPARE_BATTLE_STATS:
			BoxComparison_02038798(param0);
			BoxComparison_02038A3C(param0, 0);
			BoxComparison_02038A3C(param0, 1);
			break;
		
		case COMPARE_CONTEST_STATS:
			BoxComparison_02038808(param0, 0);
			BoxComparison_02038808(param0, 1);
			break;
		
		case COMPARE_MOVES:
			BoxComparison_02038900(param0);
			BoxComparison_02038B60(param0, 0);
			BoxComparison_02038B60(param0, 1);
			break;
	}

	param0->unk_378 = v0;
}


static void BoxComparison_0203875C(BoxComparisonManager* param0, int param1) {
	switch (param1) {
		case 0:
			BoxComparison_020387D0(param0);
			BoxComparison_02038A24(param0);
			break;
		
		case 1:
			BoxComparison_020388E0(param0, 0);
			BoxComparison_020388E0(param0, 1);
			break;
		
		case 2:
			BoxComparison_02038938(param0);
			BoxComparison_02038A24(param0);
			break;
	}
}


static void BoxComparison_02038798(BoxComparisonManager* param0) {
	static const WindowTemplate v0[] = {
		{ 4,  3, 4, 9, 16, 0, 133 },
		{ 4, 20, 4, 9, 16, 1, 277 }
	};
	
	for (int i = 0; i < NELEMS(v0); i++) {
		Window_AddFromTemplate(param0->unk_0C, &param0->unk_78[3 + i], &v0[i]);
		Window_PutToTilemap(&param0->unk_78[3 + i]);
		Window_FillTilemap(&param0->unk_78[3 + i], 0);
	}
}


static void BoxComparison_020387D0(BoxComparisonManager* param0) {
	for (int i = 3; i <= 4; i++) {
		if (param0->unk_78[i].pixels) {
			Window_ClearTilemap(&(param0->unk_78[i]));
			Window_Remove(&(param0->unk_78[i]));
			param0->unk_78[i].pixels = NULL;
		}
	}
}


static void BoxComparison_02038808(BoxComparisonManager* param0, int compareMonSlot) {
#define DEFINE_00(tx) ((128 << FX32_SHIFT) + (((((tx) - 128) << FX32_SHIFT) * 10) / 100))
#define DEFINE_01(ty) ((484 << FX32_SHIFT) + (((((ty) - 484) << FX32_SHIFT) * 10) / 100))
	
	static const struct {
		fx32  unk_00;
		fx32  unk_04;
		fx32  unk_08;
		fx32  unk_0C;
	} v0[] = {
		{ DEFINE_00(128), DEFINE_01(438), 128 << FX32_SHIFT, 438 << FX32_SHIFT },
		{ DEFINE_00(172), DEFINE_01(470), 172 << FX32_SHIFT, 470 << FX32_SHIFT },
		{ DEFINE_00(155), DEFINE_01(520), 155 << FX32_SHIFT, 520 << FX32_SHIFT },
		{ DEFINE_00(100), DEFINE_01(520), 100 << FX32_SHIFT, 520 << FX32_SHIFT },
		{ DEFINE_00(84),  DEFINE_01(470),  84 << FX32_SHIFT, 470 << FX32_SHIFT }
	};
	
	const PCCompareMon* compareMon;
	BOOL isMonUnderCursor, v3, v4;
	
	compareMon = BoxApp_GetCompareMonFrom(param0->unk_08, compareMonSlot);
	isMonUnderCursor = BoxApp_IsMonUnderCursor(param0->unk_08);
	v3 = BoxApp_GetCompareMonSlot(param0->unk_08) == compareMonSlot;
	v4 = BoxApp_CompareSlotHasMon(param0->unk_08, compareMonSlot);
	
	if (((isMonUnderCursor == TRUE && v3 == 1) || (v3 == 0 && v4 == 1)) && compareMon->isEgg == FALSE) {
		int i;
		VecFx32 v6;
		const u16* contestStat = &compareMon->cool;
		
		v6.z = 0;
		
		for (i = 0; i < CONTEST_TYPE_MAX; i++) {
			v6.x = v0[i].unk_00 + (((v0[i].unk_08 - v0[i].unk_00) / 256) * (int)(*contestStat));
			v6.y = v0[i].unk_04 + (((v0[i].unk_0C - v0[i].unk_04) / 256) * (int)(*contestStat));
			Sprite_SetPosition(param0->unk_34[compareMonSlot][i], &v6);
			BoxGraphics_SetSpritePriority(param0->unk_34[compareMonSlot][i], 0);
			BoxGraphics_SetSpritePriority(param0->unk_34[compareMonSlot ^ 1][i], 1);
			Sprite_SetDrawFlag(param0->unk_34[compareMonSlot][i], TRUE);
			contestStat++;
		}
	} else {
		BoxComparison_020388E0(param0, compareMonSlot);
	}
	
#undef DEFINE_00
#undef DEFINE_01
}


static void BoxComparison_020388E0(BoxComparisonManager* param0, int compareMonSlot) {
	for (int i = 0; i < 5; i++) {
		Sprite_SetDrawFlag(param0->unk_34[compareMonSlot][i], FALSE);
	}
}


static void BoxComparison_02038900(BoxComparisonManager* param0) {
	static const WindowTemplate v0[] = {
		{ 4,  1, 4, 11, 12, 0, 133 },
		{ 4, 20, 4, 11, 12, 1, 265 }
	};
	
	for (int i = 0; i < NELEMS(v0); i++) {
		Window_AddFromTemplate(param0->unk_0C, &param0->unk_78[5 + i], &v0[i]);
		Window_PutToTilemap(&param0->unk_78[5 + i]);
		Window_FillTilemap(&param0->unk_78[5 + i], 0);
	}
}


static void BoxComparison_02038938(BoxComparisonManager* param0) {
	for (int i = 5; i <= 6; i++) {
		if (param0->unk_78[i].pixels) {
			Window_ClearTilemap(&(param0->unk_78[i]));
			Window_Remove(&(param0->unk_78[i]));
			param0->unk_78[i].pixels = NULL;
		}
	}
}


static inline u32 BoxComparison_Inline0(Window* param0, u32 param1, const String* param2) {
	return ((param0->width * 8) - Font_CalcStringWidth(param1, param2, 0)) / 2;
}


static void BoxComparison_02038970(BoxComparisonManager* param0) {
	static const u16 compareMessages[] = {
		BOX_MESSAGE_Nature,
		BOX_MESSAGE_Level,
		BOX_MESSAGE_HP,
		BOX_MESSAGE_Attack,
		BOX_MESSAGE_Defense,
		BOX_MESSAGE_SpecialAttack,
		BOX_MESSAGE_SpecialDefense,
		BOX_MESSAGE_Speed,
	};
	
	Window* v1 = &param0->unk_78[2];
	int v2;
	Window_FillTilemap(v1, 0);
	
	switch (BoxApp_GetCompareMode(param0->unk_08)) {
		case COMPARE_BATTLE_STATS:
			for (v2 = 0; v2 < NELEMS(compareMessages); v2++) {
				MessageLoader_GetString(param0->unk_6C, compareMessages[v2], param0->boxDisplayText);
				
				Text_AddPrinterWithParamsAndColor(
					v1,
					FONT_SYSTEM,
					param0->boxDisplayText,
					BoxComparison_Inline0(v1, 0, param0->boxDisplayText),
					16 * v2,
					TEXT_SPEED_NO_TRANSFER,
					TEXT_COLOR(15, 14, 0),
					NULL);
			}
			break;
		
		case COMPARE_MOVES:
			MessageLoader_GetString(param0->unk_6C, BOX_MESSAGE_Move, param0->boxDisplayText);
			
			Text_AddPrinterWithParamsAndColor(
				v1,
				FONT_SYSTEM,
				param0->boxDisplayText,
				BoxComparison_Inline0(v1, 0, param0->boxDisplayText), 
				0,
				TEXT_SPEED_NO_TRANSFER,
				TEXT_COLOR(15, 14, 0),
				NULL);
			
			break;
	}
	
	Window_LoadTiles(v1);
}


static void BoxComparison_02038A24(BoxComparisonManager* param0) {
	Window* v0 = &param0->unk_78[2];
	Window_FillTilemap(v0, 0);
	Window_LoadTiles(v0);
}


static void BoxComparison_02038A3C(BoxComparisonManager* param0, int compareMonSlot) {
	Window* window;
	const PCCompareMon* compareMon;
	BOOL isMonUnderCursor, compareSlot, v4;
	
	window = &(param0->unk_78[3 + compareMonSlot]);
	compareMon = BoxApp_GetCompareMonFrom(param0->unk_08, compareMonSlot);
	isMonUnderCursor = BoxApp_IsMonUnderCursor(param0->unk_08);
	compareSlot = BoxApp_GetCompareMonSlot(param0->unk_08) == compareMonSlot;
	v4 = BoxApp_CompareSlotHasMon(param0->unk_08, compareMonSlot);
	
	Window_FillTilemap(window, 0);
	
	if ((isMonUnderCursor == TRUE && compareSlot == 1) || (compareSlot == 0 && v4 == 1)) {
		int v5;
		if (compareMon->isEgg == FALSE) {
			const u16* v6 = &compareMon->level;
			
			Text_AddPrinterWithParamsAndColor(
				window,
				FONT_SYSTEM,
				compareMon->nature,
				BoxComparison_Inline0(window, 0, compareMon->nature),
				0,
				TEXT_SPEED_NO_TRANSFER,
				TEXT_COLOR(15, 14, 0),
				NULL);
			
			for (v5 = 0; v5 < 7; v5++) {
				String_FormatInt(param0->boxDisplayText, v6[v5], 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
				
				Text_AddPrinterWithParamsAndColor(
					window,
					FONT_SYSTEM, 
					param0->boxDisplayText,
					BoxComparison_Inline0(window, 0, param0->boxDisplayText),
					(1 + v5) * 16,
					TEXT_SPEED_NO_TRANSFER,
					TEXT_COLOR(15, 14, 0),
					NULL);
			}
		} else {
			MessageLoader_GetString(param0->unk_6C, 44, param0->boxDisplayText);
			
			for (v5 = 0; v5 < 8; v5++) {
				Text_AddPrinterWithParamsAndColor(
					window,
					FONT_SYSTEM,
					param0->boxDisplayText,
					BoxComparison_Inline0(window, 0, param0->boxDisplayText),
					v5 * 16,
					TEXT_SPEED_NO_TRANSFER,
					TEXT_COLOR(15, 14, 0),
					NULL);
			}
		}
	}
	
	Window_LoadTiles(window);
}


static void BoxComparison_02038B60(BoxComparisonManager* param0, int compareMonSlot) {
	Window* v0;
	const PCCompareMon* compareMon;
	BOOL isMonUnderCursor, v3, v4;
	
	v0 = &(param0->unk_78[5 + compareMonSlot]);
	compareMon = BoxApp_GetCompareMonFrom(param0->unk_08, compareMonSlot);
	isMonUnderCursor = BoxApp_IsMonUnderCursor(param0->unk_08);
	v3 = BoxApp_GetCompareMonSlot(param0->unk_08) == compareMonSlot;
	v4 = BoxApp_CompareSlotHasMon(param0->unk_08, compareMonSlot);
	
	Window_FillTilemap(v0, 0);
	
	if ((isMonUnderCursor == TRUE && v3 == 1) || (v3 == 0 && v4 == 1)) {
		int i;
		if (compareMon->isEgg == FALSE) {
			for (i = 0; i < LEARNED_MOVES_MAX; i++) {
				if (compareMon->moves[i]) {
					MessageLoader_GetString(param0->unk_70, compareMon->moves[i], param0->boxDisplayText);
					Text_AddPrinterWithParamsAndColor(v0, FONT_SYSTEM, param0->boxDisplayText, BoxComparison_Inline0(v0, 0, param0->boxDisplayText), 4 + 24 * i, TEXT_SPEED_NO_TRANSFER, TEXT_COLOR(15, 14, 0), NULL);
				}
			}
		} else {
			MessageLoader_GetString(param0->unk_6C, 44, param0->boxDisplayText);
			for (i = 0; i < 4; i++) {
				Text_AddPrinterWithParamsAndColor(v0, FONT_SYSTEM, param0->boxDisplayText, BoxComparison_Inline0(v0, 0, param0->boxDisplayText), 4 + 24 * i, TEXT_SPEED_NO_TRANSFER, TEXT_COLOR(15, 14, 0), NULL);
			}
		}
	}
	
	Window_LoadTiles(v0);
}


static void BoxComparison_02038C60(BoxComparisonManager* param0) {
	static const struct {
		s16  unk_00;
		s16  unk_02;
	} v0[] = {
		{   0, 568 },
		{ 256, 568 },
	};
	
	NNSG2dImageProxy v1;
	SpriteResourcesHeader v2;
	
	// TODO naix
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		param0->unk_5C = Graphics_GetCellBank(narc, 111, 1, &(param0->unk_60), HEAP_ID_BOX_GRAPHICS);
		param0->unk_64 = Graphics_GetAnimBank(narc, 112, 1, &(param0->unk_68), HEAP_ID_BOX_GRAPHICS);
		Graphics_LoadPalette(narc, 113, 5, 0, 0x20 * 3, HEAP_ID_BOX_GRAPHICS);
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		param0->unk_5C = Graphics_GetCellBank(narc, 135, 1, &(param0->unk_60), HEAP_ID_BOX_GRAPHICS);
		param0->unk_64 = Graphics_GetAnimBank(narc, 136, 1, &(param0->unk_68), HEAP_ID_BOX_GRAPHICS);
		Graphics_LoadPalette(narc, 137, 5, 0, 0x20 * 3, HEAP_ID_BOX_GRAPHICS);
	}
	
	NNS_G2dInitImageProxy(&v1);
	
	u32 v3;
	if (gIsDiamondPearl) {
		NarcID narc = NARC_INDEX_DP_GRAPHIC__BOX;
		v3 = Graphics_LoadImageMapping(narc, 110, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0, 10, &v1);
	} else {
		NarcID narc = NARC_INDEX_PL_GRAPHIC__BOX;
		v3 = Graphics_LoadImageMapping(narc, 134, 1, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0, 10, &v1);
	}
	
	BoxGraphics_020308E8(&v2, &v1, BoxGraphics_02030878(param0->unk_04), param0->unk_60, param0->unk_68, 1);
	
	for (u32 v4 = 0; v4 < 2; v4++) {
		param0->unk_28[v4] = BoxGraphics_02030908(param0->unk_10, &v2, v0[v4].unk_00, v0[v4].unk_02, 0, NNS_G2D_VRAM_TYPE_2DSUB);
		
		Sprite_SetExplicitPalette(param0->unk_28[v4], v4);
		Sprite_SetAnim(param0->unk_28[v4], 0);
		
		for (u32 v5 = 0; v5 < 5; v5++) {
			param0->unk_34[v4][v5] = BoxGraphics_02030908(param0->unk_10, &v2, 0, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB);
			Sprite_SetAnim(param0->unk_34[v4][v5], 5 + v4);
			Sprite_SetExplicitPalette(param0->unk_34[v4][v5], v4);
			Sprite_SetDrawFlag(param0->unk_34[v4][v5], FALSE);
		}
	}
	
	Sprite_SetFlipMode(param0->unk_28[1], 1);
	param0->unk_30 = BoxGraphics_02030908(param0->unk_10, &v2, 128, 176 + 384, 0, NNS_G2D_VRAM_TYPE_2DSUB);
	Sprite_SetAnim(param0->unk_30, 3);
	Sprite_SetExplicitPalette(param0->unk_30, 2);
	param0->unk_380 = SysTask_Start(BoxComparison_02039030, param0, 1);
	
	BoxComparison_02038E30(param0, v3);
}


static void BoxComparison_02038E30(BoxComparisonManager* param0, u32 param1) {
	static const struct {
		s16  unk_00;
		s16  unk_02;
	} v0[] = {
		{ 112, 394 },
		{ 144, 394 },
	};
	
	NNSG2dImageProxy v1;
	SpriteResourcesHeader v2;
	
	BoxGraphics_020308E8(&v2, &v1, BoxGraphics_02030878(param0->unk_04), BoxIcon_02033B48(param0->unk_14), BoxIcon_02033B4C(param0->unk_14), 1);
	
	for (int i = 0; i < 2; i++) {
		param0->unk_18[i] = param1 + (i * (4 * 4) * 0x20);
		NNS_G2dInitImageProxy(&v1);
		NNS_G2dSetImageLocation(&v1, NNS_G2D_VRAM_TYPE_2DSUB, param0->unk_18[i]);
		v1.attr.mappingType = GXS_GetOBJVRamModeChar();
		param0->unk_20[i] = BoxGraphics_02030908(param0->unk_10, &v2, v0[i].unk_00, v0[i].unk_02, 1 - i, NNS_G2D_VRAM_TYPE_2DSUB);
		Sprite_SetDrawFlag(param0->unk_20[i], FALSE);
	}
}


static void BoxComparison_02038ED0(BoxComparisonManager* param0) {
	if (param0->unk_380) {
		SysTask_Done(param0->unk_380);
	}
	
	for (int i = 0; i < 2; i++) {
		Sprite_Delete(param0->unk_20[i]);
		Sprite_Delete(param0->unk_28[i]);

		for (int j = 0; j < 5; j++) {
			Sprite_Delete(param0->unk_34[i][j]);
		}
	}
	
	Sprite_Delete(param0->unk_30);
	Heap_Free(param0->unk_5C);
	Heap_Free(param0->unk_64);
}


static void BoxComparison_02038F34(BoxComparisonManager* param0) {
	int compareMonSlot = BoxApp_GetCompareMonSlot(param0->unk_08);
	const PCCompareMon* compareMon = BoxApp_GetCompareMonFrom(param0->unk_08, compareMonSlot);
	Sprite* v2 = param0->unk_20[compareMonSlot];
	BoxPokemon* boxMon = compareMon->mon;
	NNSG2dCharacterData* v4;
	
	if (BoxApp_IsMonUnderCursor(param0->unk_08)) {
		u32 v5 = 4 + PokeIconPaletteIndex(compareMon->species, compareMon->form, compareMon->isEgg);
		
		BoxIcon_02033A80(param0->unk_14, param0->unk_E8, BoxPokemon_IconSpriteIndex(boxMon), sizeof(param0->unk_E8));
		
		NNS_G2dGetUnpackedCharacterData(param0->unk_E8, &v4);
		DC_FlushRange(v4->pRawData, (4 * 4) * 0x20);
		GXS_LoadOBJ(v4->pRawData, param0->unk_18[compareMonSlot], (4 * 4) * 0x20);
		
		Sprite_SetExplicitPalette(v2, v5);
		Sprite_SetDrawFlag(v2, TRUE);
	} else {
		Sprite_SetDrawFlag(v2, FALSE);
	}
}


static void BoxComparison_02038FC8(BoxComparisonManager* param0, int compareMonSlot) {
	compareMonSlot = BoxApp_GetCompareMonSlot(param0->unk_08);
	Window* window = &(param0->unk_78[0 + compareMonSlot]);
	Window_FillTilemap(window, 0);
	
	if (BoxApp_IsMonUnderCursor(param0->unk_08)) {
		const PCCompareMon* compareMon = BoxApp_GetCompareMonFrom(param0->unk_08, compareMonSlot);
		
		Text_AddPrinterWithParamsAndColor(
			window,
			FONT_SYSTEM,
			compareMon->monName,
			BoxComparison_Inline0(window, 0, compareMon->monName),
			0,
			TEXT_SPEED_NO_TRANSFER,
			TEXT_COLOR(15, 14, 0),
			NULL);
	}
	
	Window_LoadTiles(window);
}


static void BoxComparison_02039030(SysTask* param0, void* param1) {
	BoxComparisonManager* v0 = param1;
	BOOL pressedAnimation = BoxApp_IsCompareButtonPressed(v0->unk_08);
	
	if (Sprite_GetActiveAnim(v0->unk_30) == 3) {
		if (pressedAnimation) {
			Sprite_SetAnim(v0->unk_30, 4);
		}
	} else {
		if (pressedAnimation == FALSE) {
			Sprite_SetAnim(v0->unk_30, 3);
		}
	}
}
