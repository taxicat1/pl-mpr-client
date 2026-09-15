#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_TOUCH_DIAL_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_TOUCH_DIAL_H

#include <nitro/types.h>

#include "touch_screen.h"

typedef struct TouchDial TouchDial;
typedef int (*TouchDialActionFunc)(TouchDial*, BOOL, BOOL);

struct TouchDial {
	u32   unused;
	u32   initialTouchX;
	u32   initialTouchY;
	u32   circleX;
	u32   circleY;
	u32   currentTouchX;
	u32   currentTouchY;
	BOOL  touchInBounds;
	u16   initialTouchAngle;
	u8    padding_22[2];
	int   buttonDirection;
	
	TouchScreenHitTable  innerCircle;
	TouchScreenHitTable  outerCircle;
	
	TouchDialActionFunc action;
};

typedef enum {
	TOUCH_DIAL_NO_TOUCH,
	TOUCH_DIAL_INITIAL_TOUCH,
	TOUCH_DIAL_SCROLLING,
	TOUCH_DIAL_END_SCROLL
} TouchDialState;

void TouchDial_Init(TouchDial* touchDial, int circleX, int circleY, int innerRadius, int outerRadius);
int TouchDial_HandleAction(TouchDial* touchDial);
int TouchDial_CalcScrollAmount(const TouchDial* touchDial);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_TOUCH_DIAL_H */