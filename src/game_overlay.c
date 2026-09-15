#include <nitro.h>
#include <string.h>

#include "game_overlay.h"

#include "assert.h"

typedef enum {
	OVERLAY_REGION_MAIN,
	OVERLAY_REGION_ITCM,
	OVERLAY_REGION_DTCM,
	OVERLAY_REGION_NUM,
} OverlayRegion;

typedef struct {
	FSOverlayID  id;
	BOOL         isActive;
} LoadedOverlay;

#define MAX_CONCURRECT_OVERLAYS_PER_REGION  8

typedef struct {
	LoadedOverlay  mainMemOverlays[MAX_CONCURRECT_OVERLAYS_PER_REGION];
	LoadedOverlay  itcmOverlays[MAX_CONCURRECT_OVERLAYS_PER_REGION];
	LoadedOverlay  dtcmOverlays[MAX_CONCURRECT_OVERLAYS_PER_REGION];
} OverlayTracker;

static void FreeOverlayAllocation(LoadedOverlay* loadedOverlays);
static BOOL CanOverlayBeLoaded(const FSOverlayID overlayID);
static LoadedOverlay* GetLoadedOverlaysInRegion(int param0);
static BOOL GetOverlayRamBounds(const FSOverlayID overlayID, u32* start, u32* end);
static BOOL LoadOverlayNormal(MIProcessor proc, FSOverlayID overlayID);
static BOOL LoadOverlayNoInit(MIProcessor proc, FSOverlayID overlayID);
static BOOL LoadOverlayNoInitAsync(MIProcessor proc, FSOverlayID overlayID);

static OverlayTracker sOverlayTracker;


static void FreeOverlayAllocation(LoadedOverlay* loadedOverlays) {
	GF_ASSERT(loadedOverlays->isActive == TRUE);
	GF_ASSERT(FS_UnloadOverlay(MI_PROCESSOR_ARM9, loadedOverlays->id) == TRUE);
	
	loadedOverlays->isActive = FALSE;
}


void Overlay_UnloadByID(const FSOverlayID overlayID) {
	LoadedOverlay* loadedOverlays = GetLoadedOverlaysInRegion(Overlay_GetLoadDestination(overlayID));
	int i;
	
	for (i = 0; i < MAX_CONCURRECT_OVERLAYS_PER_REGION; i++) {
		if (loadedOverlays[i].isActive == TRUE && loadedOverlays[i].id == overlayID) {
			FreeOverlayAllocation(&loadedOverlays[i]);
			return;
		}
	}
}


int Overlay_GetLoadDestination(const FSOverlayID overlayID) {
	FSOverlayInfo info;
	u32 addr;
	
	GF_ASSERT(FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, overlayID) == TRUE);
	addr = (u32)FS_GetOverlayAddress(&info);
	
	if ((addr <= HW_ITCM_END) && (addr >= HW_ITCM_IMAGE)) {
		return OVERLAY_REGION_ITCM;
	} else if ((addr <= HW_DTCM_END) && (addr >= HW_DTCM)) {
		return OVERLAY_REGION_DTCM;
	}
	
	return OVERLAY_REGION_MAIN;
}


BOOL Overlay_LoadByID(const FSOverlayID overlayID, OverlayLoadType loadType) {
	BOOL result;
	u32 dmaBak = FS_DMA_NOT_USE;
	int overlayRegion;
	LoadedOverlay* loadedOverlays;
	int i;
	
	if (!CanOverlayBeLoaded(overlayID)) {
		return FALSE;
	}
	
	overlayRegion = Overlay_GetLoadDestination(overlayID);
	loadedOverlays = GetLoadedOverlaysInRegion(overlayRegion);
	
	for (i = 0; i < MAX_CONCURRECT_OVERLAYS_PER_REGION; i++) {
		if (loadedOverlays[i].isActive == FALSE) {
			loadedOverlays = &loadedOverlays[i];
			loadedOverlays->isActive = TRUE;
			loadedOverlays->id = overlayID;
			break;
		}
	}
	
	if (i >= MAX_CONCURRECT_OVERLAYS_PER_REGION) {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	if (overlayRegion == OVERLAY_REGION_ITCM || overlayRegion == OVERLAY_REGION_DTCM) {
		dmaBak = FS_SetDefaultDMA(FS_DMA_NOT_USE);
	}
	
	switch (loadType) {
		case OVERLAY_LOAD_NORMAL:
			result = LoadOverlayNormal(MI_PROCESSOR_ARM9, overlayID);
			break;
		case OVERLAY_LOAD_NOINIT:
			result = LoadOverlayNoInit(MI_PROCESSOR_ARM9, overlayID);
			break;
		case OVERLAY_LOAD_ASYNC:
			result = LoadOverlayNoInitAsync(MI_PROCESSOR_ARM9, overlayID);
			break;
		default:
			GF_ASSERT(FALSE);
			return FALSE;
	}
	
	if (overlayRegion == OVERLAY_REGION_ITCM || overlayRegion == OVERLAY_REGION_DTCM) {
		FS_SetDefaultDMA(dmaBak);
	}
	
	if (result == FALSE) {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	return TRUE;
}


static BOOL CanOverlayBeLoaded(const FSOverlayID overlayID) {
	u32 myStart, myEnd, theirStart, theirEnd;
	LoadedOverlay* loadedOverlays;
	int i;
	
	if (!GetOverlayRamBounds(overlayID, &myStart, &myEnd)) {
		return FALSE;
	}
	
	loadedOverlays = GetLoadedOverlaysInRegion(Overlay_GetLoadDestination(overlayID));
	
	for (i = 0; i < MAX_CONCURRECT_OVERLAYS_PER_REGION; i++) {
		if (loadedOverlays[i].isActive == TRUE && GetOverlayRamBounds(loadedOverlays[i].id, &theirStart, &theirEnd) == TRUE) {
			if (((myStart >= theirStart) && (myStart < theirEnd)) || ((myEnd > theirStart) && (myEnd <= theirEnd)) || ((myStart <= theirStart) && (myEnd >= theirEnd))) {
				GF_ASSERT(FALSE);
				return FALSE;
			}
		}
	}
	
	return TRUE;
}


static LoadedOverlay* GetLoadedOverlaysInRegion(int region) {
	LoadedOverlay* ret;
	
	switch (region) {
		case OVERLAY_REGION_MAIN:
		default:
			ret = sOverlayTracker.mainMemOverlays;
			break;
		case OVERLAY_REGION_ITCM:
			ret = sOverlayTracker.itcmOverlays;
			break;
		case OVERLAY_REGION_DTCM:
			ret = sOverlayTracker.dtcmOverlays;
			break;
	}
	
	return ret;
}


static BOOL GetOverlayRamBounds(const FSOverlayID overlayID, u32* start, u32* end) {
	FSOverlayInfo info;
	
	if (!FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, overlayID)) {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	*start = (u32)FS_GetOverlayAddress(&info);
	*end = *start + FS_GetOverlayTotalSize(&info);
	
	return TRUE;
}


static BOOL LoadOverlayNormal(MIProcessor proc, FSOverlayID overlayID) {
	return FS_LoadOverlay(proc, overlayID);
}


static BOOL LoadOverlayNoInit(MIProcessor proc, FSOverlayID overlayID) {
	FSOverlayInfo info;
	
	if (!FS_LoadOverlayInfo(&info, proc, overlayID)) {
		return FALSE;
	}
	
	if (!FS_LoadOverlayImage(&info)) {
		return FALSE;
	}
	
	FS_StartOverlay(&info);
	return TRUE;
}


static BOOL LoadOverlayNoInitAsync(MIProcessor proc, FSOverlayID overlayID) {
	FSFile file;
	FSOverlayInfo info;
	
	if (!FS_LoadOverlayInfo(&info, proc, overlayID)) {
		return FALSE;
	}
	
	FS_InitFile(&file);
	FS_LoadOverlayImageAsync(&info, &file);
	FS_WaitAsync(&file);
	FS_CloseFile(&file);
	FS_StartOverlay(&info);
	
	return TRUE;
}
