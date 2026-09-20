#include <nitro.h>

#include "system_flags.h"

static BOOL CheckFlag(VarsFlags* varsFlags, u16 flagID);


static BOOL CheckFlag(VarsFlags* varsFlags, u16 flagID) {
	return VarsFlags_CheckFlag(varsFlags, flagID);
}


BOOL SystemFlag_CheckContestHallVisited(VarsFlags* varsFlags) {
	return CheckFlag(varsFlags, FLAG_CONTEST_HALL_VISITED);
}
