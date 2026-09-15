#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_DEF_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_DEF_H

#include <nitro/types.h>

#include "party.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "sprite.h"
#include "string_gf.h"
#include "string_template.h"
#include "touch_screen_actions.h"

#define PREVIEW_MON_UNDER_CURSOR     1
#define PREVIEW_MON_IN_CURSOR        2
#define PREVIEW_MON_FROM_SELECTION   4
#define PREVIEW_MON_UNDER_SELECTION  8

#define PREVIEW_MON_HELD       6
#define PREVIEW_MON_SELECTION  12

typedef enum {
	CURSOR_IN_BOX = 0,
	CURSOR_IN_PARTY,
	CURSOR_ON_BOX_HEADER,
	CURSOR_ON_PARTY_BUTTON,
	CURSOR_ON_CLOSE_BUTTON
} CursorLocation;

typedef struct {
	void*  mon;
	u8     cursorLocation;
	s8     boxCol;
	s8     boxRow;
	s8     posInBox;
	s8     posInParty;
	s8     prevCursorLocation;
	u8     isMonUnderCursor;
	u8     previewMonSource;
} BoxCursor;

typedef struct {
	BoxPokemon*  boxMon;
	u8           selectionStartCol;
	u8           selectionStartRow;
	u8           selectionEndCol;
	u8           selectionEndRow;
	u8           selectedMonCount;
	u8           origSelectionTopLeftPos;
	u8           unused;
	u8           cursorMonIsPartyMon;
	s8           selectedMonsOrigBoxPos[MAX_MONS_PER_BOX];
	u8           padding_2A[2];
} BoxMonSelection;

typedef struct {
	u8       boxID;
	u8       wallpaper;
	
	// New with MPR (previously padding)
	u8       maxRanchBoxID;
	s8       ranchBoxJump;
	
	String*  name;
	
	// New with MPR
	String*          ranchName;
	StringTemplate*  ranchStringTemplate;
} BoxCustomization;

typedef enum {
	PC_MODE_DEPOSIT = 0,
	PC_MODE_WITHDRAW,
	PC_MODE_MOVE_MONS,
	PC_MODE_MOVE_ITEMS,
	PC_MODE_COMPARE,
	
	// New with MPR
	PC_MODE_RANCH_DEPOSIT,
	PC_MODE_RANCH_WITHDRAW
} BoxMode;

typedef struct {
	u8   boxMode;
	u8   isCursorFastMode;
	u16  monSpriteTransparencyMask;
} BoxSettings;

typedef struct {
	void*    mon;
	u16      species;
	u16      heldItem;
	u16      dexNum;
	u8       level;
	u8       markings;
	u8       type1;
	u8       type2;
	u8       gender;
	u8       isEgg;
	u8       padding_10[4];
	String*  nickname;
	String*  speciesName;
	String*  heldItemName;
	String*  nature;
	String*  ability;
} PCMonPreview;

typedef enum {
	BOX_MENU_NAVIGATION_NONE = -3,
	BOX_MENU_NAVIGATION_UP_DOWN,
	BOX_MENU_NAVIGATION_B,
	BOX_MENU_JUMP,
	BOX_MENU_WALLPAPER,
	BOX_MENU_NAME,
	BOX_MENU_HEADER_CANCEL,
	BOX_MENU_FIRST_WALLPAPER_PAGE,
	BOX_MENU_SCENERY_1 = BOX_MENU_FIRST_WALLPAPER_PAGE,
	BOX_MENU_SCENERY_2,
	BOX_MENU_SCENERY_3,
	BOX_MENU_ETCETERA,
	BOX_MENU_LAST_DEFAULT_WALLPAPER_PAGE = BOX_MENU_ETCETERA,
	BOX_MENU_FRIENDS_1,
	BOX_MENU_FRIENDS_2,
	BOX_MENU_LAST_WALLPAPER_PAGE = BOX_MENU_FRIENDS_2,
	BOX_MENU_FIRST_WALLPAPER,
	BOX_MENU_FOREST = BOX_MENU_FIRST_WALLPAPER,
	BOX_MENU_CITY,
	BOX_MENU_DESERT,
	BOX_MENU_SAVANNA,
	BOX_MENU_CRAG,
	BOX_MENU_VOLCANO,
	BOX_MENU_SNOW,
	BOX_MENU_CAVE,
	BOX_MENU_BEACH,
	BOX_MENU_SEAFLOOR,
	BOX_MENU_RIVER,
	BOX_MENU_SKY,
	BOX_MENU_POKECENTER,
	BOX_MENU_MACHINE,
	BOX_MENU_CHECKS,
	BOX_MENU_SIMPLE,
	BOX_MENU_FIRST_UNLOCKABLE_WALLPAPER,
	BOX_MENU_DISTORTION = BOX_MENU_FIRST_UNLOCKABLE_WALLPAPER,
	BOX_MENU_CONTEST,
	BOX_MENU_NOSTALGIC,
	BOX_MENU_CROAGUNK,
	BOX_MENU_TRIO,
	BOX_MENU_PIKAPIKA,
	BOX_MENU_LEGEND,
	BOX_MENU_TEAM_GALACTIC,
	BOX_MENU_LAST_WALLPAPER = BOX_MENU_TEAM_GALACTIC,
	BOX_MENU_MOVE,
	BOX_MENU_PLACE,
	BOX_MENU_SHIFT,
	BOX_MENU_SUMMARY,
	BOX_MENU_WITHDRAW,
	BOX_MENU_STORE,
	BOX_MENU_ITEM,
	BOX_MENU_MARK,
	BOX_MENU_RELEASE,
	BOX_MENU_CANCEL,
	BOX_MENU_CONFIRM,
	BOX_MENU_MARK_CANCEL,
	BOX_MENU_GIVE,
	BOX_MENU_TAKE,
	BOX_MENU_INFO,
	BOX_MENU_SWITCH,
	BOX_MENU_BAG,
	BOX_MENU_ITEMS_CANCEL,
	BOX_MENU_SET_ON_LEFT,
	BOX_MENU_SET_ON_RIGHT,
	BOX_MENU_YES,
	BOX_MENU_NO,
	BOX_MENU_FIRST_MARKING,
	BOX_MENU_CIRCLE = BOX_MENU_FIRST_MARKING,
	BOX_MENU_TRIANGLE,
	BOX_MENU_SQUARE,
	BOX_MENU_HEART,
	BOX_MENU_STAR,
	BOX_MENU_DIAMOND,
	BOX_MENU_LAST_MARKING = BOX_MENU_DIAMOND,
	
	// New with MPR
	BOX_MENU_RANCH_DEPOSIT,
	BOX_MENU_RANCH_WITHDRAW,
	BOX_MENU_RANCH_10_RIGHT,
	BOX_MENU_RANCH_10_LEFT,
	BOX_MENU_RANCH_FIRST,
	BOX_MENU_RANCH_LAST
} BoxMenuItem;

typedef enum {
	BOX_MENU_TYPE_YES_NO,
	BOX_MENU_TYPE_POKEMON_OP,
	BOX_MENU_TYPE_HEADER,
	BOX_MENU_TYPE_WALLPAPER,
	BOX_MENU_TYPE_MARKING
} BoxMenuType;

#define MAX_BOX_MENU_ITEMS           8
#define MAX_BOX_WALLPAPERS_PER_PAGE  4

typedef struct {
	BoxMenuItem  menuItems[MAX_BOX_MENU_ITEMS];
	u8           selectedMenuItemIndex;
	u8           totalMenuItems;
	u8           markings;
	u8           type;
} BoxMenu;

typedef enum {
	TOUCH_DIALS_NONE,
	TOUCH_DIALS_BOX_JUMP,
	TOUCH_DIALS_MARKINGS
} TouchDials;

typedef struct {
	u8   selectedTouchDial;
	u8   markingsButtonsScrollOffset;
	u8   touchDialOffset;
	u8   padding_03;
	s16  scrollDelta;
} BoxTouchDialHelper;

typedef struct {
	void*    mon;
	u16      species;
	u8       isEgg;
	u8       form;
	u16      level;
	u16      maxHP;
	u16      attack;
	u16      defense;
	u16      spAttack;
	u16      spDefense;
	u16      speed;
	u16      cool;
	u16      beauty;
	u16      cute;
	u16      smart;
	u16      tough;
	u16      moves[4];
	String*  monName;
	String*  nature;
} PCCompareMon;

typedef enum {
	COMPARE_BATTLE_STATS,
	COMPARE_CONTEST_STATS,
	COMPARE_MOVES,
	NUM_COMPARE_MODES
} CompareMode;

typedef struct {
	u8            compareMonSlot;
	u8            compareMode;
	u8            compareSlotHasMon[2];
	BOOL          compareButtonAnimationPressed;
	PCCompareMon  compareMons[2];
} CompareModeHelper;

#define MAIN_PC_LEFT_BUTTON_ID   0
#define MAIN_PC_LEFT_BUTTON_X    0
#define MAIN_PC_RIGHT_BUTTON_ID  1
#define MAIN_PC_RIGHT_BUTTON_X   255
#define MAIN_PC_BUTTON_Y         184
#define MAIN_PC_BUTTON_RADIUS    40

#define COMPARE_MON_PC_BUTTON_ID      2
#define COMPARE_MON_PC_BUTTON_X       128
#define COMPARE_MON_PC_BUTTON_Y       178
#define COMPARE_MON_PC_BUTTON_RADIUS  12

#define PC_MARKINGS_BUTTON1_ID      0
#define PC_MARKINGS_BUTTON1_X       152
#define PC_MARKINGS_BUTTON1_Y       168
#define PC_MARKINGS_BUTTON2_ID      1
#define PC_MARKINGS_BUTTON2_X       163
#define PC_MARKINGS_BUTTON2_Y       134
#define PC_MARKINGS_BUTTON3_ID      2
#define PC_MARKINGS_BUTTON3_X       184
#define PC_MARKINGS_BUTTON3_Y       104
#define PC_MARKINGS_BUTTON4_ID      3
#define PC_MARKINGS_BUTTON4_X       216
#define PC_MARKINGS_BUTTON4_Y       84
#define PC_MARKINGS_BUTTON5_ID      4
#define PC_MARKINGS_BUTTON5_X       240
#define PC_MARKINGS_BUTTON5_Y       52
#define PC_MARKINGS_BUTTON6_ID      5
#define PC_MARKINGS_BUTTON6_X       240
#define PC_MARKINGS_BUTTON6_Y       16
#define PC_MARKINGS_BUTTONS_RADIUS  16

#define PC_MARKINGS_BUTTON_TYPES  8

#define PREVIEW_GENDER_INVALID  -1

typedef enum {
	CURSOR_NO_MOVEMENT = 0,
	CURSOR_MOVE,
	CURSOR_STOP,
	CURSOR_MOVE_TO_LEFT_BOX,
	CURSOR_MOVE_TO_RIGHT_BOX
} CursorMovementState;

typedef enum {
	RELEASED_FROM_CURSOR,
	RELEASED_FROM_BOX,
	RELEASED_FROM_PARTY
} ReleasedFrom;

#define NUM_RELEASE_BLOCKING_MOVES  ((u32)3)

typedef struct {
	u32  state;
	u8   hasReset;
	s8   boxID;
	u16  boxMessageID;
} BoxSelectorPopup;

typedef struct {
	u8           checkedCanReleaseMon;
	u8           canReleaseMon;
	u8           boxID;
	u8           monPosInBox;
	u8           hasReleaseBlockingMove[NUM_RELEASE_BLOCKING_MOVES];
	u16          monsWithReleaseBlockingMoveCount[NUM_RELEASE_BLOCKING_MOVES];
	BoxPokemon*  boxMon;
	PCBoxes*     pcBoxes;
	Party*       party;
	BOOL         monHeldInCursor;
	
	// New with MPR
	u8  pcReleaseBlockingMoveMap[MAX_PC_BOXES];
	u8  partyReleaseBlockingMoveMap;
} ReleaseMon;

// New with MPR
typedef struct {
	int  unk_00; // Sort order?
	int  species;
	int  type;
	int  move;
	int  ability;
	int  nature;
	int  mark;
} MPRFilterSettings;

typedef struct {
	u32 unk_00; // 0-10 mode of some sort
	u32 activeBucket;
	int activePage;
	
	MPRFilterSettings  currSettings;
	MPRFilterSettings  appliedSettings;
	
	u32* touchScreenButtonPressedPtr;
	BOOL unk_48;
	BOOL unk_4C;
	TouchScreenButtonState buttonStates[10];
	
	BOOL pendingRotomRevertMsg;
	
	// Pointers to cached boxes
	u8*  unk_7C;
	u8*  unk_80;
	u8*  unk_84;
	u8*  unk_88;
	u8*  unk_8C;
} MPRFilterMenu;

typedef struct {
	PCBoxes*            pcBoxes;
	Party*              party;
	BoxCursor           cursor;
	BoxMonSelection     selection;
	BoxCustomization    customization;
	BoxSettings         boxSettings;
	PCMonPreview        pcMonPreview;
	BoxMenu             boxMenu;
	u32                 boxSelectionBoxID;
	BoxTouchDialHelper  touchDialHelper;
	u8                  padding_A2[2];
	CompareModeHelper   compareModeHelper;
	u32                 boxMessageID;
	u16                 selectedBoxID;
	u16                 cursorItem;
	
	// New with MPR
	u8*  MPR_unk_11C; // Hold bank pointers
	u8*  MPR_unk_120;
} BoxApplication;

typedef struct {
	SaveData*  saveData;
	BoxMode    boxMode;
	BOOL       recordBoxUseInJournal;
} PokemonStorageSession;

// Forward struct declarations to avoid circular includes
typedef struct BoxApplicationDisplay BoxApplicationDisplay;
typedef struct BoxPanelManager BoxPanelManager;
typedef struct BoxPartyManager BoxPartyManager;
typedef struct BoxItemIconManager BoxItemIconManager;
typedef struct BoxButtonManager BoxButtonManager;
typedef struct BoxMonPreviewManager BoxMonPreviewManager;
typedef struct BoxWindowManager BoxWindowManager;
typedef struct BoxAltWindowManager BoxAltWindowManager;
typedef struct BoxTouchSelectorManager BoxTouchSelectorManager;
typedef struct BoxComparisonManager BoxComparisonManager;
typedef struct MPRBoxDisplay MPRBoxDisplay;

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_DEF_H */
