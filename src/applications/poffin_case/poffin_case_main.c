#include <nitro.h>

#include "applications/poffin_case/poffin_case_main.h"

#include "pokemon.h"
#include "applications/dummy/dummy_application.h"
#include "constants/pokemon.h"
#include "constants/natures.h"
#include "constants/flavors.h"

typedef struct {
	u8  likedFlavor;
	u8  dislikedFlavor;
} FlavorPreference;

static const FlavorPreference sFlavorPreferences[] = {
	[NATURE_HARDY]   = { FLAVOR_NONE,   FLAVOR_NONE   },
	[NATURE_LONELY]  = { FLAVOR_SPICY,  FLAVOR_SOUR   },
	[NATURE_BRAVE]   = { FLAVOR_SPICY,  FLAVOR_SWEET  },
	[NATURE_ADAMANT] = { FLAVOR_SPICY,  FLAVOR_DRY    },
	[NATURE_NAUGHTY] = { FLAVOR_SPICY,  FLAVOR_BITTER },
	[NATURE_BOLD]    = { FLAVOR_SOUR,   FLAVOR_SPICY  },
	[NATURE_DOCILE]  = { FLAVOR_NONE,   FLAVOR_NONE   },
	[NATURE_RELAXED] = { FLAVOR_SOUR,   FLAVOR_SWEET  },
	[NATURE_IMPISH]  = { FLAVOR_SOUR,   FLAVOR_DRY    },
	[NATURE_LAX]     = { FLAVOR_SOUR,   FLAVOR_BITTER },
	[NATURE_TIMID]   = { FLAVOR_SWEET,  FLAVOR_SPICY  },
	[NATURE_HASTY]   = { FLAVOR_SWEET,  FLAVOR_SOUR   },
	[NATURE_SERIOUS] = { FLAVOR_NONE,   FLAVOR_NONE   },
	[NATURE_JOLLY]   = { FLAVOR_SWEET,  FLAVOR_DRY    },
	[NATURE_NAIVE]   = { FLAVOR_SWEET,  FLAVOR_BITTER },
	[NATURE_MODEST]  = { FLAVOR_DRY,    FLAVOR_SPICY  },
	[NATURE_MILD]    = { FLAVOR_DRY,    FLAVOR_SOUR   },
	[NATURE_QUIET]   = { FLAVOR_DRY,    FLAVOR_SWEET  },
	[NATURE_BASHFUL] = { FLAVOR_NONE,   FLAVOR_NONE   },
	[NATURE_RASH]    = { FLAVOR_DRY,    FLAVOR_BITTER },
	[NATURE_CALM]    = { FLAVOR_BITTER, FLAVOR_SPICY  },
	[NATURE_GENTLE]  = { FLAVOR_BITTER, FLAVOR_SOUR   },
	[NATURE_SASSY]   = { FLAVOR_BITTER, FLAVOR_SWEET  },
	[NATURE_CAREFUL] = { FLAVOR_BITTER, FLAVOR_DRY    },
	[NATURE_QUIRKY]  = { FLAVOR_NONE,   FLAVOR_NONE   }
};


void PoffinCase_UpdateMonContestStats(Poffin* poffin, Pokemon* mon) {
	u8 i;
	u8 nature = Pokemon_GetNature(mon);
	
	u8 likedFlavor    = sFlavorPreferences[nature].likedFlavor;
	u8 dislikedFlavor = sFlavorPreferences[nature].dislikedFlavor;
	
	u8 poffinAttrs[7];
	Poffin_StoreAttributesToArray(poffin, poffinAttrs);
	
	int monContestStats[6];
	for (i = 0; i < 6; i++) {
		monContestStats[i] = Pokemon_GetValue(mon, MON_DATA_COOL + i, NULL);
	}
	
	u8 adjustedPoffinAttrs[7];
	u8 j = 0;
	for (i = 1; i <= 6; i++) {
		adjustedPoffinAttrs[j++] = poffinAttrs[i];
	}
	
	if (likedFlavor != FLAVOR_NONE) {
		float modifiedAttr = adjustedPoffinAttrs[likedFlavor] * 1.1F;
		adjustedPoffinAttrs[likedFlavor] = modifiedAttr;
		
		modifiedAttr = adjustedPoffinAttrs[dislikedFlavor] * 0.9F;
		adjustedPoffinAttrs[dislikedFlavor] = modifiedAttr;
	}
	
	for (i = 0; i < 6; i++) {
		monContestStats[i] += adjustedPoffinAttrs[i];
		if (monContestStats[i] > MAX_CONTEST_STAT) {
			monContestStats[i] = MAX_CONTEST_STAT;
		}
		
		Pokemon_SetValue(mon, MON_DATA_COOL + i, &monContestStats[i]);
	}
	
	u8 friendship = Pokemon_GetValue(mon, MON_DATA_FRIENDSHIP, NULL);
	if (friendship < MAX_FRIENDSHIP_VALUE) {
		friendship++;
		Pokemon_SetValue(mon, MON_DATA_FRIENDSHIP, &friendship);
	}
}


// Remnant unused application template, replaced with dummy functions
void Dummy_Unused_02061F60(void);
void Dummy_Unused_02061F60(void) {
	static const ApplicationManagerTemplate Dummy_02061F60 = {
		.init      = DummyFunc_0200E1A4,
		.main      = DummyFunc_0200E1AC,
		.exit      = DummyFunc_0200E19C,
		.overlayID = FS_OVERLAY_ID_NONE
	};
}


// Remnant unused application template, replaced with dummy functions
void Dummy_Unused_02061F70(void);
void Dummy_Unused_02061F70(void) {
	static const ApplicationManagerTemplate Dummy_02061F70 = {
		.init      = DummyFunc_0200E1BC,
		.main      = DummyFunc_0200E1C4,
		.exit      = DummyFunc_0200E1B4,
		.overlayID = FS_OVERLAY_ID_NONE
	};
}
