#include <nitro.h>
#include <string.h>

#include "pal_pad.h"

#include "constants/charcode.h"


int PalPad_GetSaveSize(void) {
	return sizeof(PalPad) * PAL_PAD_ENTRIES;
}


void PalPad_Init(PalPad* palPad) {
	for (int i = 0; i < PAL_PAD_ENTRIES; i++) {
		memset(&palPad[i], 0, sizeof(PalPad));
		palPad[i].trainerName[0] = CHAR_EOS;
	}
}
