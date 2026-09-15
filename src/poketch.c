#include <nitro.h>

#include "poketch.h"

#include "assert.h"
#include "game_version.h"

static const PoketchMapPosition sDefaultMapMarkers[POKETCH_MAPMARKER_COUNT] = {
    { 104, 152 },
    { 120, 152 },
    { 136, 152 },
    { 152, 152 },
    { 168, 152 },
    { 184, 152 }
};


int Poketch_GetSaveSize(void) {
	if (gIsDiamondPearl) {
		return sizeof(PoketchDP);
	} else {
		return sizeof(PoketchPt);
	}
}


void Poketch_Init(PoketchCommon* poketch) {
	int i;
	for (i = 0; i < POKETCH_REGISTRY_SIZE; i++) {
		poketch->appRegistry[i] = 0;
	}
	
	poketch->appCount           = 0;
	poketch->appIndex           = 0;
	poketch->poketchEnabled     = 0;
	poketch->screenColor        = 0;
	poketch->pedometerEnabled   = 0;
	poketch->stepCount          = 0;
	poketch->alarmSet           = 0;
	poketch->alarmHour          = 0;
	poketch->alarmMinute        = 0;
	poketch->calendarMonth      = 1;
	poketch->calendarMarkBitmap = 0;
	
	for (i = 0; i < POKETCH_MAPMARKER_COUNT; i++) {
		poketch->markMapPositions[i].x = sDefaultMapMarkers[i].x;
		poketch->markMapPositions[i].y = sDefaultMapMarkers[i].y;
	}
	
	// BUG: DP assumed (doesn't matter since it's zeroes anyway)
	PoketchDP* poketchDP = (PoketchDP*)poketch;
	for (i = 0; i < POKETCH_POKEMONHISTORY_MAX; i++) {
		poketchDP->pokemonHistoryQueue[i].species = 0;
		poketchDP->pokemonHistoryQueue[i].icon = 0;
	}
	
	poketch->dotArtModifiedByPlayer = FALSE;
	Poketch_RegisterApp(poketch, POKETCH_APPID_DIGITALWATCH);
}


BOOL Poketch_RegisterApp(PoketchCommon* poketch, PoketchAppID appID) {
	GF_ASSERT(appID >= 0 && appID < POKETCH_APPID_MAX);
	
	if (poketch->appCount < POKETCH_APPID_MAX && poketch->appRegistry[appID] == 0) {
		poketch->appRegistry[appID] = 1;
		poketch->appCount++;
		
		if (appID == POKETCH_APPID_PEDOMETER) {
			poketch->pedometerEnabled = 1;
		}
		
		return TRUE;
	}
	
	return FALSE;
}
