#ifndef MPR_APPLICATIONS_APPLICATION_UTIL_H
#define MPR_APPLICATIONS_APPLICATION_UTIL_H

#include <nitro/types.h>

#include "constants/heap.h"

#include "bg_window.h"
#include "narc.h"

typedef enum {
	BARCOLOR_EMPTY = 0,
	BARCOLOR_RED,
	BARCOLOR_YELLOW,
	BARCOLOR_GREEN,
	BARCOLOR_MAX
} BarColor;

/**
 * @brief Determine how many pixels are needed to represent a fractional value.
 *
 * @param cur       The current value; the fraction's numerator.
 * @param max       The maximum value; the fraction's denominator.
 * @param maxPixels How many pixels would be used to display max / max.
 * @return The number of pixels needed to display cur / max.
 */
u8 App_PixelCount(u32 cur, u32 max, u8 maxPixels);

/**
 * @brief Determine what color should be used for a value represented by a
 * visual bar, e.g. the health bar in battle.
 *
 * @param cur   The current value of the bar.
 * @param max   The maximum value of the bar.
 * @return The color to be used for the bar's current value.
 */
u8 App_CalcBarColor(u32 cur, u32 max);

/**
 * @brief Determine the color of the health bar.
 *
 * @param curHP     The current HP value.
 * @param maxHP     The maximum HP value.
 * @param barSize   The size of the health bar, in pixels.
 * @return The color to be used for the health bar's current value.
 */
u8 HealthBar_CalcColor(u16 curHP, u16 maxHP, u32 barSize);

void App_StartScreenFade(u8 fadeOut, HeapID heapID);

#endif /* MPR_APPLICATIONS_APPLICATION_UTIL_H */
