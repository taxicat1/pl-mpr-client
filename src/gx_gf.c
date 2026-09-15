#include <nitro/types.h>
#include <nitro/gx.h>
#include <string.h>

#include "gx_gf.h"

typedef struct {
	BOOL          swapRequested;
	GXSortMode    am;
	GXBufferMode  zm;
} GFGXContext;

static GFGXContext sGFGXContext;


void GFGX_Init(void) {
	memset(&sGFGXContext, 0, sizeof(GFGXContext));
	sGFGXContext.swapRequested = FALSE;
}


void GFGX_ResetG3X(void) {
	G3X_Reset();
}


void GFGX_SwapBuffers(void) {
	if (sGFGXContext.swapRequested) {
		// Unreachable
		G3_SwapBuffers(sGFGXContext.am, sGFGXContext.zm);
		sGFGXContext.swapRequested = FALSE;
	}
}
