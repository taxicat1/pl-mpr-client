#include <nitro.h>
#include <string.h>

#include "brightness_controller.h"

#include "constants/graphics.h"

#define IS_SCREEN_SELECTED(screenMask, screenConst) (screenMask & screenConst)

typedef struct {
	int  planeMask;
	u32  screenSelect;
	u16  stepCount;
	s16  targetBrightness;
	u8   padding[4];
} TransitionData;

typedef struct {
    TransitionData  data;
    s8              direction;
    s16             brightnessDelta;
    s16             stepDelta;
    u16             stepRemainder;
    u16             accumulator;
    s16             brightness;
    BOOL            isActive;
} TransitionController;

static TransitionController controllerMain;  // Top Screen
static TransitionController controllerSub;   // Bottom Screen


static void BrightnessController_StepTransition(TransitionController* controller) {
	BOOL transitionFinished = FALSE;
	TransitionData* data = &controller->data;
	
	if (controller->brightness + controller->direction * controller->stepDelta != data->targetBrightness && controller->brightness != data->targetBrightness) {
		controller->brightness += controller->direction * controller->stepDelta;
		controller->accumulator += controller->stepRemainder;
		
		if (controller->accumulator >= data->stepCount) {
			controller->brightness += controller->direction;
			
			if (controller->brightness != data->targetBrightness) {
				controller->accumulator -= data->stepCount;
			} else {
				transitionFinished = TRUE;
			}
		}
	} else {
		controller->brightness = data->targetBrightness;
		transitionFinished = TRUE;
	}
	
	if (IS_SCREEN_SELECTED(data->screenSelect, BRIGHTNESS_MAIN_SCREEN)) {
		G2_SetBlendBrightness(data->planeMask, controller->brightness);
	} else if (IS_SCREEN_SELECTED(data->screenSelect, BRIGHTNESS_SUB_SCREEN)) {
		G2S_SetBlendBrightness(data->planeMask, controller->brightness);
	}
	
	if (transitionFinished == TRUE) {
		controller->isActive = FALSE;
	}
}


void BrightnessController_SetScreenBrightness(s16 brightness, int planeMask, u32 screenSelect) {
	if (IS_SCREEN_SELECTED(screenSelect, BRIGHTNESS_MAIN_SCREEN)) {
		G2_SetBlendBrightness(planeMask, brightness);
	}
	
	if (IS_SCREEN_SELECTED(screenSelect, BRIGHTNESS_SUB_SCREEN)) {
		G2S_SetBlendBrightness(planeMask, brightness);
	}
	
	BrightnessController_ResetScreenController(screenSelect);
}


void BrightnessController_ResetAllControllers(void) {
	MI_CpuClear8(&controllerMain, sizeof(TransitionController));
	MI_CpuClear8(&controllerSub, sizeof(TransitionController));
	
	controllerMain.isActive = FALSE;
	controllerSub.isActive = FALSE;
}


void BrightnessController_ResetScreenController(u32 screenSelect) {
	if (IS_SCREEN_SELECTED(screenSelect, BRIGHTNESS_MAIN_SCREEN)) {
		MI_CpuClear8(&controllerMain, sizeof(TransitionController));
		controllerMain.isActive = FALSE;
	}
	
	if (IS_SCREEN_SELECTED(screenSelect, BRIGHTNESS_SUB_SCREEN)) {
		MI_CpuClear8(&controllerSub, sizeof(TransitionController));
		controllerSub.isActive = FALSE;
	}
}


void BrightnessController_Update(void) {
	if (controllerMain.isActive) {
		BrightnessController_StepTransition(&controllerMain);
	}
	
	if (controllerSub.isActive) {
		BrightnessController_StepTransition(&controllerSub);
	}
}
