// File not documented

#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_020346FC.h"

#include "applications/pc_boxes/box_def.h"

#include "mpr_client/mpr_text.h"

#include "bg_window.h"
#include "game_version.h"
#include "graphics.h"
#include "heap.h"
#include "menu.h"
#include "message.h"
#include "narc.h"
#include "render_window.h"
#include "sprite.h"
#include "string_gf.h"
#include "string_template.h"
#include "text.h"

static void BoxWindow_020348C8(BgConfig* param0, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5);
static void BoxWindow_02034A78(BoxWindowManager* param0, Window* param1, u8 markings, u32 markingID);
static void BoxWindow_02034AC4(BoxWindowManager* param0, u32 param1);
static void BoxWindow_02034BFC(BgConfig* param0, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5);
static void BoxWindow_02034C8C(BgConfig* param0, Window* param1);
static void BoxWindow_02034CD8(BgConfig* param0, Window* param1);
static void BoxWindow_02034D24(u32 messageID, String* string);


BOOL BoxWindow_020346FC(
	BoxWindowManager* param0,
	BoxApplicationDisplay* param1,
	const BoxApplication* param2,
	BgConfig* param3,
	SpriteList* param4,
	MessageLoader* boxMessagesLoader,
	const StringTemplate* messageVariableBuffer,
	int optionsFrame
) {
	param0->unk_0C = param1;
	param0->unk_00 = param3;
	param0->unk_08 = param2;
	param0->unk_2C = messageVariableBuffer;
	param0->unk_20 = optionsFrame;
	param0->unk_10 = boxMessagesLoader;
	
	// TODO naix
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	param0->unk_14 = MessageLoader_Init(MSG_LOADER_PRELOAD_ENTIRE_BANK, narc, TEXT_BANK_UNIFIED_POKEMON_STORAGE_SYSTEM, HEAP_ID_BOX_GRAPHICS);
	param0->unk_30 = String_Init(64, HEAP_ID_BOX_GRAPHICS);
	param0->unk_34 = String_Init(64, HEAP_ID_BOX_GRAPHICS);
	param0->unk_1C = 0;
	param0->unk_1E = 0;
	param0->unk_18 = NULL;
	
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	param0->unk_24 = Graphics_GetCharData(narc, 25, 1, &(param0->unk_28), HEAP_ID_BOX_GRAPHICS);
	
	if ((param0->unk_24 == NULL) || (param0->unk_30 == NULL) || (param0->unk_34 == NULL)) {
		return FALSE;
	}
	
	return TRUE;
}


void BoxWindow_02034774(BoxWindowManager* param0) {
	if (param0->unk_18 != NULL) {
		for (int v0 = 0; v0 < 2; v0++) {
			Window_Remove(&(param0->unk_18[v0]));
		}
		
		Heap_Free(param0->unk_18);
		param0->unk_18 = NULL;
	}
	
	if (param0->unk_24) {
		Heap_Free(param0->unk_24);
	}
	
	if (param0->unk_14) {
		MessageLoader_Free(param0->unk_14);
	}
	
	if (param0->unk_30) {
		String_Free(param0->unk_30);
	}
	
	if (param0->unk_34) {
		String_Free(param0->unk_34);
	}
}


void BoxWindow_020347C8(BoxWindowManager* param0) {
	static const WindowTemplate v0[] = {
		{ 1,  2, 21, 27,  2, 4, 539 },
		{ 1, 19,  3, 12, 16, 4, 593 }
	};
	
	param0->unk_18 = Window_New(HEAP_ID_BOX_GRAPHICS, 2);
	
	if (param0->unk_18) {
		int v1;

		for (v1 = 0; v1 < 2; v1++) {
			Window_AddFromTemplate(param0->unk_00, &param0->unk_18[v1], &v0[v1]);
		}
	}
	
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__WINFRAME;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__PL_WINFRAME;
	}
	Graphics_LoadTilesToBgLayer(narc, 0, param0->unk_00, 1, 512, 0, 0, HEAP_ID_BOX_GRAPHICS);
	Graphics_LoadPalette(narc, 24, 0, 7 * 0x20, 0x20, HEAP_ID_BOX_GRAPHICS);
	LoadMessageBoxGraphics(param0->unk_00, BG_LAYER_MAIN_1, 521, 8, param0->unk_20, HEAP_ID_BOX_GRAPHICS);
}


void BoxWindow_0203484C(BoxWindowManager* param0, u32 boxMessageID) {
	Window_PutToTilemap(&param0->unk_18[0]);
	BoxWindow_020348C8(param0->unk_00, param0->unk_18[0].bgLayer, param0->unk_18[0].tilemapLeft, param0->unk_18[0].tilemapTop, param0->unk_18[0].width, param0->unk_18[0].height);
	Window_FillTilemap(&param0->unk_18[0], 15);
	
	if (boxMessageID & MPR_TEXT_FLAG) {
		BoxWindow_02034D24(boxMessageID, param0->unk_30);
	} else {
		MessageLoader_GetString(param0->unk_10, boxMessageID, param0->unk_30);
	}
	
	StringTemplate_Format(param0->unk_2C, param0->unk_34, param0->unk_30);
	Text_AddPrinterWithParams(&(param0->unk_18[0]), FONT_MESSAGE, param0->unk_34, 0, 0, TEXT_SPEED_NO_TRANSFER, NULL);
	Window_LoadTiles(&(param0->unk_18[0]));
	Bg_CopyTilemapBufferToVRAM(param0->unk_00, 1);
	
	param0->unk_1C = 1;
}


static void BoxWindow_020348C8(BgConfig* param0, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5) {
	u16* v0;
	u32 v1, v2;
	
	v0 = Bg_GetTilemapBuffer(param0, param1);
	v0 += (((param3 - 1) * 32) + (param2 - 2));
	
	v0[0] = (8 << 12) | 521;
	v0[1] = (8 << 12) | (521 + 1);
	
	for (v1 = 0; v1 < param4; v1++) {
		v0[2 + v1] = (8 << 12) | (521 + 2);
	}
	
	v0[2 + param4] = (8 << 12) | (521 + 3);
	v0[3 + param4] = (8 << 12) | (521 + 4);
	v0[4 + param4] = (8 << 12) | (521 + 5);
	
	v0 += 32;
	
	for (v1 = 0; v1 < param5; v1++) {
		v0[0] = (8 << 12) | (521 + 6);
		v0[1] = (8 << 12) | (521 + 7);
		v0[2 + param4] = (8 << 12) | (521 + 9);
		v0[3 + param4] = (8 << 12) | (521 + 10);
		v0[4 + param4] = (8 << 12) | (521 + 11);
		v0 += 32;
	}
	
	v0[0] = (8 << 12) | (521 + 12);
	v0[1] = (8 << 12) | (521 + 13);
	
	for (v1 = 0; v1 < param4; v1++) {
		v0[2 + v1] = (8 << 12) | (521 + 14);
	}
	
	v0[2 + param4] = (8 << 12) | (521 + 15);
	v0[3 + param4] = (8 << 12) | (521 + 16);
	v0[4 + param4] = (8 << 12) | (521 + 17);
}


void BoxWindow_02034990(BoxWindowManager* param0, const BoxMenu* menu) {
	Window* v0;
	u32 v1, i;
	
	if (param0->unk_1E) {
		BoxWindow_02034C8C(param0->unk_00, &param0->unk_18[1]);
	}
	
	v0 = &(param0->unk_18[1]);
	Window_FillTilemap(v0, 15);
	v1 = (8 - menu->totalMenuItems) * 16;
	
	for (i = 0; i < menu->totalMenuItems; i++) {
		if ((menu->menuItems[i] >= BOX_MENU_FIRST_MARKING) && (menu->menuItems[i] <= BOX_MENU_LAST_MARKING)) {
			BoxWindow_02034A78(param0, v0, menu->markings, menu->menuItems[i] - BOX_MENU_FIRST_MARKING);
		} else {
			if (menu->menuItems[i] >= (MPR_TEXT_PCDeposit - 3) && menu->menuItems[i] < (MPR_TEXT_PCLast - 3 + 1)) { // ?
				BoxWindow_02034D24(menu->menuItems[i], param0->unk_30);
			} else {
				MessageLoader_GetString(param0->unk_14, 24 + menu->menuItems[i], param0->unk_30);
			}
			
			TextColor color;
			if ((menu->markings & (1 << i)) == 0) {
				color = TEXT_COLOR(11, 12, 15);
			} else {
				color = TEXT_COLOR(12, 15, 15);
			}
			Text_AddPrinterWithParamsAndColor(v0, FONT_SYSTEM, param0->unk_30, 10, v1, TEXT_SPEED_NO_TRANSFER, color, NULL);
		}
		
		v1 += 16;
	}
	
	BoxGraphics_DrawMenuCursorBox(param0, menu);
	BoxWindow_02034AC4(param0, menu->totalMenuItems);
	
	Window_LoadTiles(v0);
	Bg_CopyTilemapBufferToVRAM(param0->unk_00, 1);
	
	param0->unk_1E = 1;
}


static void BoxWindow_02034A78(BoxWindowManager* param0, Window* param1, u8 markings, u32 markingID) {
	Window_BlitBitmapRectWithTransparency(param1, param0->unk_28->pRawData, markingID * 8, ((markings & (1 << markingID)) ? 0 : 8), 48, 16, 44, markingID * 16 + 4, 8, 8, 0xFF);
}


static void BoxWindow_02034AC4(BoxWindowManager* param0, u32 param1) {
	u16* v0;
	u32 v1, v2, v3, v4;
	
	v0 = Bg_GetTilemapBuffer(param0->unk_00, 1);
	v0 += (3 * 32 + 19);
	v3 = 593;
	v4 = 8 - param1;
	v0 += (v4 * 2 * 32);
	v3 += (v4 * 2 * 12);
	
	for (v2 = 0; v2 < (param1 * 2); v2++) {
		for (v1 = 0; v1 < 12; v1++) {
			v0[v1] = (4 << 12) | v3;
			v3++;
		}
		
		v0 += 32;
	}
	
	BoxWindow_02034BFC(param0->unk_00, 1, 19, 3 + v4 * 2, 12, param1 * 2);
}


void BoxWindow_02034B30(BoxWindowManager* param0) {
	if (param0->unk_1C) {
		BoxWindow_02034CD8(param0->unk_00, &param0->unk_18[0]);
		param0->unk_1C = 0;
	}
	
	if (param0->unk_1E) {
		BoxWindow_02034C8C(param0->unk_00, &param0->unk_18[1]);
		param0->unk_1E = 0;
	}
	
	Bg_CopyTilemapBufferToVRAM(param0->unk_00, 1);
}


void BoxWindow_02034B64(BoxWindowManager* param0) {
	if (param0->unk_1E) {
		BoxWindow_02034C8C(param0->unk_00, &param0->unk_18[1]);
		param0->unk_1E = 0;
	}
	
	Bg_CopyTilemapBufferToVRAM(param0->unk_00, 1);
}


void BoxGraphics_DrawMenuCursorBox(BoxWindowManager* param0, const BoxMenu* menu) {
	Window* menuWindow;
	u32 v1;
	
	menuWindow = &(param0->unk_18[1]);
	v1 = (8 - menu->totalMenuItems) * 16;
	
	Window_FillRectWithColor(menuWindow, 15, 0, v1, 10, menu->totalMenuItems * 16);
	Window_DrawMenuCursor(menuWindow, 0, v1 + (menu->selectedMenuItemIndex * 16));
	Window_LoadTiles(menuWindow);
}


void BoxWindow_02034BD0(BoxWindowManager* param0, const BoxMenu* menu) {
	Window* v0;
	int v1;
	
	v0 = &(param0->unk_18[1]);
	
	for (v1 = 0; v1 < 6; v1++) {
		BoxWindow_02034A78(param0, v0, menu->markings, v1);
	}
	
	Window_LoadTiles(v0);
}


static void BoxWindow_02034BFC(BgConfig* param0, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5) {
	u16* v0;
	u32 v1;
	
	v0 = Bg_GetTilemapBuffer(param0, param1);
	v0 += (((param3 - 1) * 32) + (param2 - 1));
	*v0 = (7 << 12) | 512;
	
	for (v1 = 0; v1 < param4; v1++) {
		v0[v1 + 1] = (7 << 12) | (512 + 1);
	}
	
	v0[param4 + 1] = (7 << 12) | (512 + 2);
	v0 += 32;
	
	for (v1 = 0; v1 < param5; v1++) {
		*v0 = (7 << 12) | (512 + 3);
		v0[param4 + 1] = (7 << 12) | (512 + 5);
		v0 += 32;
	}
	
	*v0 = (7 << 12) | (512 + 6);
	
	for (v1 = 0; v1 < param4; v1++) {
		v0[v1 + 1] = (7 << 12) | (512 + 7);
	}
	
	v0[param4 + 1] = (7 << 12) | (512 + 8);
}


static void BoxWindow_02034C8C(BgConfig* param0, Window* param1) {
	u16* v0;
	u32 v1, v2, v3, v4;
	
	v0 = Bg_GetTilemapBuffer(param0, param1->bgLayer);
	v0 += (((param1->tilemapTop - 1) * 32) + (param1->tilemapLeft - 1));
	
	for (v2 = 0; v2 < (param1->height + 2); v2++) {
		for (v1 = 0; v1 < (param1->width + 2); v1++) {
			v0[v1] = 0x0;
		}
		
		v0 += 32;
	}
}


static void BoxWindow_02034CD8(BgConfig* param0, Window* param1) {
	u16* v0;
	u32 v1, v2, v3, v4;
	
	v0 = Bg_GetTilemapBuffer(param0, param1->bgLayer);
	v0 += (((param1->tilemapTop - 1) * 32) + (param1->tilemapLeft - 2));
	
	for (v2 = 0; v2 < (param1->height + 2); v2++) {
		for (v1 = 0; v1 < (param1->width + 5); v1++) {
			v0[v1] = 0x0;
		}
		
		v0 += 32;
	}
}


static void BoxWindow_02034D24(u32 messageID, String* string) {
	if (messageID & MPR_TEXT_FLAG) {
		MPRText_CopyChars(string, messageID & ~MPR_TEXT_FLAG);
	} else if (messageID >= (MPR_TEXT_PCDeposit - 3) && messageID < (MPR_TEXT_PCLast - 3 + 1)) { // ?
		MPRText_CopyChars(string, messageID + 3);
	} else {
		String_Clear(string);
	}
}
