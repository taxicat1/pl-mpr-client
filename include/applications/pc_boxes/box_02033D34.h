#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_02033D34_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_02033D34_H

#include "applications/pc_boxes/box_def.h"

#include "bg_window.h"
#include "font_special_chars.h"
#include "pokemon_sprite.h"
#include "message.h"
#include "narc.h"
#include "sprite.h"
#include "font_special_chars.h"
#include "pokemon_sprite.h"

struct BoxMonPreviewManager {
    BOOL unk_00;
    Window* unk_04;
    BgConfig* unk_08;
    SpriteList* unk_0C;
    const BoxApplication* unk_10;
    BoxApplicationDisplay* unk_14;
    FontSpecialCharsContext* unk_18;
    FontSpecialCharsContext* unk_1C;
    PokemonSpriteManager* unk_20;
    PokemonSprite* unk_24;
    void* unk_28;
    NNSG2dCellDataBank* unk_2C;
    Sprite* unk_30;
    void* unk_34;
    NNSG2dCellDataBank* unk_38;
    Sprite* unk_3C[2];
    String* unk_44;
    String* unk_48;
    SysTask* unk_4C;
    u8 padding_50[4];
};

BOOL BoxMonPreview_02033D34(BoxMonPreviewManager* param0, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4, const MessageLoader* param5);
void BoxMonPreview_02033DE8(BoxMonPreviewManager* param0);
void BoxMonPreview_02033E88(BoxMonPreviewManager* param0);
void BoxMonPreview_02033E98(BoxMonPreviewManager* param0);
BOOL BoxMonPreview_02033ECC(BoxMonPreviewManager* param0);
void BoxMonPreview_02033EE4(BoxMonPreviewManager* param0);
void BoxMonPreview_02034394(BoxMonPreviewManager* param0);
void BoxMonPreview_020343F4(BoxMonPreviewManager* param0);
void BoxMonPreview_02034474(BoxMonPreviewManager* param0);
BOOL BoxMonPreview_0203461C(BoxMonPreviewManager* param0);
void BoxMonPreview_02034620(BoxMonPreviewManager* param0);
void BoxMonPreview_020346AC(BoxMonPreviewManager* param0);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_02033D34_H */
