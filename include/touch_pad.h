#ifndef MPR_TOUCH_PAD_H
#define MPR_TOUCH_PAD_H

#include <nitro.h>

#define MAX_TOUCH_BUFFER_SIZE  8

typedef struct {
	u16     bufferSize;
	TPData  buffer[MAX_TOUCH_BUFFER_SIZE];
} TouchPadDataBuffer;

typedef enum {
	AUTO_SAMPLING_OPERATION_RESULT_BUFFER_INDEX_OUT_OF_RANGE = -1,
	AUTO_SAMPLING_OPERATION_RESULT_SUCCESS = 1,
	AUTO_SAMPLING_OPERATION_RESULT_FAILURE,
	AUTO_SAMPLING_OPERATION_RESULT_NO_DATA_WRITTEN
} AutoSamplingOperationResult;

void EnableTouchPad(void);
BOOL InitializeTouchPad(u32 autoSamplingFrequency);
u32 DisableTouchPad(void);

#endif /* MPR_TOUCH_PAD_H */