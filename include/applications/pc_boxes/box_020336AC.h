#ifndef MPR_BOX_020336AC_H
#define MPR_BOX_020336AC_H

#include <nitro/types.h>
#include <nnsys.h>

#include "applications/pc_boxes/box_def.h"

#include "narc.h"
#include "pokemon.h"
#include "sprite.h"

typedef struct {
    void* unk_00;
    NNSG2dAnimBankData* unk_04;
    void* unk_08;
    u8 padding_0C;
    NNSG2dCellDataBank* unk_10;
    void* unk_14;
    void* unk_18;
    NNSG2dCellDataBank* unk_1C;
    NNSG2dAnimBankData* unk_20;
    SpriteList* unk_24;
    u8 padding_28[24];
    const BoxApplication* unk_40;
    BoxApplicationDisplay* unk_44;
    NARC* unk_48;
    u8 unk_4C[640];
} BoxIconManager;

typedef struct {
	Sprite* unk_00;
	NNSG2dImageProxy unk_04;
	u32 unk_28;
	u16 unk_2C;
	u16 unk_2E;
	u16 unk_30;
	
	// New with MPR
	u16 unk_32;
} BoxIcon;

BOOL BoxIcon_020336AC(BoxIconManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, SpriteList* param3);
void BoxIcon_02033770(BoxIconManager* param0);
void BoxIcon_020337B8(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_020337DC(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033804(const BoxIcon* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033814(BoxIconManager* param0, BoxPokemon* boxMon, s32 param2, s32 param3, u32 param4, u32 param5, u32 param6, BoxIcon* param7);
void BoxIcon_02033934(BoxIconManager* param0, BoxPokemon* boxMon, u32 param2, u32 param3, u8* param4, s32 param5, s32 param6, u32 param7, u32 param8, u32 param9, BoxIcon* param10);
void BoxIcon_02033A28(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033A78(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033A7C(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033A80(BoxIconManager* param0, void* param1, u32 param2, u32 param3);
void BoxIcon_02033A90(BoxIconManager* param0, BoxIcon* param1, u32 param2);
void BoxIcon_02033AF4(BoxIconManager* param0, BoxIcon* param1, u32 param2);
BOOL BoxIcon_02033B1C(BoxIconManager* param0, BoxIcon* param1);
void BoxGraphics_ApplyMonInMultiSelectShading(BoxIconManager* param0, BoxIcon* param1, BOOL isSelected);
NNSG2dCellDataBank* BoxIcon_02033B48(BoxIconManager* param0);
NNSG2dAnimBankData* BoxIcon_02033B4C(BoxIconManager* param0);

#endif /* MPR_BOX_020336AC_H */