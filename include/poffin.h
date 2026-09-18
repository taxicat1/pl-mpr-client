#ifndef MPR_POFFIN_H
#define MPR_POFFIN_H

#include "constants/heap.h"
#include "savedata.h"

#define MAX_POFFINS  100

typedef struct {
	union {
		struct {
			u8 type;
			u8 spiciness;
			u8 dryness;
			u8 sweetness;
			u8 bitterness;
			u8 sourness;
			u8 smoothness;
			u8 dummy;
		};
		u8 attributes[8];
	};
} Poffin;

typedef struct {
	Poffin slot[MAX_POFFINS];
} PoffinCase;

void Poffin_Clear(Poffin* poffin);
void Poffin_StoreAttributesToArray(Poffin* poffin, u8* dest);
int PoffinCase_GetSaveSize(void);
void PoffinCase_Init(PoffinCase* poffinCase);

#endif /* MPR_POFFIN_H */
