#include <nitro.h>
#include <string.h>

#include "overlay_manager.h"

#include "game_overlay.h"
#include "heap.h"


ApplicationManager* ApplicationManager_New(const ApplicationManagerTemplate* manTemplate, void* args, const HeapID heapID) {
	ApplicationManager* appMan = Heap_Alloc(heapID, sizeof(ApplicationManager));
	
	appMan->manTemplate = *manTemplate;
	appMan->execState   = OVERLAY_EXEC_LOAD;
	appMan->procState   = 0;
	appMan->args        = args;
	appMan->data        = NULL;
	appMan->parent      = NULL;
	appMan->child       = NULL;
	
	return appMan;
}


void ApplicationManager_Free(ApplicationManager* appMan) {
	Heap_Free(appMan);
}


void* ApplicationManager_NewData(ApplicationManager* appMan, u32 size, HeapID heapID) {
	appMan->data = Heap_Alloc(heapID, size);
	return appMan->data;
}


void* ApplicationManager_GetData(ApplicationManager* appMan) {
	return appMan->data;
}


void ApplicationManager_FreeData(ApplicationManager* appMan) {
	Heap_Free(appMan->data);
	appMan->data = NULL;
}


void* ApplicationManager_GetArgs(ApplicationManager* appMan) {
	return appMan->args;
}


BOOL ApplicationManager_Exec(ApplicationManager* appMan) {
	switch (appMan->execState) {
		case OVERLAY_EXEC_LOAD:
			if (appMan->manTemplate.overlayID != FS_OVERLAY_ID_NONE) {
				Overlay_LoadByID(appMan->manTemplate.overlayID, OVERLAY_LOAD_ASYNC);
			}
			
			appMan->execState = OVERLAY_EXEC_INIT;
		
		case OVERLAY_EXEC_INIT:
			if (appMan->manTemplate.init(appMan, &appMan->procState) == TRUE) {
				appMan->execState = OVERLAY_EXEC_MAIN;
				appMan->procState = 0;
			}
			break;
		
		case OVERLAY_EXEC_MAIN:
			if (appMan->manTemplate.main(appMan, &appMan->procState) == TRUE) {
				appMan->execState = OVERLAY_EXEC_EXIT;
				appMan->procState = 0;
			}
			break;
		
		case OVERLAY_EXEC_EXIT:
			if (appMan->manTemplate.exit(appMan, &appMan->procState) == TRUE) {
				if (appMan->manTemplate.overlayID != FS_OVERLAY_ID_NONE) {
					Overlay_UnloadByID(appMan->manTemplate.overlayID);
				}
				
				return TRUE;
			}
			break;
	}
	
	return FALSE;
}
