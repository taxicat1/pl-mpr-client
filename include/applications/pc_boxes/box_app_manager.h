#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_APP_MANAGER_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_APP_MANAGER_H

#include "applications/naming_screen/naming_screen_main.h"
#include "applications/pc_boxes/box_def.h"
#include "applications/pc_boxes/box_touch_dial.h"
#include "applications/pokemon_summary_screen/summary_screen_main.h"

#include "message.h"
#include "overlay_manager.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "string_template.h"
#include "touch_screen_actions.h"

// Controls batch deposit/withdraw operations
typedef struct {
	u16  unk_00_0 : 5;
	u16  unk_00_5 : 5;
	u16  unk_00_A : 6;
	u16  unk_02_0 : 5;
	u16  unk_02_5 : 5;
	u16  unk_02_A : 5;
	u16           : 1;
} MPRBatchOp;

typedef struct BoxApplicationManager {
	BoxApplication          boxApp;
	BoxApplicationDisplay*  display;
	PokemonStorageSession*  pokemonStorageSession;
	SaveData*               saveData;
	PCBoxes*                pcBoxes;
	Party*                  party;
	NamingScreenArgs*       namingScreenArgs;
	PokemonSummary          monSummary;
	ReleaseMon              releaseMon;
	TouchScreenActions*     mainBoxAndCompareButtonsAction;
	TouchScreenActions*     markingsButtonsAction;
	u32                     touchScreenButtonPressed;
	u32                     touchScreenMarkingsButtonPressed;
	MessageLoader*          boxMessagesLoader;
	MessageLoader*          speciesNameLoader;
	MessageLoader*          natureNameLoader;
	MessageLoader*          abilityNameLoader;
	StringTemplate*         messageVariableBuffer;
	Pokemon*                mon;
	Options*                options;
	
	int (*cursorLocationInputHandler)(struct BoxApplicationManager* boxAppMan);
	void (*boxApplicationAction)(struct BoxApplicationManager* boxAppMan, u32* state);
	
	// New with MPR
	u32  MPR_unk_1E0;
	
	u32  cursorLocationHandlerState;
	u32  boxApplicationActionState;
	
	union {
		BoxMenuItem   menuItem;
		ReleasedFrom  releasedFrom;
		u32           menuTimer;
		MPRBatchOp    mprBatch;
	};
	
	BoxSelectorPopup  boxSelector;
	
	u32 unused; // Never referenced
	
	TouchDial touchDial;
	
	int unused1; // Only written to, never read from
	
	u32   boxJumpDelayCounter;
	BOOL  touchDialBoxChanged;
	BOOL  scrollEnded;
	int   touchDialScrollAmount;
	
	ApplicationManager*  applicationManager;
	void*                bagAppArgs;
	
	u32 unused2; // Never referenced
	
	// New with MPR
	MPRFilterMenu mprFilter;
} BoxApplicationManager;

BOOL BoxAppMan_Init(ApplicationManager* appMan, int* state);
BOOL BoxAppMan_Main(ApplicationManager* appMan, int* state);
BOOL BoxAppMan_Exit(ApplicationManager* appMan, int* state);
BOOL BoxAppMan_HasCheckedCanReleaseMon(const BoxApplicationManager* boxAppMan);
BOOL BoxAppMan_CanReleaseMon(const BoxApplicationManager* boxAppMan);
MessageLoader* BoxAppMan_GetBoxMessagesLoader(const BoxApplicationManager* boxAppMan);
const StringTemplate* BoxAppMan_GetMessageVariableBuffer(const BoxApplicationManager* boxAppMan);
int BoxAppMan_GetOptionsFrame(const BoxApplicationManager* boxAppMan);
MPRFilterMenu* BoxAppMan_GetMPRFilter(const BoxApplicationManager* boxAppMan);
BoxMode BoxApp_GetBoxMode(const BoxApplication* boxApp);
CursorLocation BoxApp_GetCursorLocation(const BoxApplication* boxApp);
u32 BoxApp_GetCursorBoxCol(const BoxApplication* boxApp);
u32 BoxApp_GetCursorBoxRow(const BoxApplication* boxApp);
u32 BoxApp_GetCursorBoxPosition(const BoxApplication* boxApp);
u32 BoxApp_GetCursorPartyPosition(const BoxApplication* boxApp);
BOOL BoxApp_IsMonUnderCursor(const BoxApplication* boxApp);
u32 BoxApp_GetPreviewMonSource(const BoxApplication* boxApp);
BOOL BoxApp_GetCursorMonIsPartyMon(const BoxApplication* boxApp);
BOOL BoxApp_IsMonAvailableToCursor(const BoxApplication* boxApp);
u32 BoxApp_GetCurrentBox(const BoxApplication* boxApp);
u32 BoxApp_GetMaxRanchBoxID(const BoxApplication* boxApp);
const PCMonPreview* BoxApp_GetPCMonPreview(const BoxApplication* boxApp);
u32 BoxApp_GetPreviewedMonHeldItem(const BoxApplication* boxApp);
const BoxCustomization* BoxApp_GetBoxCustomization(const BoxApplication* boxApp);
PCBoxes* BoxApp_GetPCBoxes(const BoxApplication* boxApp);
u32 BoxApp_GetBoxMessageID(const BoxApplication* boxApp);
BoxPokemon* BoxApp_GetPreviewedBoxMon(const BoxApplication* boxApp);
u32 BoxApp_GetPreviewedMonMarkings(const BoxApplication* boxApp);
u32 BoxApp_GetBoxSelectionBoxID(const BoxApplication* boxApp);
s32 BoxApp_GetTouchDialScrollDelta(const BoxApplication* boxAppMan);
u32 BoxApp_GetTouchDialOffset(const BoxApplication* boxApp);
u32 BoxApp_GetMarkingsButtonsScrollOffset(const BoxApplication* boxApp);
u32 BoxApp_GetMonSpriteTransparencyMask(const BoxApplication* boxApp);
BOOL BoxApp_IsCursorFastMode(const BoxApplication* boxApp);
void BoxApp_GetMultiSelectBoundingBox(const BoxApplication* boxApp, u32* leftCol, u32* rightCol, u32* topRow, u32* bottomRow);
BOOL BoxApp_IsMultiSelectSingleSelect(const BoxApplication* boxApp);
u32 BoxApp_GetMultiSelectTopLeftPos(const BoxApplication* boxApp);
u32 BoxApp_GetRelativeMonPosInMultiSelection(const BoxApplication* boxApp, u32 position);
u32 BoxApp_GetCursorItem(const BoxApplication* boxApp);
u32 BoxApp_GetCursorOrPreviewedItem(const BoxApplication* boxApp);
u32 BoxApp_GetCompareMonSlot(const BoxApplication* boxApp);
u32 BoxApp_GetCompareMode(const BoxApplication* boxApp);
const PCCompareMon* BoxApp_GetCompareMonFrom(const BoxApplication* boxApp, int compareSlot);
BOOL BoxApp_CompareSlotHasMon(const BoxApplication* boxApp, int compareMonSlot);
BOOL BoxApp_IsCompareButtonPressed(const BoxApplication* boxApp);
BOOL BoxApp_IsPreviewedMonEgg(const BoxApplication* boxApp);
BoxPokemon* BoxApp_0202E96C(const BoxApplication* boxApp, u32 index);
BoxPokemon* BoxApp_0202E99C(const BoxApplication* boxApp, u32 index);
BOOL BoxApp_0202E9C8(const BoxApplication* boxApp, u32 unused);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_APP_MANAGER_H */
