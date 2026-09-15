#include <nitro.h>
#include <string.h>

#include "special_ribbons.h"

#include "savedata.h"


int SpecialRibbons_GetSaveSize(void) {
	return sizeof(SpecialRibbonsSave);
}


void SpecialRibbons_Init(SpecialRibbonsSave* specialRibbons) {
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_HOENN_MARINE] = 0; 
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_HOENN_LAND]   = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_HOENN_SKY]    = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_UNUSED_03]    = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_UNUSED_04]    = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_UNUSED_05]    = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_UNUSED_06]    = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_RED]          = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_GREEN]        = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_BLUE]         = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_FESTIVAL]     = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_CARNIVAL]     = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_CLASSIC]      = 0;
	specialRibbons->receivedSpecialRibbons[SPECIAL_RIBBON_PREMIER]      = 0;
}

SpecialRibbonsSave* SaveData_GetSpecialRibbons(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_RIBBONS);
}
