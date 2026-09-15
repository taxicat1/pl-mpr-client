#include <nitro.h>

#include "ribbon.h"

#include "constants/pokemon.h"
#include "fs/graphic/pst_gra.naix"
#include "fs/msgdata/ribbons.h"

// The special ribbons are unobtainable in-game through normal means
// Their description cannot be viewed, showing only placeholder text, unless the player
// has received a Pokemon with the ribbon via Mystery Gift
#define SPECIAL_DESCRIPTION_MASK	0x8000
#define SPECIAL_DESCRIPTIONS_START  RIBBONS_2003RegionalTournament_Description

typedef struct {
	PokemonDataParam  monDataParam;
	u16               spriteID;
	u16               paletteNum;
	u16               nameID;
	u16               descriptionID;
} RibbonData;

static const RibbonData sRibbonDataTable[] = {
	[RIBBON_HOENN_CHAMPION] = {
		.monDataParam  = MON_DATA_CHAMPION_RIBBON,
		.spriteID      = hoenn_champion_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennChampion,
		.descriptionID = RIBBONS_HoennChampion_Description,
	},
	[RIBBON_HOENN_COOL] = {
		.monDataParam  = MON_DATA_COOL_RIBBON,
		.spriteID      = hoenn_contest_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennCool,
		.descriptionID = RIBBONS_HoennCool_Description,
	},
	[RIBBON_HOENN_COOL_SUPER] = {
		.monDataParam  = MON_DATA_COOL_RIBBON_SUPER,
		.spriteID      = hoenn_contest_super_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennCoolSuper,
		.descriptionID = RIBBONS_HoennCoolSuper_Description,
	},
	[RIBBON_HOENN_COOL_HYPER] = {
		.monDataParam  = MON_DATA_COOL_RIBBON_HYPER,
		.spriteID      = hoenn_contest_hyper_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennCoolHyper,
		.descriptionID = RIBBONS_HoennCoolHyper_Description,
	},
	[RIBBON_HOENN_COOL_MASTER] = {
		.monDataParam  = MON_DATA_COOL_RIBBON_MASTER,
		.spriteID      = hoenn_contest_master_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennCoolMaster,
		.descriptionID = RIBBONS_HoennCoolMaster_Description,
	},
	[RIBBON_HOENN_BEAUTY] = {
		.monDataParam  = MON_DATA_BEAUTY_RIBBON,
		.spriteID      = hoenn_contest_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennBeauty,
		.descriptionID = RIBBONS_HoennBeauty_Description,
	},
	[RIBBON_HOENN_BEAUTY_SUPER] = {
		.monDataParam  = MON_DATA_BEAUTY_RIBBON_SUPER,
		.spriteID      = hoenn_contest_super_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennBeautySuper,
		.descriptionID = RIBBONS_HoennBeautySuper_Description,
	},
	[RIBBON_HOENN_BEAUTY_HYPER] = {
		.monDataParam  = MON_DATA_BEAUTY_RIBBON_HYPER,
		.spriteID      = hoenn_contest_hyper_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennBeautyHyper,
		.descriptionID = RIBBONS_HoennBeautyHyper_Description,
	},
	[RIBBON_HOENN_BEAUTY_MASTER] = {
		.monDataParam  = MON_DATA_BEAUTY_RIBBON_MASTER,
		.spriteID      = hoenn_contest_master_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennBeautyMaster,
		.descriptionID = RIBBONS_HoennBeautyMaster_Description,
	},
	[RIBBON_HOENN_CUTE] = {
		.monDataParam  = MON_DATA_CUTE_RIBBON,
		.spriteID      = hoenn_contest_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_HoennCute,
		.descriptionID = RIBBONS_HoennCute_Description,
	},
	[RIBBON_HOENN_CUTE_SUPER] = {
		.monDataParam  = MON_DATA_CUTE_RIBBON_SUPER,
		.spriteID      = hoenn_contest_super_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_HoennCuteSuper,
		.descriptionID = RIBBONS_HoennCuteSuper_Description,
	},
	[RIBBON_HOENN_CUTE_HYPER] = {
		.monDataParam  = MON_DATA_CUTE_RIBBON_HYPER,
		.spriteID      = hoenn_contest_hyper_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_HoennCuteHyper,
		.descriptionID = RIBBONS_HoennCuteHyper_Description,
	},
	[RIBBON_HOENN_CUTE_MASTER] = {
		.monDataParam  = MON_DATA_CUTE_RIBBON_MASTER,
		.spriteID      = hoenn_contest_master_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_HoennCuteMaster,
		.descriptionID = RIBBONS_HoennCuteMaster_Description,
	},
	[RIBBON_HOENN_SMART] = {
		.monDataParam  = MON_DATA_SMART_RIBBON,
		.spriteID      = hoenn_contest_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennSmart,
		.descriptionID = RIBBONS_HoennSmart_Description,
	},
	[RIBBON_HOENN_SMART_SUPER] = {
		.monDataParam  = MON_DATA_SMART_RIBBON_SUPER,
		.spriteID      = hoenn_contest_super_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennSmartSuper,
		.descriptionID = RIBBONS_HoennSmartSuper_Description,
	},
	[RIBBON_HOENN_SMART_HYPER] = {
		.monDataParam  = MON_DATA_SMART_RIBBON_HYPER,
		.spriteID      = hoenn_contest_hyper_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennSmartHyper,
		.descriptionID = RIBBONS_HoennSmartHyper_Description,
	},
	[RIBBON_HOENN_SMART_MASTER] = {
		.monDataParam  = MON_DATA_SMART_RIBBON_MASTER,
		.spriteID      = hoenn_contest_master_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennSmartMaster,
		.descriptionID = RIBBONS_HoennSmartMaster_Description,
	},
	[RIBBON_HOENN_TOUGH] = {
		.monDataParam  = MON_DATA_TOUGH_RIBBON,
		.spriteID      = hoenn_contest_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennTough,
		.descriptionID = RIBBONS_HoennTough_Description,
	},
	[RIBBON_HOENN_TOUGH_SUPER] = {
		.monDataParam  = MON_DATA_TOUGH_RIBBON_SUPER,
		.spriteID      = hoenn_contest_super_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennToughSuper,
		.descriptionID = RIBBONS_HoennToughSuper_Description,
	},
	[RIBBON_HOENN_TOUGH_HYPER] = {
		.monDataParam  = MON_DATA_TOUGH_RIBBON_HYPER,
		.spriteID      = hoenn_contest_hyper_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennToughHyper,
		.descriptionID = RIBBONS_HoennToughHyper_Description,
	},
	[RIBBON_HOENN_TOUGH_MASTER] = {
		.monDataParam  = MON_DATA_TOUGH_RIBBON_MASTER,
		.spriteID      = hoenn_contest_master_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennToughMaster,
		.descriptionID = RIBBONS_HoennToughMaster_Description,
	},
	[RIBBON_HOENN_WINNING] = {
		.monDataParam  = MON_DATA_WINNING_RIBBON,
		.spriteID      = hoenn_winning_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennWinning,
		.descriptionID = RIBBONS_HoennWinning_Description,
	},
	[RIBBON_HOENN_VICTORY] = {
		.monDataParam  = MON_DATA_VICTORY_RIBBON,
		.spriteID      = hoenn_victory_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennVictory,
		.descriptionID = RIBBONS_HoennVictory_Description,
	},
	[RIBBON_HOENN_ARTIST] = {
		.monDataParam  = MON_DATA_ARTIST_RIBBON,
		.spriteID      = hoenn_artist_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennArtist,
		.descriptionID = RIBBONS_HoennArtist_Description,
	},
	[RIBBON_HOENN_EFFORT] = {
		.monDataParam  = MON_DATA_EFFORT_RIBBON,
		.spriteID      = hoenn_effort_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_HoennEffort,
		.descriptionID = RIBBONS_HoennEffort_Description,
	},
	[RIBBON_HOENN_MARINE] = {
		.monDataParam  = MON_DATA_MARINE_RIBBON,
		.spriteID      = hoenn_marine_land_sky_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennMarine,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 0,
	},
	[RIBBON_HOENN_LAND] = {
		.monDataParam  = MON_DATA_LAND_RIBBON,
		.spriteID      = hoenn_marine_land_sky_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennLand,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 1,
	},
	[RIBBON_HOENN_SKY] = {
		.monDataParam  = MON_DATA_SKY_RIBBON,
		.spriteID      = hoenn_marine_land_sky_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennSky,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 2,
	},
	[RIBBON_HOENN_COUNTRY] = {
		.monDataParam  = MON_DATA_COUNTRY_RIBBON,
		.spriteID      = hoenn_country_national_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_HoennCountry,
		.descriptionID = RIBBONS_HoennCountryWorld_Description,
	},
	[RIBBON_HOENN_NATIONAL] = {
		.monDataParam  = MON_DATA_NATIONAL_RIBBON,
		.spriteID      = hoenn_country_national_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_HoennNational,
		.descriptionID = RIBBONS_HoennNational_Description,
	},
	[RIBBON_HOENN_EARTH] = {
		.monDataParam  = MON_DATA_EARTH_RIBBON,
		.spriteID      = hoenn_earth_world_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_HoennEarth,
		.descriptionID = RIBBONS_HoennEarth_Description,
	},
	[RIBBON_HOENN_WORLD] = {
		.monDataParam  = MON_DATA_WORLD_RIBBON,
		.spriteID      = hoenn_earth_world_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_HoennWorld,
		.descriptionID = RIBBONS_HoennCountryWorld_Description,
	},
	[RIBBON_SINNOH_CHAMP] = {
		.monDataParam  = MON_DATA_SINNOH_CHAMP_RIBBON,
		.spriteID      = sinnoh_champion_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_SinnohChamp,
		.descriptionID = RIBBONS_SinnohChamp_Description,
	},
	[RIBBON_COOL] = {
		.monDataParam  = MON_DATA_SUPER_COOL_RIBBON,
		.spriteID      = sinnoh_contest_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Cool,
		.descriptionID = RIBBONS_Cool_Description,
	},
	[RIBBON_COOL_GREAT] = {
		.monDataParam  = MON_DATA_SUPER_COOL_RIBBON_GREAT,
		.spriteID      = sinnoh_contest_great_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_CoolGreat,
		.descriptionID = RIBBONS_CoolGreat_Description,
	},
	[RIBBON_COOL_ULTRA] = {
		.monDataParam  = MON_DATA_SUPER_COOL_RIBBON_ULTRA,
		.spriteID      = sinnoh_contest_ultra_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_CoolUltra,
		.descriptionID = RIBBONS_CoolUltra_Description,
	},
	[RIBBON_COOL_MASTER] = {
		.monDataParam  = MON_DATA_SUPER_COOL_RIBBON_MASTER,
		.spriteID      = sinnoh_contest_master_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_CoolMaster,
		.descriptionID = RIBBONS_CoolMaster_Description,
	},
	[RIBBON_BEAUTY] = {
		.monDataParam  = MON_DATA_SUPER_BEAUTY_RIBBON,
		.spriteID      = sinnoh_contest_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Beauty,
		.descriptionID = RIBBONS_Beauty_Description,
	},
	[RIBBON_BEAUTY_GREAT] = {
		.monDataParam  = MON_DATA_SUPER_BEAUTY_RIBBON_GREAT,
		.spriteID      = sinnoh_contest_great_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_BeautyGreat,
		.descriptionID = RIBBONS_BeautyGreat_Description,
	},
	[RIBBON_BEAUTY_ULTRA] = {
		.monDataParam  = MON_DATA_SUPER_BEAUTY_RIBBON_ULTRA,
		.spriteID      = sinnoh_contest_ultra_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_BeautyUltra,
		.descriptionID = RIBBONS_BeautyUltra_Description,
	},
	[RIBBON_BEAUTY_MASTER] = {
		.monDataParam  = MON_DATA_SUPER_BEAUTY_RIBBON_MASTER,
		.spriteID      = sinnoh_contest_master_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_BeautyMaster,
		.descriptionID = RIBBONS_BeautyMaster_Description,
	},
	[RIBBON_CUTE] = {
		.monDataParam  = MON_DATA_SUPER_CUTE_RIBBON,
		.spriteID      = sinnoh_contest_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_Cute,
		.descriptionID = RIBBONS_Cute_Description,
	},
	[RIBBON_CUTE_GREAT] = {
		.monDataParam  = MON_DATA_SUPER_CUTE_RIBBON_GREAT,
		.spriteID      = sinnoh_contest_great_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_CuteGreat,
		.descriptionID = RIBBONS_CuteGreat_Description,
	},
	[RIBBON_CUTE_ULTRA] = {
		.monDataParam  = MON_DATA_SUPER_CUTE_RIBBON_ULTRA,
		.spriteID      = sinnoh_contest_ultra_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_CuteUltra,
		.descriptionID = RIBBONS_CuteUltra_Description,
	},
	[RIBBON_CUTE_MASTER] = {
		.monDataParam  = MON_DATA_SUPER_CUTE_RIBBON_MASTER,
		.spriteID      = sinnoh_contest_master_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_CuteMaster,
		.descriptionID = RIBBONS_CuteMaster_Description,
	},
	[RIBBON_SMART] = {
		.monDataParam  = MON_DATA_SUPER_SMART_RIBBON,
		.spriteID      = sinnoh_contest_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_Smart,
		.descriptionID = RIBBONS_Smart_Description,
	},
	[RIBBON_SMART_GREAT] = {
		.monDataParam  = MON_DATA_SUPER_SMART_RIBBON_GREAT,
		.spriteID      = sinnoh_contest_great_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_SmartGreat,
		.descriptionID = RIBBONS_SmartGreat_Description,
	},
	[RIBBON_SMART_ULTRA] = {
		.monDataParam  = MON_DATA_SUPER_SMART_RIBBON_ULTRA,
		.spriteID      = sinnoh_contest_ultra_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_SmartUltra,
		.descriptionID = RIBBONS_SmartUltra_Description,
	},
	[RIBBON_SMART_MASTER] = {
		.monDataParam  = MON_DATA_SUPER_SMART_RIBBON_MASTER,
		.spriteID      = sinnoh_contest_master_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_SmartMaster,
		.descriptionID = RIBBONS_SmartMaster_Description,
	},
	[RIBBON_TOUGH] = {
		.monDataParam  = MON_DATA_SUPER_TOUGH_RIBBON,
		.spriteID      = sinnoh_contest_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_Tough,
		.descriptionID = RIBBONS_Tough_Description,
	},
	[RIBBON_TOUGH_GREAT] = {
		.monDataParam  = MON_DATA_SUPER_TOUGH_RIBBON_GREAT,
		.spriteID      = sinnoh_contest_great_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_ToughGreat,
		.descriptionID = RIBBONS_ToughGreat_Description,
	},
	[RIBBON_TOUGH_ULTRA] = {
		.monDataParam  = MON_DATA_SUPER_TOUGH_RIBBON_ULTRA,
		.spriteID      = sinnoh_contest_ultra_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_ToughUltra,
		.descriptionID = RIBBONS_ToughUltra_Description,
	},
	[RIBBON_TOUGH_MASTER] = {
		.monDataParam  = MON_DATA_SUPER_TOUGH_RIBBON_MASTER,
		.spriteID      = sinnoh_contest_master_NCGR,
		.paletteNum    = 4,
		.nameID        = RIBBONS_ToughMaster,
		.descriptionID = RIBBONS_ToughMaster_Description,
	},
	[RIBBON_ABILITY] = {
		.monDataParam  = MON_DATA_ABILITY_RIBBON,
		.spriteID      = ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Ability,
		.descriptionID = RIBBONS_Ability_Description,
	},
	[RIBBON_GREAT_ABILITY] = {
		.monDataParam  = MON_DATA_GREAT_ABILITY_RIBBON,
		.spriteID      = great_ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_GreatAbility,
		.descriptionID = RIBBONS_GreatAbility_Description,
	},
	[RIBBON_DOUBLE_ABILITY] = {
		.monDataParam  = MON_DATA_DOUBLE_ABILITY_RIBBON,
		.spriteID      = double_ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_DoubleAbility,
		.descriptionID = RIBBONS_DoubleAbility_Description,
	},
	[RIBBON_MULTI_ABILITY] = {
		.monDataParam  = MON_DATA_MULTI_ABILITY_RIBBON,
		.spriteID      = multi_ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_MultiAbility,
		.descriptionID = RIBBONS_MultiAbility_Description,
	},
	[RIBBON_PAIR_ABILITY] = {
		.monDataParam  = MON_DATA_PAIR_ABILITY_RIBBON,
		.spriteID      = pair_ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_PairAbility,
		.descriptionID = RIBBONS_PairAbility_Description,
	},
	[RIBBON_WORLD_ABILITY] = {
		.monDataParam  = MON_DATA_WORLD_ABILITY_RIBBON,
		.spriteID      = world_ability_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_WorldAbility,
		.descriptionID = RIBBONS_WorldAbility_Description,
	},
	[RIBBON_ALERT] = {
		.monDataParam  = MON_DATA_ALERT_RIBBON,
		.spriteID      = alert_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_Alert,
		.descriptionID = RIBBONS_Alert_Description,
	},
	[RIBBON_SHOCK] = {
		.monDataParam  = MON_DATA_SHOCK_RIBBON,
		.spriteID      = shock_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Shock,
		.descriptionID = RIBBONS_Shock_Description,
	},
	[RIBBON_DOWNCAST] = {
		.monDataParam  = MON_DATA_DOWNCAST_RIBBON,
		.spriteID      = downcast_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Downcast,
		.descriptionID = RIBBONS_Downcast_Description,
	},
	[RIBBON_CARELESS] = {
		.monDataParam  = MON_DATA_CARELESS_RIBBON,
		.spriteID      = careless_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_Careless,
		.descriptionID = RIBBONS_Careless_Description,
	},
	[RIBBON_RELAX] = {
		.monDataParam  = MON_DATA_RELAX_RIBBON,
		.spriteID      = relax_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_Relax,
		.descriptionID = RIBBONS_Relax_Description,
	},
	[RIBBON_SNOOZE] = {
		.monDataParam  = MON_DATA_SNOOZE_RIBBON,
		.spriteID      = snooze_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Snooze,
		.descriptionID = RIBBONS_Snooze_Description,
	},
	[RIBBON_SMILE] = {
		.monDataParam  = MON_DATA_SMILE_RIBBON,
		.spriteID      = smile_NCGR,
		.paletteNum    = 2,
		.nameID        = RIBBONS_Smile,
		.descriptionID = RIBBONS_Smile_Description,
	},
	[RIBBON_GORGEOUS] = {
		.monDataParam  = MON_DATA_GORGEOUS_RIBBON,
		.spriteID      = gorgeous_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Gorgeous,
		.descriptionID = RIBBONS_Gorgeous_Description,
	},
	[RIBBON_ROYAL] = {
		.monDataParam  = MON_DATA_ROYAL_RIBBON,
		.spriteID      = royal_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_Royal,
		.descriptionID = RIBBONS_Royal_Description,
	},
	[RIBBON_GORGEOUS_ROYAL] = {
		.monDataParam  = MON_DATA_GORGEOUS_ROYAL_RIBBON,
		.spriteID      = gorgeous_royal_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_GorgeousRoyal,
		.descriptionID = RIBBONS_GorgeousRoyal_Description,
	},
	[RIBBON_FOOTPRINT] = {
		.monDataParam  = MON_DATA_FOOTPRINT_RIBBON,
		.spriteID      = footprint_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Footprint,
		.descriptionID = RIBBONS_Footprint_Description,
	},
	[RIBBON_RECORD] = {
		.monDataParam  = MON_DATA_RECORD_RIBBON,
		.spriteID      = record_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Record,
		.descriptionID = RIBBONS_Record_Description,
	},
	[RIBBON_HISTORY] = {
		.monDataParam  = MON_DATA_HISTORY_RIBBON,
		.spriteID      = history_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_History,
		.descriptionID = RIBBONS_History_Description,
	},
	[RIBBON_LEGEND] = {
		.monDataParam  = MON_DATA_LEGEND_RIBBON,
		.spriteID      = legend_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Legend,
		.descriptionID = RIBBONS_Legend_Description,
	},
	[RIBBON_RED] = {
		.monDataParam  = MON_DATA_RED_RIBBON,
		.spriteID      = red_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Red,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 7,
	},
	[RIBBON_GREEN] = {
		.monDataParam  = MON_DATA_GREEN_RIBBON,
		.spriteID      = green_NCGR,
		.paletteNum    = 3,
		.nameID        = RIBBONS_Green,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 8,
	},
	[RIBBON_BLUE] = {
		.monDataParam  = MON_DATA_BLUE_RIBBON,
		.spriteID      = blue_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Blue,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 9,
	},
	[RIBBON_FESTIVAL] = {
		.monDataParam  = MON_DATA_FESTIVAL_RIBBON,
		.spriteID      = festival_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Festival,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 10,
	},
	[RIBBON_CARNIVAL] = {
		.monDataParam  = MON_DATA_CARNIVAL_RIBBON,
		.spriteID      = carnival_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Carnival,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 11,
	},
	[RIBBON_CLASSIC] = {
		.monDataParam  = MON_DATA_CLASSIC_RIBBON,
		.spriteID      = classic_NCGR,
		.paletteNum    = 1,
		.nameID        = RIBBONS_Classic,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 12,
	},
	[RIBBON_PREMIER] = {
		.monDataParam  = MON_DATA_PREMIER_RIBBON,
		.spriteID      = premier_NCGR,
		.paletteNum    = 0,
		.nameID        = RIBBONS_Premier,
		.descriptionID = SPECIAL_DESCRIPTION_MASK | 13,
	},
};


u32 Ribbon_GetData(u8 ribbonID, RibbonDataField field) {
	switch (field) {
		case RIBBON_DATA_MON_DATA_PARAM:
			return sRibbonDataTable[ribbonID].monDataParam;
		
		case RIBBON_DATA_SPRITE_ID:
			return sRibbonDataTable[ribbonID].spriteID;
		
		case RIBBON_DATA_PALETTE_NUM:
			return sRibbonDataTable[ribbonID].paletteNum;
		
		case RIBBON_DATA_NAME_ID:
			return sRibbonDataTable[ribbonID].nameID;
		
		case RIBBON_DATA_DESCRIPTION_ID:
			return sRibbonDataTable[ribbonID].descriptionID;
	}
	
	return 0;
}


u32 Ribbon_GetDescriptionID(SpecialRibbonsSave* specialRibbons, u8 ribbonID) {
	if (sRibbonDataTable[ribbonID].descriptionID & SPECIAL_DESCRIPTION_MASK) {
		return SPECIAL_DESCRIPTIONS_START + specialRibbons->receivedSpecialRibbons[(u8)sRibbonDataTable[ribbonID].descriptionID];
	}
	
	return sRibbonDataTable[ribbonID].descriptionID;
}
