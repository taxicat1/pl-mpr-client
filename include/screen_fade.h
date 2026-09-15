#ifndef MPR_SCREEN_FADE_H
#define MPR_SCREEN_FADE_H

#include "constants/graphics.h"
#include "constants/heap.h"

#include "hardware_window.h"
#include "system.h"

typedef enum {
	FADE_TYPE_BRIGHTNESS_OUT      =  0,
	FADE_TYPE_BRIGHTNESS_IN       =  1,
	FADE_TYPE_DOWNWARD_OUT        =  2,
	FADE_TYPE_DOWNWARD_IN         =  3,
	FADE_TYPE_UPWARD_OUT          =  4,
	FADE_TYPE_UNUSED_5            =  5,
	FADE_TYPE_UNUSED_6            =  6,
	FADE_TYPE_UNUSED_7            =  7,
	FADE_TYPE_UNUSED_8            =  8,
	FADE_TYPE_UNUSED_9            =  9,
	FADE_TYPE_UNUSED_10           = 10,
	FADE_TYPE_UNUSED_11           = 11,
	FADE_TYPE_UNUSED_12           = 12,
	FADE_TYPE_UNUSED_13           = 13,
	FADE_TYPE_UNUSED_14           = 14,
	FADE_TYPE_UNUSED_15           = 15,
	FADE_TYPE_CIRCLE_OUT          = 16,
	FADE_TYPE_CIRCLE_IN           = 17,
	FADE_TYPE_TOP_HALF_CIRCLE_OUT = 18,
	FADE_TYPE_TOP_HALF_CIRCLE_IN  = 19,
	FADE_TYPE_UNUSED_20           = 20,
	FADE_TYPE_UNUSED_21           = 21,
	FADE_TYPE_UNUSED_22           = 22,
	FADE_TYPE_UNUSED_23           = 23,
	FADE_TYPE_UNUSED_24           = 24,
	FADE_TYPE_UNUSED_25           = 25,
	FADE_TYPE_UNUSED_26           = 26,
	FADE_TYPE_UNUSED_27           = 27,
	FADE_TYPE_UNUSED_28           = 28,
	FADE_TYPE_UNUSED_29           = 29,
	FADE_TYPE_UNUSED_30           = 30,
	FADE_TYPE_UNUSED_31           = 31,
	FADE_TYPE_UNUSED_32           = 32,
	FADE_TYPE_UNUSED_33           = 33,
	FADE_TYPE_UNUSED_34           = 34,
	FADE_TYPE_UNUSED_35           = 35,
	FADE_TYPE_UNUSED_36           = 36,
	FADE_TYPE_UNUSED_37           = 37,
	FADE_TYPE_UNUSED_38           = 38,
	FADE_TYPE_UNUSED_39           = 39,
	FADE_TYPE_CLAMP_OUT           = 40,
	FADE_TYPE_CLAMP_IN            = 41,
	FADE_TYPE_MAX                 = 42
} FadeType;

typedef enum {
	FADE_BOTH_SCREENS = 0,
	FADE_MAIN_THEN_SUB,
	FADE_SUB_THEN_MAIN,
	FADE_MAIN_ONLY,
	FADE_SUB_ONLY
} FadeMode;

typedef enum {
	FADE_IN = 0,
	FADE_OUT
} FadeDirection;

typedef enum {
	FADE_BY_WINDOW = 0,
	FADE_BY_BRIGHTNESS
} FadeMethod;

typedef enum {
	FADE_IDLE,
	FADE_ACTIVE,
	FADE_CLEANUP,
	FADE_DONE
} FadeState;

typedef struct {
	void*     data[DS_SCREEN_MAX];
	Callback  callback[DS_SCREEN_MAX];
	BOOL      running[DS_SCREEN_MAX];
} ScreenFadeHBlanks;

typedef struct {
	FadeType                 type;
	int                      steps;
	int                      framesPerStep;
	FadeState                state;
	DSScreen                 screen;
	void*                    data;
	HardwareWindowSettings*  hwSettings;
	ScreenFadeHBlanks*       hblanks;
	HeapID                   heapID;
	u16                      color;
	FadeDirection            direction;
	FadeMethod               method;
} ScreenFade;

void StartScreenFade(FadeMode mode, FadeType typeMain, FadeType typeSub, u16 color, int steps, int framesPerStep, HeapID heapID);
void ExecScreenFade(void);
BOOL IsScreenFadeDone(void);
void ResetVisibleHardwareWindows(DSScreen screen);
void ResetScreenMasterBrightness(DSScreen screen);
void SetScreenColorBrightness(DSScreen screen, u16 color);
void SetScreenBackgroundColor(u16 color);
void SetScreenMasterBrightness(DSScreen screen, int brightness);
void RequestEnableScreenHBlank(ScreenFadeHBlanks* hblanks, void* data, Callback callback, DSScreen screen, HeapID heapID);
void RequestDisableScreenHBlank(ScreenFadeHBlanks* hblanks, DSScreen screen, HeapID heapID);

#endif /* MPR_SCREEN_FADE_H */
