#include <nitro.h>
#include <string.h>

#include "applications/pc_boxes/box_0202F090.h"

#include "applications/pc_boxes/box_02033B50.h"
#include "applications/pc_boxes/box_02033D34.h"
#include "applications/pc_boxes/box_020346FC.h"
#include "applications/pc_boxes/box_02034D5C.h"
#include "applications/pc_boxes/box_02035AAC.h"
#include "applications/pc_boxes/box_0203643C.h"
#include "applications/pc_boxes/box_0203820C.h"
#include "applications/pc_boxes/box_mpr.h"

#include "mpr_client/mpr_menu.h"

#include "assert.h"
#include "bg_window.h"
#include "common.h"
#include "font.h"
#include "game_version.h"
#include "graphics.h"
#include "gx_gf.h"
#include "gx_layers.h"
#include "heap.h"
#include "narc.h"
#include "pokemon_sprite.h"
#include "render_oam.h"
#include "screen_fade.h"
#include "sound_playback.h"
#include "sprite.h"
#include "sprite_util.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "system.h"

struct BoxApplicationDisplay { // NOTE: offsets are mostly wrong (carryover from pokeplatinum)
	SysTask* unk_00;
	SysTask* unk_04;
	SysTask* unk_08[8]; // 4 in Platinum
	SpriteList* unk_18;
	G2dRenderer unk_1C;
	NNSG2dImagePaletteProxy unk_1A8;
	PokemonSpriteManager* unk_1BC;
	BgConfig* unk_1C0;
	const BoxApplication* unk_1C4;
	MPRFilterMenu* mprFilter; // 174
	BoxIconManager unk_1C8;
	BoxPanelManager unk_494;
	BoxCursorManager unk_5E24;
	BoxButtonManager unk_65BC;
	BoxMonPreviewManager unk_6604;
	BoxWindowManager unk_6658;
	BoxAltWindowManager unk_6690;
	BoxPartyManager unk_B290;
	BoxTouchSelectorManager* unk_B408;
	MPRBoxDisplay* mprDisplay; // B3BC
	BoxItemIconManager* unk_B40C;
	BoxComparisonManager* unk_B410;
	const BoxApplicationManager* unk_B414;
};

typedef struct {
	BoxGraphicsFunction function;
	u16 unk_04;
	u16 state;
	u16 unk_08;
	u16 unk_0A;
	BoxApplicationDisplay* unk_0C;
	void* unk_10;
} BoxTaskParams;

typedef struct {
	SysTaskFunc  sysTaskFunc;
	u32          alloc; /* Unused */
} BoxGraphicsTaskHandler;

static void BoxGraphics_DrawSprites(SysTask* task, void* param);
static void BoxTaskParams_Free(BoxTaskParams* params);
static void BoxGraphics_0202F5DC(SysTask* task, void* param);
static void BoxGraphics_0202F60C(SysTask* task, void* param);
static void BoxGraphics_ScreenFadeBothToBlackLogIn(SysTask* task, void* param);
static void BoxGraphics_ScreenFadeBothToBlack1(SysTask* task, void* param);
static void BoxGraphics_ScreenFadeBothToBlack2(SysTask* task, void* param);
static void BoxGraphics_ChangeToNewBox(SysTask* task, void* param);
static void BoxGraphics_MoveCursor(SysTask* task, void* param);
static void BoxGraphics_PreviewMon(SysTask* task, void* param);
static void BoxGraphics_0202F94C(SysTask* task, void* param);
static void BoxGraphics_0202F9B0(SysTask* task, void* param);
static void BoxGraphics_PickUpMonIntoCursor(SysTask* task, void* param);
static void BoxGraphics_PlaceMonDownFromCursor(SysTask* task, void* param);
static void BoxGraphics_0202FA44(SysTask* task, void* param);
static void BoxGraphics_0202FA80(SysTask* task, void* param);
static void BoxGraphics_0202FAB0(SysTask* task, void* param);
static void BoxGraphics_PlayReleaseCursorMonAnimation(SysTask* task, void* param);
static void BoxGraphics_PlayReleaseBoxMonAnimation(SysTask* task, void* param);
static void BoxGraphics_PlayReleasePartyMonAnimation(SysTask* task, void* param);
static void BoxGraphics_DisplayItemInfo(SysTask* task, void* param);
static void BoxGraphics_CloseItemInfo(SysTask* task, void* param);
static void BoxGraphics_0202FC44(SysTask* task, void* param);
static void BoxGraphics_0202FC8C(SysTask* task, void* param);
static void BoxGraphics_0202FCD4(SysTask* task, void* param);
static void BoxGraphics_0202FD1C(SysTask* task, void* param);
static void BoxGraphics_ItemShrinkToNothing(SysTask* task, void* param);
static void BoxGraphics_DisplayBoxMessage(SysTask* task, void* param);
static void BoxGraphics_ShowMenu(SysTask* task, void* param);
static void BoxGraphics_CloseMessageBox(SysTask* task, void* param);
static void BoxGraphics_0202FE58(SysTask* task, void* param);
static void BoxGraphics_UpdateMenuCursor(SysTask* task, void* param);
static void BoxGraphics_0202FEA4(SysTask* task, void* param);
static void BoxGraphics_ShowBoxSelectionPopup(SysTask* task, void* param);
static void BoxGraphics_ScrollBoxSelectionPopup(SysTask* task, void* param);
static void BoxGraphics_0202FF48(SysTask* task, void* param);
static void BoxGraphics_TransitionWallpaper(SysTask* task, void* param);
static void BoxGraphics_PressBoxButton(SysTask* task, void* param);
static void BoxGraphics_OpenPartyPopup(SysTask* task, void* param);
static void BoxGraphics_020300D0(SysTask* task, void* param);
static void BoxGraphics_PlayAdjustPartyAnimation(SysTask* task, void* param);
static void BoxGraphics_02030190(SysTask* task, void* param);
static void BoxGraphics_SetMonMarkingsTransparency(SysTask* task, void* param);
static void BoxGraphics_OpenOrCloseSelectedTouchDial(SysTask* task, void* param);
static void BoxGraphics_02030284(SysTask* task, void* param);
static void BoxGraphics_UpdateMarkingsTouchButton(SysTask* task, void* param);
static void BoxGraphics_020302DC(SysTask* task, void* param);
static void BoxGraphics_StartDrawMultiSelect(SysTask* task, void* param);
static void BoxGraphics_02030320(SysTask* task, void* param);
static void BoxGraphics_ApplyMultiSelectMonShadingTask(SysTask* task, void* param);
static void BoxGraphics_02030358(SysTask* task, void* param);
static void BoxGraphics_02030394(SysTask* task, void* param);
static void BoxGraphics_020303B0(SysTask* task, void* param);
static void BoxGraphics_020303CC(SysTask* task, void* param);
static void BoxGraphics_ScreenFadeBothToBlackLogOff(SysTask* task, void* param);
static void BoxGraphics_0203045C(SysTask* task, void* param);
static void BoxGraphics_020304B8(SysTask* task, void* param);
static void BoxGraphics_02030504(SysTask* task, void* param);
static void BoxGraphics_02030548(BoxApplicationDisplay* display, const BoxApplication* boxApp);
static void BoxGraphics_02030660(BoxApplicationDisplay* display, const BoxApplication* boxApp);
static void BoxGraphics_020307A4(void);
static void BoxGraphics_0203081C(BoxApplicationDisplay* display, const BoxApplication* boxApp);
static int BoxGraphics_GetBoxMoveDirection(u32 sourceBoxID, u32 destBoxID, u32 maxBoxID);
static void BoxGraphics_02030A1C(void);
static void BoxGraphics_02030A28(void);

static const BoxGraphicsTaskHandler sBoxGraphicsTaskHandlers[] = {
	[FUNC_BoxGraphics_0202F60C] = { BoxGraphics_0202F60C, 0 },
	[FUNC_BoxGraphics_ScreenFadeBothToBlack0] = { BoxGraphics_ScreenFadeBothToBlackLogIn, 0 },
	[FUNC_BoxGraphics_ScreenFadeBothToBlack1] = { BoxGraphics_ScreenFadeBothToBlack1, 0 },
	[FUNC_BoxGraphics_ScreenFadeBothToBlack2] = { BoxGraphics_ScreenFadeBothToBlack2, 0 },
	[FUNC_BoxGraphics_ChangeToNewBox] = { BoxGraphics_ChangeToNewBox, 0 },
	[FUNC_BoxGraphics_MoveCursor] = { BoxGraphics_MoveCursor, 0 },
	[FUNC_BoxGraphics_PreviewMon] = { BoxGraphics_PreviewMon, 0 },
	[FUNC_BoxGraphics_0202F94C] = { BoxGraphics_0202F94C, 0 },
	[FUNC_BoxGraphics_0202F9B0] = { BoxGraphics_0202F9B0, 0 },
	[FUNC_BoxGraphics_PickUpMonIntoCursor] = { BoxGraphics_PickUpMonIntoCursor, 0 },
	[FUNC_BoxGraphics_PlaceMonDownFromCursor] = { BoxGraphics_PlaceMonDownFromCursor, 0 },
	[FUNC_BoxGraphics_0202FA44] = { BoxGraphics_0202FA44, 0 },
	[FUNC_BoxGraphics_0202FA80] = { BoxGraphics_0202FA80, 0 },
	[FUNC_BoxGraphics_0202FAB0] = { BoxGraphics_0202FAB0, 0 },
	[FUNC_BoxGraphics_PlayReleaseCursorMonAnimation] = { BoxGraphics_PlayReleaseCursorMonAnimation, 0 },
	[FUNC_BoxGraphics_PlayReleaseBoxMonAnimation] = { BoxGraphics_PlayReleaseBoxMonAnimation, 0 },
	[FUNC_BoxGraphics_PlayReleasePartyMonAnimation] = { BoxGraphics_PlayReleasePartyMonAnimation, 0 },
	[FUNC_BoxGraphics_DisplayItemInfo] = { BoxGraphics_DisplayItemInfo, 0 },
	[FUNC_BoxGraphics_CloseItemInfo] = { BoxGraphics_CloseItemInfo, 0 },
	[FUNC_BoxGraphics_0202FC44] = { BoxGraphics_0202FC44, 0 },
	[FUNC_BoxGraphics_0202FC8C] = { BoxGraphics_0202FC8C, 0 },
	[FUNC_BoxGraphics_0202FCD4] = { BoxGraphics_0202FCD4, 0 },
	[FUNC_BoxGraphics_0202FD1C] = { BoxGraphics_0202FD1C, 0 },
	[FUNC_BoxGraphics_ItemShrinkToNothing] = { BoxGraphics_ItemShrinkToNothing, 0 },
	[FUNC_BoxGraphics_DisplayBoxMessage] = { BoxGraphics_DisplayBoxMessage, 0 },
	[FUNC_BoxGraphics_ShowMenu] = { BoxGraphics_ShowMenu, 0 },
	[FUNC_BoxGraphics_CloseMessageBox] = { BoxGraphics_CloseMessageBox, 0 },
	[FUNC_BoxGraphics_0202FE58] = { BoxGraphics_0202FE58, 0 },
	[FUNC_BoxGraphics_UpdateMenuCursor] = { BoxGraphics_UpdateMenuCursor, 0 },
	[FUNC_BoxGraphics_0202FEA4] = { BoxGraphics_0202FEA4, 0 },
	[FUNC_BoxGraphics_ShowBoxSelectionPopup] = { BoxGraphics_ShowBoxSelectionPopup, 0 },
	[FUNC_BoxGraphics_ScrollBoxSelectionPopup] = { BoxGraphics_ScrollBoxSelectionPopup, 0 },
	[FUNC_BoxGraphics_0202FF48] = { BoxGraphics_0202FF48, 0 },
	[FUNC_BoxGraphics_TransitionWallpaper] = { BoxGraphics_TransitionWallpaper, 0 },
	[FUNC_BoxGraphics_PressBoxButton] = { BoxGraphics_PressBoxButton, 0 },
	[FUNC_BoxGraphics_OpenPartyPopup] = { BoxGraphics_OpenPartyPopup, 0 },
	[FUNC_BoxGraphics_020300D0] = { BoxGraphics_020300D0, 0 },
	[FUNC_BoxGraphics_PlayAdjustPartyAnimation] = { BoxGraphics_PlayAdjustPartyAnimation, 0 },
	[FUNC_BoxGraphics_02030190] = { BoxGraphics_02030190, 0 },
	[FUNC_BoxGraphics_SetMonMarkingsTransparency] = { BoxGraphics_SetMonMarkingsTransparency, 0 },
	[FUNC_BoxGraphics_OpenOrCloseSelectedTouchDial] = { BoxGraphics_OpenOrCloseSelectedTouchDial, 0 },
	[FUNC_BoxGraphics_02030284] = { BoxGraphics_02030284, 0 },
	[FUNC_BoxGraphics_UpdateMarkingsTouchButton] = { BoxGraphics_UpdateMarkingsTouchButton, 0 },
	[FUNC_BoxGraphics_020302DC] = { BoxGraphics_020302DC, 0 },
	[FUNC_BoxGraphics_StartDrawMultiSelect] = { BoxGraphics_StartDrawMultiSelect, 0 },
	[FUNC_BoxGraphics_02030320] = { BoxGraphics_02030320, 0 },
	[FUNC_BoxGraphics_ApplyMultiSelectMonShadingTask] = { BoxGraphics_ApplyMultiSelectMonShadingTask, 0 },
	[FUNC_BoxGraphics_02030358] = { BoxGraphics_02030358, 0 },
	[FUNC_BoxGraphics_02030394] = { BoxGraphics_02030394, 0 },
	[FUNC_BoxGraphics_020303B0] = { BoxGraphics_020303B0, 0 },
	[FUNC_BoxGraphics_020303CC] = { BoxGraphics_020303CC, 0 },
	[FUNC_BoxGraphics_ScreenFadeBothToBlackLogOff] = { BoxGraphics_ScreenFadeBothToBlackLogOff, 0 },
	
	// New with MPR
	[FUNC_MPR_Unk_52] = { BoxGraphics_0203045C, 0 },
	[FUNC_MPR_Unk_53] = { BoxGraphics_0203045C, 0 },
	[FUNC_MPR_Unk_54] = { BoxGraphics_020304B8, 0 },
	[FUNC_MPR_Unk_55] = { BoxGraphics_02030504, 0 }
};	


BOOL BoxGraphics_Load(BoxApplicationDisplay** outDisplay, const BoxApplication* boxApp, const BoxApplicationManager* boxAppMan) {
	BoxApplicationDisplay* display = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxApplicationDisplay));
	if (display != NULL) {
		display->unk_1C4 = boxApp;
		display->mprFilter = BoxAppMan_GetMPRFilter(boxAppMan);
		display->unk_1C0 = BgConfig_New(HEAP_ID_BOX_GRAPHICS);
		
		if (display->unk_1C0 != NULL) {
			u32 i;
			BOOL success = TRUE;
			
			SetVBlankCallback(NULL, NULL);
			DisableHBlank();
			GXLayers_DisableEngineALayers();
			GXLayers_DisableEngineBLayers();
			
			GX_SetVisiblePlane(0);
			GXS_SetVisiblePlane(0);
			GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_64K);
			GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
			NNS_G2dInitOamManagerModule();
			
			RenderOam_Init(0, 128, 0, 32, 0, 128, 0, 32, 10);
			display->unk_18 = SpriteList_InitRendering(128, &display->unk_1C, HEAP_ID_BOX_GRAPHICS);
			SetSubScreenViewRect(&display->unk_1C, 0, 384 << FX32_SHIFT);
			
			NNS_G2dInitImagePaletteProxy(&display->unk_1A8);
			
			NarcID narc;
			if (gIsDiamondPearl) {
				narc = NARC_INDEX_DP_GRAPHIC__BOX;
			} else {
				narc = NARC_INDEX_PL_GRAPHIC__BOX;
			}
			Graphics_LoadPartialPalette(narc, 26, NNS_G2D_VRAM_TYPE_2DMAIN, 0, 10, &display->unk_1A8);
			Font_UseImmediateGlyphAccess(FONT_SYSTEM, 10);
			
			display->unk_1BC = PokemonSpriteManager_New(HEAP_ID_BOX_GRAPHICS);
			
			for (i = 0; i < 8; i++) {
				display->unk_08[i] = NULL;
			}
			
			BoxGraphics_02030A1C();
			*outDisplay = display;
			
			success &= BoxIcon_020336AC(&display->unk_1C8, display, display->unk_1C4, display->unk_18);
			success &= BoxPanel_02030AC4(&display->unk_494, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			success &= BoxCursor_02031F6C(&display->unk_5E24, display, display->unk_1C4, display->unk_18);
			success &= BoxButton_02033B50(&display->unk_65BC, display, display->unk_1C4, display->unk_18);
			success &= BoxMonPreview_02033D34(&display->unk_6604, display, display->unk_1C4, display->unk_1C0, display->unk_18, BoxAppMan_GetBoxMessagesLoader(boxAppMan));
			success &= BoxWindow_020346FC(
				&display->unk_6658,
				display,
				display->unk_1C4,
				display->unk_1C0,
				display->unk_18,
				BoxAppMan_GetBoxMessagesLoader(boxAppMan),
				BoxAppMan_GetMessageVariableBuffer(boxAppMan),
				BoxAppMan_GetOptionsFrame(boxAppMan));
			
			success &= BoxAltWindow_02034D5C(&display->unk_6690, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			success &= BoxParty_02035AAC(&display->unk_B290, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			
			if (BoxApp_GetBoxMode(display->unk_1C4) != PC_MODE_RANCH_WITHDRAW) {
				success &= BoxTouchSelector_0203643C(&display->unk_B408, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			} else {
				success &= BoxMPR_02039618(&display->mprDisplay, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			}
			
			success &= BoxItemIcon_02037A18(&display->unk_B40C, display, display->unk_1C4, display->unk_1C0, display->unk_18);
			success &= BoxComparison_0203820C(&display->unk_B410, display, display->unk_1C4, display->unk_1C0, display->unk_18, BoxAppMan_GetBoxMessagesLoader(boxAppMan));
			
			for (i = 0; i < 10; i++) {
				display->mprFilter->buttonStates[i] = TOUCH_BUTTON_INVALID;
			}
			
			display->unk_B414 = boxAppMan;
			display->unk_00 = SysTask_Start(BoxGraphics_DrawSprites, display, 2);
			display->unk_04 = BoxGraphics_02030870(BoxGraphics_0202F5DC, display, 1);
			
			MPRMenu_LoadNetworkIconPalette(NNS_G2D_VRAM_TYPE_2DMAIN, 288, HEAP_ID_BOX_GRAPHICS);
			MPRMenu_LoadNetworkIconGraphic(NNS_G2D_VRAM_TYPE_2DMAIN, HEAP_ID_BOX_GRAPHICS);
			
			return success;
		}
	}
	
	return FALSE;
}


static void BoxGraphics_DrawSprites(SysTask* task, void* param) {
	BoxApplicationDisplay* display = (BoxApplicationDisplay*)param;
	
	BoxGraphics_02030A28();
	
	if (BoxApp_GetBoxMode(display->unk_1C4) == PC_MODE_RANCH_WITHDRAW) {
		if (display->mprFilter->unk_4C) {
			BoxMPR_02039B98(display->mprDisplay);
			display->mprFilter->unk_4C = FALSE;
		} else {
			BoxMPR_0203B2D0(display->mprDisplay);
		}
	}
	
	G3X_Reset();
	G3X_ResetMtxStack();
	NNS_G2dSetupSoftwareSpriteCamera();
	
	PokemonSpriteManager_DrawSprites(display->unk_1BC);
	
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
}


void BoxGraphics_Free(BoxApplicationDisplay* display) {
	SetVBlankCallback(NULL, NULL);
	SysTask_Done(display->unk_00);
	SysTask_Done(display->unk_04);
	
	BoxComparison_020382B8(display->unk_B410);
	BoxItemIcon_02037A70(display->unk_B40C);
	
	if (BoxApp_GetBoxMode(display->unk_1C4) != PC_MODE_RANCH_WITHDRAW) {
		BoxTouchSelector_02036534(display->unk_B408);
	} else {
		BoxMPR_0203969C(display->mprDisplay);
	}
	
	BoxParty_02035B3C(&display->unk_B290);
	BoxAltWindow_02034F44(&display->unk_6690);
	BoxWindow_02034774(&display->unk_6658);
	BoxMonPreview_02033DE8(&display->unk_6604);
	BoxButton_02033CE0(&display->unk_65BC);
	BoxCursor_02032298(&display->unk_5E24);
	BoxPanel_02030B40(&display->unk_494);
	BoxIcon_02033770(&display->unk_1C8);
	
	SpriteList_Delete(display->unk_18);
	Bg_FreeTilemapBuffer(display->unk_1C0, BG_LAYER_MAIN_3);
	Bg_FreeTilemapBuffer(display->unk_1C0, BG_LAYER_MAIN_2);
	Bg_FreeTilemapBuffer(display->unk_1C0, BG_LAYER_MAIN_1);
	Bg_FreeTilemapBuffer(display->unk_1C0, BG_LAYER_SUB_0);
	PokemonSpriteManager_Free(display->unk_1BC);
	Font_UseLazyGlyphAccess(FONT_SYSTEM);
	Heap_Free(display->unk_1C0);
	Heap_Free(display);
	RenderOam_Free();
}


void BoxGraphics_TaskHandler(BoxApplicationDisplay* display, BoxGraphicsFunction function) {
	if (function < NELEMS(sBoxGraphicsTaskHandlers)) {
		BoxTaskParams* taskParams;
		
		for (u32 i = 0; i < 8; i++) {
			if (display->unk_08[i] == NULL) {
				taskParams = Heap_Alloc(HEAP_ID_BOX_GRAPHICS, sizeof(BoxTaskParams) + sBoxGraphicsTaskHandlers[function].alloc);
				
				if (taskParams != NULL) {
					taskParams->function = function;
					taskParams->unk_04 = i;
					taskParams->state = 0;
					taskParams->unk_0C = display;
					taskParams->unk_10 = ((u8*)(taskParams) + sBoxGraphicsTaskHandlers[function].alloc);
					
					display->unk_08[i] = SysTask_Start(sBoxGraphicsTaskHandlers[function].sysTaskFunc, taskParams, 1);
				} else {
					GF_ASSERT(FALSE);
				}
				
				break;
			}
		}
	} else {
		GF_ASSERT(FALSE);
	}
}


BOOL BoxGraphics_IsSysTaskDone(BoxApplicationDisplay* display, BoxGraphicsFunction function) {
	BoxTaskParams* params;
	
	for (int i = 0; i < 8; i++) {
		if (display->unk_08[i] != NULL) {
			params = SysTask_GetParam(display->unk_08[i]);
			
			if (params->function == function) {
				return FALSE;
			}
		}
	}
	
	return TRUE;
}


BOOL BoxGraphics_CheckAllTasksDone(BoxApplicationDisplay* display) {
	for (int i = 0; i < 8; i++) {
		if (display->unk_08[i] != NULL) {
			return FALSE;
		}
	}
	
	return TRUE;
}


static void BoxTaskParams_Free(BoxTaskParams* params) {
	BoxApplicationDisplay* display = params->unk_0C;
	
	SysTask_Done(display->unk_08[params->unk_04]);
	display->unk_08[params->unk_04] = NULL;
	Heap_Free(params);
}


static void BoxGraphics_0202F5DC(SysTask* task, void* param) {
	BoxApplicationDisplay* display = (BoxApplicationDisplay*)param;
	
	BoxMonPreview_02033E88(&display->unk_6604);
	
	SpriteList_Update(display->unk_18);
	RenderOam_Transfer();
	
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


static void BoxGraphics_0202F60C(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxGraphics_02030548(display, boxApp);
			BoxGraphics_02030660(display, boxApp);
			BoxGraphics_0203081C(display, boxApp);
			
			if (BoxApp_GetBoxMode(boxApp) != PC_MODE_COMPARE) {
				BoxMonPreview_02033E98(&display->unk_6604);
				params->state++;
			} else {
				BoxTaskParams_Free(params);
			}
			break;
		
		case 1:
			if (BoxMonPreview_02033ECC(&display->unk_6604)) {
				BoxMonPreview_02034394(&display->unk_6604);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ScreenFadeBothToBlackLogIn(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_PC_LOGIN);
			G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_ALL, 0x6, 0xa);
			StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 8, 1, HEAP_ID_BOX_GRAPHICS);
			params->state++;
			break;
		
		case 1:
			if (IsScreenFadeDone()) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ScreenFadeBothToBlack1(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	
	switch (params->state) {
		case 0:
			G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_ALL, 6, 10);
			StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 6, 1, HEAP_ID_BOX_GRAPHICS);
			params->state++;
			break;
		
		case 1:
			if (IsScreenFadeDone()) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ScreenFadeBothToBlack2(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	
	switch (params->state) {
		case 0:
			StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 6, 1, HEAP_ID_BOX_GRAPHICS);
			params->state++;
			break;
		
		case 1:
			if (IsScreenFadeDone()) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ChangeToNewBox(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxItemIcon_02037FC0(display->unk_B40C);
			params->state++;
			// Fall through
		
		case 1:
			if (BoxItemIcon_02037FEC(display->unk_B40C) == FALSE) {
				break;
			}
			params->state++;
			// Fall through
		
		case 2:
			{
				u32 maxBoxNum;
				if (BoxApp_GetBoxMode(display->unk_1C4) != PC_MODE_RANCH_WITHDRAW) {
					maxBoxNum = MAX_PC_BOXES;
				} else {
					maxBoxNum = boxApp->customization.maxRanchBoxID + 1;
				}
				
				int boxChangeDirection = boxApp->customization.ranchBoxJump;
				if (boxChangeDirection == 0) {
					boxChangeDirection = BoxGraphics_GetBoxMoveDirection(display->unk_494.boxID, boxApp->customization.boxID, maxBoxNum);
				}
				
				BoxPanel_02030C18(&display->unk_494, &boxApp->customization, boxChangeDirection, TRUE);
				BoxPanel_02030F80(&display->unk_494, &boxApp->customization, boxChangeDirection);
				Sound_PlayEffect(SEQ_SE_DP_SELECT);
				params->state++;
			}
			// Fall through
		
		case 3:
			if (BoxPanel_0203102C(&display->unk_494) == 0) {
				break;
			}
			
			BoxCursor_02033150(&display->unk_5E24);
			BoxItemIcon_02037DA8(display->unk_B40C);
			params->state++;
			// Fall through
		
		case 4:
			if (BoxItemIcon_02037E08(display->unk_B40C) == 0) {
				break;
			}
			
			BoxTaskParams_Free(params);
			break;
	}
}


static void BoxGraphics_MoveCursor(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_SELECT);
			BoxCursor_02032374(&display->unk_5E24);
			BoxItemIcon_02037DA8(display->unk_B40C);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_02032488(&display->unk_5E24)
				&& BoxItemIcon_02037E08(display->unk_B40C)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PreviewMon(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	if (BoxApp_GetBoxMode(boxApp) != PC_MODE_COMPARE) {
		switch (params->state) {
			case 0:
				BoxMonPreview_020343F4(&display->unk_6604);
				params->state++;
				break;
			
			case 1:
				if (BoxMonPreview_0203461C(&display->unk_6604)) {
					BoxTaskParams_Free(params);
				}
				break;
		}
	} else {
		switch (params->state) {
			case 0:
				BoxComparison_020384C4(display->unk_B410);
				params->state++;
				break;
			
			case 1:
				if (BoxComparison_02038514(display->unk_B410)) {
					BoxTaskParams_Free(params);
				}
				break;
		}
	}
}


static void BoxGraphics_0202F94C(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	if (BoxApp_GetPreviewMonSource(boxApp) == PREVIEW_MON_IN_CURSOR) {
		BoxCursor_02032E78(&display->unk_5E24);
	} else {
		switch (BoxApp_GetCursorLocation(boxApp)) {
			case CURSOR_IN_BOX:
				BoxPanel_02031C40(&display->unk_494);
				break;
			
			case CURSOR_IN_PARTY:
				BoxParty_020360C8(&display->unk_B290);
				break;
		}
	}
	
	BoxMonPreview_02034620(&display->unk_6604);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_0202F9B0(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxMonPreview_02034474(&display->unk_6604);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_PickUpMonIntoCursor(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032644(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_0203268C(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PlaceMonDownFromCursor(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_0203277C(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_020327B0(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FA44(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_020328C8(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_02032944(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FA80(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032D14(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			BoxTaskParams_Free(params);
			break;
	}
}


static void BoxGraphics_0202FAB0(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxParty_02035F7C(&display->unk_B290);
			params->state++;
			break;
		
		case 1:
			BoxParty_02035CF0(&display->unk_B290);
			params->state++;
			break;
		
		case 2:
			if (BoxParty_02035E18(&display->unk_B290)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PlayReleaseCursorMonAnimation(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032D4C(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_02032D70(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PlayReleaseBoxMonAnimation(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxPanel_02031AC4(&display->unk_494, BoxApp_GetCursorBoxPosition(boxApp));
			params->state++;
			break;
		
		case 1:
			if (BoxPanel_02031AFC(&display->unk_494)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PlayReleasePartyMonAnimation(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxParty_02035FB8(&display->unk_B290, BoxApp_GetCursorPartyPosition(boxApp));
			params->state++;
			break;
		
		case 1:
			if (BoxParty_02035FDC(&display->unk_B290)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_DisplayItemInfo(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxAltWindow_02035758(&display->unk_6690);
			params->state++;
			break;
		
		case 1:
			if (BoxAltWindow_02035820(&display->unk_6690)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_CloseItemInfo(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxAltWindow_020358B0(&display->unk_6690);
			params->state++;
			break;
		
		case 1:
			if (BoxAltWindow_020358F8(&display->unk_6690)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FC44(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032F3C(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_02032F6C(&display->unk_5E24)) {
				BoxMonPreview_020346AC(&display->unk_6604);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FC8C(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032FDC(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_0203300C(&display->unk_5E24)) {
				BoxMonPreview_020346AC(&display->unk_6604);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FCD4(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_0203307C(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_020330A8(&display->unk_5E24)) {
				BoxMonPreview_020346AC(&display->unk_6604);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0202FD1C(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	u32 item = BoxApp_GetPreviewedMonHeldItem(boxApp);
	
	if (BoxApp_GetPreviewMonSource(boxApp) == PREVIEW_MON_IN_CURSOR) {
		BoxCursor_02032EC4(&display->unk_5E24, item);
	} else {
		switch (BoxApp_GetCursorLocation(boxApp)) {
			case CURSOR_IN_PARTY:
				BoxParty_02036098(&display->unk_B290, BoxApp_GetCursorPartyPosition(boxApp), item);
				break;
			
			case CURSOR_IN_BOX:
				BoxPanel_02031BEC(&display->unk_494, BoxApp_GetCursorBoxPosition(boxApp), item);
				break;
		}
	}
	
	BoxTaskParams_Free(params);
}


static void BoxGraphics_ItemShrinkToNothing(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02033118(&display->unk_5E24);
			BoxItemIcon_02038120(display->unk_B40C);
			params->state++;
			break;
		
		case 1:
			if (BoxItemIcon_02038168(display->unk_B40C)) {
				BoxCursor_02033130(&display->unk_5E24);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_DisplayBoxMessage(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	BoxWindow_0203484C(&display->unk_6658, BoxApp_GetBoxMessageID(boxApp));
	BoxTaskParams_Free(params);
}


static void BoxGraphics_ShowMenu(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	BoxWindow_0203484C(&display->unk_6658, BoxApp_GetBoxMessageID(boxApp));
	BoxWindow_02034990(&display->unk_6658, &boxApp->boxMenu);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_CloseMessageBox(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxWindow_02034B30(&display->unk_6658);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_0202FE58(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxWindow_02034B64(&display->unk_6658);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_UpdateMenuCursor(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	Sound_PlayEffect(SEQ_SE_DP_SELECT);
	BoxGraphics_DrawMenuCursorBox(&display->unk_6658, &boxApp->boxMenu);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_0202FEA4(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	Sound_PlayEffect(SEQ_SE_DP_DECIDE);
	BoxWindow_02034BD0(&display->unk_6658, &boxApp->boxMenu);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_ShowBoxSelectionPopup(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxAltWindow_02034FBC(&display->unk_6690);
			params->state++;
			break;
		
		case 1:
			if (BoxAltWindow_02034FDC(&display->unk_6690)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ScrollBoxSelectionPopup(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_DECIDE);
			BoxAltWindow_02034FE4(&display->unk_6690);
			params->state++;
			break;
		
		case 1:
			BoxTaskParams_Free(params);
			break;
	}
}


static void BoxGraphics_0202FF48(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxAltWindow_02035008(&display->unk_6690);
			params->state++;
			break;
		
		case 1:
			if (BoxAltWindow_02035014(&display->unk_6690)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_TransitionWallpaper(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxPanel_02031598(&display->unk_494);
			params->state++;
			break;
		
		case 1:
			if (BoxPanel_020315B8(&display->unk_494)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PressBoxButton(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_DECIDE);
			BoxCursor_02032AA4(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_02032B18(&display->unk_5E24)) {
				switch (BoxApp_GetCursorLocation(boxApp)) {
				case CURSOR_ON_PARTY_BUTTON:
					BoxButton_02033D04(&display->unk_65BC, 1);
					break;
				case CURSOR_ON_CLOSE_BUTTON:
					BoxButton_02033D04(&display->unk_65BC, 2);
					break;
				}
				
				params->unk_08 = 0;
				params->state++;
			}
			break;
		
		case 2:
			params->unk_08++;
			if (params->unk_08 >= 6) {
				BoxCursor_02032C10(&display->unk_5E24);
				BoxButton_02033D04(&display->unk_65BC, 0);
				params->state++;
			}
			break;
		
		case 3:
			if (BoxCursor_02032C6C(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_OpenPartyPopup(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_OPEN7);
			BoxCursor_02032E08(&display->unk_5E24);
			BoxParty_02035B84(&display->unk_B290);
			params->state++;
			break;
		
		case 1:
			if (BoxParty_02035BB4(&display->unk_B290)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_020300D0(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxItemIcon_02037FC0(display->unk_B40C);
			params->state++;
			// Fall through
		
		case 1:
			if (BoxItemIcon_02037FEC(display->unk_B40C) == 0) {
				break;
			}
			
			params->state++;
			// Fall through
		
		case 2:
			Sound_PlayEffect(SEQ_SE_DP_CLOSE7);
			BoxParty_02035C24(&display->unk_B290);
			params->state++;
			break;
		
		case 3:
			if (BoxParty_02035C44(&display->unk_B290)) {
				BoxCursor_02032E40(&display->unk_5E24);
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_PlayAdjustPartyAnimation(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxParty_02035CF0(&display->unk_B290);
			params->state++;
			break;
		
		case 1:
			if (BoxParty_02035E18(&display->unk_B290)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_02030190(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_PARTY) {
				BoxParty_02035E28(&display->unk_B290);
				params->state++;
				break;
			}
			
			BoxTaskParams_Free(params);
			break;
		
		case 1:
			if (BoxParty_02035EC4(&display->unk_B290)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_SetMonMarkingsTransparency(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxPanel_02031B9C(&display->unk_494);
	BoxParty_0203605C(&display->unk_B290);
	BoxCursor_02032EA0(&display->unk_5E24);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_OpenOrCloseSelectedTouchDial(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxTouchSelector_0203697C(display->unk_B408);
			params->state++;
			break;
		
		case 1:
			if (BoxTouchSelector_02036A60(display->unk_B408)) {
				if (boxApp->touchDialHelper.selectedTouchDial == TOUCH_DIALS_NONE) {
					BoxTaskParams_Free(params);
				} else {
					BoxTouchSelector_02036D18(display->unk_B408);
					params->state++;
				}
			}
			break;
		
		case 2:
			if (BoxTouchSelector_02036D44(display->unk_B408)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_02030284(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxTouchSelector_02036D6C(display->unk_B408);
			params->state++;
			break;
		
		case 1:
			if (BoxTouchSelector_02036E24(display->unk_B408)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_UpdateMarkingsTouchButton(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxTouchSelector_02037924(display->unk_B408);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_020302DC(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxCursor_02032EF4(&display->unk_5E24);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_StartDrawMultiSelect(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxGraphics_OpenMultiSelectCursor(&display->unk_5E24);
	BoxGraphics_ApplyMultiSelectMonShading(&display->unk_494);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_02030320(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxPanel_02031D30(&display->unk_494);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_ApplyMultiSelectMonShadingTask(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxGraphics_ApplyMultiSelectMonShading(&display->unk_494);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_02030358(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxCursor_02032644(&display->unk_5E24);
			params->state++;
			break;
		
		case 1:
			if (BoxCursor_0203268C(&display->unk_5E24)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_02030394(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxComparison_0203846C(display->unk_B410, 0);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_020303B0(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	BoxComparison_0203846C(display->unk_B410, 1);
	BoxTaskParams_Free(params);
}


static void BoxGraphics_020303CC(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	
	switch (params->state) {
		case 0:
			BoxComparison_02038518(display->unk_B410);
			params->state++;
			break;
		
		case 1:
			if (BoxComparison_02038538(display->unk_B410)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_ScreenFadeBothToBlackLogOff(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	
	switch (params->state) {
		case 0:
			Sound_PlayEffect(SEQ_SE_DP_PC_LOGOFF);
			StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 6, 1, HEAP_ID_BOX_GRAPHICS);
			params->state++;
			break;
		
		case 1:
			if (IsScreenFadeDone()) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_0203045C(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			params->unk_08 = BoxApp_GetCursorBoxPosition(boxApp);
			params->unk_0A = (params->function == FUNC_MPR_Unk_53);
			BoxPanel_02031D70(&display->unk_494, params->unk_08);
			params->state++;
			break;
		
		case 1:
			if (BoxPanel_02031D98(&display->unk_494, params->unk_08, params->unk_0A)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_020304B8(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			params->unk_08 = BoxApp_GetCursorBoxPosition(boxApp);
			BoxPanel_02031E84(&display->unk_494, params->unk_08);
			params->state++;
			break;
		
		case 1:
			if (BoxPanel_02031ED0(&display->unk_494, params->unk_08)) {
				BoxTaskParams_Free(params);
			}
			break;
	}
}


static void BoxGraphics_02030504(SysTask* task, void* param) {
	BoxTaskParams* params = (BoxTaskParams*)param;
	BoxApplicationDisplay* display = params->unk_0C;
	const BoxApplication* boxApp = display->unk_1C4;
	
	switch (params->state) {
		case 0:
			BoxPanel_02030B40(&display->unk_494);
			BoxPanel_02030C18(&display->unk_494, &boxApp->customization, 0, TRUE);
			params->state++;
			break;
		
		case 1:
			BoxTaskParams_Free(params);
			break;
	}
}


static void BoxGraphics_02030548(BoxApplicationDisplay* display, const BoxApplication* boxApp) {
	static const GXBanks v0 = {
		GX_VRAM_BG_128_B,
		GX_VRAM_BGEXTPLTT_NONE,
		GX_VRAM_SUB_BG_128_C,
		GX_VRAM_SUB_BGEXTPLTT_NONE,
		GX_VRAM_OBJ_64_E,
		GX_VRAM_OBJEXTPLTT_NONE,
		GX_VRAM_SUB_OBJ_16_I,
		GX_VRAM_SUB_OBJEXTPLTT_NONE,
		GX_VRAM_TEX_0_A,
		GX_VRAM_TEXPLTT_01_FG
	};
	
	static const GraphicsModes v1 = {
		GX_DISPMODE_GRAPHICS,
		GX_BGMODE_0,
		GX_BGMODE_0,
		GX_BG0_AS_3D,
	};
	
	static const BgTemplate v2 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x800,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = 0,
		.charBase = 0,
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 1,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v3 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x800,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xe000),
		.charBase = (GX_BG_CHARBASE_0x18000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 0,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v4 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x800,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xe800),
		.charBase = (GX_BG_CHARBASE_0x00000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 1,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v5 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x1000,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_512x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xf000),
		.charBase = (GX_BG_CHARBASE_0x10000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 3,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v6 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x800,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xf000),
		.charBase = (GX_BG_CHARBASE_0x10000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 0,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v7 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x0,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_512x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xd000),
		.charBase = (GX_BG_CHARBASE_0x00000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 1,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v8 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x0,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_512x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xe000),
		.charBase = (GX_BG_CHARBASE_0x00000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 2,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v9 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x0,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = (GX_BG_SCRBASE_0xf800),
		.charBase = (GX_BG_CHARBASE_0x00000),
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 3,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v10 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x0,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = GX_BG_SCRBASE_0xe000,
		.charBase = GX_BG_CHARBASE_0x00000,
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 1,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	static const BgTemplate v11 = {
		.x = 0,
		.y = 0,
		.bufferSize = 0x0,
		.baseTile = 0,
		.screenSize = BG_SCREEN_SIZE_256x256,
		.colorMode = GX_BG_COLORMODE_16,
		.screenBase = GX_BG_SCRBASE_0xe800,
		.charBase = GX_BG_CHARBASE_0x00000,
		.bgExtPltt = GX_BG_EXTPLTT_01,
		.priority = 2,
		.areaOver = 0,
		.mosaic = FALSE,
	};
	
	GXLayers_SetBanks(&v0);
	
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	
	SetAllGraphicsModes(&v1);
	Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_MAIN_1, &v3, 0);
	Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_MAIN_2, &v4, 0);
	Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_MAIN_3, &v5, 0);
	
	if (BoxApp_GetBoxMode(boxApp) != PC_MODE_COMPARE) {
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_0, &v6, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_1, &v7, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_2, &v8, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_3, &v9, 0);
	} else {
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_0, &v6, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_1, &v10, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_2, &v11, 0);
		Bg_InitFromTemplate(display->unk_1C0, BG_LAYER_SUB_3, &v9, 0);
	}
	
	BoxGraphics_020307A4();
}


static void BoxGraphics_02030660(BoxApplicationDisplay* display, const BoxApplication* boxApp) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_GRAPHIC__BOX;
	} else {
		narc = NARC_INDEX_PL_GRAPHIC__BOX;
	}
	
	Graphics_LoadTilesToBgLayer(narc, 4, display->unk_1C0, 1, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
	Bg_FillTilemapRect(display->unk_1C0, 1, 0x0, 0, 0, 32, 32, 17);
	Bg_CopyTilemapBufferToVRAM(display->unk_1C0, 1);
	Graphics_LoadTilesToBgLayer(narc, 1, display->unk_1C0, 2, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
	Graphics_LoadPalette(narc, 5, 0, 0, 0x20 * 7, HEAP_ID_BOX_GRAPHICS);
	
	if (BoxApp_GetBoxMode(boxApp) != PC_MODE_COMPARE) {
		Graphics_LoadTilemapToBgLayer(narc, 0, display->unk_1C0, 2, 0, 0, 1, HEAP_ID_BOX_GRAPHICS);
	}
	
	BoxPanel_02030B68(&display->unk_494);
	BoxPanel_02030C18(&display->unk_494, &boxApp->customization, 0, TRUE);
	BoxMonPreview_02033EE4(&display->unk_6604);
	BoxWindow_020347C8(&display->unk_6658);
	BoxAltWindow_02034F10(&display->unk_6690);
	
	if (BoxApp_GetBoxMode(display->unk_1C4) != PC_MODE_RANCH_WITHDRAW) {
		BoxTouchSelector_020365D8(display->unk_B408);
	} else {
		BoxMPR_0203971C(display->mprDisplay);
	}
	
	BoxComparison_020382F4(display->unk_B410);
	
	if (BoxApp_GetCursorLocation(boxApp) == CURSOR_IN_PARTY) {
		BoxParty_02035B5C(&display->unk_B290);
	}
	
	GXLayers_EngineAToggleLayers(GX_PLANEMASK_OBJ, 1);
	GXLayers_EngineBToggleLayers(GX_PLANEMASK_OBJ, 1);
	GX_DispOn();
}


static void BoxGraphics_020307A4(void) {
	NNS_G3dInit();
	G3X_InitMtxStack();
	
	GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, 1);
	
	G2_SetBG0Priority(1);
	G3X_SetShading(GX_SHADING_TOON);
	G3X_AntiAlias(1);
	G3X_AlphaTest(0, 0);
	G3X_AlphaBlend(1);
	G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 63, 0);
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
	G3_ViewPort(0, 0, 255, 191);
}


static void BoxGraphics_0203081C(BoxApplicationDisplay* display, const BoxApplication* boxApp) {
	BoxCursor_02032034(&display->unk_5E24);
	BoxCursor_02033150(&display->unk_5E24);
	BoxButton_02033BA0(&display->unk_65BC);
	BoxItemIcon_02037BB4(display->unk_B40C);
}


static int BoxGraphics_GetBoxMoveDirection(u32 sourceBoxID, u32 destBoxID, u32 maxBoxID) {
	int distanceRight, distanceLeft;
	if (destBoxID > sourceBoxID) {
		distanceRight = destBoxID - sourceBoxID;
		distanceLeft = sourceBoxID + (maxBoxID - destBoxID);
	} else {
		distanceRight = destBoxID + (maxBoxID - sourceBoxID);
		distanceLeft = sourceBoxID - destBoxID;
	}
	
	return (distanceRight >= distanceLeft) ? -1 : 1;
}


SysTask* BoxGraphics_02030870(SysTaskFunc callback, void* param, u32 priority) {
	return SysTask_ExecuteOnVBlank(callback, param, priority);
}


NNSG2dImagePaletteProxy* BoxGraphics_02030878(BoxApplicationDisplay* display) {
	return &display->unk_1A8;
}


BoxIconManager* BoxGraphics_02030880(BoxApplicationDisplay* display) {
	return &display->unk_1C8;
}


BoxPanelManager* BoxGraphics_02030888(BoxApplicationDisplay* display) {
	return &display->unk_494;
}


BoxPartyManager* BoxGraphics_02030894(BoxApplicationDisplay* display) {
	return &display->unk_B290;
}


BoxCursorManager* BoxGraphics_020308A0(BoxApplicationDisplay* display) {
	return &display->unk_5E24;
}


BoxItemIconManager* BoxGraphics_020308AC(BoxApplicationDisplay* display) {
	return display->unk_B40C;
}


BoxAltWindowManager* BoxGraphics_020308B8(BoxApplicationDisplay* display) {
	return &display->unk_6690;
}


PokemonSpriteManager* BoxGraphics_020308C4(BoxApplicationDisplay* display) {
	return display->unk_1BC;
}


int BoxGraphics_020308CC(BoxApplicationDisplay* display) {
	if (BoxApp_GetBoxMode(display->unk_1C4) != PC_MODE_COMPARE) {
		return 0;
	} else {
		return -44; // ?
	}
}


void BoxGraphics_020308E8(SpriteResourcesHeader* param0, NNSG2dImageProxy* param1, NNSG2dImagePaletteProxy* param2, NNSG2dCellDataBank* param3, NNSG2dCellAnimBankData* param4, u32 param5) {
	param0->imageProxy        = param1;
	param0->paletteProxy      = param2;
	param0->cellBank          = param3;
	param0->cellAnimBank      = param4;
	param0->priority          = param5;
	param0->charData          = NULL;
	param0->multiCellBank     = NULL;
	param0->multiCellAnimBank = NULL;
	param0->isVRamTransfer    = 0;
}


Sprite* BoxGraphics_02030908(SpriteList* param0, SpriteResourcesHeader* param1, u32 param2, u32 param3, u32 param4, int param5) {
	SpriteListTemplate spriteTemplate;
	spriteTemplate.list         = param0;
	spriteTemplate.resourceData = param1;
	spriteTemplate.position.x   = param2 * FX32_ONE;
	spriteTemplate.position.y   = param3 * FX32_ONE;
	spriteTemplate.position.z   = 0;
	spriteTemplate.priority     = param4;
	spriteTemplate.vramType     = param5;
	spriteTemplate.heapID       = HEAP_ID_BOX_GRAPHICS;
	
	OSIntrMode intrBak = OS_DisableInterrupts();
	Sprite* sprite = SpriteList_Add(&spriteTemplate);
	OS_RestoreInterrupts(intrBak);
	
	if (sprite != NULL) {
		Sprite_SetAnimateFlag(sprite, TRUE);
		Sprite_SetAnimSpeed(sprite, FX32_ONE);
	}
	
	return sprite;
}


void BoxGraphics_SetSpritePriority(Sprite* sprite, u32 priority) {
	OSIntrMode mode = OS_DisableInterrupts();
	Sprite_SetPriority(sprite, priority);
	OS_RestoreInterrupts(mode);
}


void BoxGraphics_02030974(const u16* param0, u16* param1, u32 param2, u16 param3, u32 param4) {
	int v0, v1, v2;
	int v3, v4, v5;
	int v6;
	
	v3 = (param3 & 0x1F) << 8;
	v4 = ((param3 >> 5) & 0x1F) << 8;
	v5 = ((param3 >> 10) & 0x1F) << 8;
	
	while (param2--) {
		v0 = ((*param0) & 0x1F) << 8;
		v1 = (((*param0) >> 5) & 0x1F) << 8;
		v2 = (((*param0) >> 10) & 0x1F) << 8;
		
		v0 += (((v3 - v0) / 16) * param4);
		v1 += (((v4 - v1) / 16) * param4);
		v2 += (((v5 - v2) / 16) * param4);
		
		v0 = (v0 >> 8) & 0x1F;
		v1 = (v1 >> 8) & 0x1F;
		v2 = (v2 >> 8) & 0x1F;
		
		*param1 = ((v2 << 10) | (v1 << 5) | v0);
		
		param0++;
		param1++;
	}
}


const BoxApplicationManager* BoxGraphics_02030A10(BoxApplicationDisplay* display) {
	return display->unk_B414;
}


static u32 sDAT_0213664C = 0;

static struct {
	void   (*unk_00)(void*);
	void*   unk_04;
	void*   unk_08;
} sDAT_02136650[32];

static void BoxGraphics_02030A1C(void) {
	sDAT_0213664C = 0;
}


static void BoxGraphics_02030A28(void) {
	if (sDAT_0213664C > 0) {
		for (u32 i = 0; i < sDAT_0213664C; i++) {
			Heap_Free(sDAT_02136650[i].unk_08);
			
			if (sDAT_02136650[i].unk_00) {
				sDAT_02136650[i].unk_00(sDAT_02136650[i].unk_04);
			}
		}
		
		sDAT_0213664C = 0;
	}
}


void BoxGraphics_02030A64(void* param0, void (*param1)(void*), void* param2) {
	GF_ASSERT(sDAT_0213664C < 32);
	
	sDAT_02136650[sDAT_0213664C].unk_08 = param0;
	sDAT_02136650[sDAT_0213664C].unk_00 = param1;
	sDAT_02136650[sDAT_0213664C].unk_04 = param2;
	sDAT_0213664C++;
}


MPRFilterMenu* BoxGraphics_GetMPRFilter(BoxApplicationDisplay* display) {
	return display->mprFilter;
}


void BoxGraphics_02030AAC(BoxApplicationDisplay* display, u32 boxID) {
	display->unk_494.boxID = boxID;
	BoxCursor_02033150(&display->unk_5E24);
}
