#ifndef MPR_MPR_CLIENT_MPR_SAVE_H
#define MPR_MPR_CLIENT_MPR_SAVE_H

#include <nitro/types.h>

#include "bg_window.h"
#include "overlay_manager.h"

typedef enum {
	SAVEAPP_CANCELLED,
	SAVEAPP_SAVED,
	SAVEAPP_COMM_ERROR,
	SAVEAPP_POWER_OFF
} SaveAppReturn;

typedef struct {
	BgConfig*      bgConfig;
	SaveData*      saveData;
    BOOL           dataChanged;
    BOOL           shutDownAfterSave;
    SaveAppReturn  returnValue;
} MPRSaveAppArgs;

BOOL MPRSaveApp_Init(ApplicationManager* appMan, int* state);
BOOL MPRSaveApp_Main(ApplicationManager* appMan, int* state);
BOOL MPRSaveApp_Exit(ApplicationManager* appMan, int* state);
void MPRSave_InitThread(SaveData* saveData);

#endif /* MPR_MPR_CLIENT_MPR_SAVE_H */
