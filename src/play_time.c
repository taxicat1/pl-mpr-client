#include <nitro.h>

#include "play_time.h"


void PlayTime_Init(PlayTime* playTime) {
	playTime->hours   = 0;
	playTime->minutes = 0;
	playTime->seconds = 0;
}


void PlayTime_Increment(PlayTime* playTime, u32 seconds) {
	if (
		playTime->hours   == PLAYTIME_MAX_HOURS   &&
		playTime->minutes == PLAYTIME_MAX_MINUTES &&
		playTime->seconds == PLAYTIME_MAX_SECONDS
	) {
		return;
	}
	
	u32 hours      = playTime->hours;
	u32 minutes    = playTime->minutes;
	u32 newSeconds = playTime->seconds + seconds;
	
	if (newSeconds > PLAYTIME_MAX_SECONDS) {
		minutes = playTime->minutes + newSeconds / SECONDS_PER_MINUTE;
		newSeconds %= SECONDS_PER_MINUTE;
		
		if (minutes > PLAYTIME_MAX_MINUTES) {
			hours = playTime->hours + minutes / MINUTES_PER_HOUR;
			minutes %= MINUTES_PER_HOUR;
			
			if (hours >= PLAYTIME_MAX_HOURS) {
				hours = PLAYTIME_MAX_HOURS;
				minutes = PLAYTIME_MAX_MINUTES;
				newSeconds = PLAYTIME_MAX_SECONDS;
			}
		}
	}
	
	playTime->hours = hours;
	playTime->minutes = minutes;
	playTime->seconds = newSeconds;
}
