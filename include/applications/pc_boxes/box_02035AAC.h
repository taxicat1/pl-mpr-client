#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_02035AAC_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_02035AAC_H

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_020336AC.h"
#include "applications/pc_boxes/box_02030AC4.h"

#include "bg_window.h"
#include "narc.h"
#include "sprite.h"

struct BoxPartyManager {
	BoxApplicationDisplay* unk_00;
	BgConfig* unk_04;
	SpriteList* unk_08;
	const BoxApplication* unk_0C;
	BoxPanelManager* unk_10;
	void* unk_14;
	NNSG2dScreenData* unk_18;
	void* unk_1C;
	fx32 unk_20;
	fx32 unk_24;
	u8 unk_28;
	u8 unk_29;
	u8 unk_2A;
	u8 unk_2B;
	BOOL unk_2C;
	u32 unk_30;
	int unk_34;
	BoxIconManager* unk_38;
	BoxIcon unk_3C[6];
	u8 unk_174;
	u8 padding_175[3];
};

BOOL BoxParty_02035AAC(BoxPartyManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4);
void BoxParty_02035B3C(BoxPartyManager* param0);
void BoxParty_02035B5C(BoxPartyManager* param0);
void BoxParty_02035B84(BoxPartyManager* param0);
BOOL BoxParty_02035BB4(BoxPartyManager* param0);
void BoxParty_02035C24(BoxPartyManager* param0);
BOOL BoxParty_02035C44(BoxPartyManager* param0);
void BoxParty_02035CF0(BoxPartyManager* param0);
BOOL BoxParty_02035E18(BoxPartyManager* param0);
void BoxParty_02035E28(BoxPartyManager* param0);
BOOL BoxParty_02035EC4(BoxPartyManager* param0);
void BoxParty_02035F7C(BoxPartyManager* param0);
void BoxParty_02035FB8(BoxPartyManager* param0, u32 param1);
BOOL BoxParty_02035FDC(BoxPartyManager* param0);
void BoxParty_0203605C(BoxPartyManager* param0);
void BoxParty_02036098(BoxPartyManager* param0, u32 param1, u32 param2);
void BoxParty_020360C8(BoxPartyManager* param0);
void BoxParty_020360F8(BoxPartyManager* param0, u32 param1, u32 param2, BoxIcon* param3);
void BoxParty_02036134(BoxPartyManager* param0, u32 param1, const BoxIcon* param2);
void BoxParty_02036188(BoxPartyManager* param0, u32 param1, const BoxIcon* param2);
BoxIcon* BoxParty_020361CC(BoxPartyManager* param0, u32 param1);
void BoxParty_020361E4(BoxPartyManager* param0, u32 param1, s32* param2, s32* param3);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_02035AAC_H */
