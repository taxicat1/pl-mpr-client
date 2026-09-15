#ifndef MPR_OVERLAY_MANAGER_H
#define MPR_OVERLAY_MANAGER_H

#include <nitro/fs.h>

#include "savedata.h"
#include "constants/heap.h"

#define FS_OVERLAY_ID_NONE -1

typedef struct ApplicationManager ApplicationManager;

typedef BOOL (*OverlayFunc)(ApplicationManager* appMan, int* state);

typedef struct {
	OverlayFunc        init;
	OverlayFunc        main;
	OverlayFunc        exit;
	const FSOverlayID  overlayID;
} ApplicationManagerTemplate;

typedef enum {
	OVERLAY_EXEC_LOAD = 0,
	OVERLAY_EXEC_INIT,
	OVERLAY_EXEC_MAIN,
	OVERLAY_EXEC_EXIT,
} OverlayExecState;

struct ApplicationManager {
	ApplicationManagerTemplate  manTemplate;
	OverlayExecState            execState;
	int                         procState;
	void*                       args;
	void*                       data;
	ApplicationManager*         parent;
	ApplicationManager*         child;
};

ApplicationManager* ApplicationManager_New(const ApplicationManagerTemplate* appManTemplate, void* args, const HeapID heapID);
void ApplicationManager_Free(ApplicationManager* appMan);
void* ApplicationManager_NewData(ApplicationManager* appMan, u32 size, HeapID heapID);
void* ApplicationManager_GetData(ApplicationManager* appMan);
void ApplicationManager_FreeData(ApplicationManager* appMan);
void* ApplicationManager_GetArgs(ApplicationManager* appMan);
BOOL ApplicationManager_Exec(ApplicationManager* appMan);

#endif /* MPR_OVERLAY_MANAGER_H */
