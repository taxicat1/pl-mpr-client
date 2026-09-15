#ifndef MPR_HARDWARE_WINDOW_H
#define MPR_HARDWARE_WINDOW_H

#include <nitro/gx/gx.h>

#include "constants/graphics.h"

typedef enum {
	HW_WINDOW_WND0 = 0,
	HW_WINDOW_WND1,
	HW_WINDOW_MAX,
} HardwareWindow;

typedef struct {
	GXWndMask  windowMask;
	DSScreen   screen;
} HardwareWindowVisibility;

typedef struct {
	int             wnd;
	BOOL            applyColorEffect;
	HardwareWindow  windowID;
	DSScreen        screen;
} HardwareWindowInsidePlane;

typedef struct {
	int       wnd;
	BOOL      applyColorEffect;
	DSScreen  screen;
} HardwareWindowOutsidePlane;

typedef struct {
	s16             left;
	s16             top;
	s16             right;
	s16             bottom;
	HardwareWindow  windowID;
	DSScreen        screen;
} HardwareWindowDimensions;

typedef struct {
	HardwareWindowVisibility   visible[DS_SCREEN_MAX];
	HardwareWindowInsidePlane  inside[DS_SCREEN_MAX][HW_WINDOW_MAX];
	
	u8 padding_50[24];
	
	HardwareWindowOutsidePlane  outside[DS_SCREEN_MAX];
	HardwareWindowDimensions    dimensions[DS_SCREEN_MAX][HW_WINDOW_MAX];
} HardwareWindowSettings;

/*
 * `window` is treated as a mask which will specify the visible hardware windows. The SDK defines
 * bit 0 as the on-off for WND0, bit 1 as the on-off for WND1, and bit 2 as the on-off for the OBJ
 * window.
 */
void SetVisibleHardwareWindows(GXWndMask window, DSScreen screen);

/*
 * For both functions below, `wnd` should be a 5-bit mask of which graphics layers should be visible
 * within the window. Each bit determines if that layer is displayed as part of that window. A layer
 * is only visible if its bit within a bounding window and the global DISPCNT register are set to 1.
 *
 * 00000
 * ^^^^^
 * ||||`--> BG layer 0
 * |||`---> BG layer 1
 * ||`----> BG layer 2
 * |`-----> BG layer 3
 * `------> Object layer
 *
 * The "inside" section of the visible plane is defined as the set of pixels which are contained
 * within the bounding box of *any* visible window. The "outside" section is those which are not.
 */
void SetHardwareWindowMaskInsidePlane(int wnd, BOOL applyColorEffect, HardwareWindow windowID, DSScreen screen);
void SetHardwareWindowMaskOutsidePlane(int wnd, BOOL applyColorEffect, DSScreen screen);

/*
 * Hardware windows are defined by a bounding-box defined by pixel-positions of their horizontal and
 * vertical edges. Note that despite this function accepting `int` values, the hardware only supports
 * 8-bit values for each coordinate. This partially relates to the size of the LCDs on the physical
 * machine, which are each 256 pixels wide and 192 pixels tall.
 */
void SetHardwareWindowDimensions(int left, int top, int right, int bottom, HardwareWindow windowID, DSScreen screen);

// Request a lazy-execution of SetVisibleHardwareWindows at the next VBlank.
void RequestVisibleHardwareWindows(HardwareWindowSettings* settings, GXWndMask windowMask, DSScreen screen);

// Request a lazy-execution of SetHardwareWindowMaskInsidePlane at the next VBlank.
void RequestHardwareWindowMaskInsidePlane(HardwareWindowSettings* settings, int wnd, BOOL applyColorEffect, HardwareWindow windowID, DSScreen screen);

// Request a lazy-execution of SetHardwareWindowMaskOutsidePlane at the next VBlank.
void RequestHardwareWindowMaskOutsidePlane(HardwareWindowSettings* settings, int wnd, BOOL applyColorEffect, DSScreen screen);

// Request a lazy-execution of SetHardwareWindowDimensions at the next VBlank.
void RequestHardwareWindowDimensions(HardwareWindowSettings* settings, int left, int top, int right, int bottom, HardwareWindow windowID, DSScreen screen);

#endif /* MPR_HARDWARE_WINDOW_H */
