#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_02033B50_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_02033B50_H

#include "applications/pc_boxes/box_def.h"

#include "narc.h"
#include "sprite.h"

struct BoxButtonManager {
	SpriteList* unk_00;
	Sprite* unk_04;
	void* unk_08;
	void* unk_0C;
	NNSG2dCellDataBank* unk_10;
	NNSG2dAnimBankData* unk_14;
	NNSG2dImageProxy unk_18;
	SpriteList* unk_3C;
	const BoxApplication* unk_40;
	BoxApplicationDisplay* unk_44;
};

BOOL BoxButton_02033B50(BoxButtonManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3);
void BoxButton_02033BA0(BoxButtonManager* param0);
void BoxButton_02033CE0(BoxButtonManager* param0);
void BoxButton_02033D04(BoxButtonManager* param0, int param1);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_02033B50_H */
