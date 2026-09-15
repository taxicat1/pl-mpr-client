#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_02031F6C_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_02031F6C_H

#include <nitro/types.h>
#include <nnsys.h>

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_020336AC.h"

#include "narc.h"
#include "sprite.h"

typedef struct {
	SpriteList* unk_00;
	Sprite* unk_04;
	Sprite* unk_08;
	Sprite* unk_0C;
	Sprite* unk_10;
	void* unk_14;
	void* unk_18;
	NNSG2dCellDataBank* unk_1C;
	NNSG2dAnimBankData* unk_20;
	NNSG2dImageProxy unk_24;
	BoxIconManager* unk_48;
	BoxIcon unk_4C[30];
	struct {
		fx32 unk_00;
		fx32 unk_04;
	} unk_664[30];
	u32 unk_754;
	
	BoxPanelManager* unk_758;
	BoxPartyManager* unk_75C;
	BoxItemIconManager* unk_760;
	
	s32 unk_764;
	s32 unk_768;
	s32 unk_76C;
	s32 unk_770;
	s32 unk_774;
	s32 unk_778;
	s32 unk_77C;
	s32 unk_780;
	u8 unk_784;
	u8 unk_785;
	u8 unk_786;
	u8 unk_787;
	u8 unk_788;
	s8 unk_789;
	s8 unk_78A;
	s8 unk_78B;
	u8 unk_78C;
	u8 unk_78D;
	u8 unk_78E;
	u8 padding_78F;
	const BoxApplication* unk_790;
	BoxApplicationDisplay* unk_794;
} BoxCursorManager;

BOOL BoxCursor_02031F6C(BoxCursorManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3);
void BoxCursor_02032034(BoxCursorManager* param0);
void BoxCursor_02032298(BoxCursorManager* param0);
void BoxCursor_02032374(BoxCursorManager* param0);
BOOL BoxCursor_02032488(BoxCursorManager* param0);
void BoxCursor_02032644(BoxCursorManager* param0);
BOOL BoxCursor_0203268C(BoxCursorManager* param0);
void BoxCursor_0203277C(BoxCursorManager* param0);
BOOL BoxCursor_020327B0(BoxCursorManager* param0);
void BoxCursor_020328C8(BoxCursorManager* param0);
BOOL BoxCursor_02032944(BoxCursorManager* param0);
void BoxCursor_02032AA4(BoxCursorManager* param0);
BOOL BoxCursor_02032B18(BoxCursorManager* param0);
void BoxCursor_02032C10(BoxCursorManager* param0);
BOOL BoxCursor_02032C6C(BoxCursorManager* param0);
void BoxCursor_02032D14(BoxCursorManager* param0);
void BoxCursor_02032D4C(BoxCursorManager* param0);
BOOL BoxCursor_02032D70(BoxCursorManager* param0);
void BoxCursor_02032E08(BoxCursorManager* param0);
void BoxCursor_02032E40(BoxCursorManager* param0);
void BoxCursor_02032E78(BoxCursorManager* param0);
void BoxCursor_02032EA0(BoxCursorManager* param0);
void BoxCursor_02032EC4(BoxCursorManager* param0, u32 param1);
void BoxCursor_02032EF4(BoxCursorManager* param0);
void BoxGraphics_OpenMultiSelectCursor(BoxCursorManager* param0);
void BoxCursor_02032F24(BoxCursorManager* param0, VecFx32* param1);
void BoxCursor_02032F3C(BoxCursorManager* param0);
BOOL BoxCursor_02032F6C(BoxCursorManager* param0);
void BoxCursor_02032FDC(BoxCursorManager* param0);
BOOL BoxCursor_0203300C(BoxCursorManager* param0);
void BoxCursor_0203307C(BoxCursorManager* param0);
BOOL BoxCursor_020330A8(BoxCursorManager* param0);
void BoxCursor_02033118(BoxCursorManager* param0);
void BoxCursor_02033130(BoxCursorManager* param0);
void BoxCursor_02033150(BoxCursorManager *param0);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_02031F6C_H */
