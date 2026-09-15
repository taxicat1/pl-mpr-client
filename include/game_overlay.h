#ifndef MPR_GAME_OVERLAY_H
#define MPR_GAME_OVERLAY_H

#include <nitro/fs.h>

typedef enum {
	OVERLAY_LOAD_NORMAL,
	OVERLAY_LOAD_NOINIT,
	OVERLAY_LOAD_ASYNC
} OverlayLoadType;

void Overlay_UnloadByID(const FSOverlayID overlayID);
int Overlay_GetLoadDestination(const FSOverlayID overlayID);
BOOL Overlay_LoadByID(const FSOverlayID overlayID, OverlayLoadType loadType);

#endif /* MPR_GAME_OVERLAY_H */
