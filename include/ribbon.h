#ifndef MPR_RIBBON_H
#define MPR_RIBBON_H

#include <nitro/types.h>

#include "special_ribbons.h"
#include "constants/ribbons.h"

typedef enum {
	RIBBON_DATA_MON_DATA_PARAM = 0,
	RIBBON_DATA_SPRITE_ID,
	RIBBON_DATA_PALETTE_NUM,
	RIBBON_DATA_NAME_ID,
	RIBBON_DATA_DESCRIPTION_ID
} RibbonDataField;

u32 Ribbon_GetData(u8 ribbonID, RibbonDataField field);
u32 Ribbon_GetDescriptionID(SpecialRibbonsSave* specialRibbons, u8 ribbonID);

#endif /* MPR_RIBBON_H */
