#include <nitro.h>

#include "game_version.h"

const u8 gGameVersion = VERSION_DIAMOND;
const u8 gGameLanguage = LANGUAGE_JAPANESE;


void GameVersion_Set(u8 newVersion) {
	// gGameVersion is in .rodata, must do this to override it
	*(u8*)&gGameVersion = newVersion;
}
