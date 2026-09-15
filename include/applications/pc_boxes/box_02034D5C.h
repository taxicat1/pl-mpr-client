#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_02034D5C_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_02034D5C_H

#include <nitro/types.h>
#include <nnsys.h>

#include "applications/pc_boxes/box_def.h"

#include "bg_window.h"
#include "font_special_chars.h"
#include "message.h"
#include "sprite.h"
#include "string_gf.h"
#include "sys_task_manager.h"

struct BoxAltWindowManager {
	BoxApplicationDisplay* unk_00;
	BgConfig* unk_04;
	SpriteList* unk_08;
	const BoxApplication* unk_0C;
	Window* unk_10;
	void* unk_14;
	NNSG2dCharacterData* unk_18;
	void* unk_1C;
	NNSG2dCellDataBank* unk_20;
	u8 padding_24[8];
	void* unk_2C;
	NNSG2dScreenData* unk_30;
	void* unk_34;
	NNSG2dScreenData* unk_38;
	Bitmap unk_3C;
	FontSpecialCharsContext* unk_44;
	Sprite* unk_48[18];
	Sprite* unk_90;
	u32 unk_94[18];
	SysTask* unk_DC;
	s32 unk_E0;
	u16 unk_E4[8][48];
	u8 unk_3E4[18][1024];
	u8 unk_4BE4[18];
	u8 padding_4BF6[2];
	MessageLoader* unk_4BF8;
	String* unk_4BFC;
};

BOOL BoxAltWindow_02034D5C(BoxAltWindowManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4);
void BoxAltWindow_02034F10(BoxAltWindowManager* param0);
void BoxAltWindow_02034F44(BoxAltWindowManager* param0);
void BoxAltWindow_02034FBC(BoxAltWindowManager* param0);
BOOL BoxAltWindow_02034FDC(BoxAltWindowManager* param0);
void BoxAltWindow_02034FE4(BoxAltWindowManager* param0);
void BoxAltWindow_02035008(BoxAltWindowManager* param0);
BOOL BoxAltWindow_02035014(BoxAltWindowManager* param0);
void BoxAltWindow_0203501C(BoxAltWindowManager* param0, u32 param1, u32 param2, NNS_G2D_VRAM_TYPE param3, NNSG2dImageProxy* param4);
NNSG2dCellDataBank* BoxAltWindow_020351F8(BoxAltWindowManager* param0);
void BoxAltWindow_02035758(BoxAltWindowManager* param0);
BOOL BoxAltWindow_02035820(BoxAltWindowManager* param0);
void BoxAltWindow_020358B0(BoxAltWindowManager* param0);
BOOL BoxAltWindow_020358F8(BoxAltWindowManager* param0);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_02034D5C_H */
