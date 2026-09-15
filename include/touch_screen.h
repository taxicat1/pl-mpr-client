#ifndef MPR_TOUCH_SCREEN_H
#define MPR_TOUCH_SCREEN_H

#define TOUCHSCREEN_TABLE_TERMINATOR  0xFF
#define TOUCHSCREEN_USE_CIRCLE        0xFE
#define TOUCHSCREEN_INPUT_NONE        -1

typedef enum {
	TOUCH_BUTTON_NOT_PRESSED = -1,
	TOUCH_BUTTON_PRESSED,
	TOUCH_BUTTON_RELEASED,
	TOUCH_BUTTON_HELD,
	TOUCH_BUTTON_HELD_OUT_OF_BOUNDS,
	TOUCH_BUTTON_INVALID
} TouchScreenButtonState;

typedef union {
	struct {
		u8  top;
		u8  bottom;
		u8  left;
		u8  right;
	}  rect;
	
	struct {
		u8  code;
		u8  x;
		u8  y;
		u8  r;
	}  circle;
} TouchScreenRect, TouchScreenHitTable;

int TouchScreen_CheckRectangleHeld(const TouchScreenRect rects[]);
int TouchScreen_CheckRectanglePressed(const TouchScreenRect rects[]);
BOOL TouchScreen_LocationHeld(const TouchScreenHitTable* hitTable);
BOOL TouchScreen_LocationPressed(const TouchScreenHitTable* hitTable);
BOOL TouchScreen_IsTouched(void);
BOOL TouchScreen_IsTapped(void);
BOOL TouchScreen_GetHoldState(u32* x, u32* y);

#endif /* MPR_TOUCH_SCREEN_H */
