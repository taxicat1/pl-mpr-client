#include <nitro.h>

#include "applications/application_util.h"

#include "heap.h"
#include "screen_fade.h"
#include "system.h"


u8 App_PixelCount(u32 cur, u32 max, u8 maxPixels) {
	u8 pixels = (cur * maxPixels) / max;
	if (pixels == 0 && cur > 0) {
		pixels = 1;
	}
	
	return pixels;
}


u8 App_CalcBarColor(u32 cur, u32 max) {
	cur *= 256;
	max *= 256;
	
	if (cur > (max / 2)) {
		return BARCOLOR_GREEN;
	}
	
	if (cur > (max / 5)) {
		return BARCOLOR_YELLOW;
	}
	
	if (cur > 0) {
		return BARCOLOR_RED;
	}
	
	return BARCOLOR_EMPTY;
}


u8 HealthBar_CalcColor(u16 curHP, u16 maxHP, u32 barSize) {
	if (curHP == maxHP) {
		return BARCOLOR_MAX;
	}
	
	return App_CalcBarColor(App_PixelCount(curHP, maxHP, barSize), barSize);
}


void App_StartScreenFade(u8 fadeOut, HeapID heapID) {
	if (fadeOut == FALSE) {
		StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 6, 1, heapID);
	} else {
		StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 6, 1, heapID);
	}
}
