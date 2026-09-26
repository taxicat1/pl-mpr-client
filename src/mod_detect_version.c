#include <nitro.h>

#include "mod_detect_version.h"

#include "game_version.h"

static inline BOOL FS_FileExists(const char* path);
static BOOL Mod_IsSupportedPkmnGame(void);
static void Mod_DetectVersion(void);
static void Mod_DetectLanguage(void);


static inline BOOL FS_FileExists(const char* path) {
	FSFileID id;
	return FS_ConvertPathToFileID(&id, path);
}


static BOOL Mod_IsSupportedPkmnGame(void) {
	return FS_FileExists("data/eoo.dat") && FS_FileExists("poketool/icongra/poke_icon.narc");
}


static void Mod_DetectVersion(void) {
	if (FS_FileExists("poketool/personal/pl_personal.narc")) {
		GameVersion_Set(VERSION_PLATINUM);
		gIsPlatinum = TRUE;
		gIsDiamondPearl = FALSE;
	
	} else if (FS_FileExists("poketool/personal_pearl/personal.narc")) {
		GameVersion_Set(VERSION_PEARL);
		gIsPlatinum = FALSE;
		gIsDiamondPearl = TRUE;
	
	} else if (FS_FileExists("poketool/personal/personal.narc")) {
		GameVersion_Set(VERSION_DIAMOND);
		gIsPlatinum = FALSE;
		gIsDiamondPearl = TRUE;
	}
}


static void Mod_DetectLanguage(void) {
	if (FS_FileExists("resource/eng/zukan/zukan.narc")) {
		GameLanguage_Set(LANGUAGE_ENGLISH);
	} else if (FS_FileExists("resource/ger/zukan/zukan.narc")) {
		GameLanguage_Set(LANGUAGE_GERMAN);
	} else if (FS_FileExists("resource/spa/zukan/zukan.narc")) {
		GameLanguage_Set(LANGUAGE_SPANISH);
	} else if (FS_FileExists("resource/ita/zukan/zukan.narc")) {
		GameLanguage_Set(LANGUAGE_ITALIAN);
	} else if (FS_FileExists("resource/fra/zukan/zukan.narc")) {
		GameLanguage_Set(LANGUAGE_FRENCH);
	}
}


void Mod_DetectVersionAndLanguage(void) {
	GameVersion_Set(VERSION_NONE);
	GameLanguage_Set(LANGUAGE_NONE);
	gIsPlatinum = FALSE;
	gIsDiamondPearl = FALSE;
	
	if (Mod_IsSupportedPkmnGame()) {
		Mod_DetectVersion();
		Mod_DetectLanguage();
	}
}
