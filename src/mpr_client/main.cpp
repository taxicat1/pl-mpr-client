#include <nitro.h>

extern "C" {
#include "mpr_client/main.h"

#include "mpr_client/mpr_client_common.h"
#include "mpr_client/mpr_comm.h"
#include "mpr_client/mpr_save.h"
#include "mpr_client/mpr_menu.h"

#include "assert.h"
#include "boot.h"
#include "brightness_controller.h"
#include "chatot_cry.h"
#include "font.h"
#include "save_player.h"
#include "game_overlay.h"
#include "game_version.h"
#include "gx_gf.h"
#include "heap.h"
#include "math_util.h"
#include "narc.h"
#include "play_time_manager.h"
#include "rtc.h"
#include "save_read_error.h"
#include "screen_fade.h"
#include "sound_system.h"
#include "system.h"
#include "timer.h"
#include "wireless_driver.h"
#include "applications/game_init/game_init.h"
#include "applications/dummy/dummy_application.h"
#include "applications/pc_boxes/box_app_manager.h"

#include "fs_overlay_hook.h" //[!] Overlays disabled
}

typedef struct {
	int currOverlayID;
	ApplicationManager* currApplication;
	
	int nextOverlayID;
	const ApplicationManagerTemplate* nextApplication;
	
	ApplicationArgs args;
	
	// New with MPR
	PokemonStorageSession boxAppArgs;
	
	int unused1;
	int unused2;
} Application;

FS_EXTERN_OVERLAY(dummy);

BOOL gIsPlatinum;
BOOL gIsDiamondPearl;

static Application sApplication;

static u16 sLockID;

static BOOL CardPulledOutCallback(void);
static void InitApplication(void);
static void RunApplication(void);
static void WaitFrame(void);
static void TrySystemReset(OSResetParameter param);
static void SoftReset(OSResetParameter resetParam);

#define RESET_COMBO       (PAD_BUTTON_START | PAD_BUTTON_SELECT | PAD_BUTTON_L | PAD_BUTTON_R)
#define RETURN_EOO_COMBO  (PAD_BUTTON_START | PAD_BUTTON_SELECT)

// Parameters for probing for DP
#define DP_TEST_NARC                       NARC_INDEX_DP_DEMO__TITLE__TITLEDEMO
#define DP_TEST_NARC_NUM_FILES             20

#define DP_TEST_NARC_TEST_FILE_1_IDX       1
#define DP_TEST_NARC_TEST_FILE_1_SIZE      0x6040
#define DP_TEST_NARC_TEST_FILE_1_CHECKSUM  0xA4F4

#define DP_TEST_NARC_TEST_FILE_2_IDX       3
#define DP_TEST_NARC_TEST_FILE_2_SIZE      0x6040
#define DP_TEST_NARC_TEST_FILE_2_CHECKSUM  0xA5C6

// Parameters for probing for Pt
#define PL_TEST_NARC                       NARC_INDEX_PL_DEMO__TITLE__TITLEDEMO
#define PL_TEST_NARC_NUM_FILES             29

#define PL_TEST_NARC_TEST_FILE_IDX         23
#define PL_TEST_NARC_TEST_FILE_SIZE        0x2040
#define PL_TEST_NARC_TEST_FILE_CHECKSUM    0xCD2B


void NitroMain(void) {
	InitSystem();
	InitVRAM();
	InitKeypadAndTouchpad();
	SetGBACartridgeVersion(VERSION_NONE);
	GFGX_Init();
	InitRTC();
	
	sLockID = OS_GetLockID();
	CARD_SetPulledOutCallback(CardPulledOutCallback);
	InitApplication();
	
	Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_MAIN_MENU, HEAP_SIZE_MAIN_MENU);
	
	gIsDiamondPearl = (NARC_GetFileCountByIndex(DP_TEST_NARC, FALSE) == DP_TEST_NARC_NUM_FILES);
	
	// Diamond title screen logo identification
	if (gIsDiamondPearl) {
		u32 size = NARC_GetMemberSizeByIndexPair(DP_TEST_NARC, DP_TEST_NARC_TEST_FILE_1_IDX);
		gIsDiamondPearl = (size == DP_TEST_NARC_TEST_FILE_1_SIZE);
		if (gIsDiamondPearl) {
			void* data = NARC_AllocAndReadWholeMemberByIndexPair(DP_TEST_NARC, DP_TEST_NARC_TEST_FILE_1_IDX, HEAP_ID_MAIN_MENU);
			gIsDiamondPearl = (MATH_CalcChecksum16(data, size) == DP_TEST_NARC_TEST_FILE_1_CHECKSUM);
			Heap_Free(data);
		}
	}
	
	// Pearl title screen logo identification
	if (gIsDiamondPearl) {
		u32 size = NARC_GetMemberSizeByIndexPair(DP_TEST_NARC, DP_TEST_NARC_TEST_FILE_2_IDX);
		gIsDiamondPearl = (size == DP_TEST_NARC_TEST_FILE_2_SIZE);
		if (gIsDiamondPearl) {
			void* data = NARC_AllocAndReadWholeMemberByIndexPair(DP_TEST_NARC, DP_TEST_NARC_TEST_FILE_2_IDX, HEAP_ID_MAIN_MENU);
			gIsDiamondPearl = (MATH_CalcChecksum16(data, size) == DP_TEST_NARC_TEST_FILE_2_CHECKSUM);
			Heap_Free(data);
		}
	}
	
	// Platinum title screen graphic identification
	// If DP was identified, can skip this
	if (gIsDiamondPearl) {
		gIsPlatinum = FALSE;
	} else {
		gIsPlatinum = (NARC_GetFileCountByIndex(PL_TEST_NARC, FALSE) == PL_TEST_NARC_NUM_FILES);
		if (gIsPlatinum) {
			u32 size = NARC_GetMemberSizeByIndexPair(PL_TEST_NARC, PL_TEST_NARC_TEST_FILE_IDX);
			gIsPlatinum = (size == PL_TEST_NARC_TEST_FILE_SIZE);
			if (gIsPlatinum) {
				void* data = NARC_AllocAndReadWholeMemberByIndexPair(PL_TEST_NARC, PL_TEST_NARC_TEST_FILE_IDX, HEAP_ID_MAIN_MENU);
				gIsPlatinum = (MATH_CalcChecksum16(data, size) == PL_TEST_NARC_TEST_FILE_CHECKSUM);
				Heap_Free(data);
			}
		}
	}
	
	Heap_Destroy(HEAP_ID_MAIN_MENU);
	
	Fonts_Init();
	Font_InitManager(FONT_SYSTEM,  HEAP_ID_APPLICATION);
	Font_InitManager(FONT_MESSAGE, HEAP_ID_APPLICATION);
	Font_InitManager(FONT_UNOWN,   HEAP_ID_APPLICATION);
	
	sApplication.args.unk_00 = -1;
	sApplication.args.saveData = SaveData_Init();
	
	sApplication.boxAppArgs.saveData = sApplication.args.saveData;
	sApplication.boxAppArgs.boxMode = PC_MODE_RANCH_DEPOSIT;
	
	SoundSystem_Init(
		SaveData_GetChatotCry(sApplication.args.saveData),
		SaveData_GetOptions(sApplication.args.saveData)
	);
	
	Timer_Start();
	
	if (SaveData_BackupExists(sApplication.args.saveData) == FALSE) {
		SaveReadError_ShowWindow(HEAP_ID_SYSTEM);
	}
	
	WirelessDriver_Init();
	while (!WirelessDriver_IsReady()) {
		OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	}
	
	if (!(gIsDiamondPearl == FALSE && gIsPlatinum == FALSE) && SaveData_GetLoadCheckStatus(sApplication.args.saveData) != LOAD_RESULT_CORRUPT) {
		MPRComm_Init(sApplication.args.saveData);
	}
	
	switch (OS_GetResetParameter()) {
		case RESET_CLEAN:
			sApplication.args.error = FALSE;
			EnqueueApplication(FS_OVERLAY_ID_NONE, &gGameInitApplication);
			break;
		
		case RESET_ERROR:
			/* Unreachable */
			sApplication.args.error = TRUE;
			EnqueueApplication(FS_OVERLAY_ID(dummy), &gDummyApplication);
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	gSystem.showTitleScreenIntro = TRUE;
	gSystem.frameCounter = 0;
	
	InitRNG();
	BrightnessController_ResetAllControllers();
	PlayTime_FlagNotStarted();
	
	MPRSave_InitThread(sApplication.args.saveData);
	
	while (TRUE) {
		MPRComm_TickSystem();
		HandleConsoleFold();
		ReadKeypadAndTouchpad();
		
		if (gSystem.pressedKeys) {
			MPRComm_SendKeyInput(gSystem.heldKeys);
		}
		
		// MB_IsMultiBootChild() should always return true so this is unreachable
		if (!MB_IsMultiBootChild() && (gSystem.heldKeys & RETURN_EOO_COMBO) == RETURN_EOO_COMBO) {
			RebootAndLoadROM("data/eoo.dat");
		}
		
		// BUG: They added !MB_IsMultiBootChild() to the wrong place, so this is written incorrectly and can never
		//      trigger a soft reset even if MB_IsMultiBootChild() returns false
		if (((gSystem.heldKeysRaw & RESET_COMBO) && !MB_IsMultiBootChild()) == RESET_COMBO && !gSystem.inhibitReset) {
			SoftReset(RESET_CLEAN);
		}
		
		RunApplication();
		SysTaskManager_ExecuteTasks(gSystem.mainTaskMgr);
		SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);
		
		if (gSystem.frameCounter == 0) {
			OS_WaitIrq(TRUE, OS_IE_V_BLANK);
			gSystem.vblankCounter++;
		}
		
		UpdateRTC();
		PlayTime_IncrementTimer();
		GFGX_SwapBuffers();
		SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);
		OS_WaitIrq(TRUE, OS_IE_V_BLANK);
		gSystem.vblankCounter++;
		gSystem.frameCounter = 0;
		
		CARD_LockRom(sLockID);
		CARD_CheckPulledOut();
		CARD_UnlockRom(sLockID);
		
		BrightnessController_Update();
		ExecScreenFade();
		
		if (gSystem.vblankCallback) {
			gSystem.vblankCallback(gSystem.vblankCallbackData);
		}
		
		SoundSystem_Tick();
		SysTaskManager_ExecuteTasks(gSystem.postVBlankTaskMgr);
	}
}


static BOOL CardPulledOutCallback(void) {
	// Do not automatically terminate program for card pullout
	return FALSE;
}


static void InitApplication(void) {
	sApplication.currOverlayID   = FS_OVERLAY_ID_NONE;
	sApplication.currApplication = NULL;
	sApplication.nextOverlayID   = FS_OVERLAY_ID_NONE;
	sApplication.nextApplication = NULL;
}


static void RunApplication(void) {
	if (sApplication.currApplication == NULL) {
		if (sApplication.nextApplication == NULL) {
			EnqueueApplication(FS_OVERLAY_ID(dummy), &gMPRMenuAppTemplate);
		}
		
		if (sApplication.nextOverlayID != FS_OVERLAY_ID_NONE) {
			Overlay_LoadByID(sApplication.nextOverlayID, OVERLAY_LOAD_NORMAL);
		}
		
		sApplication.currOverlayID = sApplication.nextOverlayID;
		
		// Use boxAppArgs or regular args depending on the application
		if (
			sApplication.nextApplication->init == BoxAppMan_Init ||
			sApplication.nextApplication->main == BoxAppMan_Main ||
			sApplication.nextApplication->exit == BoxAppMan_Exit
		) {
			sApplication.currApplication = ApplicationManager_New(sApplication.nextApplication, &sApplication.boxAppArgs, HEAP_ID_SYSTEM);
		} else {
			sApplication.currApplication = ApplicationManager_New(sApplication.nextApplication, &sApplication.args, HEAP_ID_SYSTEM);
		}
		
		sApplication.nextOverlayID = FS_OVERLAY_ID_NONE;
		sApplication.nextApplication = NULL;
	}
	
	if (ApplicationManager_Exec(sApplication.currApplication)) {
		ApplicationManager_Free(sApplication.currApplication);
		sApplication.currApplication = NULL;
		
		if (sApplication.currOverlayID != FS_OVERLAY_ID_NONE) {
			Overlay_UnloadByID(sApplication.currOverlayID);
		}
	}
}


void EnqueueApplication(FSOverlayID overlayID, const ApplicationManagerTemplate* appManTemplate) {
	GF_ASSERT(sApplication.nextApplication == NULL);
	
	sApplication.nextOverlayID   = overlayID;
	sApplication.nextApplication = appManTemplate;
}


static void WaitFrame(void) {
	OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	
	gSystem.vblankCounter++;
	gSystem.frameCounter = 0;
	
	if (gSystem.vblankCallback != NULL) {
		gSystem.vblankCallback(gSystem.vblankCallbackData);
	}
}


static void TrySystemReset(OSResetParameter param) {
	OS_ResetSystem(param);
	WaitFrame();
}


static void SoftReset(OSResetParameter resetParam) {
	SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_WHITE);
	SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_WHITE);
	
	while (TRUE) {
		HandleConsoleFold();
		TrySystemReset(resetParam);
	}
}


void InitRNG(void) {
	RTCDate date;
	RTCTime time;
	GetCurrentDateTime(&date, &time);
	
	u32 seed = date.year + date.month * 0x100 * date.day * 0x10000 + time.hour * 0x10000 + (time.minute + time.second) * 0x1000000 + gSystem.vblankCounter;
	
	MTRNG_SetSeed(seed);
	LCRNG_SetSeed(seed);
}


void HandleConsoleFold(void) {
	if (CARD_IsPulledOut()) {
		PM_ForceToPowerOff();
	}
	
	PMBackLightSwitch top, bottom;
	
	if (PAD_DetectFold()) {
		if ((OS_GetIrqMask() & OS_IE_CARTRIDGE) && CTRDG_IsPulledOut()) {
			PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, PM_PAD_LOGIC_OR, 0);
			PM_ForceToPowerOff();
		}
		
		PM_GetBackLight(&top, &bottom);
		if (top == PM_BACKLIGHT_ON) {
			PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
		}
	} else {
		PM_GetBackLight(&top, &bottom);
		if (top == PM_BACKLIGHT_OFF) {
			PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_ON);
		}
	}
}
