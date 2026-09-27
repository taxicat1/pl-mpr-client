#include <nitro.h>

#include "mod_detect_version.h"

#include "game_version.h"

static inline BOOL FS_FileExists(const char* path);
static BOOL Mod_IsSupportedPkmnGame(void);
static void Mod_InternalDetectVersion(void);


static inline BOOL FS_FileExists(const char* path) {
	FSFileID id;
	return FS_ConvertPathToFileID(&id, path);
}


static BOOL Mod_IsSupportedPkmnGame(void) {
	return FS_FileExists("data/eoo.dat") && FS_FileExists("poketool/icongra/poke_icon.narc") &&
		/* English only */ FS_FileExists("resource/eng/zukan/zukan.narc");
}


static void Mod_InternalDetectVersion(void) {
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


void Mod_DetectVersion(void) {
	GameVersion_Set(VERSION_NONE);
	gIsPlatinum = FALSE;
	gIsDiamondPearl = FALSE;
	
	if (Mod_IsSupportedPkmnGame()) {
		Mod_DetectVersion();
	}
}
