#ifndef MPR_TYPE_ICON_H
#define MPR_TYPE_ICON_H

#include <nitro/types.h>

#include "constants/move_classes.h"
#include "constants/pokemon_types.h"

#include "narc.h"

u32 TypeIcon_GetChar(PokemonType moveType);
u32 TypeIcon_GetPlttSrc(void);
u32 TypeIcon_GetCell(void);
u8 TypeIcon_GetPltt(PokemonType moveType);
NarcID TypeIcon_GetNARC(void);
u32 CategoryIcon_GetChar(MoveClass moveCat);
u8 CategoryIcon_GetPltt(MoveClass moveCat);
NarcID CategoryIcon_GetNARC(void);

#endif /* MPR_TYPE_ICON_H */
