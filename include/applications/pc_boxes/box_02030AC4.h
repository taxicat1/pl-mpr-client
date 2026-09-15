#ifndef MPR_BOX_02030AC4_H
#define MPR_BOX_02030AC4_H

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_020336AC.h"

#include "bg_window.h"
#include "sprite.h"

struct BoxPanelManager {
	u8 boxID;
	u8 unk_01;
	u8 unk_02;
	u8 unk_03;
	u32 unk_04;
	s32 unk_08;
	s32 unk_0C;
	s32 unk_10;
	s32 unk_14;
	u8 padding_18[24];
	u32 unk_30;
	u32 unk_34;
	fx32 unk_38[2][6];
	fx32 unk_68[2][6];
	u32 unk_98;
	BOOL unk_9C;
	u16 unk_A0;
	u8 unk_A2;
	u8 unk_A3;
	u8 unk_A4;
	u8 padding_A5[3];
	BoxIcon unk_A8[2][30];
	u8 unk_CD8[30][640];
	u16 unk_57D8[30];
	u16 unk_5814[30];
	u8 padding_5850[4];
	int unk_5854;
	int unk_5858;
	int unk_585C;
	u16 unk_5860[9][16];
	BoxIconManager* unk_58F0;
	BgConfig* unk_58F4;
	const BoxApplication* unk_58F8;
	BoxApplicationDisplay* unk_58FC;
};

BOOL BoxPanel_02030AC4(BoxPanelManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4);
void BoxPanel_02030B40(BoxPanelManager* param0);
void BoxPanel_02030B68(BoxPanelManager* param0);
void BoxPanel_02030C18(BoxPanelManager* param0, const BoxCustomization* BoxCustomization, int param2, BOOL param3);
void BoxPanel_02030F80(BoxPanelManager* param0, const BoxCustomization* BoxCustomization, int param2);
BOOL BoxPanel_0203102C(BoxPanelManager* param0);
void BoxPanel_02031598(BoxPanelManager* param0);
BOOL BoxPanel_020315B8(BoxPanelManager* param0);
void BoxPanel_02031650(BoxPanelManager* param0, u32 param1, s32 param2, BoxIcon* param3);
void BoxPanel_020316A4(BoxPanelManager* param0, u32 param1, const BoxIcon* param2);
BoxIcon* BoxPanel_02031708(BoxPanelManager* param0, u32 param1);
void BoxPanel_02031720(BoxPanelManager* param0);
void BoxPanel_02031AC4(BoxPanelManager* param0, u32 param1);
BOOL BoxPanel_02031AFC(BoxPanelManager* param0);
void BoxPanel_02031B9C(BoxPanelManager* param0);
void BoxPanel_02031BEC(BoxPanelManager* param0, u32 param1, u32 param2);
void BoxPanel_02031C40(BoxPanelManager* param0);
void BoxGraphics_ApplyMultiSelectMonShading(BoxPanelManager* param0);
void BoxPanel_02031D30(BoxPanelManager* param0);
void BoxPanel_02031D70(BoxPanelManager* param0, int param1);
BOOL BoxPanel_02031D98(BoxPanelManager* param0, int param1, BOOL param2);
void BoxPanel_02031E84(BoxPanelManager* param0, int param1);
BOOL BoxPanel_02031ED0(BoxPanelManager* param0, int param1);

#endif /* MPR_BOX_02030AC4_H */
