#include <nitro.h>
#include <string.h>

#include "screen_fade.h"

#include "constants/graphics.h"
#include "constants/heap.h"

#include "assert.h"
#include "brightness_controller.h"
#include "hardware_window.h"
#include "heap.h"
#include "screen_fade_funcs.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "system.h"

#define LOCAL_TASK_PRIORIITY  1024

typedef enum {
	ORDER_SIMULTANEOUS,
	ORDER_MAIN_FIRST,
	ORDER_SUB_FIRST
} ScreenFadeOrder;

typedef struct {
	ScreenFadeHBlanks*  hblanks;
	void*               data;
	Callback            callback;
	DSScreen            screen;
} EnableHBlankTemplate;

typedef struct {
	ScreenFadeHBlanks*  hblanks;
	DSScreen            screen;
} DisableHBlankTemplate;

typedef struct {
	ScreenFadeOrder order;
	
	BOOL  activeMain;
	BOOL  activeSub;
	BOOL  existsMain;
	BOOL  existsSub;
} ScreenFadeParams;

typedef struct {
	ScreenFadeParams        screen;
	ScreenFade              mainScreenFade;
	ScreenFade              subScreenFade;
	ScreenFadeHBlanks       hblanks;
	HardwareWindowSettings  hwSettings;
	
	u16  active;
	u8   dummy_14E;
	u8   dummy_14F;
	u16  savedColor;
} ScreenFadeManager;

static const ScreenFadeFunc sScreenFadeFuncs[FADE_TYPE_MAX] = {
	ScreenFadeFunc_BrightnessOut,
	ScreenFadeFunc_BrightnessIn,
	ScreenFadeFunc_DownwardOut,
	ScreenFadeFunc_DownwardIn,
	ScreenFadeFunc_UpwardOut,
	ScreenFadeFunc_Unused5,
	ScreenFadeFunc_Unused6,
	ScreenFadeFunc_Unused7,
	ScreenFadeFunc_Unused8,
	ScreenFadeFunc_Unused9,
	ScreenFadeFunc_Unused10,
	ScreenFadeFunc_Unused11,
	ScreenFadeFunc_Unused12,
	ScreenFadeFunc_Unused13,
	ScreenFadeFunc_Unused14,
	ScreenFadeFunc_Unused15,
	ScreenFadeFunc_CircleOut,
	ScreenFadeFunc_CircleIn,
	ScreenFadeFunc_TopHalfCircleOut,
	ScreenFadeFunc_TopHalfCircleIn,
	ScreenFadeFunc_Unused20,
	ScreenFadeFunc_Unused21,
	ScreenFadeFunc_Unused22,
	ScreenFadeFunc_Unused23,
	ScreenFadeFunc_Unused24,
	ScreenFadeFunc_Unused25,
	ScreenFadeFunc_Unused26,
	ScreenFadeFunc_Unused27,
	ScreenFadeFunc_Unused28,
	ScreenFadeFunc_Unused29,
	ScreenFadeFunc_Unused30,
	ScreenFadeFunc_Unused31,
	ScreenFadeFunc_Unused32,
	ScreenFadeFunc_Unused33,
	ScreenFadeFunc_Unused34,
	ScreenFadeFunc_Unused35,
	ScreenFadeFunc_Unused36,
	ScreenFadeFunc_Unused37,
	ScreenFadeFunc_Unused38,
	ScreenFadeFunc_Unused39,
	ScreenFadeFunc_ClampOut,
	ScreenFadeFunc_ClampIn
};

static ScreenFadeManager sScreenFadeManager;

static void ResetScreenFadeManager(ScreenFadeManager* manager);
static BOOL TryScreenFade(ScreenFadeParams* screen, ScreenFade* fadeMain, ScreenFade* fadeSub);
static void TryScreenFadeFunc(BOOL* running, ScreenFade* fade);
static BOOL CallScreenFadeFunc(ScreenFade* fade);
static void SetupScreenFadeParams(FadeMode mode, ScreenFadeParams* params);
static void InitScreenFadeParams(ScreenFadeParams* params, ScreenFadeOrder order, BOOL fadeMain, BOOL fadeSub);
static void InitScreenFade(
	ScreenFade* fade,
	FadeType type,
	int steps,
	int framesPerStep,
	FadeState state,
	void* data,
	DSScreen screen,
	HardwareWindowSettings* hwSettings,
	ScreenFadeHBlanks* hblanks,
	HeapID heapID,
	u16 color);
static void ClearHBlanks(ScreenFadeHBlanks* hblanks);
static void RunHBlankCallbacks(void* data);
static void EnableScreenHBlank(ScreenFadeHBlanks* hblanks, void* data, Callback callback, DSScreen screen);
static void DisableScreenHBlank(ScreenFadeHBlanks* hblanks, DSScreen screen);
static void Task_EnableScreenHBlank(SysTask* task, void* data);
static void Task_DisableScreenHBlank(SysTask* task, void* data);
static void DummyHBlankCallback(void* data);
static u16 GetFadeColor(ScreenFadeManager* manager, u16 color);
static u16 GetSavedFadeColor(const ScreenFadeManager* manager);
static void Task_ResetScreenMasterBrightness(SysTask* task, void* data);
static void RequestResetScreenMasterBrightness(ScreenFade* fade);
static void ResetWindowScreenFade(ScreenFade* fade);
static void ZeroScreenFadeManager(ScreenFadeManager* manager);

void StartScreenFade(FadeMode mode, FadeType typeMain, FadeType typeSub, u16 color, int steps, int framesPerStep, HeapID heapID) {
	GF_ASSERT(steps);
	GF_ASSERT(framesPerStep);
	GF_ASSERT(sScreenFadeManager.active == FALSE);

	ScreenFadeManager* manager = &sScreenFadeManager;
	ZeroScreenFadeManager(manager);
	SetupScreenFadeParams(mode, &manager->screen);
	ClearHBlanks(&manager->hblanks);

	u16 fadeColor = GetFadeColor(manager, color);
	InitScreenFade(&manager->mainScreenFade, typeMain, steps, framesPerStep, FADE_IDLE, NULL, DS_SCREEN_MAIN, &manager->hwSettings, &manager->hblanks, heapID, fadeColor);
	InitScreenFade(&manager->subScreenFade, typeSub, steps, framesPerStep, FADE_IDLE, NULL, DS_SCREEN_SUB, &manager->hwSettings, &manager->hblanks, heapID, fadeColor);

	manager->active = TRUE;
	TryScreenFadeFunc(&manager->screen.activeMain, &manager->mainScreenFade);
	TryScreenFadeFunc(&manager->screen.activeSub, &manager->subScreenFade);

	if (manager->screen.existsMain) {
		RequestResetScreenMasterBrightness(&manager->mainScreenFade);
		manager->dummy_14E = 1;
	}

	if (manager->screen.existsSub) {
		RequestResetScreenMasterBrightness(&manager->subScreenFade);
		manager->dummy_14F = 1;
	}
}


void ExecScreenFade(void) {
	ScreenFadeManager* manager = &sScreenFadeManager;
	if (manager->active && TryScreenFade(&manager->screen, &manager->mainScreenFade, &manager->subScreenFade) == TRUE) {
		ResetScreenFadeManager(manager);
	}
}


BOOL IsScreenFadeDone(void) {
	return !sScreenFadeManager.active;
}


void ResetVisibleHardwareWindows(DSScreen screen) {
	SetVisibleHardwareWindows(GX_WNDMASK_NONE, screen);
}


void ResetScreenMasterBrightness(DSScreen screen) {
	SetScreenMasterBrightness(screen, 0);
}


void SetScreenColorBrightness(DSScreen screen, u16 color) {
	int brightness;
	if (color == COLOR_WHITE) {
		brightness = BRIGHTNESS_WHITE;
	} else {
		brightness = BRIGHTNESS_BLACK;
	}

	SetScreenMasterBrightness(screen, brightness);
}


void SetScreenBackgroundColor(u16 color) {
	GX_LoadBGPltt((void*)&color, 0, sizeof(u16));
	GXS_LoadBGPltt((void*)&color, 0, sizeof(u16));
}


void SetScreenMasterBrightness(DSScreen screen, int brightness) {
	if (screen == DS_SCREEN_MAIN) {
		GX_SetMasterBrightness(brightness);
	} else {
		GXS_SetMasterBrightness(brightness);
	}
}


static void ResetScreenFadeManager(ScreenFadeManager* manager) {
	manager->active = FALSE;
	manager->savedColor = GetSavedFadeColor(manager);

	if (manager->screen.existsMain) {
		ResetWindowScreenFade(&manager->mainScreenFade);
		if (manager->mainScreenFade.direction == FADE_IN) {
			sScreenFadeManager.dummy_14E = 0;
		}
	}

	if (manager->screen.existsSub) {
		ResetWindowScreenFade(&manager->subScreenFade);
		if (manager->mainScreenFade.direction == FADE_IN) {
			sScreenFadeManager.dummy_14F = 0;
		}
	}

	ZeroScreenFadeManager(manager);
}


static BOOL TryScreenFade(ScreenFadeParams* screen, ScreenFade* fadeMain, ScreenFade* fadeSub) {
	switch (screen->order) {
	case ORDER_SIMULTANEOUS:
		TryScreenFadeFunc(&screen->activeMain, fadeMain);
		TryScreenFadeFunc(&screen->activeSub, fadeSub);
		break;

	case ORDER_MAIN_FIRST:
		if (screen->activeMain) {
			TryScreenFadeFunc(&screen->activeMain, fadeMain);
		} else {
			TryScreenFadeFunc(&screen->activeSub, fadeSub);
		}
		break;

	case ORDER_SUB_FIRST:
		if (screen->activeSub) {
			TryScreenFadeFunc(&screen->activeSub, fadeSub);
		} else {
			TryScreenFadeFunc(&screen->activeMain, fadeMain);
		}
		break;
	}

	if (screen->activeMain == FALSE && screen->activeSub == FALSE) {
		return TRUE;
	}

	return FALSE;
}


static void TryScreenFadeFunc(BOOL* running, ScreenFade* fade) {
	if (*running && CallScreenFadeFunc(fade) == TRUE) {
		*running = FALSE;
	}
}


static BOOL CallScreenFadeFunc(ScreenFade* fade) {
	return sScreenFadeFuncs[fade->type](fade);
}


static void SetupScreenFadeParams(FadeMode mode, ScreenFadeParams* params) {
	switch (mode) {
	case FADE_BOTH_SCREENS:
		InitScreenFadeParams(params, ORDER_SIMULTANEOUS, TRUE, TRUE);
		break;

	case FADE_MAIN_THEN_SUB:
		InitScreenFadeParams(params, ORDER_MAIN_FIRST, TRUE, TRUE);
		break;

	case FADE_SUB_THEN_MAIN:
		InitScreenFadeParams(params, ORDER_SUB_FIRST, TRUE, TRUE);
		break;

	case FADE_MAIN_ONLY:
		InitScreenFadeParams(params, ORDER_MAIN_FIRST, TRUE, FALSE);
		break;

	case FADE_SUB_ONLY:
		InitScreenFadeParams(params, ORDER_SUB_FIRST, FALSE, TRUE);
		break;
	}
}


static void InitScreenFadeParams(ScreenFadeParams* params, ScreenFadeOrder order, BOOL fadeMain, BOOL fadeSub) {
	params->order = order;
	params->activeMain = fadeMain;
	params->activeSub = fadeSub;
	params->existsMain = fadeMain;
	params->existsSub = fadeSub;
}


static void InitScreenFade(
	ScreenFade* fade,
	FadeType type,
	int steps,
	int framesPerStep,
	FadeState state,
	void* data,
	DSScreen screen,
	HardwareWindowSettings* hwSettings,
	ScreenFadeHBlanks* hblanks,
	HeapID heapID,
	u16 color
) {
	fade->type = type;
	fade->steps = steps;
	fade->framesPerStep = framesPerStep;
	fade->state = state;
	fade->data = data;
	fade->screen = screen;
	fade->hwSettings = hwSettings;
	fade->hblanks = hblanks;
	fade->heapID = heapID;
	fade->color = color;
}


static void ClearHBlanks(ScreenFadeHBlanks* hblanks) {
	for (int screen = 0; screen < DS_SCREEN_MAX; screen++) {
		hblanks->data[screen] = NULL;
		hblanks->callback[screen] = DummyHBlankCallback;
		hblanks->running[screen] = FALSE;
	}
}


static void RunHBlankCallbacks(void* data) {
	ScreenFadeHBlanks* hblanks = data;
	for (int screen = 0; screen < DS_SCREEN_MAX; screen++) {
		hblanks->callback[screen](hblanks->data[screen]);
	}
}


static void EnableScreenHBlank(ScreenFadeHBlanks* hblanks, void* data, Callback callback, DSScreen screen) {
	u8 validCallback = TRUE;
	GF_ASSERT(hblanks->running[screen] == FALSE);
	GF_ASSERT(hblanks->callback[screen] != NULL);

	if (hblanks->running[DS_SCREEN_MAIN] == FALSE && hblanks->running[DS_SCREEN_SUB] == FALSE) {
		validCallback = SetHBlankCallback(RunHBlankCallbacks, hblanks);
	}

	GF_ASSERT(validCallback == TRUE);

	hblanks->data[screen] = data;

	if (callback) {
		hblanks->callback[screen] = callback;
	} else {
		hblanks->callback[screen] = DummyHBlankCallback;
	}

	hblanks->running[screen] = TRUE;
}


static void DisableScreenHBlank(ScreenFadeHBlanks* hblanks, DSScreen screen) {
	hblanks->running[screen] = FALSE;

	if (hblanks->running[DS_SCREEN_MAIN] == FALSE && hblanks->running[DS_SCREEN_SUB] == FALSE) {
		DisableHBlank();
	}

	hblanks->callback[screen] = DummyHBlankCallback;
	hblanks->data[screen] = NULL;
}


void RequestEnableScreenHBlank(ScreenFadeHBlanks* hblanks, void* data, Callback callback, DSScreen screen, HeapID heapID) {
	EnableHBlankTemplate* template = Heap_AllocAtEnd(heapID, sizeof(EnableHBlankTemplate));
	template->hblanks = hblanks;
	template->data = data;
	template->callback = callback;
	template->screen = screen;

	SysTask_ExecuteAfterVBlank(Task_EnableScreenHBlank, template, LOCAL_TASK_PRIORIITY);
}


void RequestDisableScreenHBlank(ScreenFadeHBlanks* hblanks, DSScreen screen, HeapID heapID) {
	DisableHBlankTemplate* template = Heap_AllocAtEnd(heapID, sizeof(DisableHBlankTemplate));
	template->hblanks = hblanks;
	template->screen = screen;

	SysTask_ExecuteAfterVBlank(Task_DisableScreenHBlank, template, LOCAL_TASK_PRIORIITY);
}


static void Task_EnableScreenHBlank(SysTask* task, void* data) {
	EnableHBlankTemplate* template = data;
	EnableScreenHBlank(template->hblanks, template->data, template->callback, template->screen);
	SysTask_Done(task);
	Heap_Free(data);
}


static void Task_DisableScreenHBlank(SysTask* task, void* data) {
	DisableHBlankTemplate* template = data;
	DisableScreenHBlank(template->hblanks, template->screen);
	SysTask_Done(task);
	Heap_Free(data);
}


static void DummyHBlankCallback(void* data) {
	return;
}


static u16 GetFadeColor(ScreenFadeManager* manager, u16 color) {
	if (color == COLOR_SAVED) {
		return manager->savedColor;
	}

	return color;
}


static u16 GetSavedFadeColor(const ScreenFadeManager* manager) {
	const ScreenFade* fade;
	if (manager->screen.existsMain == TRUE) {
		fade = &manager->mainScreenFade;
	} else {
		fade = &manager->subScreenFade;
	}

	if (fade->direction == FADE_OUT) {
		return fade->color;
	}

	return manager->savedColor;
}


static void Task_ResetScreenMasterBrightness(SysTask* task, void* data) {
	ScreenFade* fade = data;
	SetScreenMasterBrightness(fade->screen, 0);
	SysTask_Done(task);
}


static void RequestResetScreenMasterBrightness(ScreenFade* fade) {
	if (fade->direction == FADE_IN
		&& (fade->color == COLOR_WHITE || fade->color == COLOR_BLACK)
		&& fade->method == FADE_BY_WINDOW) {
		SysTask_ExecuteAfterVBlank(Task_ResetScreenMasterBrightness, fade, LOCAL_TASK_PRIORIITY);
	}
}


static void ResetWindowScreenFade(ScreenFade* fade) {
	if (fade->direction == FADE_OUT
		&& (fade->color == COLOR_WHITE || fade->color == COLOR_BLACK)
		&& fade->method == FADE_BY_WINDOW) {
		SetScreenColorBrightness(fade->screen, fade->color);
		ResetVisibleHardwareWindows(fade->screen);
	}
}


static void ZeroScreenFadeManager(ScreenFadeManager* manager) {
	memset(&manager->screen,         0, sizeof(ScreenFadeParams));
	memset(&manager->mainScreenFade, 0, sizeof(ScreenFade));
	memset(&manager->subScreenFade,  0, sizeof(ScreenFade));
	memset(&manager->hblanks,        0, sizeof(ScreenFadeHBlanks));
	memset(&manager->hwSettings,     0, sizeof(HardwareWindowSettings));
}
