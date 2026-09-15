#include <nitro.h>
#include <string.h>

#include "daycare_save.h"

#include "pokemon.h"


int Daycare_GetSaveSize(void) {
	return sizeof(Daycare);
}


void Daycare_Init(Daycare* daycare) {
	memset(daycare, 0, sizeof(Daycare));
	
	BoxPokemon_Init(&daycare->mons[0].boxMon);
	BoxPokemon_Init(&daycare->mons[1].boxMon);
	
	daycare->offspringPersonality = 0;
	daycare->stepCounter = 0;
}
