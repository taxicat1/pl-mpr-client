#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_0202F090_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_0202F090_H

#include <nitro/types.h>
#include <nnsys.h>

#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_app_manager.h"
#include "applications/pc_boxes/box_020336AC.h"
#include "applications/pc_boxes/box_02035AAC.h"
#include "applications/pc_boxes/box_02037A18.h"

#include "sprite.h"
#include "sys_task_manager.h"

typedef enum {
	FUNC_BoxGraphics_0202F60C,
	FUNC_BoxGraphics_ScreenFadeBothToBlack0,
	FUNC_BoxGraphics_ScreenFadeBothToBlack1,
	FUNC_BoxGraphics_ScreenFadeBothToBlack2,
	FUNC_BoxGraphics_ChangeToNewBox,
	FUNC_BoxGraphics_MoveCursor,
	FUNC_BoxGraphics_PreviewMon,
	FUNC_BoxGraphics_0202F94C,
	FUNC_BoxGraphics_0202F9B0,
	FUNC_BoxGraphics_PickUpMonIntoCursor,
	FUNC_BoxGraphics_PlaceMonDownFromCursor,
	FUNC_BoxGraphics_0202FA44,
	FUNC_BoxGraphics_0202FA80,
	FUNC_BoxGraphics_0202FAB0,
	FUNC_BoxGraphics_PlayReleaseCursorMonAnimation,
	FUNC_BoxGraphics_PlayReleaseBoxMonAnimation,
	FUNC_BoxGraphics_PlayReleasePartyMonAnimation,
	FUNC_BoxGraphics_DisplayItemInfo,
	FUNC_BoxGraphics_CloseItemInfo,
	FUNC_BoxGraphics_0202FC44,
	FUNC_BoxGraphics_0202FC8C,
	FUNC_BoxGraphics_0202FCD4,
	FUNC_BoxGraphics_0202FD1C,
	FUNC_BoxGraphics_ItemShrinkToNothing,
	FUNC_BoxGraphics_DisplayBoxMessage,
	FUNC_BoxGraphics_ShowMenu,
	FUNC_BoxGraphics_CloseMessageBox,
	FUNC_BoxGraphics_0202FE58,
	FUNC_BoxGraphics_UpdateMenuCursor,
	FUNC_BoxGraphics_0202FEA4,
	FUNC_BoxGraphics_ShowBoxSelectionPopup,
	FUNC_BoxGraphics_ScrollBoxSelectionPopup,
	FUNC_BoxGraphics_0202FF48,
	FUNC_BoxGraphics_TransitionWallpaper,
	FUNC_BoxGraphics_PressBoxButton,
	FUNC_BoxGraphics_OpenPartyPopup,
	FUNC_BoxGraphics_020300D0,
	FUNC_BoxGraphics_PlayAdjustPartyAnimation,
	FUNC_BoxGraphics_02030190,
	FUNC_BoxGraphics_SetMonMarkingsTransparency,
	FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial,
	FUNC_BoxGraphics_02030284,
	FUNC_BoxGraphics_UpdateMarkingsTouchButton,
	FUNC_BoxGraphics_020302DC,
	FUNC_BoxGraphics_StartDrawMultiSelect,
	FUNC_BoxGraphics_02030320,
	FUNC_BoxGraphics_ApplyMultiSelectMonShadingTask,
	FUNC_BoxGraphics_02030358,
	FUNC_BoxGraphics_02030394,
	FUNC_BoxGraphics_020303B0,
	FUNC_BoxGraphics_020303CC,
	FUNC_BoxGraphics_ScreenFadeBothToBlackLogOff,
	
	// New with MPR
	FUNC_MPR_Unk_52,
	FUNC_MPR_Unk_53,
	FUNC_MPR_Unk_54,
	FUNC_MPR_Unk_55
} BoxGraphicsFunction;

BOOL BoxGraphics_Load(BoxApplicationDisplay** outDisplay, const BoxApplication* boxApp, const BoxApplicationManager* boxAppMan);
void BoxGraphics_Free(BoxApplicationDisplay* display);
void BoxGraphics_TaskHandler(BoxApplicationDisplay* display, BoxGraphicsFunction function);
BOOL BoxGraphics_IsSysTaskDone(BoxApplicationDisplay* display, BoxGraphicsFunction function);
BOOL BoxGraphics_CheckAllTasksDone(BoxApplicationDisplay* display);
SysTask* BoxGraphics_02030870(SysTaskFunc callback, void* param, u32 priority);
NNSG2dImagePaletteProxy* BoxGraphics_02030878(BoxApplicationDisplay* display);
BoxIconManager* BoxGraphics_02030880(BoxApplicationDisplay* display);
BoxPanelManager* BoxGraphics_02030888(BoxApplicationDisplay* display);
BoxPartyManager* BoxGraphics_02030894(BoxApplicationDisplay* display);
BoxCursorManager* BoxGraphics_020308A0(BoxApplicationDisplay* display);
BoxItemIconManager* BoxGraphics_020308AC(BoxApplicationDisplay* display);
BoxAltWindowManager* BoxGraphics_020308B8(BoxApplicationDisplay* display);
PokemonSpriteManager* BoxGraphics_020308C4(BoxApplicationDisplay* display);
int BoxGraphics_020308CC(BoxApplicationDisplay* display);
void BoxGraphics_020308E8(SpriteResourcesHeader* param0, NNSG2dImageProxy* param1, NNSG2dImagePaletteProxy* param2, NNSG2dCellDataBank* param3, NNSG2dCellAnimBankData* param4, u32 param5);
Sprite* BoxGraphics_02030908(SpriteList* param0, SpriteResourcesHeader* param1, u32 param2, u32 param3, u32 param4, int param5);
void BoxGraphics_SetSpritePriority(Sprite* sprite, u32 priority);
void BoxGraphics_02030974(const u16* param0, u16* param1, u32 param2, u16 param3, u32 param4);
const BoxApplicationManager* BoxGraphics_02030A10(BoxApplicationDisplay* display);
void BoxGraphics_02030A64(void* param0, void (*param1)(void*), void* param2);
MPRFilterMenu* BoxGraphics_GetMPRFilter(BoxApplicationDisplay* display);
void BoxGraphics_02030AAC(BoxApplicationDisplay* display, u32 boxID);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_0202F090_H */
