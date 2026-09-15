#include <nitro.h>
#include <string.h>

#include "touch_pad.h"

#include "assert.h"
#include "system.h"

#define AUTO_SAMPLING_NUM_FRAMES_TO_STORE     2
#define AUTO_SAMPLING_BUFFER_MAX_SIZE         (SPI_TP_SAMPLING_FREQUENCY_MAX * AUTO_SAMPLING_NUM_FRAMES_TO_STORE) + 1 // + 1 for the active sample
#define AUTO_SAMPLING_FREQUENCY_LIMIT         SPI_TP_SAMPLING_FREQUENCY_MAX + 1
#define AUTO_SAMPLING_NUM_ERRORS_BEFORE_FAIL  5

typedef enum {
	TOUCH_PAD_MODE_INACTIVE = 0,
	TOUCH_PAD_MODE_ACTIVE_WITH_EXTERNAL_BUFFER,
	TOUCH_PAD_MODE_ACTIVE
} TouchPadMode;

typedef struct {
	TPData*  externalBuffer;
	u32      externalBufferSize;
	u32      autoSamplingBufferSize;
	TPData   autoSamplingBuffer[AUTO_SAMPLING_BUFFER_MAX_SIZE];
	u32      currentExternalBufferIndex;
	u16      touchPadMode;
	u16      touchPadDisabled;
} TouchPadState;

static TouchPadState sTouchPadState;

static void UpdateTouchScreenState(
	TouchPadMode touchPadMode,
	BOOL autoSamplingEnabled,
	void* externalBuffer,
	u32 externalBufferSize,
	u32 currentExternalBufferIndex,
	u32 autoSamplingBufferSize
);
static void ResetTouchPadData(TPData* touchPadDataBuffer, int touchPadDataBufferSize);
static AutoSamplingOperationResult StopAutoSampling(void);
static AutoSamplingOperationResult StartAutoSampling(u32 frequency);


void EnableTouchPad(void) {
	GF_ASSERT(sTouchPadState.touchPadDisabled == FALSE);
	
	sTouchPadState.touchPadMode = TOUCH_PAD_MODE_INACTIVE;
	sTouchPadState.touchPadDisabled = FALSE;
}


BOOL InitializeTouchPad(u32 autoSamplingFrequency) {
	GF_ASSERT(sTouchPadState.touchPadDisabled == FALSE);
	
	if (autoSamplingFrequency >= AUTO_SAMPLING_FREQUENCY_LIMIT || autoSamplingFrequency <= 0) {
		return FALSE;
	}
	
	if (sTouchPadState.touchPadMode != TOUCH_PAD_MODE_INACTIVE) {
		return FALSE;
	}
	
	AutoSamplingOperationResult autoSamplingResult = StartAutoSampling(autoSamplingFrequency);
	
	if (autoSamplingResult != AUTO_SAMPLING_OPERATION_RESULT_SUCCESS) {
		return autoSamplingResult;
	}
	
	UpdateTouchScreenState(TOUCH_PAD_MODE_ACTIVE, TRUE, NULL, 0, 0, autoSamplingFrequency * AUTO_SAMPLING_NUM_FRAMES_TO_STORE);
	
	return TRUE;
}


static void UpdateTouchScreenState(
	TouchPadMode touchPadMode,
	BOOL autoSamplingEnabled,
	void* externalBuffer,
	u32 externalBufferSize,
	u32 currentExternalBufferIndex,
	u32 autoSamplingBufferSize
) {
	sTouchPadState.touchPadMode = touchPadMode;
	gSystem.touchAutoSampling = autoSamplingEnabled;
	sTouchPadState.externalBuffer = externalBuffer;
	sTouchPadState.externalBufferSize = externalBufferSize;
	sTouchPadState.autoSamplingBufferSize = autoSamplingBufferSize;
	sTouchPadState.currentExternalBufferIndex = currentExternalBufferIndex;
	
	ResetTouchPadData(sTouchPadState.autoSamplingBuffer, AUTO_SAMPLING_BUFFER_MAX_SIZE);
}


static void ResetTouchPadData(TPData* touchPadDataBuffer, int touchPadDataBufferSize) {
	int i;
	
	for (i = 0; i < touchPadDataBufferSize; i++) {
		touchPadDataBuffer[i].touch = TP_TOUCH_OFF;
	}
}


static AutoSamplingOperationResult StopAutoSampling(void) {
	u32 errorCount = 0;
	BOOL hasError;
	
	if (sTouchPadState.touchPadMode == TOUCH_PAD_MODE_INACTIVE) {
		return AUTO_SAMPLING_OPERATION_RESULT_SUCCESS;
	}
	
	do {
		TP_RequestAutoSamplingStopAsync();
		TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
		
		hasError = TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
		
		if (hasError != FALSE) {
			errorCount++;
		}
	} while ((hasError != FALSE) && (errorCount <= AUTO_SAMPLING_NUM_ERRORS_BEFORE_FAIL));
	
	if (errorCount > AUTO_SAMPLING_NUM_ERRORS_BEFORE_FAIL) {
		return AUTO_SAMPLING_OPERATION_RESULT_FAILURE;
	}
	
	return AUTO_SAMPLING_OPERATION_RESULT_SUCCESS;
}


u32 DisableTouchPad(void) {
	AutoSamplingOperationResult autoSamplingResult;
	
	GF_ASSERT(sTouchPadState.touchPadDisabled == FALSE);
	
	autoSamplingResult = StopAutoSampling();
	
	if (autoSamplingResult == AUTO_SAMPLING_OPERATION_RESULT_SUCCESS) {
		UpdateTouchScreenState(TOUCH_PAD_MODE_INACTIVE, FALSE, NULL, 0, 0, 0);
	}
	
	return autoSamplingResult;
}


static AutoSamplingOperationResult StartAutoSampling(u32 frequency) {
	int errorCount = 0;
	BOOL hasError;
	
	do {
		TP_RequestAutoSamplingStartAsync(0, frequency, sTouchPadState.autoSamplingBuffer, AUTO_SAMPLING_BUFFER_MAX_SIZE);
		TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
		
		hasError = TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
		
		if (hasError != FALSE) {
			errorCount++;
		}
	} while ((hasError != FALSE) && (errorCount <= AUTO_SAMPLING_NUM_ERRORS_BEFORE_FAIL));
	
	if (errorCount > AUTO_SAMPLING_NUM_ERRORS_BEFORE_FAIL) {
		return AUTO_SAMPLING_OPERATION_RESULT_FAILURE;
	}
	
	return AUTO_SAMPLING_OPERATION_RESULT_SUCCESS;
}








































