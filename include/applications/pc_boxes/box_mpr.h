#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_MPR_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_MPR_H

#include "applications/pc_boxes/box_def.h"

#include "bg_window.h"
#include "sprite.h"

extern const int* gTypeNamesInAlphabeticalOrder;
extern const int* gNatureNamesInAlphabeticalOrder;

void BoxMPR_SetLanguage(void);
void BoxMPR_020395E4(BoxApplication* boxApp, BoxMenuItem item);
BOOL BoxMPR_02039618(MPRBoxDisplay** outMprDisplay, BoxApplicationDisplay* param1, const BoxApplication* param2, BgConfig* param3, SpriteList* param4);
void BoxMPR_0203969C(MPRBoxDisplay* mprDisplay);
void BoxMPR_0203971C(MPRBoxDisplay* mprDisplay);
void BoxMPR_02039B98(MPRBoxDisplay* mprDisplay);
BOOL BoxMPR_0203A1EC(const int* buckets, int listCount, int currentBucket);
u16 BoxMPR_AdvanceSpeciesIndex(u16 speciesIndex, int amt, const u16* speciesNamesInAlphabeticalOrder);
u16 BoxMPR_AdvanceMoveIndex(u16 moveIndex, int amt);
u16 BoxMPR_AdvanceAbilityIndex(u16 abilityIndex, int amt);
TouchScreenActions* BoxMPR_0203B260(MPRFilterMenu* mprFilter);
void BoxMPR_0203B2D0(MPRBoxDisplay* mprDisplay);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_MPR_H */
