#include <nitro.h>

#include "type_icon.h"

#include "assert.h"
#include "common.h"
#include "game_version.h"
#include "constants/narc.h"
#include "constants/move_classes.h"
#include "constants/pokemon_types.h"
#include "constants/pokemon_types.h"
#include "constants/pokemon_contest_types.h"
#include "fs/battle/graphic/batt_obj.naix"
#include "fs/battle/graphic/pl_batt_obj.naix"

ATTRIBUTE_ALIGN(4) static const u32 sMoveTypeIconIndexDP[] = {
	[TYPE_NORMAL]   = type_icons_normal_NCGR_lz,
	[TYPE_FIGHTING] = type_icons_fighting_NCGR_lz,
	[TYPE_FLYING]   = type_icons_flying_NCGR_lz,
	[TYPE_POISON]   = type_icons_poison_NCGR_lz,
	[TYPE_GROUND]   = type_icons_ground_NCGR_lz,
	[TYPE_ROCK]     = type_icons_rock_NCGR_lz,
	[TYPE_BUG]      = type_icons_bug_NCGR_lz,
	[TYPE_GHOST]    = type_icons_ghost_NCGR_lz,
	[TYPE_STEEL]    = type_icons_steel_NCGR_lz,
	[TYPE_MYSTERY]  = type_icons_mystery_NCGR_lz,
	[TYPE_FIRE]     = type_icons_fire_NCGR_lz,
	[TYPE_WATER]    = type_icons_water_NCGR_lz,
	[TYPE_GRASS]    = type_icons_grass_NCGR_lz,
	[TYPE_ELECTRIC] = type_icons_electric_NCGR_lz,
	[TYPE_PSYCHIC]  = type_icons_psychic_NCGR_lz,
	[TYPE_ICE]      = type_icons_ice_NCGR_lz,
	[TYPE_DRAGON]   = type_icons_dragon_NCGR_lz,
	[TYPE_DARK]     = type_icons_dark_NCGR_lz,
	
	[TYPE_MAX + CONTEST_TYPE_COOL]   = type_icons_cool_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_BEAUTY] = type_icons_beauty_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_CUTE]   = type_icons_cute_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_SMART]  = type_icons_smart_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_TOUGH]  = type_icons_tough_NCGR_lz
};

ATTRIBUTE_ALIGN(4) static const u32 sMoveTypeIconIndexPt[] = {
	[TYPE_NORMAL]   = pl_type_icons_normal_NCGR_lz,
	[TYPE_FIGHTING] = pl_type_icons_fighting_NCGR_lz,
	[TYPE_FLYING]   = pl_type_icons_flying_NCGR_lz,
	[TYPE_POISON]   = pl_type_icons_poison_NCGR_lz,
	[TYPE_GROUND]   = pl_type_icons_ground_NCGR_lz,
	[TYPE_ROCK]     = pl_type_icons_rock_NCGR_lz,
	[TYPE_BUG]      = pl_type_icons_bug_NCGR_lz,
	[TYPE_GHOST]    = pl_type_icons_ghost_NCGR_lz,
	[TYPE_STEEL]    = pl_type_icons_steel_NCGR_lz,
	[TYPE_MYSTERY]  = pl_type_icons_mystery_NCGR_lz,
	[TYPE_FIRE]     = pl_type_icons_fire_NCGR_lz,
	[TYPE_WATER]    = pl_type_icons_water_NCGR_lz,
	[TYPE_GRASS]    = pl_type_icons_grass_NCGR_lz,
	[TYPE_ELECTRIC] = pl_type_icons_electric_NCGR_lz,
	[TYPE_PSYCHIC]  = pl_type_icons_psychic_NCGR_lz,
	[TYPE_ICE]      = pl_type_icons_ice_NCGR_lz,
	[TYPE_DRAGON]   = pl_type_icons_dragon_NCGR_lz,
	[TYPE_DARK]     = pl_type_icons_dark_NCGR_lz,
	
	[TYPE_MAX + CONTEST_TYPE_COOL]   = pl_type_icons_cool_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_BEAUTY] = pl_type_icons_beauty_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_CUTE]   = pl_type_icons_cute_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_SMART]  = pl_type_icons_smart_NCGR_lz,
	[TYPE_MAX + CONTEST_TYPE_TOUGH]  = pl_type_icons_tough_NCGR_lz
};

ATTRIBUTE_ALIGN(4) static const u8 sMoveTypeIconPaletteIndex[] = {
	[TYPE_NORMAL]   = 0,
	[TYPE_FIGHTING] = 0,
	[TYPE_FLYING]   = 1,
	[TYPE_POISON]   = 1,
	[TYPE_GROUND]   = 0,
	[TYPE_ROCK]     = 0,
	[TYPE_BUG]      = 2,
	[TYPE_GHOST]    = 1,
	[TYPE_STEEL]    = 0,
	[TYPE_MYSTERY]  = 2,
	[TYPE_FIRE]     = 0,
	[TYPE_WATER]    = 1,
	[TYPE_GRASS]    = 2,
	[TYPE_ELECTRIC] = 0,
	[TYPE_PSYCHIC]  = 1,
	[TYPE_ICE]      = 1,
	[TYPE_DRAGON]   = 2,
	[TYPE_DARK]     = 0,
	
	[TYPE_MAX + CONTEST_TYPE_COOL]   = 0,
	[TYPE_MAX + CONTEST_TYPE_BEAUTY] = 1,
	[TYPE_MAX + CONTEST_TYPE_CUTE]   = 1,
	[TYPE_MAX + CONTEST_TYPE_SMART]  = 2,
	[TYPE_MAX + CONTEST_TYPE_TOUGH]  = 0
};

ATTRIBUTE_ALIGN(4) static const u32 sMoveCategoryIconIndexDP[] = {
	[CLASS_PHYSICAL] = type_icons_physical_NCGR_lz,
	[CLASS_SPECIAL]  = type_icons_special_NCGR_lz,
	[CLASS_STATUS]   = type_icons_status_NCGR_lz
};

ATTRIBUTE_ALIGN(4) static const u32 sMoveCategoryIconIndexPt[] = {
	[CLASS_PHYSICAL] = pl_type_icons_physical_NCGR_lz,
	[CLASS_SPECIAL]  = pl_type_icons_special_NCGR_lz,
	[CLASS_STATUS]   = pl_type_icons_status_NCGR_lz
};

ATTRIBUTE_ALIGN(4) static const u8 sMoveCategoryIconPaletteIndex[] = {
	[CLASS_PHYSICAL] = 0,
	[CLASS_SPECIAL]  = 1,
	[CLASS_STATUS]   = 0
};


u32 TypeIcon_GetChar(PokemonType moveType) {
	GF_ASSERT(moveType < NELEMS(sMoveTypeIconIndexDP));
	if (gIsDiamondPearl) {
		return sMoveTypeIconIndexDP[moveType];
	} else {
		return sMoveTypeIconIndexPt[moveType];
	}
}


u32 TypeIcon_GetPlttSrc(void) {
	if (gIsDiamondPearl) {
		return type_icons_shared_NCLR;
	} else {
		return pl_type_icons_shared_NCLR;
	}
}


u32 TypeIcon_GetCell(void) {
	if (gIsDiamondPearl) {
		return type_icons_cell_NCER_lz;
	} else {
		return pl_type_icons_cell_NCER_lz;
	}
}


u8 TypeIcon_GetPltt(PokemonType moveType) {
	GF_ASSERT(moveType < NELEMS(sMoveTypeIconPaletteIndex));
	return sMoveTypeIconPaletteIndex[moveType];
}


NarcID TypeIcon_GetNARC(void) {
	if (gIsDiamondPearl) {
		return NARC_INDEX_DP_BATTLE__GRAPHIC__BATT_OBJ;
	} else {
		return NARC_INDEX_PL_BATTLE__GRAPHIC__PL_BATT_OBJ;
	}
}


u32 CategoryIcon_GetChar(MoveClass moveCat) {
	GF_ASSERT(moveCat < NELEMS(sMoveCategoryIconIndexDP));
	if (gIsDiamondPearl) {
		return sMoveCategoryIconIndexDP[moveCat];
	} else {
		return sMoveCategoryIconIndexPt[moveCat];
	}
}


u8 CategoryIcon_GetPltt(MoveClass moveCat) {
	GF_ASSERT(moveCat < NELEMS(sMoveCategoryIconPaletteIndex));
	return sMoveCategoryIconPaletteIndex[moveCat];
}


NarcID CategoryIcon_GetNARC(void) {
	if (gIsDiamondPearl) {
		return NARC_INDEX_DP_BATTLE__GRAPHIC__BATT_OBJ;
	} else {
		return NARC_INDEX_PL_BATTLE__GRAPHIC__PL_BATT_OBJ;
	}
}
