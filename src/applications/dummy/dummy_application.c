#include <nitro/os.h>

#include "applications/dummy/dummy_application.h"

#include "overlay_manager.h"

ApplicationManagerTemplate gDummyApplication = {
	.init      = NULL,
	.main      = NULL,
	.exit      = NULL,
	.overlayID = 0xBEEFCAFE
};


int DummyFunc_0200E19C(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}


int DummyFunc_0200E1A4(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}


int DummyFunc_0200E1AC(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}


int DummyFunc_0200E1B4(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}


int DummyFunc_0200E1BC(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}


int DummyFunc_0200E1C4(ApplicationManager* appMan, int* state) {
	#pragma unused(appMan)
	#pragma unused(state)
	OS_Halt();
}
