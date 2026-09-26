#include <nitro.h>

#include "system.h"

#include "common.h"
#include "assert.h"
#include "boot.h"
#include "heap.h"
#include "math_util.h"
#include "sys_task_manager.h"
#include "game_version.h"
#include "narc.h"
#include "graphics.h"

#include "mpr_client/mpr_text.h"

#include "mod_detect_version.h"

#define MAIN_TASK_MAX         160
#define VBLANK_TASK_MAX       16
#define POST_VBLANK_TASK_MAX  32
#define PRINT_TASK_MAX        4

System gSystem;

static void VBlankIntr(void);
static void HBlankIntr(void);
static void SetHBlankEnabled(BOOL enabled);
static void InitHeapSystem(void);
static void ApplyButtonModeToInput(void);


static void VBlankIntr(void) {
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
	MI_WaitDma(GX_DEFAULT_DMAID);
	SysTaskManager_ExecuteTasks(gSystem.vBlankTaskMgr);
	gSystem.frameCounter++;
}


void SetVBlankCallback(Callback cb, void* data) {
	gSystem.vblankCallback = cb;
	gSystem.vblankCallbackData = data;
}


void DisableHBlank(void) {
	SetHBlankEnabled(FALSE);
	gSystem.hblankCallback = NULL;
	gSystem.hblankCallbackData = NULL;
}


BOOL SetHBlankCallback(Callback cb, void* data) {
	if (cb == NULL) {
		SetHBlankEnabled(FALSE);
		gSystem.hblankCallback = NULL;
		gSystem.hblankCallbackData = NULL;
		return TRUE;
	} else {
		if (gSystem.hblankCallback == NULL) {
			gSystem.hblankCallbackData = data;
			gSystem.hblankCallback = cb;
			SetHBlankEnabled(TRUE);
			return TRUE;
		} else {
			return FALSE;
		}
	}
}


static void HBlankIntr(void) {
	if (gSystem.hblankCallback) {
		gSystem.hblankCallback(gSystem.hblankCallbackData);
	}
}


static void SetHBlankEnabled(BOOL enabled) {
	OS_DisableIrq();
	if (!enabled) {
		OSIrqMask savedMask = OS_GetIrqMask();
		OS_DisableIrqMask(OS_IE_H_BLANK);
		GX_HBlankIntr(FALSE);
	} else {
		OSIrqMask savedMask = OS_GetIrqMask();
		OS_SetIrqFunction(OS_IE_H_BLANK, HBlankIntr);
		OS_EnableIrqMask(OS_IE_H_BLANK);
		GX_HBlankIntr(TRUE);
	}
	OS_EnableIrq();
}


static const HeapParam sHeapInitParams[] = {
	{ HEAP_SIZE_SYSTEM,       OS_ARENA_MAIN },
	{ HEAP_SIZE_SAVE,         OS_ARENA_MAIN },
	{ HEAP_SIZE_DEBUG,        OS_ARENA_MAIN },
	{ HEAP_SIZE_APPLICATION,  OS_ARENA_MAIN }
};

static void InitHeapSystem(void) {
	u32 lowEntropyData[8];
	u8 md5[MATH_MD5_DIGEST_SIZE];
	
	OS_GetLowEntropyData(lowEntropyData);
	MATH_CalcMD5(md5, lowEntropyData, sizeof(lowEntropyData));
	
	u32 offset = 0;
	for (u32 i = 0; i < MATH_MD5_DIGEST_SIZE; i++) {
		offset += md5[i];
	}
	
	offset %= 256;
	while (offset % 4) {
		offset++;
	}
	
	Heap_InitSystem(sHeapInitParams, NELEMS(sHeapInitParams), HEAP_ID_MAX, offset);
}


void InitSystem(void) {
	OS_Init();
	FX_Init();
	GX_SetPower(GX_POWER_ALL);
	GX_Init();
	OS_InitTick();
	InitHeapSystem();
	
	gSystem.mainTaskMgr       = SysTaskManager_Init(MAIN_TASK_MAX,        OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(MAIN_TASK_MAX),        4));
	gSystem.vBlankTaskMgr     = SysTaskManager_Init(VBLANK_TASK_MAX,      OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(VBLANK_TASK_MAX),      4));
	gSystem.postVBlankTaskMgr = SysTaskManager_Init(POST_VBLANK_TASK_MAX, OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(POST_VBLANK_TASK_MAX), 4));
	gSystem.printTaskMgr      = SysTaskManager_Init(PRINT_TASK_MAX,       OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(PRINT_TASK_MAX),       4));
	
	GX_DispOff();
	GXS_DispOff();
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	
	OS_EnableIrqMask(OS_IE_V_BLANK);
	OS_EnableIrqMask(OS_IE_FIFO_RECV);
	OS_EnableIrq();
	GX_VBlankIntr(TRUE);
	
	CARD_Init();
	CARD_Enable(TRUE);
	
	// Hack!
	MBParam* mb = (MBParam*)MB_GetMultiBootParam();
	mb->boot_type = MB_TYPE_NORMAL;
	FS_Init(1);
	mb->boot_type = MB_TYPE_MULTIBOOT;
	
	AccessOuterRomFS();
	
	Mod_DetectVersionAndLanguage();
	NARC_SetVersion();
	MPRText_SetLanguage();
	
	u32 fsTableSize = FS_GetTableSize();
	void* fsTable = OS_AllocFromMainArenaLo(fsTableSize, 4);
	
	GF_ASSERT(fsTable != NULL);
	FS_LoadTable(fsTable, fsTableSize);
	
	gSystem.vblankCallback = NULL;
	gSystem.hblankCallback = NULL;
	gSystem.dummyCallback_10 = NULL;
	gSystem.dummyCallback_14 = NULL;
	gSystem.vblankCounter = 0;
	gSystem.whichScreenIs3D = DS_SCREEN_MAIN;
}


void InitVRAM(void) {
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();
	MI_CpuFillFast((void*)HW_OAM, 192, HW_OAM_SIZE);
	MI_CpuFillFast((void*)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
	MI_CpuClearFast((void*)HW_PLTT, HW_PLTT_SIZE);
	MI_CpuClearFast((void*)HW_DB_PLTT, HW_DB_PLTT_SIZE);
}


void* ReadFileToHeap(HeapID heapID, const char* filename) {
	FSFile file;
	FS_InitFile(&file);
	
	void* buf;
	if (FS_OpenFile(&file, filename)) {
		u32 length = FS_GetLength(&file);
		buf = Heap_Alloc(heapID, length);
		if (buf != NULL) {
			if (FS_ReadFile(&file, buf, length) != length) {
				Heap_FreeExplicit(heapID, buf);
				buf = NULL;
			}
		}
		
		FS_CloseFile(&file);
	} else {
		buf = NULL;
	}
	
	return buf;
}


void ReadFileToBuffer(const char* filename, void** buf) {
	FSFile file;
	FS_InitFile(&file);
	
	if (FS_OpenFile(&file, filename)) {
		u32 length = FS_GetLength(&file);
		if (*buf != NULL) {
			if (FS_ReadFile(&file, *buf, length) != length) {
				/* error not handled */
			}
		}
		
		FS_CloseFile(&file);
	}
}


void InitKeypadAndTouchpad(void) {
	gSystem.buttonMode = 0;
	gSystem.heldKeysRaw = 0;
	gSystem.pressedKeysRaw = 0;
	gSystem.pressedKeysRepeatableRaw = 0;
	gSystem.heldKeys = 0;
	gSystem.pressedKeys = 0;
	gSystem.pressedKeysRepeatable = 0;
	gSystem.autorepeatTimer = 0;
	gSystem.autorepeatRate = 8;
	gSystem.autorepeatDelay = 15;
	gSystem.touchX = 0;
	gSystem.touchY = 0;
	gSystem.touchPressed = 0;
	gSystem.touchHeld = 0;
	gSystem.touchAutoSampling = FALSE;
	
	TP_Init();
	
	TPCalibrateParam touchCalibration;
	if (TP_GetUserInfo(&touchCalibration) == TRUE) {
		TP_SetCalibrateParam(&touchCalibration);
	} else {
		touchCalibration.x0 = 0x02AE;
		touchCalibration.y0 = 0x058C;
		touchCalibration.xDotSize = 0x0E25;
		touchCalibration.yDotSize = 0x1208;
		TP_SetCalibrateParam(&touchCalibration);
	}
}


void SetGBACartridgeVersion(int version) {
	gSystem.gbaCartridgeVersion = version;
}


void SleepLock(u8 mask) {
	gSystem.inhibitSleep |= mask;
}


void SleepUnlock(u8 mask) {
	gSystem.inhibitSleep &= ~(mask);
}


void ReadKeypadAndTouchpad(void) {
	TPData tpRaw;
	TPData tp;
	u32 padRead;
	
	if (PAD_DetectFold()) {
		// Can't press any buttons while the lid is closed.
		gSystem.pressedKeys = 0;
		gSystem.heldKeys = 0;
		gSystem.pressedKeysRepeatable = 0;
		gSystem.touchPressed = 0;
		gSystem.touchHeld = 0;
		return;
	}
	
	padRead = PAD_Read();
	
	gSystem.pressedKeysRaw = padRead & (padRead ^ gSystem.heldKeysRaw);
	gSystem.pressedKeysRepeatableRaw = gSystem.pressedKeysRaw;
	
	if (padRead != 0 && gSystem.heldKeysRaw == padRead) {
		if (--gSystem.autorepeatTimer == 0) {
			gSystem.pressedKeysRepeatableRaw = padRead;
			gSystem.autorepeatTimer = gSystem.autorepeatRate;
		}
	} else {
		gSystem.autorepeatTimer = gSystem.autorepeatDelay;
	}
	
	gSystem.heldKeysRaw = padRead;
	
	gSystem.pressedKeys = gSystem.pressedKeysRaw;
	gSystem.heldKeys = gSystem.heldKeysRaw;
	gSystem.pressedKeysRepeatable = gSystem.pressedKeysRepeatableRaw;
	
	ApplyButtonModeToInput();
	
	if (gSystem.touchAutoSampling == FALSE) {
		while (TP_RequestRawSampling(&tpRaw) != FALSE)
			;
	} else {
		TP_GetLatestRawPointInAuto(&tpRaw);
	}
	
	TP_GetCalibratedPoint(&tp, &tpRaw);
	
	if (tp.validity == TP_VALIDITY_VALID) {
		gSystem.touchX = tp.x;
		gSystem.touchY = tp.y;
	} else {
		if (gSystem.touchHeld) {
			switch (tp.validity) {
				case TP_VALIDITY_INVALID_X:
					gSystem.touchY = tp.y;
					break;
				
				case TP_VALIDITY_INVALID_Y:
					gSystem.touchX = tp.x;
					break;
				
				case TP_VALIDITY_INVALID_XY:
					break;
				
				default:
					break;
				
			}
		} else {
			tp.touch = 0;
		}
	}

	gSystem.touchPressed = (u16)(tp.touch & (tp.touch ^ gSystem.touchHeld));
	gSystem.touchHeld = tp.touch;
}


#define CONVERT_KEY(member, convertFrom, convertTo) do {  \
    if (member & convertFrom) { member |= convertTo; }    \
} while (FALSE)

#define SWAP_KEY(member, swapA, swapB) do {  \
    u32 tmp = 0;                             \
    if (member & swapA) {                    \
        tmp |= swapB;                        \
    }                                        \
    if (member & swapB) {                    \
        tmp |= swapA;                        \
    }                                        \
    member &= ((swapA | swapB) ^ 0xFFFF);    \
    member |= tmp;                           \
} while (FALSE)

#define CLEAR_KEY(member, key) do {  \
    member &= (key ^ 0xFFFF);        \
} while (FALSE)

static void ApplyButtonModeToInput(void) {
	switch (gSystem.buttonMode) {
	default:
	case BUTTON_MODE_NORMAL:
		break;
	
	case BUTTON_MODE_START_IS_X:
		CONVERT_KEY(gSystem.pressedKeys, PAD_BUTTON_START, PAD_BUTTON_X);
		CONVERT_KEY(gSystem.heldKeys, PAD_BUTTON_START, PAD_BUTTON_X);
		CONVERT_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_START, PAD_BUTTON_X);
		break;
	
	case BUTTON_MODE_SWAP_XY:
		SWAP_KEY(gSystem.pressedKeys, PAD_BUTTON_X, PAD_BUTTON_Y);
		SWAP_KEY(gSystem.heldKeys, PAD_BUTTON_X, PAD_BUTTON_Y);
		SWAP_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_X, PAD_BUTTON_Y);
		break;
	
	case BUTTON_MODE_L_IS_A:
		CONVERT_KEY(gSystem.pressedKeys, PAD_BUTTON_L, PAD_BUTTON_A);
		CONVERT_KEY(gSystem.heldKeys, PAD_BUTTON_L, PAD_BUTTON_A);
		CONVERT_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_L, PAD_BUTTON_A);
		CLEAR_KEY(gSystem.pressedKeys, (PAD_BUTTON_L | PAD_BUTTON_R));
		CLEAR_KEY(gSystem.heldKeys, (PAD_BUTTON_L | PAD_BUTTON_R));
		CLEAR_KEY(gSystem.pressedKeysRepeatable, (PAD_BUTTON_L | PAD_BUTTON_R));
		break;
	}
}


void SetAutorepeat(int rate, int delay) {
	gSystem.autorepeatRate = rate;
	gSystem.autorepeatDelay = delay;
}
