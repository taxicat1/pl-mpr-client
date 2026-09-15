#include <nitro.h>

#include "error_handling.h"

#include "fatal_error.h"

void ErrorHandling_AssertFail(void) {
	FatalError_PrintMessageAndShutdown();
}
