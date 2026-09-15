#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_020346FC_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_020346FC_H

#include "applications/pc_boxes/box_def.h"

#include "bg_window.h"
#include "message.h"
#include "narc.h"
#include "sprite.h"
#include "string_template.h"

struct BoxWindowManager {
	BgConfig* unk_00;
	u8 padding_04[4];
	const BoxApplication* unk_08;
	BoxApplicationDisplay* unk_0C;
	MessageLoader* unk_10;
	MessageLoader* unk_14;
	Window* unk_18;
	u16 unk_1C;
	u16 unk_1E;
	int unk_20;
	void* unk_24;
	NNSG2dCharacterData* unk_28;
	const StringTemplate* unk_2C;
	String* unk_30;
	String* unk_34;
};

BOOL BoxWindow_020346FC(BoxWindowManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4, MessageLoader* param5, const StringTemplate* param6, int param7);
void BoxWindow_02034774(BoxWindowManager* param0);
void BoxWindow_020347C8(BoxWindowManager* param0);
void BoxWindow_0203484C(BoxWindowManager* param0, u32 boxMessageID);
void BoxWindow_02034990(BoxWindowManager* param0, const BoxMenu* menu);
void BoxWindow_02034B30(BoxWindowManager* param0);
void BoxWindow_02034B64(BoxWindowManager* param0);
void BoxGraphics_DrawMenuCursorBox(BoxWindowManager* param0, const BoxMenu* menu);
void BoxWindow_02034BD0(BoxWindowManager* param0, const BoxMenu* menu);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_020346FC_H */
