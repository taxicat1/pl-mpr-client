#include <nitro.h>

#include "underground.h"

#include "system.h"
#include "rtc.h"


int Underground_GetSaveSize(void) {
	return sizeof(Underground);
}


void Underground_Init(Underground* underground) {
	u32 seed = 0;
	
	RTCDate date;
	RTCTime time;
	GetCurrentDateTime(&date, &time);
	
	// "ULL" (u64 type) required to match
	seed = (((((((((((u32)date.year * 32ULL) + date.month) * 32ULL) + date.day) * 32ULL) + time.hour) * 32ULL) + time.minute) * 32ULL) + (time.second + gSystem.vblankCounter));
	
	MI_CpuFill8(underground, 0, sizeof(Underground));
	
	underground->randomSeed = seed;
	underground->shouldSpawnNewBuriedObjects = TRUE;
}
