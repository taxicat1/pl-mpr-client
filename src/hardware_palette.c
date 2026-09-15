#include <nitro.h>

#include "hardware_palette.h"

u16 *GetHardwareMainBgPaletteAddress(void) {
	return (u16*)HW_BG_PLTT;
}


u16 *GetHardwareSubBgPaletteAddress(void) {
	return (u16*)HW_DB_BG_PLTT;
}
