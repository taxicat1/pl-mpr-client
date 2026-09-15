#include <nitro.h>

#include "hardware_window.h"

#include "constants/graphics.h"
#include "sys_task.h"
#include "sys_task_manager.h"

#define LOCAL_TASK_PRIORITY 1

static void Task_SetVisibleHardwareWindows(SysTask* task, void* data);
static void Task_SetHardwareWindowMaskInsidePlane(SysTask* task, void* data);
static void Task_SetHardwareWindowMaskOutsidePlane(SysTask* task, void* data);
static void Task_SetHardwareWindowDimensions(SysTask* task, void* data);


void SetVisibleHardwareWindows(GXWndMask windowMask, DSScreen screen) {
	if (screen == DS_SCREEN_MAIN) {
		GX_SetVisibleWnd(windowMask);
	} else {
		GXS_SetVisibleWnd(windowMask);
	}
}


void SetHardwareWindowMaskInsidePlane(int wnd, BOOL applyColorEffect, HardwareWindow windowID, DSScreen screen) {
	if (windowID == HW_WINDOW_WND0) {
		if (screen == DS_SCREEN_MAIN) {
			G2_SetWnd0InsidePlane(wnd, applyColorEffect);
		} else {
			G2S_SetWnd0InsidePlane(wnd, applyColorEffect);
		}
	} else {
		if (screen == DS_SCREEN_MAIN) {
			G2_SetWnd1InsidePlane(wnd, applyColorEffect);
		} else {
			G2S_SetWnd1InsidePlane(wnd, applyColorEffect);
		}
	}
}


void SetHardwareWindowMaskOutsidePlane(int wnd, BOOL applyColorEffect, DSScreen screen) {
	if (screen == DS_SCREEN_MAIN) {
		G2_SetWndOutsidePlane(wnd, applyColorEffect);
	} else {
		G2S_SetWndOutsidePlane(wnd, applyColorEffect);
	}
}


void SetHardwareWindowDimensions(int left, int top, int right, int bottom, HardwareWindow windowID, DSScreen screen) {
	if (windowID == HW_WINDOW_WND0) {
		if (screen == DS_SCREEN_MAIN) {
			G2_SetWnd0Position(left, top, right, bottom);
		} else {
			G2S_SetWnd0Position(left, top, right, bottom);
		}
	} else {
		if (screen == DS_SCREEN_MAIN) {
			G2_SetWnd1Position(left, top, right, bottom);
		} else {
			G2S_SetWnd1Position(left, top, right, bottom);
		}
	}
}


void RequestVisibleHardwareWindows(HardwareWindowSettings* settings, GXWndMask windowMask, DSScreen screen) {
	HardwareWindowVisibility* visible = &settings->visible[screen];
	visible->windowMask = windowMask;
	visible->screen = screen;
	
	SysTask_ExecuteAfterVBlank(Task_SetVisibleHardwareWindows, visible, LOCAL_TASK_PRIORITY);
}


void RequestHardwareWindowMaskInsidePlane(HardwareWindowSettings* settings, int wnd, BOOL applyColorEffect, HardwareWindow windowID, DSScreen screen) {
	HardwareWindowInsidePlane* inside = &settings->inside[screen][windowID];
	inside->wnd = wnd;
	inside->applyColorEffect = applyColorEffect;
	inside->windowID = windowID;
	inside->screen = screen;
	
	SysTask_ExecuteAfterVBlank(Task_SetHardwareWindowMaskInsidePlane, inside, LOCAL_TASK_PRIORITY);
}


void RequestHardwareWindowMaskOutsidePlane(HardwareWindowSettings* settings, int wnd, BOOL applyColorEffect, DSScreen screen) {
	HardwareWindowOutsidePlane* outside = &settings->outside[screen];
	outside->wnd = wnd;
	outside->applyColorEffect = applyColorEffect;
	outside->screen = screen;
	
	SysTask_ExecuteAfterVBlank(Task_SetHardwareWindowMaskOutsidePlane, outside, LOCAL_TASK_PRIORITY);
}


void RequestHardwareWindowDimensions(HardwareWindowSettings* settings, int left, int top, int right, int bottom, HardwareWindow windowID, DSScreen screen) {
	HardwareWindowDimensions* dimensions = &settings->dimensions[screen][windowID];
	dimensions->left = left;
	dimensions->top = top;
	dimensions->right = right;
	dimensions->bottom = bottom;
	dimensions->windowID = windowID;
	dimensions->screen = screen;
	
	SysTask_ExecuteAfterVBlank(Task_SetHardwareWindowDimensions, dimensions, LOCAL_TASK_PRIORITY);
}


static void Task_SetVisibleHardwareWindows(SysTask* task, void* data) {
	HardwareWindowVisibility* visible = data;
	SetVisibleHardwareWindows(visible->windowMask, visible->screen);
	SysTask_Done(task);
}


static void Task_SetHardwareWindowMaskInsidePlane(SysTask* task, void* data) {
	HardwareWindowInsidePlane* inside = data;
	SetHardwareWindowMaskInsidePlane(inside->wnd, inside->applyColorEffect, inside->windowID, inside->screen);
	SysTask_Done(task);
}


static void Task_SetHardwareWindowMaskOutsidePlane(SysTask* task, void* data) {
	HardwareWindowOutsidePlane* outside = data;
	SetHardwareWindowMaskOutsidePlane(outside->wnd, outside->applyColorEffect, outside->screen);
	SysTask_Done(task);
}


static void Task_SetHardwareWindowDimensions(SysTask* task, void* data) {
	HardwareWindowDimensions* dimensions = data;
	SetHardwareWindowDimensions(
		dimensions->left,
		dimensions->top,
		dimensions->right,
		dimensions->bottom,
		dimensions->windowID,
		dimensions->screen);
	SysTask_Done(task);
}

