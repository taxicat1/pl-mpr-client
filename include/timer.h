#ifndef MPR_TIMER_H
#define MPR_TIMER_H

#include <nitro/types.h>

void Timer_Start(void);
u64 Timer_GetCurrentTimestamp(void);
u64 Timer_TicksToSeconds(u64 ticks);

#endif /* MPR_TIMER_H */
