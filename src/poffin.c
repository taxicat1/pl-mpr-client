#include <nitro.h>

#include "poffin.h"

#define POFFIN_TYPE_NONE  30


void Poffin_Clear(Poffin* poffin) {
	poffin->type       = POFFIN_TYPE_NONE;
	poffin->spiciness  = 0;
	poffin->dryness    = 0;
	poffin->sweetness  = 0;
	poffin->bitterness = 0;
	poffin->sourness   = 0;
	poffin->smoothness = 0;
	poffin->dummy      = 0;
}


void Poffin_StoreAttributesToArray(Poffin* poffin, u8* dest) {
	dest[0] = poffin->type;
	dest[1] = poffin->spiciness;
	dest[2] = poffin->dryness;
	dest[3] = poffin->sweetness;
	dest[4] = poffin->bitterness;
	dest[5] = poffin->sourness;
	dest[6] = poffin->smoothness;
}


int PoffinCase_GetSaveSize(void) {
	return sizeof(PoffinCase);
}


void PoffinCase_Init(PoffinCase* poffinCase) {
	for (int i = 0; i < MAX_POFFINS; i++) {
		Poffin_Clear(&poffinCase->slot[i]);
	}
}
