#ifndef MPR_MPR_CLIENT_MAIN_H
#define MPR_MPR_CLIENT_MAIN_H

#include <nitro/types.h>

#include "overlay_manager.h"
#include "savedata.h"

typedef enum {
	RESET_CLEAN = 0,
	RESET_ERROR
} OSResetParameter;

typedef struct {
	int        unk_00;
	BOOL       error;
	SaveData*  saveData;
} ApplicationArgs;

void EnqueueApplication(FSOverlayID overlayID, const ApplicationManagerTemplate* appManTemplate);
void InitRNG(void);
void HandleConsoleFold(void);

#endif /* MPR_MPR_CLIENT_MAIN_H */
